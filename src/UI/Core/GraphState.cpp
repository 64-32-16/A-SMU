#include "GraphState.h"
#include "DisplayFormat.h"
#include <math.h>
#include <stdio.h>

namespace
{
// Expand the full-history axis in readable steps instead of rescaling every frame.
double HistorySpan(double needed)
{
    const double decade=pow(10.0,floor(log10(needed)));
    const double fraction=needed/decade;
    return decade*(fraction<=1?1:fraction<=2?2:fraction<=5?5:10);
}
void Fit(PlotRange& range, double low, double high, unsigned divisions,bool nice=true)
{
    double span=high-low;
    if (span < 1e-12) span=fmax(fabs(low)*0.1, 1e-6);
    const double bottom=low-span*0.05,top=high+span*0.05;
    if(!nice) {range.start=bottom;range.division=(top-bottom)/divisions;return;}
    double target=span*1.1/divisions;
    for(unsigned attempt=0;attempt<8;++attempt) {
        const double decade=pow(10.0,floor(log10(target)));
        const double fraction=target/decade;
        const double factor=fraction<=1?1:fraction<=2?2:fraction<=2.5?2.5:fraction<=5?5:10;
        range.division=decade*factor;
        range.start=floor(bottom/range.division)*range.division;
        if(range.start+range.division*divisions>=top) break;
        target=range.division*1.01;
    }
}
}

void GraphState::SetIV(bool value)
{
    if(iv==value) return;
    if(value)
    {
        _timeSelected=selected;
        for(unsigned q=0;q<4;++q) _timeY[q]=y[q];
        selected=Current;
        y[Current]=_ivY;
    }
    else
    {
        _ivY=y[Current];
        selected=_timeSelected;
        for(unsigned q=0;q<4;++q) y[q]=_timeY[q];
    }
    iv=value;
    for(auto& fitted:_autoYFitted) fitted=false;
}
void GraphState::AutoScale() {
    X().automatic=true;
    for(unsigned q=0;q<4;++q) {y[q].automatic=true;_autoYFitted[q]=false;}
}
bool GraphState::Value(const BufferSample& s, Quantity q, double& value)
{
    if(s.Fault!=Hardware::Error::None) return false;
    switch(q) {
    case Voltage: value=s.Voltage; break;
    case Current: value=s.Current; break;
    case Power: value=s.GetPower(); break;
    case Resistance: if(!s.HasValidResistance()) return false; value=s.GetResistance(); break;
    }
    return isfinite(value);
}
double GraphState::Time(const BufferSample& s, uint32_t origin)
{ return static_cast<uint32_t>(s.TimeMs-origin)/1000.0; }

void GraphState::Resolve(const BufferSample* samples, uint16_t count)
{
    if(count == 0) {for(auto& fitted:_autoYFitted) fitted=false;return;}
    if(!hasOrigin) {
        timeOrigin=samples[0].TimeMs;hasOrigin=true;
        for(auto& fitted:_autoYFitted) fitted=false;
    }
    if(X().automatic)
    {
        if(!iv)
        {
            const double first=Time(samples[0],timeOrigin), last=Time(samples[count-1],timeOrigin);
            if(scrolling) {
                // Fixed-width window, advancing by one grid division only after its right edge.
                // Anchor to acquisition time, not the oldest retained ring-buffer sample.
                const double step=windowSeconds/XDivisions;
                X().start=last>windowSeconds?ceil((last-windowSeconds)/step)*step:0;
                X().division=windowSeconds/XDivisions;
            }
            else { X().start=first; X().division=HistorySpan(fmax(last-first,windowSeconds))/XDivisions; }
        }
        else
        {
            bool found=false; double low=0,high=0,v,i;
            for(uint16_t n=0;n<count;++n)
                if(Value(samples[n],Voltage,v) && Value(samples[n],Current,i))
                { if(!found) low=high=v; else { low=fmin(low,v);high=fmax(high,v); } found=true; }
            if(found) Fit(X(),low,high,XDivisions);
        }
    }
    for(unsigned q=0;q<4;++q)
    {
        if(!y[q].automatic) continue;
        bool found=false;double low=0,high=0;
        for(uint16_t n=0;n<count;++n)
        {
            double v, px=iv ? samples[n].Voltage : Time(samples[n],timeOrigin);
            if(!isfinite(px) || px<X().start || px>X().start+X().division*XDivisions ||
               !Value(samples[n],static_cast<Quantity>(q),v)) continue;
            if(!found) low=high=v; else { low=fmin(low,v); high=fmax(high,v); } found=true;
        }
        if(found) {
            const double top=y[q].start+y[q].division*YDivisions;
            if(tightYAxis) {
                // Keep a stable range until data escapes or occupies less than 75%.
                // Then fit actual extrema, with 5% reserve on each side.
                if(!_autoYFitted[q] || low<y[q].start || high>top ||
                   (high>low && high-low<(top-y[q].start)*0.75))
                    Fit(y[q],low,high,YDivisions,false);
            } else {
                if(!_autoYFitted[q]) Fit(y[q],low,high,YDivisions);
                else if(low<y[q].start || high>top)
                    Fit(y[q],fmin(low,y[q].start),fmax(high,top),YDivisions);
            }
            // A lone/constant initial sample cannot establish a useful noise span.
            if(high>low) _autoYFitted[q]=true;
        }
    }
}

bool GraphState::ValidRange(double start, double division, unsigned divisions)
{
    return isfinite(start) && isfinite(division) && fabs(start)<=1e12 && division>=1e-12 &&
           division<=1e12 && isfinite(start+division*divisions) && start+division*divisions>start;
}
bool GraphState::SetManual(bool xAxis, bool division, double value)
{
    PlotRange& range=xAxis?X():y[selected];
    const double a=division?range.start:value, b=division?value:range.division;
    if(!ValidRange(a,b,xAxis?XDivisions:YDivisions)) return false;
    range.start=a; range.division=b; range.automatic=false; return true;
}
bool GraphState::Zoom(double factor, double anchorX, double anchorY, ZoomAxis axis)
{
    if(!isfinite(factor) || factor<=0 || !isfinite(anchorX) || !isfinite(anchorY) ||
       anchorX<0 || anchorX>1 || anchorY<0 || anchorY>1 || axis>ZoomY) return false;
    PlotRange nextX=X(), nextY[4];
    if(axis!=ZoomY) {nextX.start+=nextX.division*XDivisions*(1-factor)*anchorX;nextX.division*=factor;}
    if(!ValidRange(nextX.start,nextX.division,XDivisions)) return false;
    for(unsigned q=0;q<4;++q) {
        nextY[q]=y[q];
        if(axis!=ZoomX) {nextY[q].start+=y[q].division*YDivisions*(1-factor)*anchorY;nextY[q].division*=factor;}
        if(!ValidRange(nextY[q].start,nextY[q].division,YDivisions)) return false;
    }
    X()=nextX; X().automatic=false;
    for(unsigned q=0;q<4;++q) { y[q]=nextY[q];y[q].automatic=false; }
    return true;
}
bool GraphState::Pan(double dx, double dy)
{
    if(!isfinite(dx) || !isfinite(dy)) return false;
    const double startX=X().start-dx*X().division*XDivisions;
    if(!ValidRange(startX,X().division,XDivisions)) return false;
    double starts[4];
    for(unsigned q=0;q<4;++q)
    {
        starts[q]=y[q].start;
        // Pan the whole visible plot, not just the curve whose axis is selected.
        // Keep the selected axis in sync even when its curve is currently hidden.
        if(IsCurveVisible(q) || q==selected)
        {
            starts[q]+=dy*y[q].division*YDivisions;
            if(!ValidRange(starts[q],y[q].division,YDivisions)) return false;
        }
    }
    // Commit together only after every affected range has passed validation.
    X().start=startX;X().automatic=false;
    for(unsigned q=0;q<4;++q) {y[q].start=starts[q];y[q].automatic=false;}
    return true;
}

uint16_t GraphState::MakePoints(const BufferSample* samples, uint16_t count, Quantity q, PlotPoint* out,uint8_t bucketLimit) const
{
    // Min/max envelope, in original sample order. Bounded geometry protects FT813 display-list RAM.
    if(!bucketLimit) return 0;
    if(bucketLimit>32) bucketLimit=32;
    uint16_t used=0;
    uint16_t firstSample=0,lastSample=count;
    if(!iv && count)
    {
        const double low=X().start,high=low+X().division*XDivisions;
        if(Time(samples[count-1],timeOrigin)<low || Time(samples[0],timeOrigin)>high) return 0;
        // Retain neighbors at the viewport edges so crossing segments are clipped correctly.
        while(firstSample+1<count && Time(samples[firstSample+1],timeOrigin)<low) ++firstSample;
        while(lastSample>firstSample+1 && Time(samples[lastSample-2],timeOrigin)>high) --lastSample;
    }
    auto emitBucket=[&](uint16_t begin,uint16_t end) {
        int minIndex=-1,maxIndex=-1;double low=0,high=0;bool gap=false;
        for(uint16_t n=begin;n<end;++n) {
            double v,px=iv?samples[n].Voltage:Time(samples[n],timeOrigin);
            if(!Value(samples[n],q,v) || !isfinite(px)) {gap=true;continue;}
            if(minIndex<0) {minIndex=maxIndex=n;low=high=v;}
            if(v<low) {low=v;minIndex=n;} if(v>high) {high=v;maxIndex=n;}
        }
        if(gap) {out[used++]={0,0,false};return;}
        if(minIndex<0) return;
        if(low==high) maxIndex=end-1;
        const int first=minIndex<maxIndex?minIndex:maxIndex,last=minIndex<maxIndex?maxIndex:minIndex;
        for(int pass=0;pass<(first==last?1:2);++pass) {
            const auto& sample=samples[pass==0?first:last];double v;Value(sample,q,v);
            out[used++]={iv?sample.Voltage:Time(sample,timeOrigin),v,true};
        }
    };
    if(!iv) {
        // Anchor buckets to the viewport time axis, not the growing sample count.
        // Appending samples can only change the unfinished bucket at the right.
        const double low=X().start,width=X().division*XDivisions;
        if(!isfinite(low) || !isfinite(width) || width<=0) return 0;
        uint16_t next=firstSample;
        if(next<lastSample && Time(samples[next],timeOrigin)<low) {
            emitBucket(next,next+1);++next; // left clipping neighbor
        }
        const double bucketWidth=width/bucketLimit;
        const double firstBucket=floor(low/bucketWidth);
        // Absolute time bins keep overlapping history identical after a window jump.
        // A partially visible bin at each edge needs at most one extra bin.
        for(unsigned bucket=0;bucket<=bucketLimit;++bucket) {
            const uint16_t begin=next;
            const double edge=fmin(low+width,(firstBucket+bucket+1)*bucketWidth);
            while(next<lastSample) {
                const double time=Time(samples[next],timeOrigin);
                if(time>edge || (time==edge && edge<low+width)) break;
                ++next;
            }
            emitBucket(begin,next);
            if(edge>=low+width) break;
        }
        if(next<lastSample) emitBucket(next,next+1); // right clipping neighbor
    } else {
        // I-V retains acquisition order; voltage need not be monotonic.
        const uint16_t visibleCount=lastSample-firstSample;
        const uint16_t buckets=visibleCount<bucketLimit?visibleCount:bucketLimit;
        for(uint16_t b=0;b<buckets;++b)
            emitBucket(firstSample+static_cast<uint32_t>(b)*visibleCount/buckets,
                       firstSample+static_cast<uint32_t>(b+1)*visibleCount/buckets);
    }
    // Always retain the newest visible endpoint, even inside a wide min/max bin.
    if(!iv && lastSample>firstSample) {
        const auto& tail=samples[lastSample-1];double value;
        const double time=Time(tail,timeOrigin);
        if(Value(tail,q,value) && (!used || !out[used-1].valid || out[used-1].x!=time))
            out[used++]={time,value,true};
    }
    return used;
}

void GraphState::FormatValue(double value,const char* unit,char* text,size_t size,unsigned precision)
{
    // Engineering prefixes keep axis labels readable across V/I/P/R ranges.
    const double magnitude=fabs(value);double scale=1;const char* prefix="";
    if(magnitude>0 && magnitude<1e-6) {scale=1e9;prefix="n";}
    else if(magnitude>0 && magnitude<1e-3) {scale=1e6;prefix="u";}
    else if(magnitude>0 && magnitude<1) {scale=1e3;prefix="m";}
    else if(magnitude>=1e6) {scale=1e-6;prefix="M";}
    else if(magnitude>=1e3) {scale=1e-3;prefix="k";}
    if(precision<1) precision=1;if(precision>7) precision=7;
    snprintf(text,size,"%.*g %s%s",static_cast<int>(precision),value*scale,prefix,unit);
}

void GraphState::FormatTime(double seconds,double tickSpacing,char* text,size_t size)
{
    if(!isfinite(seconds) || fabs(seconds)>1e12) {snprintf(text,size,"--:--");return;}
    unsigned decimals=0;
    if(tickSpacing>0 && tickSpacing<1) decimals=static_cast<unsigned>(fmin(6,ceil(-log10(tickSpacing))));
    const double scale=pow(10.0,decimals),rounded=floor(fabs(seconds)*scale+0.5)/scale;
    auto whole=static_cast<unsigned long long>(floor(rounded));
    unsigned fraction=static_cast<unsigned>(floor((rounded-whole)*scale+0.5));
    if(fraction>=scale) {++whole;fraction=0;}
    const char* sign=seconds<0 && rounded>0?"-":"";
    char minutes[21];DisplayFormat::FormatUnsigned(whole/60,minutes,sizeof(minutes),2);
    if(decimals) snprintf(text,size,"%s%s:%02u.%0*u",sign,minutes,static_cast<unsigned>(whole%60),static_cast<int>(decimals),fraction);
    else snprintf(text,size,"%s%s:%02u",sign,minutes,static_cast<unsigned>(whole%60));
}
void GraphState::FormatAxis(const PlotRange& range,unsigned divisions,unsigned tick,const char* unit,char* text,size_t size)
{
    const double magnitude=fmax(fabs(range.start),fabs(range.start+range.division*divisions));
    double scale=1;const char* prefix="";
    if(magnitude>0 && magnitude<1e-6) {scale=1e9;prefix="n";}
    else if(magnitude>0 && magnitude<1e-3) {scale=1e6;prefix="u";}
    else if(magnitude>0 && magnitude<1) {scale=1e3;prefix="m";}
    else if(magnitude>=1e6) {scale=1e-6;prefix="M";}
    else if(magnitude>=1e3) {scale=1e-3;prefix="k";}
    const double step=fabs(range.division*scale);
    unsigned decimals=step>0 && step<1?static_cast<unsigned>(fmin(9,ceil(-log10(step)))):0;
    // Keep fractional nice steps such as 2.5, and don't round manual steps away.
    if(decimals<9) {
        const double scaled=step*pow(10.0,decimals);
        if(fabs(scaled-floor(scaled+0.5))>=1e-7*fmax(1,scaled)) ++decimals;
    }
    double value=(range.start+range.division*tick)*scale;
    if(fabs(value)<0.5/pow(10.0,decimals)) value=0;
    snprintf(text,size,"%.*f %s%s",static_cast<int>(decimals),value,prefix,unit);
}
