#include "MainConfigPanel.h"

#include "../Core/Theme.h"

MainConfigPanel::MainConfigPanel()
    : Panel(),
      _system(&System)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    _topDivider.SetColor(Theme::SourceBlue);
    _topDivider.SetThickness(16);

    SetupButton(_configButton, "CONFIG");
    SetupButton(_awgButton, "AWG");
    SetupButton(_traceButton, "TRACE");
    SetupButton(_outputButton, "ON");

    AddChild(&_topDivider);
    AddChild(&_configButton);
    AddChild(&_awgButton);
    AddChild(&_traceButton);
    AddChild(&_outputButton);

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

Button& MainConfigPanel::GetOutputButton()
{
    return _outputButton;
}

void MainConfigPanel::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateLayout();
    Panel::Draw();
}

void MainConfigPanel::UpdateLayout()
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t w = GetWidth();
    const int16_t buttonY = y + 18;
    const int16_t buttonH = Theme::ButtonHeight;
    const int16_t buttonW = 118;
    const int16_t gap = 10;

    _topDivider.SetBounds(x, y, w, 1);
    _configButton.SetBounds(x + 8, buttonY, buttonW, buttonH);
    _awgButton.SetBounds(x + 8 + buttonW + gap, buttonY, buttonW, buttonH);
    _traceButton.SetBounds(x + 8 + ((buttonW + gap) * 2), buttonY, buttonW, buttonH);
    _outputButton.SetBounds(x + w - 134, buttonY, 126, buttonH);
}

void MainConfigPanel::SetupButton(Button& button, const char* text)
{
    button.SetFont(Theme::FontButton);
    button.SetBackgroundColor(Theme::ToolbarButtonBackground);
    button.SetTextColor(Theme::ToolbarButtonText);
    button.SetTextOptions(OPT_CENTER);
    button.SetText(text);
}
