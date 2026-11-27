from pathlib import Path
from ASMU_parametrisch import COLORS

def preview(parts,p,out):
    """CAD mesh preview with a depth buffer, not generated imagery."""
    import numpy as np
    from PIL import Image, ImageDraw, ImageFont
    background=(243,245,247)
    def render(exploded):
        # Orthographic software z-buffer. No browser or GPU required.
        ww,hh=1100,800
        canvas=np.full((hh,ww,3),background,dtype=np.uint8)
        zbuffer=np.full((hh,ww),-np.inf)
        shifts={"Boden":(0,0,0),"Deckel":(0,0,75),"Frontplatte":(0,-45,0),"Rueckplatte":(0,45,0),"Steg_links":(0,0,0),"Steg_rechts":(0,0,0)}
        target=np.array([120.,150.,90. if exploded else 50.])
        view=np.array([660.,-760.,600.])-target;view/=np.linalg.norm(view)
        right=np.cross([0.,0.,1.],view);right/=np.linalg.norm(right)
        up=np.cross(view,right)
        scale=1.8 if exploded else 2.2
        light=np.array([-.6,-.8,1.]);light/=np.linalg.norm(light)
        for name,wp in parts.items():
            verts,tris=wp.val().tessellate(.1,.15)
            world=np.array([[v.x,v.y,v.z] for v in verts])
            if exploded:
                delta=shifts.get(name,(0,0,75) if name.endswith("oben") else (0,0,0))
                world+=np.array(delta)
            rel=world-target
            screen=np.column_stack((ww/2+(rel@right)*scale,hh/2-(rel@up)*scale,rel@view))
            for ids in tris:
                q=screen[np.array(ids)]
                x0=max(0,int(np.floor(q[:,0].min())));x1=min(ww-1,int(np.ceil(q[:,0].max())))
                y0=max(0,int(np.floor(q[:,1].min())));y1=min(hh-1,int(np.ceil(q[:,1].max())))
                if x0>x1 or y0>y1:continue
                den=(q[1,1]-q[2,1])*(q[0,0]-q[2,0])+(q[2,0]-q[1,0])*(q[0,1]-q[2,1])
                if abs(den)<1e-10:continue
                yy,xx=np.mgrid[y0:y1+1,x0:x1+1]
                xx=xx+.5;yy=yy+.5
                a=((q[1,1]-q[2,1])*(xx-q[2,0])+(q[2,0]-q[1,0])*(yy-q[2,1]))/den
                b=((q[2,1]-q[0,1])*(xx-q[2,0])+(q[0,0]-q[2,0])*(yy-q[2,1]))/den
                c=1-a-b
                zz=a*q[0,2]+b*q[1,2]+c*q[2,2]
                old=zbuffer[y0:y1+1,x0:x1+1]
                mask=(a>=-1e-8)&(b>=-1e-8)&(c>=-1e-8)&(zz>old)
                if not mask.any():continue
                tri=world[np.array(ids)]
                n=np.cross(tri[1]-tri[0],tri[2]-tri[0]);length=np.linalg.norm(n)
                shade=.56+.44*abs(n@light)/max(length,1e-12)
                color=np.clip(np.array(COLORS.get(name,(.60,.65,.70)))*shade*255,0,255).astype(np.uint8)
                canvas[y0:y1+1,x0:x1+1][mask]=color
                old[mask]=zz[mask]
        return Image.fromarray(canvas)
    img=Image.new("RGB",(2250,1200),"#f3f5f7")
    img.paste(render(False),(15,240));img.paste(render(True),(1135,240))
    draw=ImageDraw.Draw(img)
    font_dir=Path("/usr/share/fonts/truetype/dejavu")
    def font(size,bold=False):
        try:return ImageFont.truetype(str(font_dir/("DejaVuSans-Bold.ttf" if bold else "DejaVuSans.ttf")),size)
        except OSError:return ImageFont.load_default()
    draw.text((90,50),"A–SMU  /  PROTOTYP 100 mm",fill="#18283a",font=font(48,True))
    draw.text((90,132),"240 × 100 × 300 mm  ·  Alu-U-Schalen 2 mm  ·  12 seitliche M3-Befestigungen",fill="#526176",font=font(27))
    draw.text((340,215),"Zusammengebaut",fill="#253244",font=font(30))
    draw.text((1350,215),"Geöffnet · durchgehende Innenstege",fill="#253244",font=font(30))
    draw.text((90,1050),"Front: 235,6 × 95,6 × 3 mm  ·  Displayausschnitt 124 × 79 mm  ·  Schlitze 2 × 40 mm",fill="#526176",font=font(25))
    draw.text((90,1100),"Rückseite vorerst offen. Hülsenbohrungen vorläufig; Tragfähigkeit und Bauteilfreiräume noch zu prüfen.",fill="#526176",font=font(23))
    img.save(out/"ASMU_Vorschau.png")

