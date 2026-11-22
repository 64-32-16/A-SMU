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
