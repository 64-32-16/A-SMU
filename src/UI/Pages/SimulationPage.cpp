#include "SimulationPage.h"
#include "../Core/GDClass.h"
#include "../Core/GraphState.h"
#include "../Core/Theme.h"
#include <stdio.h>
namespace {
const double factors[]={1,1000,1000000};
const char* units[]={"Ohm","kOhm","MOhm"};
void Text(int x,int y,const char* text,uint32_t color=Theme::ToolbarText,uint8_t font=26,uint16_t options=0) {
    GD.ColorRGB(color);GD.cmd_text(x,y,font,options,text);
}
}
SimulationPage::SimulationPage(int16_t x,int16_t y,int16_t w,int16_t h):Panel(x,y,w,h) {
    SetShowBackground(false);SetShowTitle(false);SetShowBorder(false);SetVisible(false);
    for(auto& b:_buttons) {
        b.SetFont(28);b.SetBackgroundColor(Theme::MainButtonBackground);b.SetTextColor(Theme::MainButtonText);
        b.SetDisabledBackgroundColor(0x303030);b.SetDisabledTextColor(0x909090);AddChild(&b);
    }
    AddChild(&_editor);
}
void SimulationPage::Open() {Panel::TouchEnd(-1,-1);_editor.Close();SetVisible(true);Layout();}
void SimulationPage::Configure(unsigned id,const char* text,int x,int y,int w,int h) {
    snprintf(_captions[id],sizeof(_captions[id]),"%s",text);_buttons[id].SetText(_captions[id]);
    _buttons[id].SetBounds(x,y,w,h);
}
void SimulationPage::Layout() {
    for(auto& b:_buttons) {b.SetVisible(!_editor.IsVisible());b.SetEnabled(true);b.SetBackgroundColor(Theme::MainButtonBackground);}
    if(_editor.IsVisible()) return;
    Configure(0,"BACK",8,52,80,36);Configure(1,"HOME",96,52,80,36);
    Configure(2,_system->GetSimulationEnabled()?"Simulation: ON":"Simulation: OFF",400,52,384,36);
    const auto& s=_system->GetSimulationSettings();
    const char* loads[]={"RESISTOR","OPEN","SHORT"};
    for(unsigned i=0;i<3;++i) {Configure(3+i,loads[i],16+i*256,123,240);if(i==static_cast<unsigned>(s.load)) _buttons[3+i].SetBackgroundColor(Theme::SourceBlue);}
    char text[48];snprintf(text,sizeof(text),"R: %.7g %s",s.resistance/factors[_unit],units[_unit]);
    Configure(6,text,16,177,392);
    for(unsigned i=0;i<3;++i) {Configure(7+i,units[i],424+i*120,177,112);if(i==_unit) _buttons[7+i].SetBackgroundColor(Theme::SourceBlue);}
    _buttons[6].SetEnabled(s.load==Hardware::LoadType::Resistor);
    for(unsigned i=0;i<2;++i) {
        const auto r=static_cast<VoltageRange>(i);const bool available=_system->IsVoltageRangeAvailable(r);
        snprintf(text,sizeof(text),"%s: %s",i?"+/-30 V":"+/-5 V",available?"ON":"OFF");
        Configure(10+i,text,16+i*192,257,176);
        _buttons[10+i].SetEnabled(r!=_system->GetVoltageRange());
    }
    for(unsigned i=0;i<4;++i) {
        const auto r=SystemClass::GetCurrentRangeAt(i);const bool available=_system->IsCurrentRangeAvailable(r);
        snprintf(text,sizeof(text),"%s: %s",SystemClass::GetCurrentRangeText(r),available?"ON":"OFF");
        Configure(12+i,text,16+i*192,309,176);
        _buttons[12+i].SetEnabled(r!=_system->GetCurrentRange());
    }
}
void SimulationPage::Action(unsigned id) {
    if(id==2) _system->SetSimulationEnabled(!_system->GetSimulationEnabled());
    else if(id>=3 && id<=5) _system->SetSimulationLoad(static_cast<Hardware::LoadType>(id-3));
    else if(id==6) {
        char title[48];snprintf(title,sizeof(title),"SET RESISTANCE [%s]",units[_unit]);
        _editor.Open(title,_system->GetSimulationSettings().resistance/factors[_unit],AcceptResistance,this);
    }
    else if(id>=7 && id<=9) _unit=id-7;
    else if(id==10 || id==11) {const auto r=static_cast<VoltageRange>(id-10);_system->SetVoltageRangeAvailable(r,!_system->IsVoltageRangeAvailable(r));}
    else if(id>=12 && id<=15) {const auto r=SystemClass::GetCurrentRangeAt(id-12);_system->SetCurrentRangeAvailable(r,!_system->IsCurrentRangeAvailable(r));}
    Layout();
}
bool SimulationPage::AcceptResistance(void* context,double value) {
    auto& page=*static_cast<SimulationPage*>(context);
    return page._system->SetSimulationResistance(value*factors[page._unit]);
}
void SimulationPage::TouchStart(int16_t x,int16_t y) {
    if(!IsVisible() || !IsEnabled()) return;
    if(_editor.IsVisible()) _editor.TouchStart(x,y);else Panel::TouchStart(x,y);
}
void SimulationPage::TouchMove(int16_t x,int16_t y) {
    if(_editor.IsVisible()) _editor.TouchMove(x,y);else Panel::TouchMove(x,y);
}
void SimulationPage::TouchEnd(int16_t x,int16_t y) {
    if(!IsVisible() || !IsEnabled()) return;
    if(_editor.IsVisible()) {_editor.TouchEnd(x,y);Layout();return;}
    int id=-1;
    for(unsigned i=0;i<16;++i) if(_buttons[i].IsEnabled() && _buttons[i].IsPressed() && _buttons[i].Contains(x,y)) id=i;
    Panel::TouchEnd(x,y);if(id>=2) Action(id);
}
void SimulationPage::Draw() {
    if(!IsVisible()) return;
    if(_editor.IsVisible()) {_editor.Draw();return;}
    Layout();
    GD.DrawSolidRoundedRect(8,4,784,40,0,Theme::TabInactive);
    Text(20,10,"SIMULATION",Theme::ToolbarText,29);
    Text(780,26,_system->GetBackendText(),Theme::FocusYellow,28,OPT_RIGHTX|OPT_CENTERY);
    Text(16,95,"Load",Theme::SourceBlue,28);
    Text(16,228,"Available ranges (active range locked)",Theme::SourceBlue,26);
    Text(16,359,"Select active ranges, source and limit on Main.",Theme::ToolbarText,26);
    const auto& m=_system->GetMeasurement();char u[32],i[32],p[32],text[128];
    if(m.Fault==Hardware::Error::None) {
        GraphState::FormatValue(m.Voltage,"V",u,sizeof(u),5);GraphState::FormatValue(m.Current,"A",i,sizeof(i),5);
        GraphState::FormatValue(m.GetPower(),"W",p,sizeof(p),5);
        snprintf(text,sizeof(text),"U: %s    I: %s    P: %s",u,i,p);
    } else snprintf(text,sizeof(text),"U: --    I: --    P: --");
    Text(16,392,text,Theme::ValueGreen,28);
    snprintf(text,sizeof(text),"OUTPUT %s | %s | %s",(m.Flags&Hardware::OutputOn)?"ON":"OFF",
        m.Mode==Hardware::Regulation::CC?"CC":m.Mode==Hardware::Regulation::CV?"CV":"--",_system->GetHardwareStatusText());
    Text(16,434,text,Theme::FocusYellow,26);
    Panel::Draw();
}
