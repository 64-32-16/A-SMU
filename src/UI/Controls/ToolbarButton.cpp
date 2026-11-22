#include "ToolbarButton.h"
#include "../Core/Theme.h"

ToolbarButton::ToolbarButton()
{
    SetFont(Theme::FontButton);
    SetBackgroundColor(Theme::FlatToolbarBackground);
    SetTextColor(Theme::ToolbarButtonText);
}

void ToolbarButton::Draw()
{
    // Share the native 3D widget and pressed-state rendering with every button.
    Button::Draw();
}
