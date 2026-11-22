#include "Histogram.h"
#include <math.h>
namespace {
bool Value(const BufferSample& sample,BufferQuantity quantity,double& value) {
    if(sample.Fault!=Hardware::Error::None) return false;
    switch(quantity) {
        case BufferQuantity::Voltage:value=sample.Voltage;break;
        case BufferQuantity::Current:value=sample.Current;break;
        case BufferQuantity::Power:value=sample.GetPower();break;
        case BufferQuantity::Resistance:if(!sample.HasValidResistance()) return false;value=sample.GetResistance();break;
        default:return false;
    }
    return isfinite(value);
}
}
bool ValidHistogramSettings(const HistogramSettings& s)
{
    return static_cast<unsigned>(s.quantity)<=3 && static_cast<unsigned>(s.method)<=3 &&
        s.bins>=1 && s.bins<=HistogramSettings::MaxBins && s.yMaximum>=1 && s.yMaximum<=BufferClass::Capacity &&
        isfinite(s.minimum) && isfinite(s.maximum) && fabs(s.minimum)<=1e12 && fabs(s.maximum)<=1e12 &&
        s.maximum>s.minimum && (s.maximum-s.minimum)/s.bins>=1e-12;
}
int HistogramResult::BinAt(double value) const
{
    if(!isfinite(value)) return -1;
    if(value<minimum) return 0;
    if(value>maximum) return bins+1;
    if(value==maximum) return bins;
    // Binary search gives interior boundaries to the bin on their right.
    unsigned low=0,high=bins;
    while(low+1<high) {
        const unsigned mid=(low+high)/2;
        if(value>=minimum+mid*width) low=mid;else high=mid;
    }
    return low+1;
}
void CalculateHistogram(const BufferSample* samples,uint16_t count,const HistogramSettings& s,HistogramResult& r)
{
    r=HistogramResult();
    if(!ValidHistogramSettings(s)) return;
    r.bins=s.bins;r.minimum=s.minimum;r.maximum=s.maximum;r.yMaximum=s.yMaximum;
    r.width=(r.maximum-r.minimum)/r.bins;
    if(!s.enabled || !samples) return;
    for(uint16_t i=0;i<count;++i) {
        double value;
        if(!Value(samples[i],s.quantity,value)) {++r.invalid;continue;}
        if(!r.hasData) r.dataMin=r.dataMax=value;
        else {r.dataMin=fmin(r.dataMin,value);r.dataMax=fmax(r.dataMax,value);}
        r.hasData=true;++r.valid;
    }
    if(r.hasData) {
        const bool rebin=s.method==HistogramScale::AutoBin || (s.method==HistogramScale::SmartScale &&
            (r.dataMin<r.minimum || r.dataMax>r.maximum || r.dataMax-r.dataMin<(r.maximum-r.minimum)*0.25));
        if(rebin) {
            r.minimum=r.dataMin;r.maximum=r.dataMax;
            if(r.maximum-r.minimum<1e-10) {
                const double pad=fmax(fabs(r.minimum)*0.05,1e-9);
                r.minimum-=pad;r.maximum+=pad;
            }
            r.width=(r.maximum-r.minimum)/r.bins;
        }
        for(uint16_t i=0;i<count;++i) {
            double value;if(!Value(samples[i],s.quantity,value)) continue;
            const int bin=r.BinAt(value);++r.counts[bin];
            if(r.counts[bin]>r.peak) r.peak=r.counts[bin];
        }
    }
    if(s.method!=HistogramScale::Off) {
        // Four integer-count divisions; every bar, including outliers, fits.
        r.yMaximum=static_cast<uint16_t>((r.peak+3)/4*4);
        if(r.yMaximum<4) r.yMaximum=4;
    }
}
