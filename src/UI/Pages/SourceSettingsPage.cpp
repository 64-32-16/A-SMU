#include "SourceSettingsPage.h"
#include "MenuPageStyle.h"
#include "../Core/DisplayFormat.h"
#include <stdio.h>

SourceSettingsPage::SourceSettingsPage(int16_t x,int16_t y,int16_t w,int16_t h):Panel(x,y,w,h) {
    SetShowBackground(false);SetShowTitle(false);SetShowBorder(false);SetVisible(false);
    for(auto& button:_buttons) {
        button.SetFont(MenuPageStyle::TextFont);
        button.SetBackgroundColor(Theme::MainButtonBackground);button.SetTextColor(Theme::MainButtonText);
        AddChild(&button);
    }
    _buttons[0].SetText("BACK");_buttons[1].SetText("HOME");Layout();
}
void SourceSettingsPage::Open() {Panel::TouchEnd(-1,-1);SetVisible(true);Layout();}
void SourceSettingsPage::Layout() {
    const int x=GetX(),y=GetY();
    MenuPageStyle::Navigation(_buttons[0],_buttons[1],x,y);
    for(unsigned i=0;i<4;++i)
        _buttons[i+2].SetBounds(x+MenuPageStyle::ValueX,y+MenuPageStyle::RowY(i),MenuPageStyle::ValueWidth,MenuPageStyle::ValueHeight);
    const bool voltage=_system->GetSourceMode()==SourceMode::Voltage;
    _buttons[2].SetText(voltage?"Voltage":"Current");
    const bool automatic=(voltage?_system->GetVoltageRangeMode():_system->GetCurrentRangeMode())==RangeMode::Auto;
    _buttons[3].SetText(automatic?"Auto":voltage?_system->GetVoltageRangeText():_system->GetCurrentRangeText());
    DisplayFormatState format;char value[32],unit[12];
    DisplayFormat::FormatValue(voltage?DisplayQuantity::Current:DisplayQuantity::Voltage,
        voltage?_system->GetCurrentLimitValue():_system->GetVoltageLimitValue(),format,false,value,sizeof(value),unit,sizeof(unit));
    snprintf(_limitCaption,sizeof(_limitCaption),"%s %s",value,unit);_buttons[4].SetText(_limitCaption);
    _buttons[5].SetText(_system->GetOutputOffStateText());
}
void SourceSettingsPage::TouchEnd(int16_t x,int16_t y) {
    if(!IsVisible() || !IsEnabled()) return;
    int selected=-1;
    for(unsigned i=2;i<6;++i)
        if(_buttons[i].IsVisible() && _buttons[i].IsEnabled() && _buttons[i].IsPressed() && _buttons[i].Contains(x,y)) selected=i-2;
    Panel::TouchEnd(x,y);
    if(selected>=0 && _onSetting) {ClearFocus();_onSetting(static_cast<Setting>(selected));}
}
void SourceSettingsPage::Draw() {
    if(!IsVisible()) return;
    Layout();const int x=GetX(),y=GetY();
    MenuPageStyle::Header("SOURCE SETTINGS",x,y);
    const bool voltage=_system->GetSourceMode()==SourceMode::Voltage;
    const char* labels[]={"Source Function","Source Range",voltage?"Current Limit":"Voltage Limit","Output Off State"};
    const char* hints[]={"Voltage or current","Auto or manual",voltage?"Current compliance":"Voltage compliance","When output is off"};
    for(unsigned i=0;i<4;++i) {
        GD.ColorRGB(Theme::SourceBlue);
        GD.cmd_text(x+MenuPageStyle::LabelX,y+MenuPageStyle::RowY(i),MenuPageStyle::TextFont,OPT_RIGHTX,labels[i]);
        GD.ColorRGB(Theme::ToolbarText);
        GD.cmd_text(x+MenuPageStyle::LabelX,y+MenuPageStyle::RowY(i)+31,MenuPageStyle::HintFont,OPT_RIGHTX,hints[i]);
    }
    GD.ColorRGB(Theme::ToolbarText);
    GD.cmd_text(x+400,y+385,MenuPageStyle::HintFont,OPT_CENTERX,"Shared with Main; changes apply immediately.");
    GD.ColorRGB(Theme::FocusYellow);
    GD.cmd_text(x+784,y+445,MenuPageStyle::HintFont,OPT_RIGHTX|OPT_CENTERY,_system->GetBackendText());
    Panel::Draw();
}
