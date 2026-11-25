#include "HistogramPage.h"
#include "../Core/GDClass.h"
#include "../Core/GraphState.h"
#include "../Core/Theme.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
namespace {
constexpr int px=84,py=112,pw=688,ph=224;
const char* names[]={"Voltage","Current","Power","Resistance"};
const char* units[]={"V","A","W","Ohm"};
void Text(int x,int y,const char* text,uint32_t color=0xF2F2F2,uint8_t font=28,uint16_t options=0)
{GD.ColorRGB(color);GD.cmd_text(x,y,font,options,text);}
void Format(double value,const char* unit,char* text,size_t n) {GraphState::FormatValue(value,unit,text,n);}
}
HistogramPage::HistogramPage(int16_t x,int16_t y,int16_t w,int16_t h):Panel(x,y,w,h),_system(&System)
{
    SetShowBackground(false);SetShowBorder(false);SetShowTitle(false);
    for(auto& button:_buttons) {
        button.SetFont(28);button.SetBackgroundColor(Theme::MainButtonBackground);button.SetTextColor(Theme::MainButtonText);
        AddChild(&button);
    }
    _tabs.AddTab("Histogram");_tabs.AddTab("Data");_tabs.AddTab("Scale");_tabs.SetBounds(8,4,480,44);
    AddChild(&_tabs);AddChild(&_editor);SetVisible(false);
}
void HistogramPage::SetSystem(SystemClass* system) {_system=system?system:&System;}
void HistogramPage::SetOnBack(ButtonClickHandler handler) {_buttons[0].SetOnClick(handler);}
void HistogramPage::SetOnHome(ButtonClickHandler handler) {_buttons[1].SetOnClick(handler);}
void HistogramPage::Open() {Panel::TouchEnd(-1,-1);_editor.Close();_popup=0;_pressedBin=-1;SetVisible(true);Refresh();Layout();}
void HistogramPage::Refresh()
{
    CalculateHistogram(_system->Buffer.View(),_system->GetHistogramSettings(),_result);
}
void HistogramPage::Configure(unsigned id,const char* text,int x,int y,int w,int h)
{
    snprintf(_captions[id],sizeof(_captions[id]),"%s",text);_buttons[id].SetText(_captions[id]);
    _buttons[id].SetBounds(x,y,w,h);_buttons[id].SetVisible(true);
}
void HistogramPage::Layout()
{
    for(auto& b:_buttons) {b.SetVisible(false);b.SetBackgroundColor(Theme::MainButtonBackground);}
    _tabs.SetVisible(!_popup && !_editor.IsVisible());
    if(_editor.IsVisible()) return;
    if(_popup) {
        Configure(18,"Close",530,347,140,58);
        if(_popup==2) {Configure(19,"Active Buffer",160,160,480,58);_buttons[19].SetBackgroundColor(Theme::SourceBlue);}
        return;
    }
    Configure(0,"BACK",8,52,80,34);Configure(1,"HOME",96,52,80,34);Configure(2,"AUTO SCALE",650,8,142,36);
    const auto& s=_system->GetHistogramSettings();
    for(unsigned i=0;i<4;++i) {Configure(3+i,names[i],192+i*146,52,138,34);
        if(i==static_cast<unsigned>(s.quantity)) _buttons[3+i].SetBackgroundColor(Theme::SourceBlue);}
    char text[64],value[32];
    if(_tabs.GetSelectedIndex()==1) {
        Configure(9,"Buffer: Active",24,150,350,48);
        Configure(7,s.enabled?"Remove source":"Add source",24,230,350,48);
        Configure(8,"CLEAR BUFFER",410,230,366,48);
    }
    else if(_tabs.GetSelectedIndex()==2) {
        Format(_result.minimum,units[static_cast<unsigned>(s.quantity)],value,sizeof(value));
        snprintf(text,sizeof(text),"Minimum: %s",value);Configure(10,text,24,126,350,48);
        Format(_result.maximum,units[static_cast<unsigned>(s.quantity)],value,sizeof(value));
        snprintf(text,sizeof(text),"Maximum: %s",value);Configure(11,text,410,126,366,48);
        snprintf(text,sizeof(text),"Number of bins: %u",s.bins);Configure(12,text,24,194,350,48);
        snprintf(text,sizeof(text),"Y maximum: %u",_result.yMaximum);Configure(17,text,410,194,366,48);
        const char* methods[]={"SmartScale","Auto Bin","Fit","Off"};
        for(unsigned i=0;i<4;++i) {Configure(13+i,methods[i],24+i*192,302,176,48);
            if(i==static_cast<unsigned>(s.method)) _buttons[13+i].SetBackgroundColor(Theme::SourceBlue);}
    }
}
void HistogramPage::Action(unsigned id)
{
    auto s=_system->GetHistogramSettings();
    if(id==18 || id==19) {_popup=0;Layout();return;}
    if(id==9) {_popup=2;GD.RequestBackdropCapture();Layout();return;}
    if(id>=10 && id<=12) {Edit(id-10);return;}
    if(id==17) {Edit(3);return;}
    if(id==2) s.method=HistogramScale::SmartScale;
    else if(id>=3 && id<=6) {s.quantity=static_cast<BufferQuantity>(id-3);s.method=HistogramScale::SmartScale;}
    else if(id==7) s.enabled=!s.enabled;
    else if(id==8) {_system->ClearMeasurementBuffer();Refresh();Layout();return;}
    else if(id>=13 && id<=16) {
        s.minimum=_result.minimum;s.maximum=_result.maximum;s.yMaximum=_result.yMaximum;
        s.method=static_cast<HistogramScale>(id-13);
    }
    _system->SetHistogramSettings(s);Refresh();Layout();
}
void HistogramPage::Edit(unsigned field)
{
    _field=field;const char* labels[]={"MINIMUM","MAXIMUM","NUMBER OF BINS","Y MAXIMUM"};
    char title[80];snprintf(title,sizeof(title),"SET %s [%s]",labels[field],field<2?units[static_cast<unsigned>(_system->GetHistogramSettings().quantity)]:"count");
    const double value=field==0?_result.minimum:field==1?_result.maximum:field==2?_result.bins:_result.yMaximum;
    _editor.Open(title,value,AcceptValue,this);Layout();
}
bool HistogramPage::AcceptValue(void* context,double value)
{
    auto& page=*static_cast<HistogramPage*>(context);auto s=page._system->GetHistogramSettings();
    s.minimum=page._result.minimum;s.maximum=page._result.maximum;s.yMaximum=page._result.yMaximum;
    if(page._field==0) {s.minimum=value;s.method=HistogramScale::Fit;}
    else if(page._field==1) {s.maximum=value;s.method=HistogramScale::Fit;}
    else if(page._field==2) {if(value<1 || value>HistogramSettings::MaxBins || floor(value)!=value) return false;s.bins=static_cast<uint16_t>(value);}
    else {if(value<1 || value>BufferClass::Capacity || floor(value)!=value) return false;s.yMaximum=static_cast<uint16_t>(value);s.method=HistogramScale::Off;}
    return page._system->SetHistogramSettings(s);
}
int HistogramPage::BinAtPoint(int x,int y) const
{
    if(x<px || x>=px+pw || y<py || y>py+ph) return -1;
    return static_cast<int32_t>(x-px)*(_result.bins+2)/pw;
}
void HistogramPage::TouchStart(int16_t x,int16_t y)
{
    if(!IsVisible() || !IsEnabled() || _multiBlocked) return;
    if(_editor.IsVisible()) {_editor.TouchStart(x,y);return;}
    _touchX=x;_touchY=y;_moved=false;
    _pressedBin=(!_popup && _tabs.GetSelectedIndex()==0 && _system->GetHistogramSettings().enabled)?BinAtPoint(x,y):-1;
    if(_pressedBin<0) Panel::TouchStart(x,y);
}
void HistogramPage::TouchMove(int16_t x,int16_t y)
{
    if(_editor.IsVisible()) {_editor.TouchMove(x,y);return;}
    if(abs(x-_touchX)>12 || abs(y-_touchY)>12) _moved=true;
    if(_pressedBin<0) Panel::TouchMove(x,y);
}
void HistogramPage::TouchEnd(int16_t x,int16_t y)
{
    if(!IsVisible() || !IsEnabled()) return;
    if(_editor.IsVisible()) {_editor.TouchEnd(x,y);Layout();return;}
    if(_pressedBin>=0) {
        const int bin=_pressedBin;_pressedBin=-1;
        if(!_moved && BinAtPoint(x,y)==bin && _result.counts[bin]) {
            _selectedBin=bin;_popup=1;GD.RequestBackdropCapture();Layout();
        }
        return;
    }
    int id=-1;for(unsigned i=0;i<20;++i) if(_buttons[i].IsVisible() && _buttons[i].IsPressed() && _buttons[i].Contains(x,y)) id=i;
    Panel::TouchEnd(x,y);if(id>=2) Action(id);
}
bool HistogramPage::HandleMultiTouch(const TouchFrame& frame,bool allowStart)
{
    if(!_multiBlocked && (!allowStart || !IsVisible() || frame.count<2)) return false;
    if(!_multiBlocked) {_multiBlocked=true;_pressedBin=-1;Panel::TouchEnd(-1,-1);}
    if(frame.count==0) _multiBlocked=false;
    return true;
}
void HistogramPage::Draw()
{
    if(!IsVisible()) return;
    if(_editor.IsVisible()) {_editor.Draw();return;}
    if(_popup) {Layout();DrawPopup();return;}
    Refresh();Layout();
    char status[48];snprintf(status,sizeof(status),"%s %u",_system->GetBackendText(),_system->Buffer.GetCount());
    Text(500,17,status,Theme::ToolbarText,26);
    GD.SaveContext();GD.VertexFormat(0);GD.ColorRGB(Theme::TabActive);GD.LineWidth(32);GD.Begin(LINES);GD.Vertex2f(8,46);GD.Vertex2f(792,46);GD.RestoreContext();
    if(_tabs.GetSelectedIndex()==0) DrawHistogram();else DrawSettings();
    Panel::Draw();
}
void HistogramPage::DrawHistogram()
{
    GD.DrawPlotGrid(px,py,pw,ph,5,4,0x303030);
    GD.DrawHistogramBars(px,py,pw,ph,_result.counts,_result.bins+2,_result.yMaximum,0x40FF60);
    char text[160],a[32],b[32],c[32];const auto& s=_system->GetHistogramSettings();const char* unit=units[static_cast<unsigned>(s.quantity)];
    Text(12,88,"Count",Theme::ToolbarText,26);
    for(unsigned i=0;i<=4;++i) {snprintf(text,sizeof(text),"%.3g",_result.yMaximum*i/4.0);Text(12,py+ph-i*ph/4,text,Theme::ToolbarText,26,OPT_CENTERY);}
    for(unsigned i=0;i<=2;++i) {
        Format(_result.minimum+(_result.maximum-_result.minimum)*i/2,unit,text,sizeof(text));
        const int x=px+pw*(1.0+_result.bins*i/2.0)/(_result.bins+2);
        Text(x,py+ph+18,text,Theme::ToolbarText,26,i==0?OPT_CENTERY:i==2?(OPT_RIGHTX|OPT_CENTERY):OPT_CENTER);
    }
    Text(px,py+ph+18,"<",Theme::SourceBlue,26,OPT_CENTERY);Text(px+pw,py+ph+18,">",Theme::SourceBlue,26,OPT_RIGHTX|OPT_CENTERY);
    if(!s.enabled || !_result.hasData) Text(px+pw/2,py+ph/2,!s.enabled?"No source selected":"No valid samples",Theme::ToolbarText,28,OPT_CENTER);
    snprintf(text,sizeof(text),"Active Buffer | %s | N: %u | Bins: %u (+2)",names[static_cast<unsigned>(s.quantity)],_result.valid,_result.bins);Text(12,384,text,Theme::SourceBlue,26);
    Format(_result.width,unit,a,sizeof(a));Format(_result.minimum,unit,b,sizeof(b));Format(_result.maximum,unit,c,sizeof(c));
    snprintf(text,sizeof(text),"Width: %s | Lower: %s | Upper: %s",a,b,c);Text(12,414,text,Theme::ToolbarText,26);
    if(_result.hasData) {Format(_result.dataMin,unit,a,sizeof(a));Format(_result.dataMax,unit,b,sizeof(b));}
    else {snprintf(a,sizeof(a),"--");snprintf(b,sizeof(b),"--");}
    snprintf(text,sizeof(text),"Data min: %s | Data max: %s | Invalid: %u",a,b,_result.invalid);Text(12,444,text,Theme::ToolbarText,26);
}
void HistogramPage::DrawSettings()
{
    if(_tabs.GetSelectedIndex()==1) {
        Text(24,108,"Select the measurement above, then add or remove its source.",Theme::ToolbarText,26);
        Text(24,320,"One active measurement buffer is currently available.",Theme::SourceBlue,28);
        Text(24,370,"Remove hides the source and keeps all measurements.",Theme::ToolbarText,26);
        Text(24,410,"CLEAR BUFFER deletes the shared data used by Graph and Statistics.",Theme::ToolbarText,26);
    } else {
        Text(24,96,"Bounds use the selected unit; 1 to 64 equal-width bins.",Theme::ToolbarText,26);
        Text(24,262,"Scaling method:",Theme::SourceBlue,28);
        Text(24,370,"SmartScale: select Auto Bin or Fit from the data spread.",Theme::ToolbarText,26);
        Text(24,402,"Auto Bin: data limits + fit. Fit: heights only. Off: fixed axes.",Theme::ToolbarText,26);
        Text(24,434,"Values below / above the limits go into two extra outlier bins.",Theme::ToolbarText,26);
    }
}
void HistogramPage::DrawPopup()
{
    GD.DrawCapturedBackdrop();GD.DrawModalWindow(0,0,800,480,100,25,600,430,Theme::PopupBackdrop,Theme::PopupBorder,Theme::PopupBackground);
    if(_popup==2) {
        Text(400,53,"SELECT BUFFER",Theme::HeaderText,Theme::FontPopupTitle,OPT_CENTERX);
        Text(146,260,"Only the active measurement buffer exists.",Theme::ToolbarText,26);
    } else {
        const char* unit=units[static_cast<unsigned>(_system->GetHistogramSettings().quantity)];char title[64],text[100],a[32],b[32];
        snprintf(title,sizeof(title),_selectedBin==0?"LOW OUTLIER":_selectedBin==_result.bins+1?"HIGH OUTLIER":"BIN %d",_selectedBin);
        Text(400,53,title,Theme::HeaderText,Theme::FontPopupTitle,OPT_CENTERX);
        if(_selectedBin==0) {Format(_result.minimum,unit,a,sizeof(a));snprintf(text,sizeof(text),"Value < %s",a);}
        else if(_selectedBin==_result.bins+1) {Format(_result.maximum,unit,a,sizeof(a));snprintf(text,sizeof(text),"Value > %s",a);}
        else {Format(_result.minimum+(_selectedBin-1)*_result.width,unit,a,sizeof(a));Format(_result.minimum+_selectedBin*_result.width,unit,b,sizeof(b));snprintf(text,sizeof(text),"%s <= value %s %s",a,_selectedBin==_result.bins?"<=":"<",b);}
        Text(146,150,text,Theme::ToolbarText,26);
        snprintf(text,sizeof(text),"Count: %u",_result.counts[_selectedBin]);Text(146,220,text,Theme::ValueGreen,30);
        Text(146,285,"Values shown at the time the bin was selected.",Theme::ToolbarText,26);
    }
    Panel::Draw();
}
