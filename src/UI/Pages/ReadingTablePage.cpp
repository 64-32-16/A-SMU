#include "ReadingTablePage.h"
#include "../Core/DisplayFormat.h"
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
namespace {
constexpr int tableX=12,tableY=176,rowH=30,previewX=448,previewY=56,previewW=328,previewH=68;
const int16_t columns[]={0,66,174,318,462,606,768};
const char* names[]={"Voltage","Current","Power","Resistance"};
const char* units[]={"V","A","W","Ohm"};
void Text(int x,int y,const char* text,uint32_t color=0xF2F2F2,uint8_t font=26,uint16_t options=0)
{GD.ColorRGB(color);GD.cmd_text(x,y,font,options,text);}
}
ReadingTablePage::ReadingTablePage(int16_t x,int16_t y,int16_t w,int16_t h):Panel(x,y,w,h),_system(&System)
{
    SetShowBackground(false);SetShowBorder(false);SetShowTitle(false);SetVisible(false);
    for(auto& b:_buttons) {
        b.SetFont(28);b.SetBackgroundColor(Theme::MainButtonBackground);b.SetTextColor(Theme::MainButtonText);
        b.SetDisabledBackgroundColor(0x303030);b.SetDisabledTextColor(0x808080);AddChild(&b);
    }
}
void ReadingTablePage::SetSystem(SystemClass* system)
{_system=system?system:&System;_table.SetLive(_system->Buffer);}
void ReadingTablePage::SetOnBack(ButtonClickHandler handler) {_buttons[0].SetOnClick(handler);}
void ReadingTablePage::SetOnHome(ButtonClickHandler handler) {_buttons[1].SetOnClick(handler);}
void ReadingTablePage::Open()
{
    Panel::TouchEnd(-1,-1);_popup=0;_pressedRow=-1;_previewPressed=false;
    SetVisible(true);_table.Refresh(_system->Buffer);Layout();
}
void ReadingTablePage::Configure(unsigned id,const char* text,int x,int y,int w,int h)
{_buttons[id].SetText(text);_buttons[id].SetBounds(x,y,w,h);_buttons[id].SetVisible(true);}
void ReadingTablePage::Layout()
{
    for(auto& b:_buttons) {b.SetVisible(false);b.SetEnabled(true);}
    if(_popup) {
        Configure(8,"Close",530,347,140,58);
        if(_popup==2) {Configure(9,"Active Buffer",160,170,480,58);_buttons[9].SetBackgroundColor(Theme::SourceBlue);}
        return;
    }
    Configure(0,"BACK",8,52,80,36);Configure(1,"HOME",96,52,80,36);
    Configure(2,"Active Buffer",192,52,232,36);
    Configure(3,"Previous",8,432,100);Configure(4,"Next",116,432,92);
    Configure(5,_table.IsHeld()?"LIVE":"HOLD",216,432,108);
    Configure(6,"First",608,432,80);Configure(7,"Last",696,432,96);
    _buttons[3].SetEnabled(_table.Offset()>0);_buttons[6].SetEnabled(_table.Offset()>0);
    _buttons[4].SetEnabled(_table.Offset()<_table.LastOffset());_buttons[7].SetEnabled(_table.Offset()<_table.LastOffset());
}
void ReadingTablePage::Action(unsigned id)
{
    if(id==2) {_popup=2;GD.RequestBackdropCapture();}
    else if(id==3) _table.Page(-1);
    else if(id==4) _table.Page(1);
    else if(id==5) {if(_table.IsHeld()) _table.SetLive(_system->Buffer);else _table.Hold();}
    else if(id==6) _table.First();
    else if(id==7) _table.Last();
    else if(id==8 || id==9) _popup=0;
    Layout();
}
void ReadingTablePage::Time(uint32_t ms,char* text,size_t size)
{snprintf(text,size,"%lu.%03lu",static_cast<unsigned long>(ms/1000),static_cast<unsigned long>(ms%1000));}
void ReadingTablePage::Value(const BufferSample& sample,GraphState::Quantity quantity,char* text,size_t size)
{
    double value;
    if(!GraphState::Value(sample,quantity,value)) {snprintf(text,size,"--");return;}
    GraphState::FormatValue(value,units[quantity],text,size,6);
}
void ReadingTablePage::Draw()
{
    if(!IsVisible()) return;
    if(_popup) {DrawPopup();return;}
    _table.Refresh(_system->Buffer);Layout();
    GD.DrawSolidRoundedRect(8,4,784,40,0,Theme::TabInactive);
    Text(20,10,"READING TABLE",Theme::ToolbarText,29);
    char text[64];snprintf(text,sizeof(text),"%s | %s | %u",_system->GetBackendText(),_table.IsHeld()?"HOLD":"LIVE",_table.Count());
    Text(780,24,text,_table.IsHeld()?0xFF80DC:Theme::ToolbarText,26,OPT_RIGHTX|OPT_CENTERY);
    Text(16,106,"Time since buffer start [s]",Theme::SourceBlue,26);
    DrawPreview();DrawRows();
    if(_table.PageUnavailable()) Text(16,130,"Page expired - select LIVE",0xFFAA40,26);
    if(_table.Count()) snprintf(text,sizeof(text),"%u-%u / %u",_table.Offset()+1,
        static_cast<unsigned>(_table.Offset()+ReadingTableState::Rows<_table.Count()?_table.Offset()+ReadingTableState::Rows:_table.Count()),_table.Count());
    else snprintf(text,sizeof(text),"0 / 0");
    Text(464,451,text,Theme::ToolbarText,26,OPT_CENTER);
    Panel::Draw();
}
void ReadingTablePage::DrawRows()
{
    const char* headers[]={"Index","Time [s]","Voltage","Current","Power","Resistance"};
    for(unsigned i=0;i<6;++i) Text(tableX+columns[i]+6,148,headers[i],Theme::ToolbarText,26);
    const int selected=_table.Selected();
    if(selected>=_table.Offset() && selected<_table.Offset()+ReadingTableState::Rows)
        GD.DrawSolidRoundedRect(tableX+1,tableY+(selected-_table.Offset())*rowH+1,766,rowH-2,0,0x28152A);
    GD.DrawTableGrid(tableX,tableY,rowH,ReadingTableState::Rows,columns,6,0x505050);
    if(!_table.Count()) {Text(400,290,"Buffer empty",Theme::ToolbarText,28,OPT_CENTER);return;}
    for(unsigned row=0;row<ReadingTableState::Rows;++row) {
        const uint16_t index=_table.Offset()+row;if(index>=_table.Count()) break;
        const int y=tableY+row*rowH+rowH/2;const uint32_t color=index==selected?0xFF80DC:Theme::ToolbarText;
        char text[40];DisplayFormat::FormatUnsigned(_table.Index(index),text,sizeof(text));
        if(strlen(text)>8) snprintf(text,sizeof(text),"%.3g",static_cast<double>(_table.Index(index)));
        Text(tableX+columns[1]-6,y,text,color,strlen(text)>7?24:26,OPT_RIGHTX|OPT_CENTERY);
        Time(_table.TimeMs(index),text,sizeof(text));Text(tableX+columns[2]-6,y,text,color,26,OPT_RIGHTX|OPT_CENTERY);
        for(unsigned q=0;q<4;++q) {
            Value(_table.Sample(index),static_cast<GraphState::Quantity>(q),text,sizeof(text));
            Text(tableX+columns[q+3]-6,y,text,color,26,OPT_RIGHTX|OPT_CENTERY);
        }
    }
    const int height=ReadingTableState::Rows*rowH;
    GD.DrawSolidRoundedRect(787,tableY,5,height,0,0x303030);
    int thumb=_table.Count()>ReadingTableState::Rows?height*ReadingTableState::Rows/_table.Count():height;if(thumb<12) thumb=12;
    const int top=_table.LastOffset()?(_table.Offset()*(height-thumb))/_table.LastOffset():0;
    GD.DrawSolidRoundedRect(787,tableY+top,5,thumb,0,Theme::TabActive);
}
void ReadingTablePage::DrawPreview()
{
    GD.DrawPlotGrid(previewX,previewY,previewW,previewH,4,2,0x303030);
    GraphState state;state.scrolling=false;state.hasOrigin=true;state.timeOrigin=_table.Origin();
    switch(_system->GetMeasureMode()) {
        case MeasureMode::Voltage:state.selected=GraphState::Voltage;break;
        case MeasureMode::Current:state.selected=GraphState::Current;break;
        case MeasureMode::Power:state.selected=GraphState::Power;break;
        case MeasureMode::Resistance:state.selected=GraphState::Resistance;break;
    }
    // The preview spans only the cached page, matching SelectTime's mapping.
    state.X().automatic=false;
    if(_table.CachedCount()) {
        const uint16_t first=_table.CacheOffset(),last=first+_table.CachedCount()-1;
        state.X().start=_table.TimeMs(first)/1000.0;
        const double span=(_table.TimeMs(last)-_table.TimeMs(first))/1000.0;
        state.X().division=(span>0?span:0.001)/GraphState::XDivisions;
    }
    state.Resolve(_table.CachedSamples());
    const GraphState::Trace trace{&state,_table.CachedSamples(),state.selected};
    const auto style=_system->GetGraphSettings().style;
    GD.DrawPlotTrace(previewX,previewY,previewW,previewH,trace.Source(_table.CachedCount()),state.X(),state.y[state.selected],0x40FF60,style!=GraphStyle::Points,style!=GraphStyle::Lines);
    if(_table.Selected()>=0) {
        const double position=(_table.TimeMs(_table.Selected())/1000.0-state.X().start)/(state.X().division*10);
        const int marker=previewX+position*previewW;
        GD.DrawSolidRoundedRect(marker,previewY,2,previewH,0,0xFF80DC);
    }
    char caption[40];snprintf(caption,sizeof(caption),"%s | %u readings",names[state.selected],_table.CachedCount());
    Text(previewX,130,caption,Theme::SourceBlue,26);
}
int ReadingTablePage::RowAt(int x,int y) const
{
    if(x<tableX || x>=780 || y<tableY || y>=tableY+ReadingTableState::Rows*rowH) return -1;
    const int row=_table.Offset()+(y-tableY)/rowH;return row<_table.Count()?row:-1;
}
bool ReadingTablePage::InPreview(int x,int y) const
{return x>=previewX && x<=previewX+previewW && y>=previewY && y<=previewY+previewH;}
void ReadingTablePage::TouchStart(int16_t x,int16_t y)
{
    if(!IsVisible() || !IsEnabled() || _multiBlocked) return;
    _touchX=x;_touchY=y;_moved=false;
    _pressedRow=_popup?-1:RowAt(x,y);_previewPressed=!_popup && InPreview(x,y);
    if(_pressedRow>=0 || _previewPressed) _table.Hold(); // Freeze the data the finger actually touched.
    else Panel::TouchStart(x,y);
}
void ReadingTablePage::TouchMove(int16_t x,int16_t y)
{
    if(abs(x-_touchX)>12 || abs(y-_touchY)>12) _moved=true;
    if(_pressedRow<0 && !_previewPressed) Panel::TouchMove(x,y);
}
void ReadingTablePage::TouchEnd(int16_t x,int16_t y)
{
    if(!IsVisible() || !IsEnabled()) return;
    if(_previewPressed) {
        _previewPressed=false;if(!_moved && InPreview(x,y)) _table.SelectTime((x-previewX)/static_cast<double>(previewW));
        Layout();return;
    }
    if(_pressedRow>=0) {
        const int row=_pressedRow;_pressedRow=-1;
        if(!_moved && RowAt(x,y)==row && _table.Select(row)) {_popup=1;GD.RequestBackdropCapture();Layout();}
        return;
    }
    int id=-1;for(unsigned i=0;i<10;++i) if(_buttons[i].IsVisible() && _buttons[i].IsEnabled() && _buttons[i].IsPressed() && _buttons[i].Contains(x,y)) id=i;
    Panel::TouchEnd(x,y);if(id>=2) Action(id);
}
bool ReadingTablePage::HandleMultiTouch(const TouchFrame& frame,bool allowStart)
{
    if(!_multiBlocked && (!allowStart || !IsVisible() || frame.count<2)) return false;
    if(!_multiBlocked) {_multiBlocked=true;_pressedRow=-1;_previewPressed=false;Panel::TouchEnd(-1,-1);}
    if(frame.count==0) _multiBlocked=false;return true;
}
void ReadingTablePage::DrawPopup()
{
    GD.DrawCapturedBackdrop();GD.DrawModalWindow(0,0,800,480,100,25,600,430,Theme::PopupBackdrop,Theme::PopupBorder,Theme::PopupBackground);
    if(_popup==2) {
        Text(400,53,"SELECT BUFFER",Theme::HeaderText,Theme::FontPopupTitle,OPT_CENTERX);
        Text(146,270,"Only the active measurement buffer exists.",Theme::ToolbarText,26);
    } else {
        const auto index=static_cast<uint16_t>(_table.Selected());char text[80];
        char indexText[21];DisplayFormat::FormatUnsigned(_table.Index(index),indexText,sizeof(indexText));
        snprintf(text,sizeof(text),"READING %s",indexText);
        Text(400,53,text,Theme::HeaderText,Theme::FontPopupTitle,OPT_CENTERX);
        Text(146,110,"Time [s]:",Theme::SourceBlue,28);Time(_table.TimeMs(index),text,sizeof(text));Text(328,110,text,Theme::ValueGreen,28);
        for(unsigned q=0;q<4;++q) {
            Text(146,158+q*42,names[q],Theme::SourceBlue,28);
            Value(_table.Sample(index),static_cast<GraphState::Quantity>(q),text,sizeof(text));Text(328,158+q*42,text,Theme::ValueGreen,28);
        }
        Text(146,322,"Held page; acquisition continues.",Theme::ToolbarText,26);
    }
    Panel::Draw();
}
