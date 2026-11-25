#include "SystemSettingsPage.h"
#include "MenuPageStyle.h"
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
#include <stdio.h>
#include <math.h>
namespace {
constexpr int SliderX=160,SliderY=209,SliderWidth=480,SliderHeight=40;
void Text(int x,int y,const char* text,uint32_t color=Theme::ToolbarText,int font=26,uint16_t options=0) {
    GD.ColorRGB(color);GD.cmd_text(x,y,font,options,text);
}
}
SystemSettingsPage::SystemSettingsPage(int16_t x,int16_t y,int16_t w,int16_t h):Panel(x,y,w,h) {
    SetShowBackground(false);SetShowBorder(false);SetShowTitle(false);SetVisible(false);
    const char* labels[]={"BACK","HOME","ON","","","-","+","Done"};
    for(unsigned i=0;i<8;++i) {
        auto& b=_buttons[i];b.SetText(labels[i]);b.SetFont(28);
        b.SetBackgroundColor(Theme::MainButtonBackground);b.SetTextColor(Theme::MainButtonText);AddChild(&b);
    }
    AddChild(&_number);
    _dateTime.SetText("Unavailable");_dateTime.SetFont(28);_dateTime.SetEnabled(false);
    _dateTime.SetDisabledBackgroundColor(0x303030);_dateTime.SetDisabledTextColor(0x909090);
    AddChild(&_dateTime);Layout();
}
void SystemSettingsPage::Open() {Panel::TouchEnd(-1,-1);_number.Close();_sliderOpen=_dragging=false;SetVisible(true);Layout();}
void SystemSettingsPage::Layout() {
    for(unsigned i=0;i<8;++i) _buttons[i].SetVisible(!_number.IsVisible() && (i<5?!_sliderOpen:_sliderOpen));
    MenuPageStyle::Navigation(_buttons[0],_buttons[1]);
    for(unsigned i=2;i<5;++i) _buttons[i].SetBounds(MenuPageStyle::ValueX,MenuPageStyle::RowY(i-2),MenuPageStyle::ValueWidth,MenuPageStyle::ValueHeight);
    _dateTime.SetBounds(MenuPageStyle::ValueX,MenuPageStyle::RowY(3),MenuPageStyle::ValueWidth,MenuPageStyle::ValueHeight);
    _dateTime.SetVisible(!_number.IsVisible() && !_sliderOpen);
    _buttons[5].SetBounds(160,290,100,50);_buttons[6].SetBounds(540,290,100,50);_buttons[7].SetBounds(320,373,160,50);
    for(unsigned i=5;i<8;++i) {_buttons[i].SetBackgroundColor(Theme::ToolbarButtonBackground);_buttons[i].SetTextColor(Theme::ToolbarButtonText);}
    const auto& settings=_system->GetUserInterfaceSettings();
    _buttons[2].SetText(settings.keyClick?"ON":"OFF");
    snprintf(_backlightText,sizeof(_backlightText),"%u %%",unsigned(settings.backlightPercent));_buttons[3].SetText(_backlightText);
    if(settings.dimmerMinutes) snprintf(_dimmerText,sizeof(_dimmerText),"%u min",unsigned(settings.dimmerMinutes));
    else snprintf(_dimmerText,sizeof(_dimmerText),"OFF");
    _buttons[4].SetText(_dimmerText);
    _buttons[5].SetEnabled(settings.backlightPercent>UserInterfaceSettings::MinBacklight);
    _buttons[6].SetEnabled(settings.backlightPercent<UserInterfaceSettings::MaxBacklight);
}
void SystemSettingsPage::MoveSlider(int x) {
    const int left=SliderX+SliderHeight/2,range=SliderWidth-SliderHeight;
    const int clamped=x<left?0:x>left+range?range:x-left;
    _system->SetBacklightPercent(UserInterfaceSettings::MinBacklight+
        (clamped*(UserInterfaceSettings::MaxBacklight-UserInterfaceSettings::MinBacklight)+range/2)/range);
    Layout();
}
void SystemSettingsPage::TouchStart(int16_t x,int16_t y) {
    if(!IsVisible() || !IsEnabled()) return;
    if(_number.IsVisible()) {_number.TouchStart(x,y);return;}
    if(_sliderOpen && x>=SliderX-12 && x<=SliderX+SliderWidth+12 && y>=SliderY-16 && y<=SliderY+SliderHeight+16) {
        _dragging=true;MoveSlider(x);return;
    }
    Panel::TouchStart(x,y);
}
void SystemSettingsPage::TouchMove(int16_t x,int16_t y) {
    if(!IsVisible() || !IsEnabled()) return;
    if(_number.IsVisible()) _number.TouchMove(x,y);
    else if(_dragging) MoveSlider(x);
    else Panel::TouchMove(x,y);
}
void SystemSettingsPage::TouchEnd(int16_t x,int16_t y) {
    if(!IsVisible() || !IsEnabled()) return;
    if(_number.IsVisible()) {_number.TouchEnd(x,y);Layout();return;}
    if(_dragging) {_dragging=false;return;}
    int id=-1;
    for(unsigned i=0;i<8;++i) if(_buttons[i].IsVisible() && _buttons[i].IsEnabled() && _buttons[i].IsPressed() && _buttons[i].Contains(x,y)) id=i;
    Panel::TouchEnd(x,y);
    const auto& settings=_system->GetUserInterfaceSettings();
    if(id==2) _system->SetKeyClick(!settings.keyClick);
    else if(id==3) {_sliderOpen=true;ClearFocus();GD.RequestBackdropCapture();}
    else if(id==4) {
        char title[80];snprintf(title,sizeof(title),"DIMMER [min] 0=OFF, max %u",UserInterfaceSettings::MaxDimmerMinutes);
        _number.Open(title,settings.dimmerMinutes,AcceptDimmer,this);
    }
    else if(id==5) _system->SetBacklightPercent(settings.backlightPercent-1);
    else if(id==6) _system->SetBacklightPercent(settings.backlightPercent+1);
    else if(id==7) _sliderOpen=false;
    Layout();
}
bool SystemSettingsPage::AcceptDimmer(void* context,double value) {
    if(!isfinite(value) || value<0 || value>UserInterfaceSettings::MaxDimmerMinutes || value!=floor(value)) return false;
    return static_cast<SystemSettingsPage*>(context)->_system->SetDimmerMinutes(static_cast<unsigned>(value));
}
void SystemSettingsPage::Draw() {
    if(!IsVisible()) return;
    Layout();if(_number.IsVisible()) {_number.Draw();return;}
    if(_sliderOpen) {
        GD.DrawCapturedBackdrop();GD.DrawModalWindow(0,0,800,480,100,25,Theme::NumericPopupWidth,Theme::NumericPopupHeight,
            Theme::PopupBackdrop,Theme::PopupBorder,Theme::PopupBackground);
        Text(400,53,"BACKLIGHT",Theme::HeaderText,29,OPT_CENTERX);
        GD.DrawSolidRoundedRect(128,85,544,56,0,0xF0F0E0);GD.DrawSolidRoundedRect(134,91,532,44,0,0x0A0A0A);
        Text(400,113,_backlightText,Theme::ValueGreen,31,OPT_CENTER);
        Text(400,160,"Drag to adjust - changes apply immediately",Theme::ToolbarText,26,OPT_CENTERX);
        const auto percent=_system->GetUserInterfaceSettings().backlightPercent;
        GD.DrawSlider(SliderX,SliderY,SliderWidth,SliderHeight,
            (percent-UserInterfaceSettings::MinBacklight)*100/(UserInterfaceSettings::MaxBacklight-UserInterfaceSettings::MinBacklight),
            Theme::StatusMuted,Theme::SourceBlue,Theme::ToolbarText);
        char label[20];snprintf(label,sizeof(label),"%u %%",UserInterfaceSettings::MinBacklight);Text(SliderX,261,label);
        snprintf(label,sizeof(label),"%u %%",UserInterfaceSettings::MaxBacklight);Text(SliderX+SliderWidth,261,label,Theme::ToolbarText,26,OPT_RIGHTX);
        Panel::Draw();return;
    }
    MenuPageStyle::Header("SYSTEM SETTINGS");
    const char* labels[]={"KeyClick","Backlight","Backlight Dimmer"};
    char range[32];snprintf(range,sizeof(range),"%u to %u %%",UserInterfaceSettings::MinBacklight,UserInterfaceSettings::MaxBacklight);
    const char* hints[]={"Touch feedback",range,"Inactivity time; 0 = OFF"};
    for(unsigned i=0;i<3;++i) {Text(MenuPageStyle::LabelX,MenuPageStyle::RowY(i),labels[i],Theme::SourceBlue,MenuPageStyle::TextFont,OPT_RIGHTX);Text(MenuPageStyle::LabelX,MenuPageStyle::RowY(i)+31,hints[i],Theme::ToolbarText,MenuPageStyle::HintFont,OPT_RIGHTX);}
    Text(394,314,"Date / Time",Theme::SourceBlue,28,OPT_RIGHTX);
    Text(394,345,"RTC not connected",Theme::ToolbarText,26,OPT_RIGHTX);
    char hint[90];snprintf(hint,sizeof(hint),"Dims to %u %%. First touch wakes the display only.",UserInterfaceSettings::MinBacklight);
    Text(400,385,hint,Theme::ToolbarText,26,OPT_CENTERX);Panel::Draw();
}
