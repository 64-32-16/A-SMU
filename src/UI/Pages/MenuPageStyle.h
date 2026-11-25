#pragma once
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
#include "../Controls/Button.h"

// Shared layout for menu destination pages. See StyleGuide.md.
namespace MenuPageStyle {
constexpr int LabelX=394, ValueX=414, RowTop=74, RowStep=80;
constexpr int ValueWidth=240, ValueHeight=54;
constexpr int TextFont=28, HintFont=26;
inline int RowY(unsigned row) {return RowTop+row*RowStep;}
inline void Navigation(Button& back,Button& home,int x=0,int y=0) {
    back.SetBounds(x+584,y+8,96,40);home.SetBounds(x+688,y+8,96,40);
}
inline void Header(const char* title,int x=0,int y=0) {
    GD.DrawSolidRoundedRect(x+8,y+4,784,52,0,Theme::TabInactive);
    GD.ColorRGB(Theme::ToolbarText);GD.cmd_text(x+20,y+14,29,0,title);
}
}
