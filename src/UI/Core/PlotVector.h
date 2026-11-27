#pragma once
#include "PlotGeometry.h"

// Screen-space vertices, never a second measurement history. A move starts a
// fresh strip; invalid samples and clipped-out excursions cannot bridge gaps.
namespace PlotVector {
struct Vertex {int16_t x,y;uint16_t move;};
struct Result {unsigned count=0,strips=0,points=0;bool any=false,complete=true;};
inline Result Build(const PlotPointSource& source,const PlotRange& xr,const PlotRange& yr,
                    int width,int height,Vertex* out,unsigned capacity,
                    Vertex* dots=nullptr,unsigned dotCapacity=0) {
    Result result;
    const double xs=xr.division*10,ys=yr.division*6;
    if(!source.read || !out || width<=0 || height<=0 || width>16383 || height>16383 || xs<=0 || ys<=0 ||
       !isfinite(xs) || !isfinite(ys)) {result.complete=false;return result;}
    const double sx=1/xs,sy=1/ys;
    if(!isfinite(sx) || !isfinite(sy)) {result.complete=false;return result;}
    bool previous=false,connected=false;double px=0,py=0;
    auto inside=[](double x,double y) {return x>=0 && x<=1 && y>=0 && y<=1;};
    auto vertex=[&](double x,double y,bool move) {
        if(result.count==capacity) {result.complete=false;return;}
        out[result.count++]={static_cast<int16_t>(x*width+0.5),
                            static_cast<int16_t>((1-y)*height+0.5),static_cast<uint16_t>(move)};
        if(move) ++result.strips;
    };
    for(uint16_t i=0;i<source.count;++i) {
        const auto point=source.read(source.context,i);
        if(!point.valid) {previous=connected=false;continue;}
        const double dx=point.x-xr.start,dy=point.y-yr.start;
        const double nx=dx==xs?1:dx*sx,ny=dy==ys?1:dy*sy;
        if(!isfinite(nx) || !isfinite(ny)) {previous=connected=false;continue;}
        if(inside(nx,ny)) {
            result.any=true;
            if(dots) {
                if(result.points==dotCapacity) {result.complete=false;return result;}
                dots[result.points++]={static_cast<int16_t>(nx*width+0.5),
                    static_cast<int16_t>((1-ny)*height+0.5),0};
            }
        }
        if(previous) {
            double a=px,b=py,c=nx,d=ny;
            const bool previousInside=inside(px,py);
            if((previousInside && inside(nx,ny)) || ClipPlotLine(a,b,c,d)) {
                if(!connected || !previousInside) vertex(a,b,true);
                vertex(c,d,false);
                result.any=true;connected=true;
            } else connected=false;
        }
        if(!result.complete) return result;
        px=nx;py=ny;previous=true;
    }
    return result;
}
// BEGIN + END per strip, and eight setup/restore commands.
inline unsigned Words(const Result& result,bool dots=false) {
    return result.count+2*result.strips+8+(dots?result.points+3:0);
}
inline bool Fits(const Result& result,unsigned usedBytes,unsigned reserveBytes,bool dots=false) {
    return result.complete && usedBytes<=8192 && reserveBytes<=8192-usedBytes &&
           Words(result,dots)*4<=8192-usedBytes-reserveBytes;
}
}
