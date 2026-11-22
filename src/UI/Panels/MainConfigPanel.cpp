#include "MainConfigPanel.h"

#include "../Core/Theme.h"

MainConfigPanel::MainConfigPanel()
    : Panel(),
      _system(&System)
{
    SetShowBackground(true);
    SetBackgroundColor(Theme::MainScreenBackground);
    SetShowBorder(false);
    SetShowTitle(false);


    SetupButton(_configButton, "Config");
    SetupButton(_awgButton, "AWG");
    SetupButton(_traceButton, "Trace");

    AddChild(&_configButton);
    AddChild(&_awgButton);
    AddChild(&_traceButton);

    UpdateLayout();
}

MainConfigPanel::MainConfigPanel(int16_t x, int16_t y, int16_t w, int16_t h)
    : MainConfigPanel()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void MainConfigPanel::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

SystemClass* MainConfigPanel::GetSystem() const
{
    return _system;
}

Button& MainConfigPanel::GetConfigButton()
{
    return _configButton;
}

Button& MainConfigPanel::GetAwgButton()
{
    return _awgButton;
}

Button& MainConfigPanel::GetTraceButton()
{
    return _traceButton;
}

void MainConfigPanel::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateLayout();
    GD.SaveContext();
    // Rectangle coordinates are pixels, independent of preceding EVE widgets.
    GD.VertexFormat(0);
    GD.ColorA(255);
    GD.LineWidth(1);
    Panel::Draw();
    GD.RestoreContext();
}

void MainConfigPanel::UpdateLayout()
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t buttonH = GetHeight();
    const int16_t buttonW = 118;
    const int16_t gap = 4;

    _configButton.SetBounds(x, y, buttonW, buttonH);
    _awgButton.SetBounds(x + buttonW + gap, y, buttonW, buttonH);
    _traceButton.SetBounds(x + (buttonW + gap) * 2, y, buttonW, buttonH);
}

void MainConfigPanel::SetupButton(Button& button, const char* text)
{
    button.SetFont(Theme::FontButton);
    button.SetBackgroundColor(Theme::FlatToolbarBackground);
    button.SetTextColor(Theme::ToolbarButtonText);
    button.SetTextOptions(OPT_CENTER);
    button.SetText(text);
}
