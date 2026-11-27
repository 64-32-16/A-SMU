"""3D printable A-SMU enclosure, 240 x 100 x 300 mm, CadQuery.

Recessed removable front/rear in grooves. Blank rear. Two-piece shells fit
CORE One INDX; separate rails and bolted joining straps. No metal variant edits.
X=width,Y=front to rear,Z=up. STL exports are individually oriented for printing.
"""
from pathlib import Path
from itertools import combinations
import json
import math
import struct
from collections import Counter
import cadquery as cq
from ASMU_Frontausschnitte import apply_front_cutouts

ROOT=Path(__file__).resolve().parent


def box(x,y,z,dx,dy,dz):
    return cq.Workplane('XY').box(dx,dy,dz,centered=False).translate((x,y,z))


def bore(origin,direction,diameter,length):
    return cq.Solid.makeCylinder(diameter/2,length,cq.Vector(*origin),cq.Vector(*direction))


def flip(wp,p):
    return wp.rotate((0,0,0),(0,1,0),180).translate((p['width'],0,p['height']))


def build(p):
    w,h,d,t=[p[k] for k in ('width','height','depth','wall')]
    g=p['seam_gap'];half=(h-g)/2
    assert t>p['outer_radius']>0
    ro=p['outer_radius']
    outer=box(0,0,0,w,d,half).edges('|Y and <Z').fillet(ro)
    bottom=outer.cut(box(t,-1,t,w-2*t,d+2,half+1))
    rim=t+p['groove_depth'];rl=p['rim_length']
    for y in (0,d-rl):
        reinforcement=box(0,y,0,w,rl,half).cut(box(rim,y-1,rim,w-2*rim,rl+2,half+1))
        bottom=bottom.union(reinforcement.intersect(outer))
    top=flip(bottom,p)
    pt=p['panel_thickness'];gw=p['groove_width'];ps=p['panel_setback']
    for y in (ps-(gw-pt)/2,d-ps-pt-(gw-pt)/2):
        slot=box(t,y,t,w-2*t,gw,h-2*t)
        bottom=bottom.cut(slot);top=top.cut(slot)
    # The source sketch includes a 136 x 93 mm display frame reference.
    # Relieve the front reinforcement BEHIND the panel, preserving shell wall.
    dx0,dz0,dx1,dz1=p['display_reference_bbox_xz']
    relief=box(dx0-.3,ps+pt+(gw-pt)/2,t,dx1-dx0+.6,
               rl-ps-pt-(gw-pt)/2+.1,h-2*t)
    bottom=bottom.cut(relief);top=top.cut(relief)
    parts={'Boden':bottom,'Deckel':top}
    for name,x,sign in [('Steg_links',t+p['rail_clearance'],1),
                         ('Steg_rechts',w-t-p['rail_clearance'],-1)]:
        rx=x if sign==1 else x-p['rail_thickness']
        rail=box(rx,p['rail_end_inset'],(h-p['rail_height'])/2,
                 p['rail_thickness'],d-2*p['rail_end_inset'],p['rail_height'])
        # Contact pads bridge the 0.2-mm sliding clearance at the screws only.
        for y in p['screw_y']:
            for z in p['screw_z']:
                rail=rail.union(bore((x,y,z),(-sign,0,0),9,p['rail_clearance']))
                rail=rail.cut(bore((x-sign*(p['rail_clearance']+.01),y,z),(sign,0,0),
                                  p['insert_bore'],p['insert_depth']+p['rail_clearance']+.01))
                shell='Boden' if z<h/2 else 'Deckel'
                start=-1 if sign==1 else w+1
                parts[shell]=parts[shell].cut(bore((start,y,z),(sign,0,0),p['screw_clearance'],t+2))
        parts[name]=rail.clean()
    vents=[]
    for x in p['slot_bank_x']:
        for k in range(p['slot_count_per_bank']):
            y=p['slot_y_start']+k*p['slot_pitch']
            vents.append(cq.Workplane('XY',origin=(0,0,h-t-1)).center(x,y)
                         .slot2D(p['slot_length'],p['slot_width']).extrude(t+2).val())
    parts['Deckel']=parts['Deckel'].cut(cq.Compound.makeCompound(vents)).clean()
    pc=t+p['panel_edge_clearance']
    parts['Frontplatte']=box(pc,ps,pc,w-2*pc,pt,h-2*pc)
    parts['Rueckplatte']=box(pc,d-ps-pt,pc,w-2*pc,pt,h-2*pc)
    parts,cutmeta=apply_front_cutouts(parts,p,ROOT/'Front_Quelle.sc')
    # Separate joining straps on inner planar faces; captive M3 nuts.
    for i,x in enumerate(p['join_x'],1):
        strap=box(x-7,p['split_y']-20,t,14,40,6)
        for y in p['join_y']:
            strap=strap.cut(bore((x,y,t-1),(0,0,1),p['screw_clearance'],8))
            pocket=cq.Workplane('XY',origin=(x,y,t+3.4)).polygon(6,5.8/math.cos(math.pi/6)).extrude(2.7)
            strap=strap.cut(pocket)
            parts['Boden']=parts['Boden'].cut(bore((x,y,-1),(0,0,1),p['screw_clearance'],t+2))
            parts['Deckel']=parts['Deckel'].cut(bore((w-x,y,h-t-1),(0,0,1),p['screw_clearance'],t+2))
        parts[f'Verbinder_Boden_{i}']=strap.clean()
        parts[f'Verbinder_Deckel_{i}']=flip(strap,p).clean()
    # Flat butt joint at Y=171 with 0.3-mm clearance, bridged by rails/straps.
    whole={name:parts[name].clean() for name in ('Boden','Deckel')}
    for name in ('Boden','Deckel'):
        wp=parts.pop(name)
        a=p['split_y']-p['split_gap']/2;b=p['split_y']+p['split_gap']/2
        parts[name+'_vorne']=wp.intersect(box(-1,-1,-1,w+2,a+1,h+2)).clean()
        parts[name+'_hinten']=wp.intersect(box(-1,b,-1,w+2,d-b+1,h+2)).clean()
    return {k:v.clean() for k,v in parts.items()},whole,cutmeta


COLORS={'Boden_vorne':(.28,.31,.36),'Boden_hinten':(.28,.31,.36),
        'Deckel_vorne':(.28,.31,.36),'Deckel_hinten':(.28,.31,.36),
        'Frontplatte':(.79,.81,.84),'Rueckplatte':(.50,.55,.62),
        'Steg_links':(.83,.55,.20),'Steg_rechts':(.83,.55,.20)}


def shift_for(name,exploded=False):
    if not exploded:return (0,0,0)
    if name.startswith('Deckel') or name.startswith('Verbinder_Deckel'):return (0,0,75)
    if name=='Frontplatte':return (0,-45,0)
    if name=='Rueckplatte':return (0,45,0)
    return (0,0,0)


def assembly(parts,exploded=False):
    a=cq.Assembly(name='ASMU_Druckgehaeuse_100mm')
    for name,wp in parts.items():
        a.add(wp,name=name,color=cq.Color(*COLORS.get(name,(.59,.66,.73))),
              loc=cq.Location(cq.Vector(*shift_for(name,exploded))))
    return a


def print_orientation(name,wp,p):
    if name.startswith('Deckel') or name.startswith('Verbinder_Deckel'):
        wp=flip(wp,p)
    elif name in ('Frontplatte','Rueckplatte'):
        wp=wp.rotate((0,0,0),(1,0,0),90)
    elif name.startswith('Steg_'):
        # Make insert bores vertical and face up, 260-mm length diagonal on bed.
        angle=90 if name.endswith('links') else -90
        wp=wp.rotate((0,0,0),(0,1,0),angle)
        wp=wp.rotate((0,0,0),(0,0,1),-50)
    b=wp.val().BoundingBox()
    return wp.translate((-b.xmin,-b.ymin,-b.zmin))


def stl_check(path):
    raw=path.read_bytes();n=struct.unpack_from('<I',raw,80)[0]
    assert len(raw)==84+50*n
    edges=Counter();volume=0.0
    for i in range(n):
        vals=struct.unpack_from('<12fH',raw,84+50*i)
        v=[tuple(round(x,5) for x in vals[j:j+3]) for j in (3,6,9)]
        # Include only non-degenerate triangles in the topology check.
        assert len(set(v))==3,(path.name,i)
        for a,b in ((v[0],v[1]),(v[1],v[2]),(v[2],v[0])):
            edges[tuple(sorted((a,b)))]+=1
        a,b,c=v
        volume+=(a[0]*(b[1]*c[2]-b[2]*c[1])+a[1]*(b[2]*c[0]-b[0]*c[2])+a[2]*(b[0]*c[1]-b[1]*c[0]))/6
    assert all(n==2 for n in edges.values()),(path.name,Counter(edges.values()))
    assert volume>0,(path.name,volume)
    return {'triangles':n,'closed_two_manifold_edges':True,'mesh_volume_mm3':round(volume,3)}


def validate(parts,p,out,cutmeta):
    from OCP.BRepTools import BRepTools
    for wp in parts.values():BRepTools.Clean_s(wp.val().wrapped)
    report={'unit':'mm','parts':{},'collisions':{},'lid_lifts_mm':[],
            'printer_xyz_mm':p['printer_xyz'],'brim_allowance_mm':p['brim_allowance'],
            'strength':'not assessed','holes_lowered_mm':p['upper_right_drop']}
    for name,wp in parts.items():
        sh=wp.val();assert sh.isValid() and len(sh.Solids())==1,name
        imported=cq.importers.importStep(str(out/f'{name}.step')).val()
        assert imported.isValid() and len(imported.Solids())==1
        assert abs(imported.Volume()-sh.Volume())<.02
        pr=print_orientation(name,wp,p);bb=pr.val().BoundingBox()
        sizes=[bb.xlen,bb.ylen,bb.zlen]
        assert sizes[0]+2*p['brim_allowance']<=p['printer_xyz'][0]+1e-5,(name,sizes)
        assert sizes[1]+2*p['brim_allowance']<=p['printer_xyz'][1]+1e-5,(name,sizes)
        assert sizes[2]<=p['printer_xyz'][2]+1e-5
        mesh=stl_check(ROOT/'STL'/f'{name}.stl')
        assert abs(mesh['mesh_volume_mm3']-sh.Volume())/sh.Volume()<.005,(name,mesh)
        report['parts'][name]={'valid_solid':True,'step_roundtrip':True,
                               'print_XYZ_mm':[round(v,3) for v in sizes],
                               'fits_with_brim':True,**mesh}
    for (na,a),(nb,b) in combinations(parts.items(),2):
        v=a.val().intersect(b.val()).Volume()
        assert v<1e-4,(na,nb,v)
        report['collisions'][na+'/'+nb]=round(v,8)
    moving=[k for k in parts if k.startswith('Deckel') or k.startswith('Verbinder_Deckel')]
    stationary=[k for k in parts if k not in moving]
    for dz in [.15,1,4,8,15,40,80]:
        for m in moving:
            for s in stationary:
                assert parts[m].val().translate((0,0,dz)).intersect(parts[s].val()).Volume()<1e-4,(m,s,dz)
        report['lid_lifts_mm'].append(dz)
    for name in ('Frontplatte','Rueckplatte'):
        for dz in [.15,1,10,70,110]:
            for n in ('Boden_vorne','Boden_hinten'):
                assert parts[name].val().translate((0,0,dz)).intersect(parts[n].val()).Volume()<1e-4,(name,n,dz)
    report['panels_insert_from_above_without_lid']=True
    dx0,dz0,dx1,dz1=p['display_reference_bbox_xz']
    frame=box(dx0,p['panel_setback']+p['panel_thickness'],dz0,dx1-dx0,6,dz1-dz0)
    for name in ('Boden_vorne','Boden_hinten','Deckel_vorne','Deckel_hinten'):
        assert frame.val().intersect(parts[name].val()).Volume()<1e-4,('display frame clearance',name)
    report['display_reference_136x93_clear_of_shells']=True
    front=parts['Frontplatte'];fb=front.val().BoundingBox()
    assert len(front.faces('<Y').val().innerWires())==8
    for c in cutmeta['cutouts']:
        x,z=c['global_center_xz_mm']
        assert fb.xmin<x-c['width_mm']/2 and x+c['width_mm']/2<fb.xmax
        assert fb.zmin<z-c['height_mm']/2 and z+c['height_mm']/2<fb.zmax
    b=cq.Compound.makeCompound([v.val() for v in parts.values()]).BoundingBox()
    dims=[b.xlen,b.ylen,b.zlen]
    assert all(abs(a-b)<1e-5 for a,b in zip(dims,[p['width'],p['depth'],p['height']])),dims
    report['assembly_XYZ_mm']=[round(v,4) for v in dims]
    imported=cq.importers.importStep(str(out/'ASMU_Druckgehaeuse_100mm.step')).val()
    assert imported.isValid() and len(imported.Solids())==len(parts)
    report['assembly_step_solids']=len(parts)
    report['front_cutouts']=cutmeta
    return report


def coupon(p):
    # A small insert/clearance sample; print before full enclosure.
    sample=box(0,0,0,38,18,8)
    for x in (8,19,30):
        sample=sample.cut(bore((x,9,2),(0,0,1),p['insert_bore']+(x-19)/110,6.1))
        sample=sample.cut(bore((x,9,-.1),(0,0,1),p['screw_clearance'],2.2))
    return sample.clean()


def main():
    p=json.loads((ROOT/'parameter.json').read_text())
    out=ROOT/'CAD';stl=ROOT/'STL';out.mkdir(exist_ok=True);stl.mkdir(exist_ok=True)
    parts,whole,cutmeta=build(p)
    for name,wp in parts.items():
        cq.exporters.export(wp,str(out/f'{name}.step'))
        cq.exporters.export(print_orientation(name,wp,p),str(stl/f'{name}.stl'),tolerance=.04,angularTolerance=.12)
    # Large-printer reference models, outside the fitted STL directory.
    for name,wp in whole.items():
        cq.exporters.export(wp,str(out/f'{name}_ungeteilt.step'))
    assembly(parts).save(str(out/'ASMU_Druckgehaeuse_100mm.step'))
    assembly(parts,True).save(str(out/'ASMU_Explosionsansicht.step'))
    sample=coupon(p)
    cq.exporters.export(sample,str(stl/'Teststueck_M3_Huelsen.stl'),tolerance=.04,angularTolerance=.12)
    report=validate(parts,p,out,cutmeta)
    report['coupon']=stl_check(stl/'Teststueck_M3_Huelsen.stl')
    (out/'Pruefprotokoll.json').write_text(json.dumps(report,indent=2,ensure_ascii=False))
    from Vorschau import preview
    preview(parts,p,out)
    print(json.dumps({'parts':len(parts),'dimensions':report['assembly_XYZ_mm'],'all_print_parts_fit':True,'upper_right_drop':p['upper_right_drop']}))


if __name__=='__main__':main()
