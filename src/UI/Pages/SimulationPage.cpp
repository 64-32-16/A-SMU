#include "SimulationPage.h"
#include "MenuPageStyle.h"
#include "../Core/GDClass.h"
#include "../Core/GraphState.h"
#include "../Core/Theme.h"
#include <stdio.h>
namespace {
constexpr int LabelX=240, ValueX=260, RowTop=74, RowStep=64, FieldHeight=48;
int RowY(unsigned row) {return RowTop+row*RowStep;}
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
    _buttons[0].SetText("BACK");_buttons[1].SetText("HOME");
    MenuPageStyle::Navigation(_buttons[0],_buttons[1]);
    Configure(2,_system->GetSimulationEnabled()?"ON":"OFF",ValueX,RowY(0),MenuPageStyle::ValueWidth,FieldHeight);
    const auto& s=_system->GetSimulationSettings();
    const char* loads[]={"Resistor","Open","Short"};
    for(unsigned i=0;i<3;++i) {
        Configure(3+i,loads[i],ValueX+i*176,RowY(1),168,FieldHeight);
        if(i==static_cast<unsigned>(s.load)) _buttons[3+i].SetBackgroundColor(Theme::SourceBlue);
    }
    char text[48];snprintf(text,sizeof(text),"%.7g",s.resistance/factors[_unit]);
    Configure(6,text,ValueX,RowY(2),MenuPageStyle::ValueWidth,FieldHeight);
    for(unsigned i=0;i<3;++i) {
        Configure(7+i,units[i],ValueX+248+i*92,RowY(2),84,FieldHeight);
        if(i==_unit) _buttons[7+i].SetBackgroundColor(Theme::SourceBlue);
    }
    _buttons[6].SetEnabled(s.load==Hardware::LoadType::Resistor);
    for(unsigned i=0;i<SystemClass::VoltageRangeCount;++i) {
        const auto r=SystemClass::GetVoltageRangeAt(i);const bool available=_system->IsVoltageRangeAvailable(r);
        snprintf(text,sizeof(text),"%s: %s",SystemClass::GetVoltageRangeText(r),available?"ON":"OFF");
        Configure(10+i,text,ValueX+i*176,RowY(3),168,FieldHeight);
        _buttons[10+i].SetEnabled(r!=_system->GetVoltageRange());
    }
    for(unsigned i=0;i<SystemClass::CurrentRangeCount;++i) {
        const auto r=SystemClass::GetCurrentRangeAt(i);const bool available=_system->IsCurrentRangeAvailable(r);
        // Range names stay readable; availability is shown by ON/OFF below each button.
        Configure(12+i,SystemClass::GetCurrentRangeText(r),ValueX+i*132,RowY(4),124,36);
        _buttons[12+i].SetBackgroundColor(available?Theme::SourceBlue:Theme::MainButtonBackground);
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
    MenuPageStyle::Header("SIMULATION");
    const char* labels[]={"Simulation","Load","Resistance","V ranges","I ranges"};
    const char* hints[]={"Virtual device","Select test load","Resistor load only","Available ranges","Active range locked"};
    for(unsigned row=0;row<5;++row) {
        Text(LabelX,RowY(row),labels[row],Theme::SourceBlue,MenuPageStyle::TextFont,OPT_RIGHTX);
        Text(LabelX,RowY(row)+29,hints[row],Theme::ToolbarText,MenuPageStyle::HintFont,OPT_RIGHTX);
    }
    Text(784,RowY(0)+24,_system->GetBackendText(),Theme::FocusYellow,28,OPT_RIGHTX|OPT_CENTERY);
    for(unsigned i=0;i<SystemClass::CurrentRangeCount;++i) {
        const bool available=_system->IsCurrentRangeAvailable(SystemClass::GetCurrentRangeAt(i));
        Text(ValueX+i*132+62,RowY(4)+38,available?"ON":"OFF",Theme::ToolbarText,MenuPageStyle::TextFont,OPT_CENTERX);
    }
    const auto& m=_system->GetMeasurement();char u[32],i[32],p[32],text[128];
    if(m.Fault==Hardware::Error::None) {
        GraphState::FormatValue(m.Voltage,"V",u,sizeof(u),5);GraphState::FormatValue(m.Current,"A",i,sizeof(i),5);
        GraphState::FormatValue(m.GetPower(),"W",p,sizeof(p),5);
        snprintf(text,sizeof(text),"U: %s    I: %s    P: %s",u,i,p);
    } else snprintf(text,sizeof(text),"U: --    I: --    P: --");
    Text(16,403,text,Theme::ValueGreen,28);
    snprintf(text,sizeof(text),"OUTPUT %s | %s | %s",(m.Flags&Hardware::OutputOn)?"ON":"OFF",
        m.Mode==Hardware::Regulation::CC?"CC":m.Mode==Hardware::Regulation::CV?"CV":"--",_system->GetHardwareStatusText());
    Text(16,444,text,Theme::FocusYellow,26);
    Panel::Draw();
}
