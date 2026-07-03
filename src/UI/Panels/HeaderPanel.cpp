#include "HeaderPanel.h"
#include "../Core/Theme.h"

#include <stdio.h>

HeaderPanel::HeaderPanel()
    : Panel(),
      _system(&System),
      _timeTextBuffer{0}
{
    SetupControls();
}

HeaderPanel::HeaderPanel(int16_t x, int16_t y, int16_t w, int16_t h)
    : Panel(x, y, w, h),
      _system(&System),
      _timeTextBuffer{0}
{
    SetupControls();
}

void HeaderPanel::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
    UpdateTexts();
}

SystemClass* HeaderPanel::GetSystem() const
{
    return _system;
}

void HeaderPanel::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateLayout();
    UpdateTexts();
    Panel::Draw();
}

void HeaderPanel::UpdateLayout()
{
    const int16_t y = GetY() + ((GetHeight() - Theme::ButtonHeight) / 2);
    const int16_t h = Theme::ButtonHeight;
    const int16_t labelCenterY = GetY() + (GetHeight() / 2);

    _homeButton.SetBounds(GetX(), y, 82, h);
    _menuButton.SetBounds(GetX() + 90, y, 82, h);
    _triggerLabel.SetBounds(GetX() + 245, labelCenterY, 90, 24);
    _controlLabel.SetBounds(GetX() + 370, labelCenterY, 110, 24);
    _lanLabel.SetBounds(GetX() + 515, labelCenterY, 95, 24);
    _timeLabel.SetBounds(GetX() + GetWidth() - 52, labelCenterY, 104, 24);
}

void HeaderPanel::UpdateTexts()
{
    if (_system == nullptr)
    {
        _triggerLabel.SetText("AUTO");
        _controlLabel.SetText("--");
        _lanLabel.SetText("LAN: Off");
        _timeLabel.SetText("--:--:--");
        return;
    }

    const uint32_t totalSeconds = millis() / 1000UL;
    const uint8_t hours = (totalSeconds / 3600UL) % 24UL;
    const uint8_t minutes = (totalSeconds / 60UL) % 60UL;
    const uint8_t seconds = totalSeconds % 60UL;

    snprintf(_timeTextBuffer, sizeof(_timeTextBuffer), "%02u:%02u:%02u",
        static_cast<unsigned int>(hours),
        static_cast<unsigned int>(minutes),
        static_cast<unsigned int>(seconds));

    _triggerLabel.SetText("AUTO");
    _controlLabel.SetText("LOCAL");
    _lanLabel.SetText("LAN: Off");
    _timeLabel.SetText(_timeTextBuffer);
}

void HeaderPanel::SetupControls()
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    _homeButton.SetText("HOME");
    _homeButton.SetFont(Theme::FontHeader);
    _homeButton.SetBackgroundColor(Theme::ToolbarButtonBackground);
    _homeButton.SetTextColor(Theme::ToolbarButtonText);
    _homeButton.SetTextOptions(OPT_CENTER);

    _menuButton.SetText("MENU");
    _menuButton.SetFont(Theme::FontHeader);
    _menuButton.SetBackgroundColor(Theme::ToolbarButtonBackground);
    _menuButton.SetTextColor(Theme::ToolbarButtonText);
    _menuButton.SetTextOptions(OPT_CENTER);

    _triggerLabel.SetFont(Theme::FontHeader);
    _triggerLabel.SetTextColor(Theme::ToolbarText);
    _triggerLabel.SetTextOptions(OPT_CENTER);

    _controlLabel.SetFont(Theme::FontHeader);
    _controlLabel.SetTextColor(Theme::ToolbarText);
    _controlLabel.SetTextOptions(OPT_CENTER);

    _lanLabel.SetFont(Theme::FontHeader);
    _lanLabel.SetTextColor(Theme::ToolbarText);
    _lanLabel.SetTextOptions(OPT_CENTER);

    _timeLabel.SetFont(Theme::FontHeader);
    _timeLabel.SetTextColor(Theme::ToolbarText);
    _timeLabel.SetTextOptions(OPT_CENTER);

    AddChild(&_homeButton);
    AddChild(&_menuButton);
    AddChild(&_triggerLabel);
    AddChild(&_controlLabel);
    AddChild(&_lanLabel);
    AddChild(&_timeLabel);

    UpdateLayout();
    UpdateTexts();
}
