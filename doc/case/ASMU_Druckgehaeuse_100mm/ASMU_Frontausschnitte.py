"""Transfer closed cutout profiles from the user's SolidCraft sketch.

Curves remain exact arcs/lines/circles. Only a translation centers the existing
layout on the larger plate; there is no scaling, mirroring, or shape correction.
Open reference geometry and the outer plate boundary are not cutting profiles.
"""
from pathlib import Path
from collections import defaultdict
import copy
import json
import math
import zipfile
import cadquery as cq


def apply_front_cutouts(parts, p, source):
    with zipfile.ZipFile(source) as z:
        doc=json.loads(z.read("document.json"))
    sk=next(f for f in doc["features"] if f["type"]=="sketch" and len(f["sketch"]["curves"])>0)
    sketch=sk["sketch"]
    pts=sketch["points"]
    for point in pts:
        if point.get("id") in ("c3.center","c4.center"):
            point["pos"][1]-=p.get("upper_right_drop",0.0)
    curves=sketch["curves"]
    extrusion=next(f for f in doc["features"] if f["type"]=="extrude" and f["sketch"]==sk["id"])
    boundary_ids=set(extrusion["profiles"]["loops"][0])
    boundary=[c for c in curves if c["id"] in boundary_ids]
    boundary_pts=[pts[c[k]]["pos"] for c in boundary for k in ["a","b"]]
    sx0=min(v[0] for v in boundary_pts);sx1=max(v[0] for v in boundary_pts)
    sy0=min(v[1] for v in boundary_pts);sy1=max(v[1] for v in boundary_pts)
    old_width=sx1-sx0;old_height=sy1-sy0
    sh=parts["Frontplatte"].val()
    b=sh.BoundingBox()
    centering=[(b.xlen-old_width)/2,(b.zlen-old_height)/2]
    shift=[b.xmin+centering[0]-sx0,b.zmin+centering[1]-sy0]
    y=b.ymax+1
    cut_depth=b.ylen+2

    def vec(pos):return cq.Vector(pos[0]+shift[0],y,pos[1]+shift[1])
    def key(i):return tuple(round(x,7) for x in pts[i]["pos"])
    def edge(c):
        a,b=[pts[c[k]]["pos"] for k in ["a","b"]]
        if c["type"]=="line":return cq.Edge.makeLine(vec(a),vec(b))
        center=pts[c["c"]]["pos"]
        r=math.dist(a,center)
        assert abs(math.dist(b,center)-r)<1e-6
        aa=math.atan2(a[1]-center[1],a[0]-center[0])
        ab=math.atan2(b[1]-center[1],b[0]-center[0])
        sweep=(ab-aa)%(2*math.pi)
        mid=[center[0]+r*math.cos(aa+sweep/2),center[1]+r*math.sin(aa+sweep/2)]
        return cq.Edge.makeThreePointArc(vec(a),vec(mid),vec(b))

    eligible=[c for c in curves if not c.get("construction",False) and c["id"] not in boundary_ids]
    adjacency=defaultdict(list)
    for c in eligible:
        if c["type"] in ["line","arc"]:
            for k in ["a","b"]:adjacency[key(c[k])].append(c)
    used=set()
    profiles=[]
    ignored=[]
    for c in eligible:
        if c["id"] in used:continue
        if c["type"]=="circle":
            used.add(c["id"])
            wire=cq.Wire.makeCircle(c["r"],vec(pts[c["c"]]["pos"]),cq.Vector(0,-1,0))
            profiles.append(([c],wire))
            continue
        group=[];todo=[c]
        while todo:
            cc=todo.pop()
            if cc["id"] in used:continue
            used.add(cc["id"]);group.append(cc)
            for k in ["a","b"]:
                todo.extend(adjacency[key(cc[k])])
        vertices={key(cc[k]) for cc in group for k in ["a","b"]}
        if not all(len(adjacency[v])==2 for v in vertices):
            ignored.append([cc["id"] for cc in group])
            continue
        wire=cq.Wire.assembleEdges([edge(cc) for cc in group])
        assert wire.IsClosed() and wire.isValid()
        profiles.append((group,wire))

    assert len(profiles)==8,f"Expected 8 apertures, found {len(profiles)}"
    cutouts=[];cutters=[];jack_centers=[]
    for group,wire in profiles:
        face=cq.Face.makeFromWires(wire)
        cutter=cq.Solid.extrudeLinear(wire,[],cq.Vector(0,-cut_depth,0))
        assert cutter.isValid() and len(cutter.Solids())==1
        cutters.append(cutter)
        bb=face.BoundingBox()
        curves_by_type={cc["type"] for cc in group}
        if curves_by_type=={"circle"}:
            kind="Rundbohrung"
            center=pts[group[0]["c"]]["pos"]
            diameter=2*group[0]["r"]
        elif "arc" in curves_by_type:
            kind="Buchse_abgeflacht"
            center=pts[next(cc for cc in group if cc["type"]=="arc")["c"]]["pos"]
            diameter=12
            jack_centers.append([center[0]+shift[0],center[1]+shift[1]])
        else:
            kind="Displayausschnitt"
            center=[(bb.xmin+bb.xmax)/2-shift[0],(bb.zmin+bb.zmax)/2-shift[1]]
            diameter=None
        entry={"type":kind,"curves":[cc["id"] for cc in group],"center_from_panel_lower_left_mm":[center[0]+shift[0]-b.xmin,center[1]+shift[1]-b.zmin],"global_center_xz_mm":[center[0]+shift[0],center[1]+shift[1]],"width_mm":bb.xlen,"height_mm":bb.zlen,"area_mm2":face.Area()}
        if diameter is not None:entry["diameter_mm"]=diameter
        cutouts.append(entry)
    for a in range(len(cutters)):
        for bb in range(a):assert cutters[a].intersect(cutters[bb]).Volume()<1e-6
    new=copy.copy(parts)
    new["Frontplatte"]=parts["Frontplatte"].cut(cq.Compound.makeCompound(cutters)).clean()
    result=new["Frontplatte"].val()
    assert result.isValid() and len(result.Solids())==1
    expected_removed=sum(e["area_mm2"] for e in cutouts)*b.ylen
    removed=sh.Volume()-result.Volume()
    assert abs(expected_removed-removed)<1e-5
    assert len(new["Frontplatte"].faces("<Y").val().innerWires())==8
    xs=sorted({round(v[0],8) for v in jack_centers})
    zs=sorted({round(v[1],8) for v in jack_centers})
    assert len(xs)==2 and len(zs)==2
    assert abs(xs[1]-xs[0]-19.05)<1e-7 and abs(zs[1]-zs[0]-19.05)<1e-7
    meta={"source":Path(source).name,"source_panel_mm":[old_width,old_height],"target_panel_mm":[b.xlen,b.zlen,b.ylen],"translation_relative_to_panel_lower_left_mm":centering,"scaling":1,"jack_pitch_mm":[19.05,19.05],"aperture_count":len(cutouts),"cutouts":cutouts,"ignored_open_geometry":ignored,"removed_front_volume_mm3":removed,"verified_through_apertures":8}
    return new,meta

