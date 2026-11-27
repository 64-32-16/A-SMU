#pragma once
#include <stdint.h>
#include <math.h>

struct PlotPoint { double x; double y; bool valid; };
struct PlotRange { double start = 0; double division = 1; bool automatic = true; };

// Clip a normalized line to [0,1] x [0,1] before converting to display integers.
inline bool ClipPlotLine(double& x0, double& y0, double& x1, double& y1)
{
    if (!isfinite(x0) || !isfinite(y0) || !isfinite(x1) || !isfinite(y1)) return false;
    const double dx = x1-x0, dy = y1-y0;
    const double p[] = {-dx, dx, -dy, dy};
    const double q[] = {x0, 1-x0, y0, 1-y0};
    double lo=0, hi=1;
    for (unsigned i=0; i<4; ++i)
    {
        if (p[i] == 0) { if (q[i] < 0) return false; }
        else
        {
            const double t=q[i]/p[i];
            if (p[i]<0) { if(t>hi) return false; if(t>lo) lo=t; }
            else { if(t<lo) return false; if(t<hi) hi=t; }
        }
    }
    x1=x0+hi*dx; y1=y0+hi*dy; x0+=lo*dx; y0+=lo*dy;
    return true;
}

// Generic streamed point source; no dependency on measurement buffers or SMU types.
struct PlotPointSource {
    const void* context;
    PlotPoint (*read)(const void*,uint16_t);
    uint16_t count;
};

// Non-owning drawing view: offset,offset+2,... . The caller anchors offset
// to stable sample numbers, so advancing a ring does not change selection.
// Consume synchronously while this adapter and the original source are alive.
struct PlotEverySecondSource {
    PlotPointSource original;
    uint8_t offset=0;
    static PlotPoint Read(const void* context,uint16_t index) {
        const auto& self=*static_cast<const PlotEverySecondSource*>(context);
        const auto& source=self.original;
        const unsigned n=unsigned(index)*2+(self.offset&1);
        auto point=source.read(source.context,static_cast<uint16_t>(n));
        // Do not bridge an invalid measurement hidden by the sampling step.
        if(index) {
            const auto skipped=source.read(source.context,static_cast<uint16_t>(n-1));
            if(!skipped.valid || !isfinite(skipped.x) || !isfinite(skipped.y)) point.valid=false;
        }
        return point;
    }
    PlotPointSource Source() const {
        const unsigned first=offset&1;
        return {this,Read,static_cast<uint16_t>(original.count>first?(original.count-first+1)/2:0)};
    }
};
