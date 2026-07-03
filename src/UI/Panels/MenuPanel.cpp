#include "MenuPanel.h"

#include "../Core/Theme.h"

MenuPanel::MenuPanel()
    : Panel(),
      _system(&System)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    auto setupButton = [](Button& button, const char* text)
    {
        button.SetFont(Theme::FontButton);
        button.SetBackgroundColor(Theme::ButtonBackground);
        button.SetTextColor(Theme::ButtonTextColor);
        button.SetTextOptions(OPT_CENTER);
        button.SetText(text);
    };

    setupButton(_outputOffStateButton, "Output-Off-State");
    setupButton(_ovpButton, "OVP-Setting");
    setupButton(_ocpButton, "OCP-Setting");
    setupButton(_sweepButton, "Sweep-Setting");
    setupButton(_quicksetButton, "Quickset");
    setupButton(_graphButton, "Graph");

    AddChild(&_outputOffStateButton);
    AddChild(&_ovpButton);
    AddChild(&_ocpButton);
    AddChild(&_sweepButton);
    AddChild(&_quicksetButton);
    AddChild(&_graphButton);
}

MenuPanel::MenuPanel(int16_t x, int16_t y, int16_t w, int16_t h)
    : MenuPanel()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void MenuPanel::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

SystemClass* MenuPanel::GetSystem() const
{
    return _system;
}

Button& MenuPanel::GetOutputOffStateButton()
{
    return _outputOffStateButton;
}

Button& MenuPanel::GetOvpButton()
{
    return _ovpButton;
}

Button& MenuPanel::GetOcpButton()
{
    return _ocpButton;
}

Button& MenuPanel::GetSweepButton()
{
    return _sweepButton;
}

Button& MenuPanel::GetQuicksetButton()
{
    return _quicksetButton;
}

Button& MenuPanel::GetGraphButton()
{
    return _graphButton;
}

void MenuPanel::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateLayout();
    Panel::Draw();
}

void MenuPanel::UpdateLayout()
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t w = GetWidth();
    const int16_t buttonW = 280;
    const int16_t buttonH = 44;
    const int16_t columnGap = 26;
    const int16_t rowGap = 16;
    const int16_t leftX = x + ((w - ((buttonW * 2) + columnGap)) / 2);
    const int16_t rightX = leftX + buttonW + columnGap;
    const int16_t row0 = y + 24;
    const int16_t row1 = row0 + buttonH + rowGap;
    const int16_t row2 = row1 + buttonH + rowGap;

    _outputOffStateButton.SetBounds(leftX, row0, buttonW, buttonH);
    _ovpButton.SetBounds(rightX, row0, buttonW, buttonH);
    _ocpButton.SetBounds(leftX, row1, buttonW, buttonH);
    _sweepButton.SetBounds(rightX, row1, buttonW, buttonH);
    _quicksetButton.SetBounds(leftX, row2, buttonW, buttonH);
    _graphButton.SetBounds(rightX, row2, buttonW, buttonH);
}
