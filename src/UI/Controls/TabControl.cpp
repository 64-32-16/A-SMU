#include "TabControl.h"
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
#include "../../Core/Beeper.h"

TabControl::TabControl() : _active(Theme::TabActive), _inactive(Theme::TabInactive),
    _pressedColor(Theme::TabPressed), _text(Theme::ToolbarText) {}

bool TabControl::AddTab(const char* caption)
{
    if(!caption || _count>=MaxTabs) return false;
    _captions[_count++]=caption;return true;
}
void TabControl::SetSelectedIndex(uint8_t index)
{
    if(index>=_count || index==_selected) return;
    _selected=index;
    if(_onChanged) _onChanged(this,index);
}
void TabControl::SetColors(uint32_t active,uint32_t inactive,uint32_t pressed,uint32_t text)
{ _active=active;_inactive=inactive;_pressedColor=pressed;_text=text; }

int TabControl::HitTab(int16_t x,int16_t y) const
{
    if(!_count || _w<=0 || _h<6 || x<_x || x>=_x+_w || y<_y || y>=_y+_h-3) return -1;
    for(uint8_t i=0;i<_count;++i)
    {
        const int left=_x+_w*i/_count, right=_x+_w*(i+1)/_count;
        if(x<left || x>=right) continue;
        const int width=right-left, slope=width/5, height=_h-3;
        if(slope>0 && x>right-slope && (y-_y)*slope<(x-(right-slope))*height) return -1;
        return i;
    }
    return -1;
}
void TabControl::TouchStart(int16_t x,int16_t y)
{
    _tracking=_pressed=(IsVisible() && IsEnabled())?HitTab(x,y):-1;
}
void TabControl::TouchMove(int16_t x,int16_t y)
{
    _pressed=IsVisible() && IsEnabled() && HitTab(x,y)==_tracking?_tracking:-1;
}
void TabControl::TouchEnd(int16_t x,int16_t y)
{
    const int selected=HitTab(x,y);
    const bool activate=IsVisible() && IsEnabled() && _tracking>=0 && selected==_tracking;
    _tracking=_pressed=-1;
    if(activate && selected!=_selected) {Beeper.Click();SetSelectedIndex(selected);}
}
void TabControl::Draw()
{
    if(!IsVisible() || !_count || _w<=0 || _h<6) return;
    for(uint8_t i=0;i<_count;++i)
    {
        const int left=_x+_w*i/_count, right=_x+_w*(i+1)/_count;
        const int width=right-left,slope=width/5;
        const uint32_t color=!IsEnabled()?_inactive:i==_pressed?_pressedColor:i==_selected?_active:_inactive;
        GD.DrawTabShape(left,_y,width,_h-3,slope,color);
        GD.ColorRGB(_text);
        GD.cmd_text(left+(width-slope/2)/2,_y+(_h-3)/2,_font,OPT_CENTER,_captions[i]);
    }
    GD.SaveContext();GD.VertexFormat(0);GD.ColorRGB(_active);GD.LineWidth(32);
    GD.Begin(LINES);GD.Vertex2f(_x,_y+_h-2);GD.Vertex2f(_x+_w,_y+_h-2);GD.RestoreContext();
}
