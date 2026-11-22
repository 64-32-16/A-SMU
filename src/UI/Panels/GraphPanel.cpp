#include "GraphPanel.h"
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
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
    _state=GraphState();_previousCount=0;
}
SystemClass* GraphPanel::GetSystem() const {return _system;}

void GraphPanel::Draw()
{
    if(!IsVisible()) return;
    const int16_t plotX=GetX()+116,plotY=GetY()+14;
    const int16_t plotW=GetWidth()-136,plotH=GetHeight()-48;
    if(plotW<=0 || plotH<=0) return;
    switch(_system->GetMeasureMode())
    {
        case MeasureMode::Voltage: _state.selected=GraphState::Voltage;break;
        case MeasureMode::Current: _state.selected=GraphState::Current;break;
        case MeasureMode::Power: _state.selected=GraphState::Power;break;
        case MeasureMode::Resistance: _state.selected=GraphState::Resistance;break;
    }
    const uint16_t count=_system->Buffer.CopyLatest(_samples,BufferClass::Capacity);
    if(count==0 || count<_previousCount)
    {
        const auto selected=_state.selected;_state=GraphState();_state.selected=selected;
    }
    _previousCount=count;
    for(unsigned q=0;q<4;++q) _state.enabled[q]=q==_state.selected;
    _state.tightYAxis=true;
    _state.Resolve(_samples,count);
    const auto& xr=_state.X();const auto& yr=_state.y[_state.selected];
    const char* units[]={"V","A","W","Ohm"};
    constexpr uint32_t curveColor=0x40FF60;
    GD.SaveContext();GD.VertexFormat(0);GD.ColorA(255);
    GD.ColorRGB(Theme::MainScreenBackground);GD.LineWidth(8);GD.Begin(RECTS);
    GD.Vertex2f(GetX(),GetY());GD.Vertex2f(GetX()+GetWidth()-1,GetY()+GetHeight()-1);
    GD.DrawPlotGrid(plotX,plotY,plotW,plotH,GraphState::XDivisions,GraphState::YDivisions,0x303030);
    char label[40];
    for(unsigned i=0;i<=GraphState::YDivisions;i+=2)
    {
        GraphState::FormatAxis(yr,GraphState::YDivisions,i,units[_state.selected],label,sizeof(label));
        GD.ColorRGB(curveColor);
        GD.cmd_text(GetX()+4,plotY+plotH-i*plotH/GraphState::YDivisions,26,OPT_CENTERY,label);
    }
    for(unsigned i=0;i<=GraphState::XDivisions;i+=2)
    {
        GraphState::FormatTime(xr.start+xr.division*i,xr.division*2,label,sizeof(label));
        GD.ColorRGB(Theme::ToolbarText);
        GD.cmd_text(plotX+plotW*i/GraphState::XDivisions,plotY+plotH+17,26,
                    i==GraphState::XDivisions?(OPT_RIGHTX|OPT_CENTERY):OPT_CENTER,label);
    }
    const GraphState::Trace trace{&_state,_samples,_state.selected};
    const bool any=GD.DrawPlotTrace(plotX,plotY,plotW,plotH,trace.Source(count),xr,yr,curveColor,
                       _system->GetGraphSettings().style!=GraphStyle::Points,
                       _system->GetGraphSettings().style!=GraphStyle::Lines);
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
