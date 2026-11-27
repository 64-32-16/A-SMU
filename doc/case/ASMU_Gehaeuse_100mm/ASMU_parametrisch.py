"""A-SMU, 100-mm sheet-metal prototype, mm, CadQuery.

X=width, Y=front to back, Z=up. Rear panel intentionally deferred.
Source profiles for eight front cutouts are preserved in Front_Quelle.sc.
M3 insert pilot dimensions are provisional. No threads/fasteners modelled.
Run: python3 ASMU_parametrisch.py
"""
from pathlib import Path
from itertools import combinations
import json
import cadquery as cq
from ASMU_Frontausschnitte import apply_front_cutouts

ROOT = Path(__file__).resolve().parent


def box(x,y,z,dx,dy,dz):
    return cq.Workplane('XY').box(dx,dy,dz,centered=False).translate((x,y,z))


def bore(origin,direction,diameter,length):
    return cq.Solid.makeCylinder(diameter/2,length,cq.Vector(*origin),cq.Vector(*direction))


def build(p):
    w,h,d,t = [p[k] for k in ('width','height','depth','sheet')]
    ri=p['inside_bend_radius']; ro=ri+t
    half=(h-p['seam_gap'])/2
    outer=box(0,0,0,w,d,half).edges('|Y and <Z').fillet(ro)
    cavity=box(t,-1,t,w-2*t,d+2,half+2).edges('|Y and <Z').fillet(ri)
    bottom=outer.cut(cavity).clean()
    top=bottom.rotate((w/2,0,h/2),(w/2,1,h/2),180)
    parts={'Boden':bottom,'Deckel':top}
    # The same three Y positions are used on both sides, above and below seam.
    for name,x,sign in [('Steg_links',t,1),('Steg_rechts',w-t,-1)]:
        rx=x if sign==1 else x-p['rail_thickness']
        rail=box(rx,p['rail_end_inset'],(h-p['rail_height'])/2,
                 p['rail_thickness'],d-2*p['rail_end_inset'],p['rail_height'])
        for y in p['screw_y']:
            for z in p['screw_z']:
                rail=rail.cut(bore((x-sign*.01,y,z),(sign,0,0),
                                   p['insert_pilot_diameter'],p['insert_pilot_depth']+.01))
                shell='Boden' if z<h/2 else 'Deckel'
                start=-1 if sign==1 else w+1
                parts[shell]=parts[shell].cut(bore((start,y,z),(sign,0,0),p['screw_clearance'],t+2))
        parts[name]=rail.clean()
    # Rounded 2 x 40 mm slots in two banks on the horizontal lid surface.
    slots=[]
    for x in p['slot_bank_x']:
        for k in range(p['slot_count_per_bank']):
            y=p['slot_y_start']+k*p['slot_pitch']
            slots.append(cq.Workplane('XY',origin=(0,0,h-t-1)).center(x,y)
                         .slot2D(p['slot_length'],p['slot_width']).extrude(t+2).val())
    parts['Deckel']=parts['Deckel'].cut(cq.Compound.makeCompound(slots)).clean()
    pc=t+p['panel_clearance']
    parts['Frontplatte']=box(pc,p['panel_setback'],pc,w-2*pc,p['panel_thickness'],h-2*pc).edges('|Y').fillet(ri)
    # Translate the exact 124 x 79 mm display cutout and all other existing
    # apertures together by -15 mm relative to the former 130-mm enclosure.
    parts,cutmeta=apply_front_cutouts(parts,p,ROOT/'Front_Quelle.sc')
    # Four small internal front mounting angles; right angles are mirrored.
    # Front screws use M3 tapped pilot holes, independent of seam inserts.
    fy=p['panel_setback']+p['panel_thickness']
    front_holes=[]
    for side in ('links','rechts'):
        left=side=='links'
        for level in ('unten','oben'):
            z0=5.0 if level=='unten' else h-17.0
            zc=11.0 if level=='unten' else h-11.0
            x0=t if left else w-t-12
            angle=box(x0,fy,z0,12,2,12).union(
                box(t if left else w-t-2,fy+2,z0,2,12,12))
            xc=t+6 if left else w-t-6
            angle=angle.cut(bore((xc,fy-.1,zc),(0,1,0),2.5,2.2))
            parts['Frontplatte']=parts['Frontplatte'].cut(
                bore((xc,p['panel_setback']-1,zc),(0,1,0),p['screw_clearance'],p['panel_thickness']+2))
            sy=fy+8
            sign=1 if left else -1
            sx=-1 if left else w+1
            shell='Boden' if level=='unten' else 'Deckel'
            parts[shell]=parts[shell].cut(bore((sx,sy,zc),(sign,0,0),p['screw_clearance'],t+2))
            ax=t-.1 if left else w-t+.1
            angle=angle.cut(bore((ax,sy,zc),(sign,0,0),2.5,2.2))
            parts[f'Frontwinkel_{side}_{level}']=angle.clean()
            front_holes.append([xc,zc])
    return {k:v.clean() for k,v in parts.items()},cutmeta,front_holes


COLORS={'Boden':(.28,.31,.35),'Deckel':(.28,.31,.35),
        'Frontplatte':(.79,.81,.84),'Steg_links':(.82,.55,.20),'Steg_rechts':(.82,.55,.20)}


def assembly(parts,exploded=False):
    a=cq.Assembly(name='ASMU_100mm')
    for name,wp in parts.items():
        shift=(0,0,0)
        if exploded:
            if name=='Deckel':shift=(0,0,80)
            elif name=='Frontplatte':shift=(0,-45,0)
            elif name.startswith('Frontwinkel') and name.endswith('oben'):shift=(0,0,80)
        a.add(wp,name=name,color=cq.Color(*COLORS.get(name,(.60,.65,.70))),
              loc=cq.Location(cq.Vector(*shift)))
    return a


def validate(parts,p,out,cutmeta):
    report={'unit':'mm','parts':{},'intersections_mm3':{},'lid_lift_mm':[],
            'rear_panel':'deferred; open rear', 'load_capacity':'not evaluated',
            'insert_pilots':'provisional; match selected insert before manufacture'}
    for name,wp in parts.items():
        sh=wp.val();assert sh.isValid() and len(sh.Solids())==1,name
        imported=cq.importers.importStep(str(out/f'{name}.step')).val()
        assert imported.isValid() and len(imported.Solids())==1,name
        assert abs(sh.Volume()-imported.Volume())<.01,name
        report['parts'][name]={'valid':True,'step_reimport':True,'volume_mm3':round(sh.Volume(),4)}
    for (na,a),(nb,b) in combinations(parts.items(),2):
        v=a.val().intersect(b.val()).Volume()
        assert v<1e-5,(na,nb,v)
        report['intersections_mm3'][na+'/'+nb]=round(v,8)
    moving=['Deckel']+[k for k in parts if k.startswith('Frontwinkel') and k.endswith('oben')]
    # Remove the front panel and screws before raising the complete lid.
    stationary=[k for k in parts if k not in moving and k!='Frontplatte']
    for dz in [.15,1,4,8,15,40,80]:
        for m in moving:
            for s in stationary:
                assert parts[m].val().translate((0,0,dz)).intersect(parts[s].val()).Volume()<1e-5,(m,s,dz)
        report['lid_lift_mm'].append(dz)
    comp=cq.Compound.makeCompound([v.val() for v in parts.values()])
    b=comp.BoundingBox()
    dims=[b.xlen,b.ylen,b.zlen]
    assert all(abs(a-b)<1e-5 for a,b in zip(dims,[p['width'],p['depth'],p['height']])),dims
    report['bounds_XYZ_mm']=[round(x,5) for x in dims]
    # A through aperture is enclosed by a continuous material margin.
    front=parts['Frontplatte'].val(); fb=front.BoundingBox()
    for c in cutmeta['cutouts']:
        x,z=c['global_center_xz_mm'];half_w=c['width_mm']/2;half_h=c['height_mm']/2
        assert fb.xmin<x-half_w and x+half_w<fb.xmax
        assert fb.zmin<z-half_h and z+half_h<fb.zmax
    faces=parts['Frontplatte'].faces('<Y').vals()
    assert len(faces)==1
    report['front_through_apertures']=len(faces[0].innerWires())
    assert report['front_through_apertures']==12 # Eight original + four fasteners.
    imported=cq.importers.importStep(str(out/'ASMU_Gehaeuse_100mm.step')).val()
    assert imported.isValid() and len(imported.Solids())==len(parts)
    report['assembly_step_solids']=len(imported.Solids())
    report['side_fasteners']={'per_side_per_shell':3,'total':12,
                             'y_mm':p['screw_y'],'z_mm':p['screw_z']}
    report['vents']={'count':2*p['slot_count_per_bank'],
                     'slot_mm':[p['slot_length'],p['slot_width']],
                     'web_mm':p['slot_pitch']-p['slot_width'],
                     'location':'horizontal lid only; 40 mm is slot length'}
    return report


def main():
    p=json.loads((ROOT/'parameter.json').read_text())
    parts,cutmeta,holes=build(p)
    out=ROOT/'CAD';out.mkdir(exist_ok=True)
    for name,wp in parts.items():
        cq.exporters.export(wp,str(out/f'{name}.step'))
    assembly(parts).save(str(out/'ASMU_Gehaeuse_100mm.step'))
    assembly(parts,True).save(str(out/'ASMU_Explosionsansicht.step'))
    report=validate(parts,p,out,cutmeta)
    (out/'Pruefprotokoll.json').write_text(json.dumps(report,indent=2,ensure_ascii=False))
    (out/'Frontausschnitte.json').write_text(json.dumps(cutmeta,indent=2,ensure_ascii=False))
    from Vorschau import preview
    preview(parts,p,out)
    print(json.dumps({'dimensions':report['bounds_XYZ_mm'],'parts':len(parts),'holes':report['front_through_apertures'],'valid':True}))


if __name__=='__main__':main()
