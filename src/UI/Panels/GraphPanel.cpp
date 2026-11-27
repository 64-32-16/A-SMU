#include "GraphPanel.h"
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
#include "../Core/UiTiming.h"
#include "../Core/GraphPlot.h"
#include <stdlib.h>

GraphPanel::GraphPanel():Panel(),_system(&System)
{
    SetShowBackground(false);SetShowBorder(false);SetShowTitle(false);
}
GraphPanel::GraphPanel(int16_t x,int16_t y,int16_t w,int16_t h):GraphPanel()
{ SetBounds(x,y,w,h); }
void GraphPanel::SetSystem(SystemClass* system)
{
    _system=system?system:&System;
    _state=GraphState();
}
SystemClass* GraphPanel::GetSystem() const {return _system;}

void GraphPanel::Draw()
{
    if(!IsVisible()) return;
    const int16_t plotX=GetX()+116,plotY=GetY()+14;
    const int16_t plotW=GetWidth()-136,plotH=GetHeight()-48;
    if(plotW<=0 || plotH<=0) return;
    UI_TIMING_SCOPE(MainGraph);
    switch(_system->GetMeasureMode())
    {
        case MeasureMode::Voltage: _state.selected=GraphState::Voltage;break;
        case MeasureMode::Current: _state.selected=GraphState::Current;break;
        case MeasureMode::Power: _state.selected=GraphState::Power;break;
        case MeasureMode::Resistance: _state.selected=GraphState::Resistance;break;
    }
    const auto samples=_system->Buffer.View();
    const uint16_t count=samples.Count();
    for(unsigned q=0;q<4;++q) _state.enabled[q]=q==_state.selected;
    _state.tightYAxis=true;
    UI_TIMED(PlotPrepare,
        _state.Resolve(_system->Buffer,samples,MeasurementTiming::PairTimeUs(_system->GetNplcProfile()),true));
    const auto& xr=_state.X();const auto& yr=_state.y[_state.selected];
#if ASMU_UI_TIMING
    UiTiming::PlotWindow(xr.start);
#endif
    const char* units[]={"V","A","W","Ohm"};
    constexpr uint32_t curveColor=0x40FF60;
    GD.SaveContext();GD.VertexFormat(0);GD.ColorA(255);
    GD.ColorRGB(Theme::MainScreenBackground);GD.LineWidth(8);GD.Begin(RECTS);
    GD.Vertex2f(GetX(),GetY());GD.Vertex2f(GetX()+GetWidth()-1,GetY()+GetHeight()-1);
    const uint32_t colors[4]={curveColor,curveColor,curveColor,curveColor};
    const GraphPlot::Layout area{plotX,plotY,plotW,plotH,GetX()+4,17,Theme::ToolbarText,false,true};
    const bool any=GraphPlot::Draw(_state,samples,_system->Buffer,_system->GetGraphSettings().style,area,colors);
    if(!any)
    {
        GD.ColorRGB(Theme::ToolbarText);
        GD.cmd_text(plotX+plotW/2,plotY+plotH/2,26,OPT_CENTER,count?"No valid samples":"Buffer empty");
    }
    GD.RestoreContext();
}

bool GraphPanel::InPlot(int x,int y) const
{
    return GetWidth()>136 && GetHeight()>48 && x>=GetX()+116 && x<=GetX()+GetWidth()-20 &&
           y>=GetY()+14 && y<=GetY()+GetHeight()-34;
}
void GraphPanel::CancelTouch() {_drag=false;_moved=false;_hasTap=false;}
void GraphPanel::TouchStart(int16_t x,int16_t y)
{
    if(!IsVisible() || !IsEnabled() || _multiBlocked || !InPlot(x,y)) return;
    _drag=true;_moved=false;_lastX=x;_lastY=y;
}
void GraphPanel::TouchMove(int16_t x,int16_t y)
{
    if(!_drag || _multiBlocked) return;
    const int dx=x-_lastX,dy=y-_lastY;
    if(!_moved && abs(dx)<3 && abs(dy)<3) return;
    _moved=true;_hasTap=false;
    // Never move axes or interpret a drag as a tap; SourcePanel handles page swipes.
}
void GraphPanel::TouchEnd(int16_t x,int16_t y)
{
    if(!_drag) return;
    if(x<0 || y<0) {CancelTouch();return;}
    TouchMove(x,y);_drag=false;
    if(!_moved && InPlot(x,y))
    {
        const uint32_t now=millis();
        if(_hasTap && now-_lastTapMs<=350 && abs(x-_tapX)<24 && abs(y-_tapY)<24)
        {_state.AutoScale();_hasTap=false;}
        else {_hasTap=true;_lastTapMs=now;_tapX=x;_tapY=y;}
    }
}
bool GraphPanel::HandleMultiTouch(const TouchFrame& frame,bool allowStart)
{
    if(!_multiBlocked && (!allowStart || !IsVisible() || frame.count<2)) return false;
    // MainPage has no pinch zoom. Consume all contacts until complete release
    // so a rejected pinch cannot become a pan, double tap or page swipe.
    if(!_multiBlocked) {_multiBlocked=true;CancelTouch();}
    if(frame.count==0) _multiBlocked=false;
    return true;
}
