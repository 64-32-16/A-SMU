#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include "UI/Pages/GraphPage.h"
#include "UI/Pages/HistogramPage.h"
#include "UI/Pages/ReadingTablePage.h"
#include "UI/Pages/SimulationPage.h"
#include "UI/Panels/GraphPanel.h"
#include "UI/Panels/XYGraphPopup.h"
#include "UI/Panels/SourcePanel.h"
#include "UI/Pages/MenuPage.h"
#include "Core/Beeper.h"
#include "UI/Core/PlotRaster.h"
#include "UI/Core/PlotVector.h"
#include "UI/Core/DisplayFormat.h"

static std::vector<std::string> texts;
GDClass GD; BeeperClass Beeper;
BeeperClass::BeeperClass() {} void BeeperClass::Click() {}
static unsigned captures=0, backdropDraws=0;
void GDClass::RequestBackdropCapture() {++captures;}
bool GDClass::DrawCapturedBackdrop() {++backdropDraws;return true;}
static unsigned modalDraws=0, nativeButtons=0, compactButtons=0, flatButtons=0, customButtons=0;
void GDClass::DrawModalWindow(int16_t,int16_t,int16_t,int16_t,int16_t,int16_t,int16_t w,int16_t h,uint32_t,uint32_t,uint32_t) {assert(w==600 && h==430);++modalDraws;}
void GDClass::ColorA(byte) {}
void GDClass::ColorRGB(uint32_t) {} void GDClass::Begin(Primitive) {}
void GDClass::PointSize(uint16_t) {}
void GDClass::LineWidth(uint16_t) {} void GDClass::Vertex2f(int16_t,int16_t) {}
void GDClass::VertexFormat(byte) {} void GDClass::SaveContext() {} void GDClass::RestoreContext() {}
static uint32_t lastForeground=0;
void GDClass::cmd_fgcolor(uint32_t color) {lastForeground=color;}
void GDClass::cmd_text(int16_t,int16_t,byte,uint16_t,const char* s) {texts.push_back(s);}
void GDClass::cmd_button(int16_t,int16_t,uint16_t,uint16_t,byte,uint16_t options,const char* s) {++nativeButtons;if(options&OPT_FLAT) ++flatButtons;texts.push_back(s);}
void GDClass::DrawSolidRoundedRect(int16_t,int16_t,int16_t,int16_t,uint16_t,uint32_t) {++compactButtons;}
void GDClass::DrawRoundedGradientRect(int16_t,int16_t,int16_t,int16_t,uint16_t,uint32_t,uint32_t) {++customButtons;}
void GDClass::DrawTabShape(int16_t,int16_t,int16_t,int16_t,uint16_t,uint32_t) {}
static unsigned tableDraws=0;
void GDClass::DrawTableGrid(int16_t,int16_t,int16_t,uint16_t rows,const int16_t*,uint8_t columns,uint32_t) {assert(rows==8 && columns==6);++tableDraws;}
static unsigned barDraws=0;
void GDClass::DrawHistogramBars(int16_t,int16_t,int16_t,int16_t,const uint16_t*,uint16_t count,uint16_t maximum,uint32_t) {assert(count>=3 && count<=66 && maximum>0);++barDraws;}
void GDClass::DrawPlotGrid(int16_t,int16_t,int16_t,int16_t,uint8_t,uint8_t,uint32_t) {}
static std::vector<PlotPoint> lastPoints;
static PlotRange lastX,lastY;
static bool lastLines=false,lastDots=false,lastPreferVectors=false;
static unsigned traceCalls=0;
static uint32_t lastTraceColor=0;
void GDClass::DrawPlotSeries(int16_t,int16_t,int16_t,int16_t,const PlotPoint* p,uint16_t count,
                             const PlotRange& xr,const PlotRange& yr,uint32_t,bool lines,bool dots,uint8_t,uint8_t)
{ assert(count<=GraphState::MaxPoints);assert(xr.division>0 && yr.division>0);lastPoints.assign(p,p+count);lastX=xr;lastY=yr;lastLines=lines;lastDots=dots; }
bool GDClass::DrawPlotTrace(int16_t,int16_t,int16_t,int16_t,const PlotPointSource& source,
                            const PlotRange& xr,const PlotRange& yr,uint32_t color,bool lines,bool dots,bool preferVectors) {
    lastPreferVectors=preferVectors;
    ++traceCalls;
    lastTraceColor=color;lastPoints.clear();bool any=false;
    for(uint16_t i=0;i<source.count;++i) {auto point=source.read(source.context,i);lastPoints.push_back(point);any|=point.valid;}
    lastX=xr;lastY=yr;lastLines=lines;lastDots=dots;return any;
}
static void Near(double a,double b) {if(!(fabs(a-b)<1e-7*(1+fabs(b)))) fprintf(stderr,"Near failed: %.12g vs %.12g\n",a,b);assert(fabs(a-b)<1e-7*(1+fabs(b)));}
static void Tap(Panel& panel,Control* control)
{
    assert(control->IsVisible() && control->IsEnabled());
    const int x=control->GetX()+control->GetWidth()/2,y=control->GetY()+control->GetHeight()/2;
    panel.TouchStart(x,y);panel.TouchEnd(x,y);
}
static void Press(GraphPage& page,unsigned id)
{
    if(id<4)
    {
        auto& tabs=page.GetTabs();
        const int x=tabs.GetX()+(id*2+1)*tabs.GetWidth()/8,y=tabs.GetY()+20;
        page.TouchStart(x,y);page.TouchEnd(x,y);
    }
    else Tap(page,page.GetChild(id));
    page.Draw();
}
static unsigned homeCalls=0,backCalls=0,graphCalls=0;
static void Home(Button*) {++homeCalls;} static void Back(Button*) {++backCalls;}
static void Graph(Button*) {++graphCalls;}
static unsigned simulationCalls=0;static void Simulation(Button*) {++simulationCalls;}
static unsigned tableCalls=0;static void ReadingTable(Button*) {++tableCalls;}
static unsigned histogramCalls=0;static void Histogram(Button*) {++histogramCalls;}

// Guard against reintroducing full measurement arrays into persistent UI objects.
static_assert(sizeof(GraphPanel)<4096,"Main graph must not own a sample buffer");
static_assert(sizeof(GraphPage)<8192,"Graph page must not own a sample buffer");
static_assert(sizeof(HistogramPage)<8192,"Histogram must not own a sample buffer");
static_assert(sizeof(XYGraphPopup)<2048,"XY popup must not own a sample buffer");
static_assert(sizeof(ReadingTableState)<1024,"Table storage must remain one 50-sample page");
static void CheckBufferViews()
{
    BufferClass buffer;
    assert(buffer.View().Count()==0 && BufferView(nullptr,12).Count()==0);
    for(unsigned n=1;n<=2u*BufferClass::Capacity+52;++n) {
        buffer.AddSample(n*10,n,n%17?1:0);
        const auto all=buffer.View();
        const unsigned retained=std::min<unsigned>(n,BufferClass::Capacity);
        assert(all.Count()==retained && &all[retained-1]==buffer.GetLatest());
        Near(all[0].Voltage,n-retained+1);
        for(uint16_t limit:{uint16_t(0),uint16_t(1),ReadingTableState::PageCapacity,uint16_t(256),BufferClass::Capacity,UINT16_MAX}) {
            const auto tail=buffer.View(limit);
            const unsigned count=retained<limit?retained:limit;
            assert(tail.Count()==count);
            for(uint16_t i=0;i<count;++i) {
                assert(&tail[i]==&all[retained-count+i]);
                assert(tail[i].TimeMs==(n-count+i+1)*10);
            }
        }
    }
    // All algorithms agree for wrapped ring storage and a contiguous reference copy.
    BufferSample copied[BufferClass::Capacity];
    const uint16_t count=buffer.CopyLatest(copied,BufferClass::Capacity);
    for(unsigned iv=0;iv<2;++iv) {
        GraphState direct,reference;direct.SetIV(iv!=0);reference.SetIV(iv!=0);
        direct.Resolve(buffer.View());reference.Resolve(copied,count);
        Near(direct.X().start,reference.X().start);Near(direct.X().division,reference.X().division);
        for(unsigned q=0;q<4;++q) {
            const auto quantity=static_cast<GraphState::Quantity>(q);
            Near(direct.y[q].start,reference.y[q].start);Near(direct.y[q].division,reference.y[q].division);
            const GraphState::Trace a{&direct,buffer.View(),quantity},b{&reference,BufferView(copied,count),quantity};
            for(uint16_t i=0;i<count;++i) {
                const auto pa=GraphState::Trace::Read(&a,i),pb=GraphState::Trace::Read(&b,i);
                assert(pa.valid==pb.valid);Near(pa.x,pb.x);Near(pa.y,pb.y);
            }
            PlotPoint pa[GraphState::MaxPoints],pb[GraphState::MaxPoints];
            const auto na=direct.MakePoints(buffer.View(),quantity,pa);
            const auto nb=reference.MakePoints(copied,count,quantity,pb);assert(na==nb);
            for(uint16_t i=0;i<na;++i) {assert(pa[i].valid==pb[i].valid);Near(pa[i].x,pb[i].x);Near(pa[i].y,pb[i].y);}
            HistogramSettings settings;settings.quantity=static_cast<BufferQuantity>(q);
            HistogramResult ha,hb;CalculateHistogram(buffer.View(),settings,ha);CalculateHistogram(copied,count,settings,hb);
            assert(ha.valid==hb.valid && ha.invalid==hb.invalid);Near(ha.minimum,hb.minimum);Near(ha.maximum,hb.maximum);
            for(unsigned i=0;i<ha.bins+2;++i) assert(ha.counts[i]==hb.counts[i]);
        }
    }
    buffer.Reset();assert(buffer.View().Count()==0);
    buffer.AddSample(7,42,1);assert(buffer.View()[0].TimeMs==7);
}

static void CheckVectorScroll()
{
    // Exercise the actual chronological ring view and Main's axis resolver,
    // including grid shifts both before and after the first ring overwrite.
    for(uint32_t origin : {0u,0xfffff000u}) {
        BufferClass buffer;GraphState state;state.tightYAxis=true;
        PlotVector::Vertex vertices[BufferClass::Capacity*2];
        unsigned shifts=0,wraps=0;double previousStart=0;
        for(unsigned n=0;n<BufferClass::Capacity*3u;++n) {
            buffer.AddSample(origin+n*50,5.0f+0.01f*float(sin(n*.7)),.001f);
            auto view=buffer.View();
            state.Resolve(buffer,view,50000,true);
            if(state.X().start!=previousStart) ++shifts;
            previousStart=state.X().start;
            if(n>=BufferClass::Capacity) ++wraps;
            const GraphState::Trace trace{&state,view,GraphState::Voltage};
            const auto source=trace.Source(view.Count());
            auto result=PlotVector::Build(source,state.X(),state.y[0],664,160,
                                         vertices,BufferClass::Capacity*2);
            assert(result.complete && result.any);
            for(unsigned i=0;i<result.count;++i) {
                assert(vertices[i].x>=0 && vertices[i].x<=664);
                assert(vertices[i].y>=0 && vertices[i].y<=160);
                if(i) assert(vertices[i].x>=vertices[i-1].x);
            }
            // Independent per-segment reference using the old raster's
            // normalization and clipping, checking every emitted endpoint.
            unsigned segment=0;
            for(unsigned i=1;i<source.count;++i) {
                const auto a=source.read(source.context,i-1),b=source.read(source.context,i);
                assert(b.x>a.x);
                double x0=(a.x-state.X().start)/(state.X().division*10);
                double x1=(b.x-state.X().start)/(state.X().division*10);
                double y0=(a.y-state.y[0].start)/(state.y[0].division*6);
                double y1=(b.y-state.y[0].start)/(state.y[0].division*6);
                if(!ClipPlotLine(x0,y0,x1,y1)) continue;
                while(segment+1<result.count && vertices[segment+1].move) ++segment;
                assert(segment+1<result.count);
                assert(abs(vertices[segment].x-int(x0*664+.5))<=1);
                assert(abs(vertices[segment].y-int((1-y0)*160+.5))<=1);
                ++segment;
                assert(abs(vertices[segment].x-int(x1*664+.5))<=1);
                assert(abs(vertices[segment].y-int((1-y1)*160+.5))<=1);
            }
            assert(!result.count || segment+1==result.count);
        }
        assert(shifts>10 && wraps>0);
    }
    puts("PASS: vector grid shifts, ring overwrite, timestamp rollover, bounds and every clipped segment");
}

int main()
{
    {
        BufferClass history;GraphState fixed;
        assert(fixed.scrolling);
        double previous=0,step=0;
        unsigned shifts=0;
        for(unsigned i=0;i<BufferClass::Capacity*2u;++i) {
            history.AddSample(i*50,5,.001f);
            fixed.Resolve(history,history.View(),50000,true);
            if(!i) {step=fixed.X().division;Near(fixed.X().start,0);}
            Near(fixed.X().division,step);
            if(fixed.X().start!=previous) {
                Near(fixed.X().start,previous+step);
                ++shifts;
            }
            previous=fixed.X().start;
        }
        assert(shifts>1);
    }
    {
        BufferClass history;GraphState full;full.scrolling=false;
        for(unsigned i=0;i<BufferClass::Capacity*2u;++i) {
            history.AddSample(i*50,5,.001f);
            auto view=history.View();full.Resolve(history,view,50000,true);
            const double newest=GraphState::Time(view[view.Count()-1],full.timeOrigin);
            const double right=full.X().start+full.X().division*GraphState::XDivisions;
            if(view.Count()>1) Near(right,newest);
            else assert(right>newest);
        }
    }
    CheckVectorScroll();
    {
        SystemClass stable;GraphPanel panel(0,0,800,240);panel.SetSystem(&stable);
        GraphPage menu(0,0,800,480);menu.SetSystem(&stable);menu.Open();
        stable.SetGraphStyle(GraphStyle::Lines);
        // A selected sample must remain selected until it expires from history,
        // both during filling and across repeated ring overwrites.
        for(unsigned n=0;n<BufferClass::Capacity*3u;++n) {
            stable.SetGraphStyle(n&1?GraphStyle::Both:GraphStyle::Lines);
            stable.Buffer.AddSample(n*10,float(n),.001f);panel.Draw();
            assert(lastLines && lastDots==bool(n&1) && lastPreferVectors);
            const auto mainPoints=lastPoints;
            menu.Draw();assert(lastPreferVectors && lastPoints.size()==mainPoints.size());
            texts.clear();
            for(unsigned i=0;i<lastPoints.size();++i) {
                Near(lastPoints[i].x,mainPoints[i].x);Near(lastPoints[i].y,mainPoints[i].y);
            }
            const unsigned first=n+1-stable.Buffer.GetCount();
            const unsigned firstEven=first+(first&1),lastEven=n-(n&1);
            assert(lastPoints.size()==(lastEven-firstEven)/2+1);
            for(unsigned i=0;i<lastPoints.size();++i) {
                assert(lastPoints[i].valid);
                Near(lastPoints[i].y,firstEven+2*i);
            }
        }
        Press(menu,1);
        for(unsigned q=1;q<4;++q) {Press(menu,7+q);Press(menu,11);}
        traceCalls=0;Press(menu,0);
        assert(traceCalls==4 && lastPreferVectors && lastPoints.size()==BufferClass::Capacity/2);
        stable.SetGraphStyle(GraphStyle::Points);menu.Draw();
        assert(!lastPreferVectors && !lastLines && lastDots && lastPoints.size()==BufferClass::Capacity);
        stable.SetGraphStyle(GraphStyle::Both);Press(menu,1);Press(menu,16);Press(menu,0);
        assert(lastPreferVectors && lastLines && lastDots && lastPoints.size()==BufferClass::Capacity/2);
        stable.ClearMeasurementBuffer();stable.Buffer.AddSample(50000,77,.001f);panel.Draw();
        assert(lastPoints.size()==1);Near(lastPoints[0].y,77);
        puts("PASS: stable every-second acquisition indices, newest-point selection, ring wraps and reset");
    }
    {
        PlotVector::Vertex lines[8],dots[8];
        PlotPoint points[]={{-1,2,true},{2,3,true},{4,4,false},{6,2,true},{11,3,true}};
        PlotPointSource source{points,[](const void* p,uint16_t i) {
            return static_cast<const PlotPoint*>(p)[i];
        },5};
        auto result=PlotVector::Build(source,{0,1,true},{0,1,true},600,180,lines,8,dots,8);
        assert(result.complete && result.points==2 && result.strips==2);
        assert(dots[0].x==120 && dots[1].x==360); // No dots at clipped crossings.
        assert(PlotVector::Words(result,true)==PlotVector::Words(result)+5);
        const auto bytes=PlotVector::Words(result,true)*4;
        assert(PlotVector::Fits(result,8192-bytes-2048,2048,true));
        assert(!PlotVector::Fits(result,8192-bytes-2048+4,2048,true));
        result=PlotVector::Build(source,{0,1,true},{0,1,true},600,180,lines,8,dots,1);
        assert(!result.complete);
        source.count=1;points[0]={2,3,true};
        result=PlotVector::Build(source,{0,1,true},{0,1,true},600,180,lines,8,dots,8);
        assert(result.complete && result.count==0 && result.points==1 && result.any);
        assert(dots[0].x==120 && dots[0].y==90);
    }
    {
        PlotPoint points[10];
        for(unsigned i=0;i<10;++i) points[i]={double(i),double(i),true};
        for(uint16_t count=0;count<=10;++count) {
            PlotEverySecondSource reduced{{points,[](const void* p,uint16_t i) {
                return static_cast<const PlotPoint*>(p)[i];
            },count}};
            const auto source=reduced.Source();
            assert(source.count==(count+1)/2);
            for(uint16_t i=0;i<source.count;++i) {
                const auto point=source.read(source.context,i);
                assert(point.valid);Near(point.x,unsigned(i)*2);
            }
            if(count) Near(source.read(source.context,source.count-1).x,2*((count-1)/2));
        }
        points[1].valid=false;
        PlotEverySecondSource reduced{{points,[](const void* p,uint16_t i) {
            return static_cast<const PlotPoint*>(p)[i];
        },10}};
        assert(!reduced.Source().read(&reduced,1).valid);
    }
    {
        PlotVector::Vertex vertices[BufferClass::Capacity*2];
        PlotRange xr{0,1,true},yr{0,1,true};
        PlotPointSource source{nullptr,[](const void*,uint16_t i) {
            return PlotPoint{10.0*i/(BufferClass::Capacity-1),3,true};
        },BufferClass::Capacity};
        auto result=PlotVector::Build(source,xr,yr,600,180,vertices,BufferClass::Capacity*2);
        assert(result.complete && result.any && result.count==BufferClass::Capacity && result.strips==1);
        assert(vertices[0].x==0 && vertices[result.count-1].x==600);
        assert(PlotVector::Words(result)==BufferClass::Capacity+10);
        const unsigned bytes=PlotVector::Words(result)*4;
        assert(PlotVector::Fits(result,8192-2048-bytes,2048));
        assert(!PlotVector::Fits(result,8192-2048-bytes+4,2048));
        assert(!PlotVector::Fits(result,8193,0));
        auto shortResult=PlotVector::Build(source,xr,yr,600,180,vertices,3);
        assert(!shortResult.complete && shortResult.count==3 && !PlotVector::Fits(shortResult,0,0));
        PlotPoint points[]={{0,0,true},{2,2,true},{3,3,false},{4,4,true},{5,5,true}};
        source={points,[](const void* p,uint16_t i) {return static_cast<const PlotPoint*>(p)[i];},5};
        result=PlotVector::Build(source,xr,yr,600,180,vertices,10);
        assert(result.complete && result.count==4 && result.strips==2);
        assert(vertices[0].move && !vertices[1].move && vertices[2].move);
        // Leaving and re-entering the viewport must not join along its edge.
        points[0]={2,2,true};points[1]={12,3,true};points[2]={2,4,true};source.count=3;
        result=PlotVector::Build(source,xr,yr,600,180,vertices,10);
        assert(result.count==4 && result.strips==2);
        for(unsigned i=0;i<result.count;++i) assert(vertices[i].x>=0 && vertices[i].x<=600 && vertices[i].y>=0 && vertices[i].y<=180);
        points[0]={-5,2,true};points[1]={-2,4,true};source.count=2;
        result=PlotVector::Build(source,xr,yr,600,180,vertices,10);
        assert(!result.any && result.count==0);
        points[0]={0,0,true};points[1]={10,6,true};
        result=PlotVector::Build(source,xr,yr,600,180,vertices,10);
        assert(result.count==2 && vertices[0].y==180 && vertices[1].y==0);
        source.count=1;result=PlotVector::Build(source,xr,yr,600,180,vertices,10);
        assert(result.any && result.count==0);
        points[0].y=NAN;result=PlotVector::Build(source,xr,yr,600,180,vertices,10);
        assert(!result.any && result.count==0);
    }
    CheckBufferViews();
    constexpr unsigned overwritten=76, totalReadings=BufferClass::Capacity+overwritten;
    constexpr unsigned lastRows=BufferClass::Capacity-ReadingTableState::Rows;
    constexpr unsigned lastCache=BufferClass::Capacity-ReadingTableState::PageCapacity;
    nativeButtons=flatButtons=compactButtons=customButtons=0;
    Button native;native.SetBounds(10,10,80,40);native.SetText("1");native.SetNativeFlat(true);
    native.SetBackgroundColor(0x204060);native.SetCornerRadius(6);
    native.Draw();assert(lastForeground==0x204060 && nativeButtons==1 && flatButtons==1);
    native.TouchStart(20,20);native.Draw();assert(lastForeground==0x102030);
    native.TouchEnd(-1,-1);native.Draw();assert(lastForeground==0x204060);
    native.SetNativeFlat(false);native.SetCornerRadius(0);
    nativeButtons=0;flatButtons=0;native.Draw();assert(nativeButtons==1 && flatButtons==0 && lastForeground==0x204060);
    native.TouchStart(20,20);native.Draw();assert(flatButtons==1 && lastForeground==0x204060);
    native.TouchEnd(-1,-1);
    TouchFrame decoded;
    decoded.Add(0x80008000,0);decoded.Add(0x80000001,1);assert(decoded.count==0);
    decoded.Add((123u<<16)|321u,2);assert(decoded.count==1);
    assert(decoded.points[0].x==123 && decoded.points[0].y==321 && decoded.points[0].id==2);
    TabControl tabs;tabs.SetBounds(8,4,480,44);
    assert(tabs.AddTab("Graph") && tabs.AddTab("Data") && tabs.AddTab("Scale") && tabs.AddTab("Trigger"));
    tabs.TouchStart(180,24);tabs.TouchEnd(180,24);assert(tabs.GetSelectedIndex()==1);
    tabs.TouchStart(300,24);tabs.TouchMove(790,100);tabs.TouchEnd(790,100);assert(tabs.GetSelectedIndex()==1);
    tabs.TouchStart(300,24);tabs.TouchEnd(420,24);assert(tabs.GetSelectedIndex()==1);
    tabs.TouchStart(365,5);tabs.TouchEnd(365,5);assert(tabs.GetSelectedIndex()==1); // empty slope corner
    tabs.SetEnabled(false);tabs.TouchStart(300,24);tabs.TouchEnd(300,24);assert(tabs.GetSelectedIndex()==1);
    tabs.SetEnabled(true);tabs.SetSelectedIndex(3);assert(tabs.GetSelectedIndex()==3);
    tabs.SetSelectedIndex(7);assert(tabs.GetSelectedIndex()==3);tabs.Draw();
    BufferClass buffer;
    for(unsigned i=0;i<100;++i) buffer.AddSample(10000+i*i,static_cast<float>(i-0.0),2);
    BufferSample samples[BufferClass::Capacity];
    auto count=buffer.CopyLatest(samples,BufferClass::Capacity);
    {
        GraphState single,all;
        single.tightYAxis=all.tightYAxis=true;
        for(unsigned q=0;q<4;++q) {
            single.selected=all.selected=static_cast<GraphState::Quantity>(q);
            PlotRange previous[4];for(unsigned i=0;i<4;++i) previous[i]=single.y[i];
            single.Resolve(buffer,buffer.View(),MeasurementTiming::PairTimeUs(0),true);
            all.Resolve(buffer,buffer.View(),MeasurementTiming::PairTimeUs(0));
            Near(single.X().start,all.X().start);Near(single.X().division,all.X().division);
            Near(single.y[q].start,all.y[q].start);Near(single.y[q].division,all.y[q].division);
            for(unsigned i=0;i<4;++i) if(i!=q) {
                Near(single.y[i].start,previous[i].start);
                Near(single.y[i].division,previous[i].division);
            }
        }
        single.y[single.selected]={2,3,false};
        single.Resolve(buffer,buffer.View(),MeasurementTiming::PairTimeUs(0),true);
        Near(single.y[single.selected].start,2);Near(single.y[single.selected].division,3);
    }
    GraphState state;state.windowSeconds=10;state.scrolling=true;state.Resolve(samples,count);
    Near(state.X().start,0);Near(state.X().division*10,10);
    assert(state.y[0].start<=0 && state.y[0].start+state.y[0].division*6>=99);
    auto old=state.X();assert(state.Zoom(0.5));Near(state.X().division,old.division/2);assert(!state.X().automatic);
    assert(state.Pan(0.1,0.1));assert(!state.y[0].automatic);
    const auto beforeInvalid=state.X();assert(!state.Zoom(0));assert(!state.Zoom(NAN));assert(!state.Zoom(0.5,-0.1,0.5));
    Near(state.X().division,beforeInvalid.division);Near(state.X().start,beforeInvalid.start);
    assert(!state.SetManual(true,true,0));assert(!state.SetManual(true,true,NAN));assert(!state.SetManual(false,true,-1));
    assert(state.SetManual(true,true,0.25));state.Resolve(samples,count);Near(state.X().division,0.25);
    state.SetIV(true);state.Resolve(samples,count);assert(state.selected==GraphState::Current);
    assert(state.X().start<=0 && state.X().start+state.X().division*10>=99);
    state.SetIV(false);Near(state.X().division,0.25);
    state.AutoScale();state.Resolve(samples,count);Near(state.X().division,1);
    PlotPoint points[GraphState::MaxPoints];
    auto n=state.MakePoints(samples,count,GraphState::Current,points);
    assert(n<=GraphState::MaxPoints && n>0);Near(points[n-1].x,9.801);
    samples[50].Current=500;state.MakePoints(samples,count,GraphState::Current,points);
    n=state.MakePoints(samples,count,GraphState::Current,points);bool spike=false;
    for(unsigned i=0;i<n;++i) spike|=points[i].valid && points[i].y==500;assert(spike);
    samples[30].Current=0;
    n=state.MakePoints(samples,count,GraphState::Resistance,points);bool gap=false;
    for(unsigned i=0;i<n;++i) gap|=!points[i].valid;assert(gap);
    double x0=-1,y0=0.5,x1=2,y1=0.5;assert(ClipPlotLine(x0,y0,x1,y1));Near(x0,0);Near(x1,1);
    x0=-2;y0=-1;x1=-1;y1=-2;assert(!ClipPlotLine(x0,y0,x1,y1));
    assert(state.SetManual(true,false,2.4));assert(state.SetManual(true,true,0.01));
    n=state.MakePoints(samples,count,GraphState::Current,points);
    assert(n>0 && n<20);assert(points[0].x<=2.4 && points[n-1].x>=2.5);
    state.AutoScale();state.Resolve(samples,count);
    // Appending live samples must not redraw already closed time buckets differently.
    SystemClass stableSystem;stableSystem.SetSimulationEnabled(true);
    stableSystem.SetVoltageSourceValue(5);stableSystem.SetCurrentLimitValue(.02f);
    stableSystem.ClearMeasurementBuffer();
    BufferSample stableSamples[100], earlier[100];PlotPoint oldTrace[GraphState::MaxPoints],newTrace[GraphState::MaxPoints];
    GraphState stableTrace;stableTrace.hasOrigin=true;stableTrace.timeOrigin=100;
    stableTrace.X().start=0;stableTrace.X().division=1;stableTrace.X().automatic=false;
    for(unsigned step=1;step<=90;++step) {
        const auto before=stableSystem.Buffer.CopyLatest(earlier,100);
        stableSystem.ExecuteAt(step*100);
        const auto after=stableSystem.Buffer.CopyLatest(stableSamples,100);
        assert(after==before+1);
        for(unsigned j=0;j<before;++j) {
            assert(earlier[j].TimeMs==stableSamples[j].TimeMs);
            assert(earlier[j].Voltage==stableSamples[j].Voltage && earlier[j].Current==stableSamples[j].Current);
            assert(earlier[j].GetPower()==stableSamples[j].GetPower() && earlier[j].Flags==stableSamples[j].Flags);
        }
        if(before<2) continue;
        auto oldCount=stableTrace.MakePoints(earlier,before,GraphState::Voltage,oldTrace);
        auto newCount=stableTrace.MakePoints(stableSamples,after,GraphState::Voltage,newTrace);
        const double closedUntil=floor(GraphState::Time(earlier[before-1],100)/(10.0/32))*(10.0/32);
        unsigned oldClosed=0,newClosed=0;
        while(oldClosed<oldCount && oldTrace[oldClosed].x<closedUntil) ++oldClosed;
        while(newClosed<newCount && newTrace[newClosed].x<closedUntil) ++newClosed;
        assert(oldClosed==newClosed);
        for(unsigned j=0;j<oldClosed;++j) {
            assert(oldTrace[j].x==newTrace[j].x && oldTrace[j].y==newTrace[j].y);
        }
    }
    // Explicit stepped view keeps a fixed scale and closed history.
    GraphState fullHistory;fullHistory.windowSeconds=10;fullHistory.scrolling=true;
    fullHistory.Resolve(stableSamples,40);
    assert(fullHistory.X().start==0 && fullHistory.X().division==1);
    const auto oldHistoryCount=fullHistory.MakePoints(stableSamples,40,GraphState::Voltage,oldTrace);
    for(unsigned size=41;size<=90;++size) {
        fullHistory.Resolve(stableSamples,size);
        assert(fullHistory.X().start==0 && fullHistory.X().division==1);
        const auto newHistoryCount=fullHistory.MakePoints(stableSamples,size,GraphState::Voltage,newTrace);
        unsigned closed=0;while(closed<oldHistoryCount && oldTrace[closed].x<3.75) ++closed;
        assert(newHistoryCount>=closed);
        for(unsigned j=0;j<closed;++j) assert(oldTrace[j].x==newTrace[j].x && oldTrace[j].y==newTrace[j].y);
    }
    // Stable automatic Y bounds when later samples stay within the established range.
    BufferSample bounded[]={Hardware::MakeMeasurement(0,4.9995f,.005f),
        Hardware::MakeMeasurement(100,5.0005f,.005f),Hardware::MakeMeasurement(200,5,.005f)};
    GraphState quiet;quiet.windowSeconds=10;quiet.scrolling=false;quiet.Resolve(bounded,2);const auto fixedY=quiet.y[0];quiet.Resolve(bounded,3);
    assert(quiet.y[0].start==fixedY.start && quiet.y[0].division==fixedY.division);
    bounded[2].Voltage=6;quiet.Resolve(bounded,3);assert(quiet.y[0].start+quiet.y[0].division*6>=6);
    bounded[2].Voltage=5;quiet.Resolve(bounded,3);assert(quiet.y[0].division<.001);
    // Expired extremes and displaced plateaus refit without pressing AUTO SCALE.
    BufferSample shifted[]={Hardware::MakeMeasurement(0,0,.1f),Hardware::MakeMeasurement(1,10,.1f)};
    GraphState tracking;tracking.Resolve(shifted,2);
    shifted[0].Voltage=2;shifted[1].Voltage=3;tracking.Resolve(shifted,2);
    GraphState fresh;fresh.Resolve(shifted,2);
    Near(tracking.y[0].start,fresh.y[0].start);Near(tracking.y[0].division,fresh.y[0].division);
    const auto settled=tracking.y[0];
    for(unsigned n=0;n<50;++n) {tracking.Resolve(shifted,2);Near(tracking.y[0].start,settled.start);Near(tracking.y[0].division,settled.division);}
    // Small upper escapes must not accumulate the prior axis padding.
    for(unsigned n=0;n<20;++n) {
        shifted[1].Voltage=3+n*.1f;tracking.Resolve(shifted,2);
        fresh.AutoScale();fresh.Resolve(shifted,2);
        assert(tracking.y[0].division<=fresh.y[0].division/0.75);
        assert(tracking.y[0].start<=shifted[0].Voltage && tracking.y[0].start+tracking.y[0].division*6>=shifted[1].Voltage);
    }
    tracking.SetManual(false,false,-10);tracking.SetManual(false,true,5);
    shifted[0].Voltage=2;shifted[1].Voltage=3;tracking.Resolve(shifted,2);
    Near(tracking.y[0].start,-10);Near(tracking.y[0].division,5);
    quiet.AutoScale();quiet.Resolve(bounded,3);assert(quiet.y[0].division<.001);
    bounded[2].TimeMs=10100;quiet.Resolve(bounded,3);Near(quiet.X().division,1.01);
    bounded[2].TimeMs=20100;quiet.Resolve(bounded,3);Near(quiet.X().division,2.01);
    // Explicit stepped scrolling: fixed span, exact edges, delayed frames and ring retention.
    GraphState stepped;assert(stepped.scrolling && stepped.windowSeconds==100);stepped.hasOrigin=true;stepped.timeOrigin=0;
    BufferSample stepSamples[]={Hardware::MakeMeasurement(0,5,.005f),Hardware::MakeMeasurement(0,5,.005f)};
    const uint32_t times[]={0,99999,100000,100001,110000,110001,119999,120000,120001,250000,1200000};
    const double starts[]={0,0,0,10,10,20,20,20,30,150,1100};
    for(unsigned j=0;j<sizeof(times)/sizeof(times[0]);++j) {
        stepSamples[1].TimeMs=times[j];stepped.Resolve(stepSamples,2);
        Near(stepped.X().start,starts[j]);Near(stepped.X().division,10);
        assert(times[j]/1000.0<=stepped.X().start+100);
    }
    stepSamples[0].TimeMs=25000;stepped.Resolve(stepSamples,2);Near(stepped.X().start,1100);
    stepped.windowSeconds=5;stepSamples[0].TimeMs=0;stepSamples[1].TimeMs=5100;
    stepped.Resolve(stepSamples,2);Near(stepped.X().start,.5);Near(stepped.X().division,.5);
    assert(stepped.SetManual(true,false,7));stepSamples[1].TimeMs=20000;stepped.Resolve(stepSamples,2);Near(stepped.X().start,7);
    // A 2 s jump preserves selected peaks in fully visible, completed absolute bins.
    BufferSample jumping[151];
    for(unsigned j=0;j<151;++j) jumping[j]=Hardware::MakeMeasurement(j*100,5+sin(j*1.7)*.001,.005);
    GraphState jump; jump.hasOrigin=true;jump.timeOrigin=0;jump.X().automatic=false;jump.X().division=1;
    auto leftCount=jump.MakePoints(jumping,151,GraphState::Voltage,oldTrace);
    jump.X().start=2;auto rightCount=jump.MakePoints(jumping,151,GraphState::Voltage,newTrace);
    for(unsigned j=0;j<leftCount;++j) if(oldTrace[j].x>=2.5 && oldTrace[j].x<9.6875) {
        bool found=false;
        for(unsigned k=0;k<rightCount;++k) found|=oldTrace[j].x==newTrace[k].x && oldTrace[j].y==newTrace[k].y;
        assert(found);
    }
    // Repeated I/V switches must not enable extra time curves or leak I/V Y scales.
    GraphState switching; switching.scrolling=false;switching.Resolve(samples,count);
    switching.SetManual(false,true,0.5);
    for(unsigned cycle=0;cycle<500;++cycle)
    {
        switching.SetIV(true);switching.Resolve(samples,count);
        assert(switching.IsCurveVisible(GraphState::Current));
        switching.SetManual(false,true,0.001);
        switching.SetIV(false);switching.Resolve(samples,count);
        assert(switching.selected==GraphState::Voltage && switching.enabled[0] && !switching.enabled[1]);
        Near(switching.y[0].division,0.5);
        for(unsigned q=0;q<4;++q) {
            n=switching.MakePoints(samples,count,static_cast<GraphState::Quantity>(q),points);
            assert(n<=GraphState::MaxPoints);
        }
    }
    // Same drag moves every visible trace by the same screen fraction, regardless
    // of selected quantity or unit scale (including a selected hidden Current).
    for(unsigned selection=0;selection<4;++selection)
    {
        GraphState pan;pan.selected=static_cast<GraphState::Quantity>(selection);
        pan.enabled[0]=pan.enabled[1]=true;pan.enabled[2]=pan.enabled[3]=false;
        pan.X().start=2;pan.X().division=0.5;
        for(unsigned q=0;q<4;++q) {pan.y[q].start=q;pan.y[q].division=pow(0.01,q);}
        const auto originalPan=pan;
        assert(pan.Pan(0.125,-0.2));
        Near(pan.X().start,originalPan.X().start-0.125*5);
        for(unsigned q=0;q<4;++q)
        {
            const bool affected=pan.IsCurveVisible(q) || q==selection;
            Near((pan.y[q].start-originalPan.y[q].start)/(pan.y[q].division*6),affected?-0.2:0);
        }
        pan.Resolve(samples,count);Near(pan.X().start,originalPan.X().start-0.125*5);
        const auto validPan=pan;assert(!pan.Pan(NAN,0));assert(!pan.Pan(0,INFINITY));
        Near(pan.X().start,validPan.X().start);
    }
    GraphState hiddenSelection;hiddenSelection.selected=GraphState::Current;
    assert(!hiddenSelection.enabled[1]);assert(hiddenSelection.Pan(0,0.1));
    Near(hiddenSelection.y[0].start,0.6);Near(hiddenSelection.y[1].start,0.6);
    GraphState ivPan;ivPan.SetIV(true);const auto ivOld=ivPan;
    assert(ivPan.Pan(0.1,0.2));Near(ivPan.y[1].start,ivOld.y[1].start+1.2);
    Near(ivPan.y[0].start,ivOld.y[0].start);

    BufferSample wrap={};wrap.TimeMs=100;Near(GraphState::Time(wrap,0xFFFFFF00),0.356);

    // MainPage's compact plot shares the timestamp renderer, includes older peaks,
    // and draws no buttons. MeasureMode order differs from GraphState for P/R.
    SystemClass compactSystem;compactSystem.Reset();
    GraphPanel compact(20,272,760,208);compact.SetSystem(&compactSystem);
    compact.Draw();assert(lastPoints.empty());
    for(unsigned i=0;i<BufferClass::Capacity;++i) compactSystem.Buffer.AddSample(10000+i*2,i==50?100.0f:2.0f,0.5f);
    const MeasureMode modes[]={MeasureMode::Voltage,MeasureMode::Current,MeasureMode::Power,MeasureMode::Resistance};
    const double peaks[]={100,0.5,50,200};
    for(unsigned mode=0;mode<4;++mode) {
        compactSystem.SetMeasureMode(modes[mode]);nativeButtons=0;compact.Draw();assert(nativeButtons==0);
        assert(compactSystem.Buffer.GetCount()==BufferClass::Capacity);
        assert(lastPoints.size()==BufferClass::Capacity/2);
        double peak=-1;for(const auto& point:lastPoints) if(point.valid) peak=std::max(peak,point.y);
        Near(peak,peaks[mode]);Near(lastPoints.back().x,(BufferClass::Capacity-2)*0.002);
    }
    compactSystem.ClearMeasurementBuffer();compact.Draw();assert(lastPoints.empty());
    compactSystem.Buffer.AddSample(20000,1,0);compact.Draw();
    for(const auto& point:lastPoints) assert(!point.valid); // Invalid resistance remains a gap.
    compactSystem.SetMeasureMode(MeasureMode::Voltage);compact.Draw();Near(lastPoints[0].x,0);

    // Automatic windows fit the retained duration at every NPLC. A full ring
    // never expires data inside the automatic viewport as it advances.
    const double expectedWindows[]={0.4,4,20,40,200,400};
    for(unsigned profile=0;profile<MeasurementTiming::Count;++profile) {
        BufferClass history;GraphState automatic;automatic.scrolling=true;
        const auto period=MeasurementTiming::PairTimeUs(profile);
        automatic.Resolve(history,history.View(),period);
        Near(automatic.windowSeconds,expectedWindows[profile]);
        for(unsigned n=0;n<BufferClass::Capacity*3;++n) {
            history.AddSample(static_cast<uint32_t>(uint64_t(n)*period/1000),1,.1f);
            automatic.Resolve(history,history.View(),period);
            if(history.GetCount()==history.GetCapacity()) {
                const auto view=history.View();
                const double first=GraphState::Time(view[0],automatic.timeOrigin);
                const double last=GraphState::Time(view[view.Count()-1],automatic.timeOrigin);
                assert(automatic.X().start+1e-9>=first);
                assert(automatic.X().start<=last);
                assert(automatic.X().start+automatic.windowSeconds+1e-9>=last);
            }
        }
        assert(automatic.timeOrigin==0);
        automatic.SetWindowSeconds(100);automatic.Resolve(history,history.View(),period);
        Near(automatic.windowSeconds,100);assert(!automatic.automaticWindow);
        automatic.SetManual(true,false,12);automatic.SetManual(true,true,3);
        automatic.Resolve(history,history.View(),MeasurementTiming::PairTimeUs(0));
        Near(automatic.X().start,12);Near(automatic.X().division,3);
        automatic.AutoScale();automatic.Resolve(history,history.View(),period);
        Near(automatic.windowSeconds,expectedWindows[profile]);
        history.Reset();automatic.Resolve(history,history.View(),period);
        Near(automatic.X().start,0);Near(automatic.X().division,expectedWindows[profile]/GraphState::XDivisions);
    }
    // Slowing NPLC must not expand over history that the fast mode already lost.
    BufferClass mixedHistory;GraphState mixedWindow;
    for(unsigned n=0;n<BufferClass::Capacity;++n) mixedHistory.AddSample(n*40,1,.1f);
    mixedWindow.Resolve(mixedHistory,mixedHistory.View(),MeasurementTiming::PairTimeUs(5));
    Near(mixedWindow.windowSeconds,40);
    mixedWindow.SetWindowSeconds(75);mixedHistory.Reset();
    mixedWindow.Resolve(mixedHistory,mixedHistory.View(),MeasurementTiming::PairTimeUs(0));
    Near(mixedWindow.windowSeconds,75);Near(mixedWindow.X().division,7.5);
    mixedWindow.SetIV(true);mixedWindow.AutoScale();assert(!mixedWindow.automaticWindow);
    // Both views observe a reset even when they miss the empty buffer entirely.
    SystemClass resetSystem;resetSystem.Reset();
    GraphPanel resetPanel(20,272,760,208);resetPanel.SetSystem(&resetSystem);
    GraphPage resetPage(0,0,800,480);resetPage.SetSystem(&resetSystem);
    resetSystem.Buffer.AddSample(10000,1,0.1f);
    resetSystem.Buffer.AddSample(560000,2,0.2f);
    resetPanel.Draw();resetPage.Open();resetPage.Draw();
    Near(resetPanel.GetGraphState().X().start,512);
    Near(resetPage.GetGraphState().X().start,512);
    resetPage.TouchStart(400,220);resetPage.TouchMove(450,220);resetPage.TouchEnd(450,220);
    assert(!resetPage.GetGraphState().X().automatic);
    resetSystem.ClearMeasurementBuffer();
    // Refill to the same count before either view draws: count comparison cannot detect this.
    resetSystem.Buffer.AddSample(600000,3,0.3f);
    resetSystem.Buffer.AddSample(601000,4,0.4f);
    resetPanel.Draw();resetPage.Draw();
    for(const GraphState* state:{&resetPanel.GetGraphState(),&resetPage.GetGraphState()}) {
        Near(state->X().start,0);Near(state->X().division,4);
        assert(state->X().automatic && state->timeOrigin==600000);
    }
    Near(lastPoints.front().x,0);Near(lastPoints.back().x,0); // Stable Lines selection: index 0.
    resetSystem.ClearMeasurementBuffer();resetPanel.Draw();resetPage.Draw();
    Near(resetPanel.GetGraphState().X().start,0);Near(resetPage.GetGraphState().X().start,0);
    assert(!resetPanel.GetGraphState().hasOrigin && !resetPage.GetGraphState().hasOrigin);
    // Opening after ring overwrite still shares the acquisition origin; overwrite is not a reset.
    for(unsigned i=0;i<totalReadings;++i) resetSystem.Buffer.AddSample(700000+i*1000,1,0.1f);
    resetPanel.Draw();resetPage.Draw();
    assert(resetPanel.GetGraphState().timeOrigin==700000 && resetPage.GetGraphState().timeOrigin==700000);
    Near(resetPage.GetGraphState().X().start,ceil((totalReadings-1-40)/4.0)*4);
    // Clearing from Graph/Data also resets the time axis while I-V is selected.
    resetPage.GetTabs().SetSelectedIndex(1);resetPage.Draw();Press(resetPage,16);
    Press(resetPage,17);
    Near(resetPage.GetGraphState().x[0].start,0);
    resetSystem.Buffer.AddSample(2000000,2,0.2f);
    Press(resetPage,15);resetPage.GetTabs().SetSelectedIndex(0);resetPage.Draw();resetPanel.Draw();
    Near(resetPage.GetGraphState().X().start,0);Near(resetPanel.GetGraphState().X().start,0);
    Near(lastPoints.front().x,0);

    SystemClass system;system.Reset();system.SetSimulationEnabled(true);system.Buffer.Reset();system.SetOutputOn(true);
    system.Buffer.AddSample(0,1,0.1f);system.Buffer.AddSample(1000,2,0.2f);
    GraphPage page(0,0,800,480);page.SetSystem(&system);page.SetOnHome(Home);page.SetOnBack(Back);page.Open();page.Draw();
    assert(!page.GetChild(27)->IsVisible() && !page.GetChild(28)->IsVisible());
    auto fingers=[](int x1,int y1,int x2,int y2) {
        TouchFrame frame;frame.count=2;frame.points[0]={static_cast<int16_t>(x1),static_cast<int16_t>(y1),0};
        frame.points[1]={static_cast<int16_t>(x2),static_cast<int16_t>(y2),1};return frame;
    };
    TouchFrame released,one;one.count=1;one.points[0]={350,220,0};
    const auto original=page.GetGraphState().X();
    const double anchor=(400.0-136)/636;
    page.TouchStart(350,220);
    assert(page.HandleMultiTouch(fingers(350,220,450,220)));
    assert(page.HandleMultiTouch(fingers(300,220,500,220)));
    Near(page.GetGraphState().X().division,original.division/2);
    Near(page.GetGraphState().X().start+anchor*page.GetGraphState().X().division*10,
         original.start+anchor*original.division*10);
    assert(!page.GetGraphState().X().automatic);
    page.HandleMultiTouch(fingers(350,220,450,220));Near(page.GetGraphState().X().division,original.division);
    // One remaining contact cannot pan, click or restart a pinch until full release.
    assert(page.HandleMultiTouch(one));
    page.HandleMultiTouch(fingers(300,220,500,220));Near(page.GetGraphState().X().division,original.division);
    assert(page.HandleMultiTouch(released));assert(!page.HandleMultiTouch(one));
    // Gestures beginning outside the plot cancel pressed buttons without invoking HOME.
    page.TouchStart(120,65);page.HandleMultiTouch(fingers(120,65,450,220));
    page.HandleMultiTouch(fingers(200,220,600,220));page.HandleMultiTouch(released);assert(homeCalls==0);
    Near(page.GetGraphState().X().division,original.division);
    // Third contact stops the current gesture; replacing a slot also cancels zoom.
    page.HandleMultiTouch(fingers(350,220,450,220));
    auto three=fingers(300,220,500,220);three.count=3;three.points[2]={600,220,2};
    page.HandleMultiTouch(three);page.HandleMultiTouch(fingers(250,220,550,220));
    Near(page.GetGraphState().X().division,original.division);page.HandleMultiTouch(released);
    page.HandleMultiTouch(fingers(350,220,450,220));
    auto swapped=fingers(300,220,500,220);swapped.points[1].id=3;page.HandleMultiTouch(swapped);
    Near(page.GetGraphState().X().division,original.division);page.HandleMultiTouch(released);
    // MainPage gestures must reach the plot rather than the page swipe handler.
    SourcePanel source(0,240,800,240);source.SetSystem(&system);source.Draw();
    source.TouchStart(436,256);source.TouchEnd(436,256);source.Draw();
    auto& mainGraph=source.GetGraphPanel();assert(mainGraph.IsVisible());
    const auto mainBefore=mainGraph.GetGraphState().X();
    const auto mainYBefore=mainGraph.GetGraphState().y[GraphState::Voltage];
    assert(source.HandleMultiTouch(fingers(350,360,450,360)));
    assert(source.HandleMultiTouch(fingers(300,360,500,360)));source.Draw();
    Near(mainGraph.GetGraphState().X().division,mainBefore.division);
    assert(mainGraph.GetGraphState().X().automatic && mainGraph.IsVisible());
    one.points[0]={350,360,0};assert(source.HandleMultiTouch(one));
    source.HandleMultiTouch(fingers(250,360,550,360));
    Near(mainGraph.GetGraphState().X().division,mainBefore.division);
    source.HandleMultiTouch(released);
    Near(mainGraph.GetGraphState().X().start,mainBefore.start);
    Near(mainGraph.GetGraphState().y[GraphState::Voltage].division,mainYBefore.division);
    Near(mainGraph.GetGraphState().y[GraphState::Voltage].start,mainYBefore.start);
    auto beforeDrag=mainGraph.GetGraphState().X();
    source.TouchStart(400,360);source.TouchMove(460,380);source.TouchEnd(460,380);source.Draw();
    assert(!mainGraph.IsVisible() && source.GetStatisticsPanel().IsVisible());
    source.TouchStart(460,360);source.TouchMove(400,360);source.TouchEnd(400,360);source.Draw();
    assert(mainGraph.IsVisible());
    // Vertical movement and cancelled gestures neither navigate nor move the curve.
    source.TouchStart(400,360);source.TouchMove(400,420);source.TouchEnd(400,420);source.Draw();
    assert(mainGraph.IsVisible());
    source.TouchStart(400,360);source.TouchEnd(-1,-1);assert(mainGraph.IsVisible());
    Near(mainGraph.GetGraphState().X().start,beforeDrag.start);
    assert(mainGraph.GetGraphState().X().automatic);
    Near(mainGraph.GetGraphState().y[GraphState::Voltage].start,mainYBefore.start);
    Near(mainGraph.GetGraphState().y[GraphState::Voltage].division,mainYBefore.division);
    auto frozen=mainGraph.GetGraphState().X();system.Buffer.AddSample(1500,3,0.3f);source.Draw();
    Near(mainGraph.GetGraphState().X().start,frozen.start);assert(mainGraph.GetGraphState().X().division>=frozen.division);
    source.TouchStart(400,360);source.TouchEnd(400,360);
    source.TouchStart(400,360);source.TouchEnd(400,360);source.Draw();assert(mainGraph.GetGraphState().X().automatic);
    // A modal above MainPage blocks starting a pinch.
    assert(!source.HandleMultiTouch(fingers(350,360,450,360),false));
    // Existing navigation dots still return to Source and back to Graph.
    source.TouchStart(364,256);source.TouchEnd(364,256);assert(!mainGraph.IsVisible());
    source.TouchStart(436,256);source.TouchEnd(436,256);source.Draw();assert(mainGraph.IsVisible());
    system.Buffer.Reset();system.Buffer.AddSample(0,1,0.1f);system.Buffer.AddSample(1000,2,0.2f);
    auto manualDivision=page.GetGraphState().X().division;
    Press(page,1);Press(page,11);assert(!page.GetGraphState().enabled[0]);assert(system.Buffer.GetCount()==2);
    Press(page,11);assert(page.GetGraphState().enabled[0]);Press(page,8);Press(page,11);assert(page.GetGraphState().enabled[1]);
    // DATA changes are read by both views; clearing data must not reset settings.
    assert(!system.SetGraphStyle(static_cast<GraphStyle>(99)));
    for(unsigned style=0;style<3;++style) {
        Press(page,12+style);source.Draw();
        assert(lastLines==(style!=1) && lastDots==(style!=0));
        assert(lastPreferVectors==(style!=1));
        Press(page,0);assert(lastLines==(style!=1) && lastDots==(style!=0));
        assert(lastPreferVectors==(style!=1));Press(page,1);
    }
    Press(page,14);assert(system.GetGraphSettings().style==GraphStyle::Both);
    Press(page,16);assert(page.GetGraphState().iv);Press(page,15);assert(!page.GetGraphState().iv);Near(page.GetGraphState().X().division,manualDivision);
    Press(page,16);Press(page,0); // Pinch also works on the I/V axes.
    const auto ivBefore=page.GetGraphState().X();
    page.HandleMultiTouch(fingers(350,220,450,220));page.HandleMultiTouch(fingers(300,220,500,220));
    Near(page.GetGraphState().X().division,ivBefore.division/2);page.HandleMultiTouch(released);
    Press(page,1);Press(page,15);Near(page.GetGraphState().X().division,manualDivision);
    Press(page,2);Press(page,12);
    auto* editor=static_cast<Panel*>(page.GetChild(29));assert(editor->IsVisible());
    assert(modalDraws>0);
    nativeButtons=0;compactButtons=0;flatButtons=0;customButtons=0;page.Draw();
    assert(nativeButtons==17 && flatButtons==0 && compactButtons==0 && customButtons==0); // Only modal keys; background is a single bitmap.
    assert(captures>0 && backdropDraws>0);
    const unsigned capturesBefore=captures;page.Draw();assert(captures==capturesBefore);
    const auto beforeModal=page.GetGraphState().X();
    // Underlying HOME, tabs and multitouch cannot escape or alter a modal edit.
    page.TouchStart(120,65);page.TouchEnd(120,65);assert(homeCalls==0 && editor->IsVisible());
    page.TouchStart(180,24);page.TouchEnd(180,24);assert(page.GetTabs().GetSelectedIndex()==2);
    page.HandleMultiTouch(fingers(350,220,450,220));page.HandleMultiTouch(fingers(300,220,500,220));
    page.HandleMultiTouch(released);Near(page.GetGraphState().X().division,beforeModal.division);
    // Use graph input dispatch for editor keys.
    auto key=[&](unsigned index) { auto* k=editor->GetChild(index);int x=k->GetX()+10,y=k->GetY()+10;page.TouchStart(x,y);page.TouchEnd(x,y);page.Draw(); };
    key(14);key(0);key(15);assert(editor->IsVisible()); // Zero division rejected.
    key(14);key(1);key(15);assert(!editor->IsVisible());Near(page.GetGraphState().X().division,1);
    Press(page,12);key(14);key(9);key(16);assert(!editor->IsVisible());Near(page.GetGraphState().X().division,1);
    const std::string capacityHint="Full buffer = retained measurements (maximum "+
        std::to_string(BufferClass::Capacity)+" readings).";
    assert(std::find(texts.begin(),texts.end(),capacityHint)!=texts.end());
    // Signed exponent input and all remaining scale targets share modal validation.
    Press(page,11);key(14);key(11);key(1);key(12);key(11);key(3);key(15);
    Near(page.GetGraphState().X().start,-0.001);assert(!editor->IsVisible());
    Press(page,13);key(14);key(2);key(15);Near(page.GetGraphState().y[page.GetGraphState().selected].start,2);
    Press(page,14);key(14);key(0);key(15);assert(editor->IsVisible());key(16);
    Press(page,17);key(14);key(0);key(15);assert(editor->IsVisible());
    key(14);key(1);key(12);key(11);key(3);key(15);Near(page.GetGraphState().windowSeconds,0.001);
    assert(!editor->IsVisible());
    Press(page,12);key(14);key(1);key(15); // Restore manual X after window editing enables autoscale.
    Press(page,0);Press(page,26);Press(page,11);assert(page.GetGraphState().cursorMode==GraphState::Vertical);
    page.TouchStart(454,220);page.TouchMove(613,220);page.TouchEnd(613,220);Near(page.GetGraphState().cursorX[0],0.75);
    Near(page.GetGraphState().X().division,1); // Cursor drag must not pan.
    Press(page,4);assert(page.GetGraphState().X().automatic);
    Press(page,3);assert(!page.GetChild(11)->IsEnabled());assert(system.GetOutputOn());
    Press(page,1);Press(page,17);assert(system.Buffer.IsEmpty());assert(system.GetOutputOn());
    Press(page,5);Press(page,6);assert(backCalls==1 && homeCalls==1);
    page.Open();assert(system.GetGraphSettings().style==GraphStyle::Both);
    MenuPage menu(0,0,800,480);menu.SetOnGraph(Graph);menu.Open();Tap(menu,menu.GetChild(12));assert(graphCalls==1);
    // All 100 readings in the first 10 seconds reach the pixel raster.
    BufferSample denseSamples[100];
    for(unsigned j=0;j<100;++j) denseSamples[j]=Hardware::MakeMeasurement(j*100,2+(j%3),1);
    GraphState dense;dense.hasOrigin=true;dense.timeOrigin=0;
    dense.X().start=0;dense.X().division=10;dense.y[0].start=0;dense.y[0].division=1;
    GraphState::Trace denseTrace{&dense,denseSamples,GraphState::Voltage};
    uint8_t rasterBits[80*121];PlotRaster raster(rasterBits,637,121);
    assert(raster.Draw(denseTrace.Source(100),dense.X(),dense.y[0],false,true));
    for(unsigned j=0;j<100;++j) {
        const auto point=GraphState::Trace::Read(&denseTrace,j);
        const int x=static_cast<int>(point.x/100*636+.5),y=static_cast<int>((1-point.y/6)*120+.5);
        assert(rasterBits[y*80+x/8]&(0x80u>>(x&7)));
    }
    // Invalid samples break lines; off-screen endpoints are clipped before rasterizing.
    PlotPoint rasterPoints[]={{-1,3,true},{101,3,true},{0,0,false},{1,1,true}};
    PlotPointSource rasterSource{rasterPoints,[](const void* p,uint16_t i) {return static_cast<const PlotPoint*>(p)[i];},4};
    assert(raster.Draw(rasterSource,dense.X(),dense.y[0],true,false));
    for(unsigned x=0;x<637;++x) assert(rasterBits[60*80+x/8]&(0x80u>>(x&7)));
    rasterPoints[0]={0,0,true};rasterPoints[1]={0,0,false};rasterPoints[2]={100,6,true};rasterSource.count=3;
    raster.Draw(rasterSource,dense.X(),dense.y[0],true,false);
    for(auto byte:rasterBits) assert(byte==0); // no line across the invalid sample
    {
        // Corner inclusion for decimal spans and the extreme reciprocal fallback.
        uint8_t guarded[7]={0xA5,0,0,0,0,0,0x5A};
        PlotRaster small(guarded+1,5,5);
        PlotPoint corners[4];
        PlotPointSource source{corners,[](const void* p,uint16_t i) {
            return static_cast<const PlotPoint*>(p)[i];},4};
        for(double division:{0.03,0.7,1e-310}) {
            PlotRange xr{0,division,false},yr{0,division,false};
            const double xmax=division*10,ymax=division*6;
            corners[0]={0,0,true};corners[1]={xmax,0,true};
            corners[2]={0,ymax,true};corners[3]={xmax,ymax,true};
            assert(small.Draw(source,xr,yr,false,true));
            assert((guarded[1]&0x88)==0x88 && (guarded[5]&0x88)==0x88);
            assert(guarded[0]==0xA5 && guarded[6]==0x5A);
        }
        // A point exactly on a pixel-rounding midpoint rounds upward as before.
        corners[0]={0.125,0.5,true};source.count=1;
        assert(small.Draw(source,{0,0.1,false},{0,1.0/6,false},false,true));
        assert(guarded[2]&0x40); // upper arm of the dot centered at x=1, y=2
    }
    compact.Draw();assert(lastPoints.size()==compactSystem.Buffer.GetCount());
    char axisLabel[64];
    DisplayFormat::FormatUnsigned(0,axisLabel,sizeof(axisLabel));assert(std::string(axisLabel)=="0");
    DisplayFormat::FormatUnsigned(1,axisLabel,sizeof(axisLabel),2);assert(std::string(axisLabel)=="01");
    DisplayFormat::FormatUnsigned(UINT64_MAX,axisLabel,sizeof(axisLabel));assert(std::string(axisLabel)=="18446744073709551615");
    DisplayFormat::FormatUnsigned(4294967296ULL,axisLabel,sizeof(axisLabel));assert(std::string(axisLabel)=="4294967296");
    char tiny[2]={'x','x'};DisplayFormat::FormatUnsigned(123,tiny,sizeof(tiny));assert(tiny[0]=='1' && tiny[1]=='\0');
    DisplayFormat::FormatUnsigned(0,nullptr,0);
    GraphState::FormatTime(0,20,axisLabel,sizeof(axisLabel));assert(std::string(axisLabel)=="00:00");
    GraphState::FormatTime(100,20,axisLabel,sizeof(axisLabel));assert(std::string(axisLabel)=="01:40");
    GraphState::FormatTime(3600,20,axisLabel,sizeof(axisLabel));assert(std::string(axisLabel)=="60:00");
    GraphState::FormatTime(-10,20,axisLabel,sizeof(axisLabel));assert(std::string(axisLabel)=="-00:10");
    GraphState::FormatTime(59.9999,.001,axisLabel,sizeof(axisLabel));assert(std::string(axisLabel)=="01:00.000");
    BufferSample narrow[]={Hardware::MakeMeasurement(0,.09999f,.001f),Hardware::MakeMeasurement(100,.10001f,.001f)};
    GraphState labeled;labeled.Resolve(narrow,2);std::string previousLabel;
    for(unsigned tick=0;tick<=6;++tick) {
        GraphState::FormatAxis(labeled.y[0],6,tick,"V",axisLabel,sizeof(axisLabel));
        assert(std::string(axisLabel)!=previousLabel);previousLabel=axisLabel;
        assert(previousLabel.find("mV")!=std::string::npos);
    }
    Near(labeled.y[0].division,5e-6);
    GraphState tight;tight.tightYAxis=true;tight.Resolve(narrow,2);
    const double noiseSpan=narrow[1].Voltage-narrow[0].Voltage;
    Near(tight.y[0].division*6,noiseSpan*1.1);
    Near(tight.y[0].start,narrow[0].Voltage-noiseSpan*.05);
    const auto still=tight.y[0];tight.Resolve(narrow,2);
    assert(tight.y[0].start==still.start && tight.y[0].division==still.division);
    tight.y[0].start=.099;tight.y[0].division=.001;
    tight.Resolve(narrow,2);Near(tight.y[0].division*6,noiseSpan*1.1);
    for(unsigned tick=0;tick<=6;++tick) {
        GraphState::FormatAxis(tight.y[0],6,tick,"V",axisLabel,sizeof(axisLabel));
        assert(std::string(axisLabel).size()<16);
    }
    GraphState axisZoom;axisZoom.X().start=10;axisZoom.X().division=2;
    for(unsigned q=0;q<4;++q) {axisZoom.y[q].start=q;axisZoom.y[q].division=q+1;}
    auto beforeAxisZoom=axisZoom;
    assert(axisZoom.Zoom(.5,.25,.75,GraphState::ZoomX));
    Near(axisZoom.X().division,1);Near(axisZoom.X().start,12.5);
    for(unsigned q=0;q<4;++q) {Near(axisZoom.y[q].start,beforeAxisZoom.y[q].start);Near(axisZoom.y[q].division,beforeAxisZoom.y[q].division);}
    beforeAxisZoom=axisZoom;assert(axisZoom.Zoom(.5,.25,.75,GraphState::ZoomY));
    Near(axisZoom.X().start,beforeAxisZoom.X().start);Near(axisZoom.X().division,beforeAxisZoom.X().division);
    Near(axisZoom.y[0].division,.5);Near(axisZoom.y[0].start,2.25);
    GraphPage axisPage(0,0,800,480);axisPage.SetSystem(&system);axisPage.Open();axisPage.Draw();
    auto axisBefore=axisPage.GetGraphState();
    axisPage.HandleMultiTouch(fingers(350,220,450,220));axisPage.HandleMultiTouch(fingers(300,220,500,220));axisPage.HandleMultiTouch(released);
    Near(axisPage.GetGraphState().X().division,axisBefore.X().division/2);
    Near(axisPage.GetGraphState().y[0].division,axisBefore.y[0].division);
    axisPage.Draw(); // next gesture infers Y independently
    axisBefore=axisPage.GetGraphState();
    axisPage.HandleMultiTouch(fingers(400,200,400,250));axisPage.HandleMultiTouch(fingers(400,175,400,275));axisPage.HandleMultiTouch(released);
    Near(axisPage.GetGraphState().X().division,axisBefore.X().division);
    Near(axisPage.GetGraphState().y[0].division,axisBefore.y[0].division/2);
    axisBefore=axisPage.GetGraphState();
    axisPage.HandleMultiTouch(fingers(350,200,450,250));
    axisPage.HandleMultiTouch(fingers(300,175,500,275));axisPage.HandleMultiTouch(released);
    Near(axisPage.GetGraphState().X().division,axisBefore.X().division/2);
    Near(axisPage.GetGraphState().y[0].division,axisBefore.y[0].division/2);
    axisBefore=axisPage.GetGraphState();
    axisPage.HandleMultiTouch(fingers(350,200,450,250));
    axisPage.HandleMultiTouch(fingers(349,200,451,250)); // jitter must not zoom
    Near(axisPage.GetGraphState().X().division,axisBefore.X().division);
    axisPage.HandleMultiTouch(fingers(300,200,500,250)); // horizontal motion, despite diagonal placement
    Near(axisPage.GetGraphState().y[0].division,axisBefore.y[0].division);
    axisPage.HandleMultiTouch(fingers(300,175,500,275)); // direction stays X during this gesture
    Near(axisPage.GetGraphState().y[0].division,axisBefore.y[0].division);
    axisPage.HandleMultiTouch(released);
    // Histogram math: boundaries, two outlier bins, invalid values, all modes,
    // identical readings, removal and rebinning of the retained ring buffer.
    HistogramSettings hs;hs.bins=4;hs.minimum=0;hs.maximum=4;hs.method=HistogramScale::Off;hs.yMaximum=2;
    BufferSample histSamples[9]={};const float values[]={-1,0,1,2,3,4,5,NAN,INFINITY};
    for(unsigned i=0;i<9;++i) {histSamples[i].Voltage=values[i];histSamples[i].Current=values[i];}
    HistogramResult hr;
    CalculateHistogram(histSamples,9,hs,hr);assert(hr.valid==7 && hr.invalid==2);
    const unsigned expected[]={1,1,1,1,2,1};for(unsigned i=0;i<6;++i) assert(hr.counts[i]==expected[i]);
    assert(hr.BinAt(0)==1 && hr.BinAt(1)==2 && hr.BinAt(4)==4 && hr.BinAt(5)==5);
    Near(hr.minimum,0);Near(hr.maximum,4);assert(hr.yMaximum==2);
    hs.method=HistogramScale::Fit;CalculateHistogram(histSamples,9,hs,hr);assert(hr.yMaximum>=hr.peak);Near(hr.minimum,0);
    hs.method=HistogramScale::AutoBin;CalculateHistogram(histSamples,9,hs,hr);Near(hr.minimum,-1);Near(hr.maximum,5);assert(hr.counts[0]==0 && hr.counts[5]==0);
    hs.method=HistogramScale::SmartScale;CalculateHistogram(histSamples,9,hs,hr);Near(hr.minimum,-1);
    for(unsigned q=0;q<4;++q) {hs.quantity=static_cast<BufferQuantity>(q);CalculateHistogram(histSamples,9,hs,hr);assert(hr.valid==(q==3?6:7));}
    hs.quantity=BufferQuantity::Voltage;histSamples[0].Voltage=2;CalculateHistogram(histSamples,1,hs,hr);
    assert(hr.hasData && hr.minimum<2 && hr.maximum>2 && hr.width>0 && hr.valid==1);
    hs.bins=64;CalculateHistogram(histSamples,1,hs,hr);assert(hr.bins==64 && hr.valid==1);
    CalculateHistogram(nullptr,0,hs,hr);assert(!hr.hasData && hr.valid==0 && hr.width>0);
    hs.enabled=false;CalculateHistogram(histSamples,9,hs,hr);assert(!hr.hasData && hr.valid==0);
    hs.bins=0;assert(!ValidHistogramSettings(hs));hs.bins=65;assert(!ValidHistogramSettings(hs));
    hs.bins=4;hs.minimum=hs.maximum;assert(!ValidHistogramSettings(hs));
    BufferClass ring;for(unsigned i=0;i<totalReadings;++i) ring.AddSample(i,i,1);
    const auto retained=ring.CopyLatest(samples,BufferClass::Capacity);hs=HistogramSettings();hs.method=HistogramScale::AutoBin;
    CalculateHistogram(samples,retained,hs,hr);assert(hr.valid==BufferClass::Capacity);Near(hr.dataMin,overwritten);Near(hr.dataMax,totalReadings-1);
    unsigned total=0;for(unsigned i=0;i<hr.bins+2;++i) total+=hr.counts[i];assert(total==BufferClass::Capacity);

    SystemClass histSystem;histSystem.Reset();histSystem.SetSimulationEnabled(true);histSystem.Buffer.Reset();histSystem.SetOutputOn(true);
    hs=HistogramSettings();hs.bins=4;hs.minimum=0;hs.maximum=4;hs.method=HistogramScale::Fit;assert(histSystem.SetHistogramSettings(hs));
    histSystem.Buffer.AddSample(0,0.5f,0.1f);histSystem.Buffer.AddSample(1000,2,0.2f);
    HistogramPage histogram(0,0,800,480);histogram.SetSystem(&histSystem);histogram.SetOnHome(Home);histogram.SetOnBack(Back);
    histogram.Open();histogram.Draw();assert(barDraws>0 && histogram.GetHistogram().valid==2);
    auto htab=[&](unsigned tab) {auto& t=histogram.GetTabs();const int x=t.GetX()+(tab*2+1)*t.GetWidth()/6;histogram.TouchStart(x,24);histogram.TouchEnd(x,24);histogram.Draw();};
    auto hpress=[&](unsigned id) {Tap(histogram,histogram.GetChild(id));histogram.Draw();};
    // First normal bin (slot 1 of 6); open detail popup and close it.
    histogram.TouchStart(250,250);histogram.TouchEnd(250,250);histogram.Draw();assert(histogram.GetChild(18)->IsVisible());
    assert(histSystem.GetOutputOn());hpress(18);
    htab(1);hpress(7);assert(!histSystem.GetHistogramSettings().enabled && histSystem.Buffer.GetCount()==2);
    hpress(7);assert(histogram.GetHistogram().valid==2);hpress(9);assert(histogram.GetChild(19)->IsVisible());hpress(19);
    htab(2);hpress(12);auto* histEditor=static_cast<Panel*>(histogram.GetChild(21));assert(histEditor->IsVisible());
    const unsigned homeBefore=homeCalls;histogram.TouchStart(120,65);histogram.TouchEnd(120,65);assert(homeCalls==homeBefore);
    auto hkey=[&](unsigned k) {auto* key=histEditor->GetChild(k);const int x=key->GetX()+10,y=key->GetY()+10;histogram.TouchStart(x,y);histogram.TouchEnd(x,y);histogram.Draw();};
    hkey(14);hkey(0);hkey(15);assert(histEditor->IsVisible()); // zero bins rejected
    hkey(14);hkey(8);hkey(15);assert(!histEditor->IsVisible() && histSystem.GetHistogramSettings().bins==8);
    hpress(10);hkey(14);hkey(9);hkey(15);assert(histEditor->IsVisible());hkey(16);Near(histSystem.GetHistogramSettings().minimum,0);
    hpress(16);assert(histSystem.GetHistogramSettings().method==HistogramScale::Off);
    hpress(2);assert(histSystem.GetHistogramSettings().method==HistogramScale::SmartScale);
    histSystem.Buffer.AddSample(2000,9,0.3f);htab(0);assert(histogram.GetHistogram().valid==3);
    histogram.SetVisible(false);histogram.Open();assert(histSystem.GetHistogramSettings().bins==8);
    htab(1);hpress(8);assert(histSystem.Buffer.IsEmpty() && histogram.GetHistogram().valid==0 && histSystem.GetOutputOn());
    const unsigned hb=homeCalls,bb=backCalls;hpress(0);hpress(1);assert(homeCalls==hb+1 && backCalls==bb+1);
    menu.SetOnHistogram(Histogram);menu.Open();Tap(menu,menu.GetChild(13));assert(histogramCalls==1);

    // LIVE keeps the latest eight rows in one cache of at most 50 samples.
    BufferClass liveRows;ReadingTableState liveTable;
    for(unsigned j=1;j<=totalReadings;++j) {
        liveRows.AddSample(j*100,j,1);liveTable.Refresh(liveRows);
        const unsigned count=std::min<unsigned>(j,BufferClass::Capacity);
        assert(liveTable.Count()==count && liveTable.CachedCount()==(std::min<unsigned>(count,ReadingTableState::PageCapacity)));
        assert(liveTable.Offset()==(count>8?count-8:0));
        assert(liveTable.Index(liveTable.Count()-1)==j);
        if(j>=8) assert(liveTable.Index(liveTable.Offset())==j-7);
        Near(liveTable.Sample(liveTable.Count()-1).Voltage,j);
    }
    liveRows.Reset();for(unsigned j=0;j<20;++j) liveRows.AddSample(j,j,1);
    liveTable.SetLive(liveRows);assert(liveTable.Offset()==12);
    liveTable.First();liveTable.Page(1);assert(liveTable.Offset()==8);
    liveTable.Page(1);assert(liveTable.Offset()==12);
    liveTable.Page(1);assert(liveTable.Offset()==12);
    liveTable.Select(19);assert(liveTable.Offset()==12 && liveTable.Selected()==19);
    BufferClass tableBuffer;ReadingTableState tableState;tableState.Refresh(tableBuffer);
    assert(tableState.Count()==0 && !tableState.SelectTime(0.5));
    for(unsigned i=0;i<totalReadings;++i) tableBuffer.AddSample(5000+i*10,i,1);
    assert(tableBuffer.GetTotalSamples()==totalReadings && tableBuffer.GetStartTimeMs()==5000);
    tableState.Refresh(tableBuffer);assert(tableState.Count()==BufferClass::Capacity && tableState.Index(0)==overwritten+1);
    assert(tableState.CachedCount()==ReadingTableState::PageCapacity && tableState.CacheOffset()==lastCache);
    tableState.First();assert(tableState.IsHeld() && tableState.Offset()==0);
    assert(tableState.TimeMs(0)==overwritten*10 && tableState.Index(BufferClass::Capacity-1)==totalReadings);
    // Every screenful is backed by the cache, across 50-sample and ring boundaries.
    for(unsigned step=0;step<=BufferClass::Capacity/ReadingTableState::Rows;++step) {
        const uint16_t offset=tableState.Offset();
        assert(offset>=tableState.CacheOffset() && offset+8<=tableState.CacheOffset()+tableState.CachedCount());
        for(unsigned row=0;row<8;++row) Near(tableState.Sample(offset+row).Voltage,overwritten+offset+row);
        tableState.Page(1);
    }
    assert(tableState.Offset()==lastRows);
    for(unsigned step=0;step<=BufferClass::Capacity/ReadingTableState::Rows;++step) tableState.Page(-1);
    assert(tableState.Offset()==0);
    tableState.SelectTime(1);assert(tableState.Selected()==ReadingTableState::PageCapacity-1 && tableState.Offset()==ReadingTableState::PageCapacity-ReadingTableState::Rows);
    tableState.First();tableState.SelectTime(0);assert(tableState.Selected()==0);
    tableState.Last();assert(tableState.Offset()==lastRows);
    tableState.SelectTime(0);assert(tableState.Selected()==lastCache && tableState.Offset()==lastCache);
    tableState.SelectTime(1);assert(tableState.Selected()==BufferClass::Capacity-1 && tableState.Offset()==lastRows);
    const auto frozenFirst=tableState.CachedSamples()[0].Voltage;
    const auto frozenGeneration=tableBuffer.GetResetGeneration();
    for(unsigned i=0;i<BufferClass::Capacity+1u;++i) tableBuffer.AddSample(5000+totalReadings*10+i,9999,1);
    tableState.Refresh(tableBuffer);Near(tableState.CachedSamples()[0].Voltage,frozenFirst);
    tableState.First();assert(tableState.PageUnavailable() && tableState.Offset()==lastRows);
    assert(tableState.SelectTime(0) && tableState.Selected()==lastCache); // cached page remains usable
    tableBuffer.Reset();tableState.Refresh(tableBuffer);assert(tableState.Count()==BufferClass::Capacity);
    assert(tableBuffer.GetResetGeneration()!=frozenGeneration);
    tableState.First();assert(tableState.PageUnavailable());
    tableState.Last();assert(!tableState.PageUnavailable() && tableState.Offset()==lastRows);
    // Same reading indices in a new generation cannot replace the frozen history.
    for(unsigned i=0;i<totalReadings;++i) tableBuffer.AddSample(i,12345,1);
    tableState.First();assert(tableState.PageUnavailable());
    Near(tableState.CachedSamples()[0].Voltage,frozenFirst);
    tableBuffer.Reset();tableState.SetLive(tableBuffer);assert(tableState.Count()==0 && tableState.CachedCount()==0);
    tableBuffer.AddSample(0xFFFFFF00,1,0);tableBuffer.AddSample(100,2,1);
    tableState.Refresh(tableBuffer);assert(tableState.TimeMs(1)==356 && tableState.Index(0)==1);
    assert(!tableState.Select(2) && !tableState.SelectTime(NAN));
    // Preview selection uses timestamps within the cached page.
    tableBuffer.Reset();tableBuffer.AddSample(0,1,1);tableBuffer.AddSample(100,2,1);tableBuffer.AddSample(10000,3,1);
    tableState.Refresh(tableBuffer);tableState.SelectTime(0.5);assert(tableState.Selected()==1);
    // Partial overwrite: load a still available page without reading older expired slots.
    tableBuffer.Reset();for(unsigned i=0;i<BufferClass::Capacity;++i) tableBuffer.AddSample(i,i,1);
    tableState.SetLive(tableBuffer);tableState.Hold();
    for(unsigned i=BufferClass::Capacity;i<BufferClass::Capacity+30u;++i) tableBuffer.AddSample(i,i,1);
    assert(tableState.Select(32));Near(tableState.Sample(32).Voltage,32);
    assert(tableState.CacheOffset()==30);
    tableState.First();assert(tableState.PageUnavailable());
    tableState.SetLive(tableBuffer);assert(!tableState.PageUnavailable());

    SystemClass readings;readings.Reset();readings.SetSimulationEnabled(true);readings.Buffer.Reset();readings.SetOutputOn(true);
    for(unsigned i=0;i<20;++i) readings.Buffer.AddSample(10000+i*100,i,i?0.1f:0);
    ReadingTablePage tablePage(0,0,800,480);tablePage.SetSystem(&readings);tablePage.SetOnHome(Home);tablePage.SetOnBack(Back);
    tablePage.Open();tablePage.Draw();assert(tableDraws>0 && tablePage.GetTable().Offset()==12);
    auto tpress=[&](unsigned id) {Tap(tablePage,tablePage.GetChild(id));tablePage.Draw();};
    tpress(3);assert(tablePage.GetTable().IsHeld() && tablePage.GetTable().Offset()==4);
    readings.Buffer.AddSample(12000,100,0.1f);tablePage.Draw();assert(tablePage.GetTable().Count()==20);
    tpress(6);assert(tablePage.GetTable().Offset()==0);
    tablePage.TouchStart(200,190);readings.Buffer.Reset();tablePage.Draw();tablePage.TouchEnd(200,190);tablePage.Draw();
    assert(tablePage.GetChild(8)->IsVisible() && tablePage.GetTable().Selected()==0);
    assert(std::find(texts.begin(),texts.end(),"--")!=texts.end());
    const unsigned beforeTableHome=homeCalls;tablePage.TouchStart(120,65);tablePage.TouchEnd(120,65);assert(homeCalls==beforeTableHome);
    tpress(8);tablePage.SetVisible(false);tablePage.Open();assert(tablePage.GetTable().Count()==20 && tablePage.GetTable().IsHeld());
    tpress(5);assert(tablePage.GetTable().Count()==0 && !tablePage.GetTable().IsHeld());
    for(unsigned i=0;i<20;++i) readings.Buffer.AddSample(30000+i*100,i,0.1f);
    tablePage.Draw();tablePage.TouchStart(448,80);tablePage.TouchEnd(448,80);tablePage.Draw();
    assert(tablePage.GetTable().Selected()==0 && tablePage.GetTable().Offset()==0 && tablePage.GetTable().IsHeld());
    tpress(2);assert(tablePage.GetChild(9)->IsVisible());tpress(9);
    const unsigned th=homeCalls,tb=backCalls;tpress(0);tpress(1);assert(homeCalls==th+1 && backCalls==tb+1 && readings.GetOutputOn());
    menu.SetOnReadingTable(ReadingTable);menu.Open();Tap(menu,menu.GetChild(14));assert(tableCalls==1);
    // UI renders only the cached preview and all visible rows, including HOLD after clear.
    readings.Buffer.Reset();for(unsigned i=0;i<120;++i) readings.Buffer.AddSample(700000+i*100,i,0.1f);
    tpress(5);assert(tablePage.GetTable().CachedCount()==50 && tablePage.GetTable().Offset()==112);
    assert(lastPoints.size()==50);Near(lastPoints.front().x,7);Near(lastPoints.back().x,11.9);
    tablePage.TouchStart(200,190);readings.ClearMeasurementBuffer();tablePage.TouchEnd(200,190);tablePage.Draw();
    assert(tablePage.GetTable().Selected()==112 && tablePage.GetChild(8)->IsVisible());
    Near(tablePage.GetTable().Sample(112).Voltage,112);
    tpress(8);texts.clear();tpress(6);
    assert(tablePage.GetTable().PageUnavailable() && tablePage.GetTable().Offset()==112);
    assert(std::find(texts.begin(),texts.end(),"Page expired - select LIVE")!=texts.end());
    tpress(5);assert(tablePage.GetTable().Count()==0 && tablePage.GetTable().CachedCount()==0);



    BufferSample failed=Hardware::MakeMeasurement(1,12,1);failed.Fault=Hardware::Error::UnsupportedMode;
    double ignored;assert(!GraphState::Value(failed,GraphState::Voltage,ignored));
    BufferClass failedBuffer;failedBuffer.AddSample(failed);assert(failedBuffer.GetStatistics(BufferQuantity::Voltage).Count==0);
    HistogramResult failedHistogram;CalculateHistogram(&failed,1,HistogramSettings(),failedHistogram);assert(failedHistogram.valid==0);
    SystemClass simulated;simulated.SetSimulationEnabled(true);
    simulated.SetVoltageSourceValue(5);simulated.SetCurrentLimitValue(0.02f);simulated.ExecuteAt(100);
    SimulationPage simulation(0,0,800,480);simulation.SetSystem(&simulated);simulation.SetOnBack(Back);simulation.SetOnHome(Home);
    simulation.Open();simulation.Draw();
    assert(!simulation.GetChild(10)->IsEnabled()); // active voltage range
    assert(!simulation.GetChild(14)->IsEnabled()); // active 100 mA range
    Tap(simulation,simulation.GetChild(11));assert(!simulated.IsVoltageRangeAvailable(VoltageRange::Range30V));
    Tap(simulation,simulation.GetChild(4));simulated.ExecuteAt(101);Near(simulated.GetCurrent(),0);
    Tap(simulation,simulation.GetChild(5));simulated.ExecuteAt(102);assert(fabs(simulated.GetCurrent()-0.02)<0.00000201);assert(simulated.GetMeasurement().Mode==Hardware::Regulation::CC);
    Tap(simulation,simulation.GetChild(3));Tap(simulation,simulation.GetChild(8)); // kOhm editor
    Tap(simulation,simulation.GetChild(6));assert(simulation.GetChild(16)->IsVisible());
    auto* resistanceEditor=static_cast<Panel*>(simulation.GetChild(16));
    auto* zero=resistanceEditor->GetChild(0);Tap(*resistanceEditor,zero);Tap(*resistanceEditor,resistanceEditor->GetChild(15));
    assert(resistanceEditor->IsVisible()); // zero rejected
    Tap(*resistanceEditor,resistanceEditor->GetChild(14));Tap(*resistanceEditor,resistanceEditor->GetChild(2));
    Tap(*resistanceEditor,resistanceEditor->GetChild(15));simulation.Draw();
    Near(simulated.GetSimulationSettings().resistance,2000);assert(!resistanceEditor->IsVisible());
    Tap(simulation,simulation.GetChild(2));assert(!simulated.GetSimulationEnabled() && !simulated.GetOutputOn());
    simulation.Draw();Tap(simulation,simulation.GetChild(2));assert(simulated.GetSimulationEnabled() && simulated.GetOutputOn());
    simulation.SetVisible(false);simulation.Open();Near(simulated.GetSimulationSettings().resistance,2000);
    menu.Open();menu.SetOnSimulation(Simulation);Tap(menu,menu.GetChild(32));assert(simulationCalls==1);

    // SIM Open: selecting Current must draw the actual zero-current samples,
    // not leave the noisy voltage curve behind a current-labeled Y axis.
    {
        SystemClass openSystem;openSystem.SetSimulationEnabled(true);openSystem.SetOutputOn(true);
        openSystem.SetVoltageSourceValue(5);openSystem.SetCurrentLimitValue(.01f);
        openSystem.SetSimulationLoad(Hardware::LoadType::Open);
        for(unsigned n=0;n<100;++n) openSystem.ExecuteAt(n*50);
        assert(openSystem.GetCurrent()==0);
        for(unsigned n=0;n<openSystem.Buffer.GetCount();++n) assert(openSystem.Buffer.View()[n].Current==0);
        const auto retainedBefore=openSystem.Buffer.GetCount();
        GraphPage openPage(0,0,800,480);openPage.SetSystem(&openSystem);openPage.Open();openPage.Draw();
        assert(!lastPoints.empty() && lastPoints.back().y>4.99);
        Press(openPage,8);
        assert(openPage.GetGraphState().IsCurveVisible(GraphState::Current));
        assert(!openPage.GetGraphState().IsCurveVisible(GraphState::Voltage));
        for(const auto& point:lastPoints) assert(point.y==0);
        assert(std::find(texts.begin(),texts.end(),"Current [Y]")!=texts.end());
        GraphPanel compact(0,0,800,240);compact.SetSystem(&openSystem);openSystem.SetMeasureMode(MeasureMode::Current);compact.Draw();
        for(const auto& point:lastPoints) assert(point.y==0); // both graphs agree
        // Explicit Data/Add keeps the existing current curve and adds voltage.
        Press(openPage,1);Press(openPage,7);assert(!openPage.GetGraphState().enabled[0]);
        Press(openPage,11);Press(openPage,0);Press(openPage,8);
        assert(openPage.GetGraphState().enabled[0] && openPage.GetGraphState().enabled[1]);
        texts.clear();openPage.Draw();
        assert(std::find(texts.begin(),texts.end(),"Voltage")!=texts.end());
        assert(std::find(texts.begin(),texts.end(),"Current [Y]")!=texts.end());
        for(const auto& point:lastPoints) assert(point.y==0);
        Press(openPage,9);assert(openPage.GetGraphState().enabled[0] && openPage.GetGraphState().enabled[1] && openPage.GetGraphState().enabled[2]);
        const auto retained=openSystem.Buffer.GetCount();assert(retained==retainedBefore && openSystem.GetOutputOn());
    }

    // Histogram quantity selection must switch actual data and units together.
    // Open load has noisy voltage but exactly zero current/power and invalid R.
    {
        SystemClass openHist;openHist.SetSimulationEnabled(true);openHist.SetOutputOn(true);
        openHist.SetVoltageSourceValue(5);openHist.SetCurrentLimitValue(.01f);
        openHist.SetSimulationLoad(Hardware::LoadType::Open);openHist.ClearMeasurementBuffer();
        for(unsigned n=0;n<BufferClass::Capacity+17u;++n) openHist.ExecuteAt(n*50);
        const auto retained=openHist.Buffer.GetCount();assert(retained==BufferClass::Capacity);
        HistogramPage view(0,0,800,480);view.SetSystem(&openHist);view.Open();view.Draw();
        assert(view.GetHistogram().dataMin>4.99 && view.GetHistogram().dataMax>view.GetHistogram().dataMin);
        for(unsigned repeat=0;repeat<3;++repeat) {
            Tap(view,view.GetChild(4));texts.clear();view.Draw(); // Current
            const auto& result=view.GetHistogram();
            assert(openHist.GetHistogramSettings().quantity==BufferQuantity::Current);
            assert(result.valid==retained && result.invalid==0 && result.dataMin==0 && result.dataMax==0);
            assert(result.counts[result.BinAt(0)]==retained && result.counts[0]==0 && result.counts[result.bins+1]==0);
            unsigned occupied=0,totalCounts=0;
            for(unsigned bin=0;bin<result.bins+2u;++bin) {occupied+=result.counts[bin]>0;totalCounts+=result.counts[bin];}
            assert(occupied==1 && totalCounts==retained);
            assert(std::find(texts.begin(),texts.end(),"Data min: 0 A | Data max: 0 A | Invalid: 0")!=texts.end());
            Tap(view,view.GetChild(3));view.Draw(); // Voltage again, same retained samples
            assert(view.GetHistogram().dataMin>4.99 && view.GetHistogram().dataMax>view.GetHistogram().dataMin);
        }
        Tap(view,view.GetChild(5));view.Draw();assert(view.GetHistogram().dataMin==0 && view.GetHistogram().dataMax==0); // Power
        Tap(view,view.GetChild(6));view.Draw();assert(!view.GetHistogram().hasData && view.GetHistogram().invalid==retained); // Open resistance
        Tap(view,view.GetChild(4));view.Draw();assert(view.GetHistogram().valid==retained);
        // Old nonzero values are real history, not stale histogram output.
        openHist.Buffer.AddSample((BufferClass::Capacity+18u)*50,5,0.0000004f);view.Draw();
        assert(view.GetHistogram().dataMax>0 && view.GetHistogram().dataMin==0);
        openHist.ClearMeasurementBuffer();view.Draw();assert(!view.GetHistogram().hasData && view.GetHistogram().valid==0);
        openHist.ExecuteAt((BufferClass::Capacity+20u)*50);view.Draw();
        assert(view.GetHistogram().valid==1 && view.GetHistogram().dataMin==0 && view.GetHistogram().dataMax==0);
        assert(openHist.GetOutputOn());
    }
    // Curve colors must not rotate with the quantity active on first opening.
    for(unsigned initial=0;initial<4;++initial) {
        SystemClass colorSystem;colorSystem.Buffer.AddSample(1,5,.001f);
        const MeasureMode modes[]={MeasureMode::Voltage,MeasureMode::Current,MeasureMode::Power,MeasureMode::Resistance};
        colorSystem.SetMeasureMode(modes[initial]);
        GraphPage colorPage(0,0,800,480);colorPage.SetSystem(&colorSystem);colorPage.Open();
        const uint32_t expected[]={0x40FF60,0x40D8FF,0xFFE050,0xFF80DC};
        for(unsigned q=0;q<4;++q) {
            Press(colorPage,7+q);assert(lastTraceColor==expected[q]);
        }
    }
    puts("PASS: zero-copy ring views, 50-sample page cache/expiry, timestamp axes, autoscale, manual values, pinch zoom/release/cancellation, zoom/pan, I-V, envelope/gaps, clipping, cursor, tabs, clear/remove, graph route, histogram bins/scaling/modals/routes, reading table hold/index/preview/routes, BACK/HOME, output preserved");
}
