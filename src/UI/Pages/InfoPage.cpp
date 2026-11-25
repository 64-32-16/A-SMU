#include "InfoPage.h"
#include "MenuPageStyle.h"
#include "../../Core/System.h"
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
#include <stdio.h>
#ifdef ARDUINO
#include "BuildInfo.h"
#else
#define ASMU_BUILD_TIMESTAMP "Host test build"
#endif

namespace {
void Text(int x,int y,const char* text,uint32_t color=Theme::ToolbarText,int font=26,uint16_t options=0) {
    GD.ColorRGB(color);GD.cmd_text(x,y,font,options,text);
}
}
InfoPage::InfoPage(int16_t x,int16_t y,int16_t w,int16_t h):Panel(x,y,w,h) {
    SetShowBackground(false);SetShowBorder(false);SetShowTitle(false);SetVisible(false);
    _back.SetText("BACK");_home.SetText("HOME");
    Button* buttons[]={&_back,&_home};
    for(auto* button : buttons) {
        button->SetFont(28);button->SetBackgroundColor(Theme::MainButtonBackground);
        button->SetTextColor(Theme::MainButtonText);AddChild(button);
    }
    MenuPageStyle::Navigation(_back,_home,x,y);
}
void InfoPage::Open() {Panel::TouchEnd(-1,-1);SetVisible(true);}
void InfoPage::Draw() {
    if(!IsVisible()) return;
    const int x=GetX(),y=GetY();
    MenuPageStyle::Header("INFO",x,y);
    const char* labels[]={"Device","V measurement ranges","I measurement ranges","Firmware build"};
    const char* hints[]={"Source Measure Unit","Bipolar ranges","Bipolar ranges","UTC"};
    for(unsigned i=0;i<4;++i) {
        Text(x+MenuPageStyle::LabelX,y+MenuPageStyle::RowY(i),labels[i],Theme::SourceBlue,MenuPageStyle::TextFont,OPT_RIGHTX);
        Text(x+MenuPageStyle::LabelX,y+MenuPageStyle::RowY(i)+31,hints[i],Theme::ToolbarText,MenuPageStyle::HintFont,OPT_RIGHTX);
    }
    Text(x+MenuPageStyle::ValueX,y+MenuPageStyle::RowY(0),"A-SMU",Theme::ToolbarText,MenuPageStyle::TextFont);
    Text(x+MenuPageStyle::ValueX,y+MenuPageStyle::RowY(0)+32,"Hobby SMU",Theme::ToolbarText,MenuPageStyle::TextFont);
    char label[32];
    for(uint8_t i=0;i<SystemClass::VoltageRangeCount;++i) {
        snprintf(label,sizeof(label),"+/- %s",SystemClass::GetVoltageRangeText(SystemClass::GetVoltageRangeAt(i)));
        Text(x+MenuPageStyle::ValueX+(i%2)*180,y+MenuPageStyle::RowY(1)+(i/2)*32,label,Theme::ToolbarText,MenuPageStyle::TextFont);
    }
    for(uint8_t i=0;i<SystemClass::CurrentRangeCount;++i) {
        snprintf(label,sizeof(label),"+/- %s",SystemClass::GetCurrentRangeText(SystemClass::GetCurrentRangeAt(i)));
        Text(x+MenuPageStyle::ValueX+(i%2)*180,y+MenuPageStyle::RowY(2)+(i/2)*32,label,Theme::ToolbarText,MenuPageStyle::TextFont);
    }
#ifdef ARDUINO
    // Split date and time to retain the same readable font as the labels.
    snprintf(label,sizeof(label),"%.10s",ASMU_BUILD_TIMESTAMP);
    Text(x+MenuPageStyle::ValueX,y+MenuPageStyle::RowY(3),label,Theme::ToolbarText,MenuPageStyle::TextFont);
    Text(x+MenuPageStyle::ValueX,y+MenuPageStyle::RowY(3)+32,ASMU_BUILD_TIMESTAMP+11,Theme::ToolbarText,MenuPageStyle::TextFont);
#else
    Text(x+MenuPageStyle::ValueX,y+MenuPageStyle::RowY(3),ASMU_BUILD_TIMESTAMP,Theme::ToolbarText,MenuPageStyle::TextFont);
#endif
    Text(x+768,y+444,System.GetBackendText(),Theme::FocusYellow,26,OPT_RIGHTX|OPT_CENTERY);
    Panel::Draw();
}
