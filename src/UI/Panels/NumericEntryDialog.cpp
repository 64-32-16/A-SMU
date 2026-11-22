#include "NumericEntryDialog.h"
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
namespace {const char* keys[]={"0","1","2","3","4","5","6","7","8","9",".","-","E","Back","Clear","OK","Cancel"};}
NumericEntryDialog::NumericEntryDialog():Panel(0,0,800,480)
{
    SetShowBackground(false);SetShowBorder(false);SetShowTitle(false);SetVisible(false);
    const int x=100,y=25;
    for(unsigned i=0;i<17;++i) {
        _keys[i].SetText(keys[i]);_keys[i].SetFont(Theme::FontPopupButton);
        _keys[i].SetBackgroundColor(Theme::ToolbarButtonBackground);_keys[i].SetTextColor(Theme::ToolbarButtonText);
        AddChild(&_keys[i]);
    }
    for(unsigned i=1;i<=9;++i) _keys[i].SetBounds(x+30+((i-1)%3)*94,y+124+((i-1)/3)*66,86,58);
    _keys[0].SetBounds(x+30,y+322,180,58);_keys[10].SetBounds(x+312,y+322,86,58);
    _keys[11].SetBounds(x+218,y+322,86,58);_keys[12].SetBounds(x+312,y+124,86,58);
    _keys[13].SetBounds(x+430,y+124,140,58);_keys[14].SetBounds(x+430,y+190,140,58);
    _keys[15].SetBounds(x+430,y+322,140,58);_keys[16].SetBounds(x+430,y+256,140,58);
}
void NumericEntryDialog::Open(const char* title,double value,AcceptHandler accept,void* context)
{
    Panel::TouchEnd(-1,-1);snprintf(_title,sizeof(_title),"%s",title);snprintf(_input,sizeof(_input),"%.8g",value);
    _accept=accept;_context=context;_replace=true;_invalid=false;SetVisible(true);ClearFocus();GD.RequestBackdropCapture();
}
void NumericEntryDialog::Close() {Panel::TouchEnd(-1,-1);SetVisible(false);}
void NumericEntryDialog::TouchEnd(int16_t x,int16_t y)
{
    if(!IsVisible() || !IsEnabled()) return;
    int key=-1;for(unsigned i=0;i<17;++i) if(_keys[i].IsPressed() && _keys[i].Contains(x,y)) key=i;
    Panel::TouchEnd(x,y);if(key>=0) Key(key);
}
void NumericEntryDialog::Key(unsigned key)
{
    if(key==16) {Close();return;}
    if(key==15) {
        char* end=nullptr;const double value=strtod(_input,&end);
        if(end!=_input && *end=='\0' && isfinite(value) && _accept && _accept(_context,value)) Close();
        else _invalid=true;
        return;
    }
    if(key==14) {_input[0]='\0';_replace=false;}
    else if(key==13) {const size_t n=strlen(_input);if(n) _input[n-1]='\0';_replace=false;}
    else {
        if(_replace) {_input[0]='\0';_replace=false;}
        const size_t n=strlen(_input);if(n+1<sizeof(_input)) {_input[n]=keys[key][0];_input[n+1]='\0';}
    }
    _invalid=false;
}
void NumericEntryDialog::Draw()
{
    if(!IsVisible()) return;
    GD.DrawCapturedBackdrop();
    GD.DrawModalWindow(0,0,800,480,100,25,Theme::NumericPopupWidth,Theme::NumericPopupHeight,
                       Theme::PopupBackdrop,Theme::PopupBorder,Theme::PopupBackground);
    GD.ColorRGB(Theme::HeaderText);GD.cmd_text(400,53,Theme::FontPopupTitle,OPT_CENTERX,_title);
    GD.DrawSolidRoundedRect(128,85,544,56,0,0xF0F0E0);
    GD.DrawSolidRoundedRect(134,91,532,44,0,0x0A0A0A);
    GD.ColorRGB(Theme::ValueGreen);GD.cmd_text(146,113,strlen(_input)>22?26:strlen(_input)>16?28:Theme::FontPopupInput,OPT_CENTERY,_input);
    GD.ColorRGB(_invalid?0xFF6060:Theme::ToolbarText);
    GD.cmd_text(400,417,26,OPT_CENTERX,_invalid?"Invalid value or range":"Unit as titled; E = exponent (e.g. 1E-6)");
    Panel::Draw();
}
