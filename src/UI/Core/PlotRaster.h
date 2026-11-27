#pragma once
#include "PlotGeometry.h"
#include <string.h>
#include <stdlib.h>

// One-bit plot raster. Every supplied point participates; only pixel overlap merges marks.
class PlotRaster {
public:
    PlotRaster(uint8_t* pixels,int width,int height):_pixels(pixels),_width(width),_height(height),_stride((width+7)/8) {}
    bool Draw(const PlotPointSource& source,const PlotRange& xr,const PlotRange& yr,
              bool lines,bool dots,unsigned xDivisions=10,unsigned yDivisions=6) {
        memset(_pixels,0,_stride*_height);
        if(!source.read || !xDivisions || !yDivisions || xr.division<=0 || yr.division<=0) return false;
        const double xSpan=xr.division*xDivisions,ySpan=yr.division*yDivisions;
        if(!isfinite(xSpan) || !isfinite(ySpan)) return false;
        const double xScale=1.0/xSpan,yScale=1.0/ySpan;
        // Keep division as a fallback for extreme ranges whose reciprocal
        // overflows. Ordinary graph ranges need only two divisions per draw.
        const bool multiplyX=isfinite(xScale),multiplyY=isfinite(yScale);
        bool previous=false,any=false;double px=0,py=0;
        for(uint16_t n=0;n<source.count;++n) {
            const PlotPoint point=source.read(source.context,n);
            if(!point.valid) {previous=false;continue;}
            const double dx=point.x-xr.start,dy=point.y-yr.start;
            // Preserve exact upper endpoints: reciprocal rounding must not
            // turn an on-edge point into an out-of-bounds value above 1.
            const double x=dx==xSpan?1.0:(multiplyX?dx*xScale:dx/xSpan);
            const double y=dy==ySpan?1.0:(multiplyY?dy*yScale:dy/ySpan);
            if(!isfinite(x) || !isfinite(y)) {previous=false;continue;}
            if(lines && previous) {
                double a=px,b=py,c=x,d=y;
                if(ClipPlotLine(a,b,c,d)) {Line(X(a),Y(b),X(c),Y(d));any=true;}
            }
            if(x>=0 && x<=1 && y>=0 && y<=1) {
                if(dots) {const int ix=X(x),iy=Y(y);Pixel(ix,iy);Pixel(ix-1,iy);Pixel(ix+1,iy);Pixel(ix,iy-1);Pixel(ix,iy+1);}
                any=true;
            }
            px=x;py=y;previous=true;
        }
        return any;
    }
private:
    int X(double x) const {return static_cast<int>(x*(_width-1)+0.5);}
    int Y(double y) const {return static_cast<int>((1-y)*(_height-1)+0.5);}
    void Pixel(int x,int y) {
        if(x>=0 && x<_width && y>=0 && y<_height) _pixels[y*_stride+x/8]|=0x80u>>(x&7);
    }
    void Line(int x,int y,int endX,int endY) {
        const int dx=abs(endX-x),sx=x<endX?1:-1,dy=-abs(endY-y),sy=y<endY?1:-1;
        int error=dx+dy;
        for(;;) {Pixel(x,y);if(x==endX && y==endY) break;
            const int twice=2*error;if(twice>=dy) {error+=dy;x+=sx;}if(twice<=dx) {error+=dx;y+=sy;}}
    }
    uint8_t* _pixels;int _width,_height,_stride;
};
