#include "MeasureSettingsPage.h"
#include "MenuPageStyle.h"
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
#include <stdio.h>

namespace {
void Text(int x,int y,const char* text,uint32_t color=Theme::ToolbarText,uint8_t font=26,uint16_t options=0) {
    GD.ColorRGB(color);GD.cmd_text(x,y,font,options,text);
}
void Style(Button& b) {
    b.SetFont(28);b.SetBackgroundColor(Theme::MainButtonBackground);
    b.SetTextColor(Theme::MainButtonText);
}
void Field(int right,int y,const char* title,const char* hint) {
    Text(right,y,title,Theme::SourceBlue,28,OPT_RIGHTX);
    Text(right,y+31,hint,Theme::ToolbarText,26,OPT_RIGHTX);
}
}
MeasureSettingsPage::MeasureSettingsPage(int16_t x,int16_t y,int16_t w,int16_t h):Panel(x,y,w,h) {
    SetShowBackground(false);SetShowTitle(false);SetShowBorder(false);SetVisible(false);
    Style(_back);_back.SetText("BACK");AddChild(&_back);
    Style(_home);_home.SetText("HOME");AddChild(&_home);
    for(auto& b:_choices) {Style(b);AddChild(&b);}
    for(auto& b:_values) {Style(b);AddChild(&b);}
    Style(_cancel);_cancel.SetText("Cancel");AddChild(&_cancel);
    Layout();
}
void MeasureSettingsPage::Open() {Panel::TouchEnd(-1,-1);_editor=Editor::None;SetVisible(true);Layout();}
void MeasureSettingsPage::Layout() {
    const bool editing=_editor!=Editor::None;
    MenuPageStyle::Navigation(_back,_home);
    _back.SetVisible(!editing);_home.SetVisible(!editing);
    _cancel.SetVisible(editing);_cancel.SetBounds(320,379,160,48);
    _cancel.SetBackgroundColor(Theme::ToolbarButtonBackground);_cancel.SetTextColor(Theme::ToolbarButtonText);
    snprintf(_valueCaptions[0],sizeof(_valueCaptions[0]),"%.3g",MeasurementTiming::Profiles[_system->GetNplcProfile()].nplc);
    snprintf(_valueCaptions[1],sizeof(_valueCaptions[1]),"%s",_system->GetVoltageRangeMode()==RangeMode::Auto?"Auto":_system->GetVoltageRangeText());
    snprintf(_valueCaptions[2],sizeof(_valueCaptions[2]),"%s",_system->GetCurrentRangeMode()==RangeMode::Auto?"Auto":_system->GetCurrentRangeText());
    for(unsigned i=0;i<3;++i)
        _values[i].SetBounds(MenuPageStyle::ValueX,MenuPageStyle::RowY(i==0?2:i-1),MenuPageStyle::ValueWidth,MenuPageStyle::ValueHeight);
    for(unsigned i=0;i<3;++i) {_values[i].SetText(_valueCaptions[i]);_values[i].SetVisible(!editing);}
    const unsigned count=editing?MeasurementTiming::Count:0;
    for(unsigned i=0;i<MeasurementTiming::Count;++i) {
        auto& b=_choices[i];b.SetVisible(i<count);b.SetEnabled(true);bool selected=false;
        if(i>=count) continue;
        selected=i==_system->GetNplcProfile();
        snprintf(_captions[i],sizeof(_captions[i]),"%.3g NPLC",MeasurementTiming::Profiles[i].nplc);
        b.SetText(_captions[i]);b.SetBounds(120+(i%3)*190,157+(i/3)*100,180,54);
        b.SetBackgroundColor(selected?Theme::SourceBlue:Theme::ToolbarButtonBackground);
        b.SetTextColor(selected?Theme::FocusYellow:Theme::ToolbarButtonText);
    }
}
void MeasureSettingsPage::Select(unsigned option) {
    if(_editor==Editor::Nplc) _system->SetNplcProfile(option);
    _editor=Editor::None;
}
void MeasureSettingsPage::TouchEnd(int16_t x,int16_t y) {
    if(!IsVisible() || !IsEnabled()) return;
    int selected=-1,field=-1;
    for(unsigned i=0;i<MeasurementTiming::Count;++i)
        if(_choices[i].IsVisible() && _choices[i].IsEnabled() && _choices[i].IsPressed() && _choices[i].Contains(x,y)) selected=i;
    for(unsigned i=0;i<3;++i)
        if(_values[i].IsVisible() && _values[i].IsPressed() && _values[i].Contains(x,y)) field=i;
    const bool cancel=_cancel.IsVisible() && _cancel.IsPressed() && _cancel.Contains(x,y);
    Panel::TouchEnd(x,y);
    if(selected>=0) Select(selected);
    else if(cancel) _editor=Editor::None;
    else if(field==0) {
        _editor=Editor::Nplc;
        ClearFocus();GD.RequestBackdropCapture();
    } else if(field>0 && _onRange) {
        ClearFocus();_onRange(field==1?SourceMode::Voltage:SourceMode::Current);
    }
    Layout();
}
void MeasureSettingsPage::Draw() {
    if(!IsVisible()) return;
    Layout();char text[100];
    if(_editor!=Editor::None) {
        GD.DrawCapturedBackdrop();
        GD.DrawModalWindow(0,0,800,480,100,25,Theme::NumericPopupWidth,Theme::NumericPopupHeight,
                           Theme::PopupBackdrop,Theme::PopupBorder,Theme::PopupBackground);
        Text(400,53,"SELECT NPLC",Theme::HeaderText,29,OPT_CENTERX);
        GD.DrawSolidRoundedRect(128,85,544,56,0,0xF0F0E0);
        GD.DrawSolidRoundedRect(134,91,532,44,0,0x0A0A0A);
        Text(400,113,"50 Hz - higher NPLC = less noise",Theme::ValueGreen,26,OPT_CENTER);
        Panel::Draw();
        if(_editor==Editor::Nplc) {
            for(unsigned i=0;i<MeasurementTiming::Count;++i) {
                const double rate=MeasurementTiming::PairsPerSecond(i);
                snprintf(text,sizeof(text),rate>=10?"~%.0f measurements/s":"~%.1f measurements/s",rate);
                Text(_choices[i].GetX()+90,_choices[i].GetY()+70,text,Theme::ToolbarText,26,OPT_CENTER);
            }
            Text(400,346,"One measurement = one U/I pair (sequential).",Theme::ToolbarText,26,OPT_CENTERX);
        }
        return;
    }
    MenuPageStyle::Header("MEASURE SETTINGS");
    Field(MenuPageStyle::LabelX,MenuPageStyle::RowY(0),"Voltage Range","Auto or manual");
    Field(MenuPageStyle::LabelX,MenuPageStyle::RowY(1),"Current Range","Auto or manual");
    const auto index=_system->GetNplcProfile();const double rate=MeasurementTiming::PairsPerSecond(index);
    snprintf(text,sizeof(text),rate>=10?"~%.0f measurements/s":"~%.1f measurements/s",rate);
    Field(MenuPageStyle::LabelX,MenuPageStyle::RowY(2),"NPLC",text);
    Field(MenuPageStyle::LabelX,MenuPageStyle::RowY(3),"Time / Channel","Equivalent filter time");
    snprintf(text,sizeof(text),"~%.3g ms",MeasurementTiming::Profiles[index].channelTimeUs/1000.0);
    Text(MenuPageStyle::ValueX,MenuPageStyle::RowY(3),text,Theme::ToolbarText,MenuPageStyle::TextFont);
    snprintf(text,sizeof(text),"Line Frequency: %u Hz   |   Averaging: Off   |   Function: %s",
             MeasurementTiming::LineFrequencyHz,_system->GetMeasureModeText());
    Text(400,385,text,Theme::ToolbarText,26,OPT_CENTERX);
    Text(400,415,"One measurement = one U/I pair, measured in sequence.",Theme::ToolbarText,26,OPT_CENTERX);
    Text(400,445,"Nominal rates; extra settling reduces speed.",Theme::ToolbarText,26,OPT_CENTERX);
    Panel::Draw();
}
