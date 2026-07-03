#include "MeasureModePopup.h"

#include "../Core/GDClass.h"
#include "../Core/Theme.h"

namespace
{
MeasureModePopup* g_activeMeasureModePopup = nullptr;

void ApplyDialogButtonTheme(Button& button)
{
    button.SetBackgroundColor(Theme::ToolbarButtonBackground);
    button.SetTextColor(Theme::ToolbarButtonText);
}
}

MeasureModePopup::MeasureModePopup()
    : Panel(),
      _system(&System),
      _onClose(nullptr)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);
    SetVisible(false);

    _titleLabel.SetFont(Theme::FontPopupTitle);
    _titleLabel.SetTextColor(Theme::HeaderText);
    _titleLabel.SetText("SELECT MEASURE MODE");

    _currentButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_currentButton);
    _currentButton.SetTextOptions(OPT_CENTER);
    _currentButton.SetText("Current");
    _currentButton.SetOnClick(OnOptionClick);

    _voltageButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_voltageButton);
    _voltageButton.SetTextOptions(OPT_CENTER);
    _voltageButton.SetText("Voltage");
    _voltageButton.SetOnClick(OnOptionClick);

    _powerButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_powerButton);
    _powerButton.SetTextOptions(OPT_CENTER);
    _powerButton.SetText("Power");
    _powerButton.SetOnClick(OnOptionClick);

    _resistanceButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_resistanceButton);
    _resistanceButton.SetTextOptions(OPT_CENTER);
    _resistanceButton.SetText("Resistor");
    _resistanceButton.SetOnClick(OnOptionClick);

    _cancelButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_cancelButton);
    _cancelButton.SetTextOptions(OPT_CENTER);
    _cancelButton.SetText("Cancel");
    _cancelButton.SetOnClick(OnCancelClick);

    AddChild(&_titleLabel);
    AddChild(&_currentButton);
    AddChild(&_voltageButton);
    AddChild(&_powerButton);
    AddChild(&_resistanceButton);
    AddChild(&_cancelButton);
}

MeasureModePopup::MeasureModePopup(int16_t x, int16_t y, int16_t w, int16_t h)
    : MeasureModePopup()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void MeasureModePopup::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

void MeasureModePopup::Open()
{
    g_activeMeasureModePopup = this;

    SetVisible(true);
    UpdateLayout();
    ClearFocus();
}

void MeasureModePopup::Close()
{
    SetVisible(false);
    if (g_activeMeasureModePopup == this)
    {
        g_activeMeasureModePopup = nullptr;
    }
}

bool MeasureModePopup::IsOpen() const
{
    return IsVisible();
}

void MeasureModePopup::SetOnClose(MeasureModePopupCloseHandler handler)
{
    _onClose = handler;
}

void MeasureModePopup::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    GD.ColorRGB(Theme::PopupBackdrop);
    GD.ColorA(160);
    GD.Begin(RECTS);
    GD.Vertex2f(GetX(), GetY());
    GD.Vertex2f(GetX() + GetWidth(), GetY() + GetHeight());
    GD.ColorA(255);

    GD.ColorRGB(Theme::PopupBorder);
    GD.Begin(RECTS);
    GD.Vertex2f(GetWindowX(), GetWindowY());
    GD.Vertex2f(GetWindowX() + GetWindowWidth(), GetWindowY() + GetWindowHeight());

    GD.ColorRGB(Theme::PopupBackground);
    GD.Begin(RECTS);
    GD.Vertex2f(GetWindowX() + 2, GetWindowY() + 2);
    GD.Vertex2f(GetWindowX() + GetWindowWidth() - 2, GetWindowY() + GetWindowHeight() - 2);

    UpdateLayout();
    Button* selected = GetSelectedButton();
    _currentButton.SetTextColor(selected == &_currentButton ? Theme::FocusYellow : Theme::ButtonText);
    _voltageButton.SetTextColor(selected == &_voltageButton ? Theme::FocusYellow : Theme::ButtonText);
    _powerButton.SetTextColor(selected == &_powerButton ? Theme::FocusYellow : Theme::ButtonText);
    _resistanceButton.SetTextColor(selected == &_resistanceButton ? Theme::FocusYellow : Theme::ButtonText);
    Container::Draw();
    DrawSelectedButton();
}

void MeasureModePopup::UpdateLayout()
{
    const int16_t x = GetWindowX();
    const int16_t y = GetWindowY();
    const int16_t w = GetWindowWidth();
    const int16_t buttonW = 180;
    const int16_t buttonH = 54;
    const int16_t gap = 16;
    const int16_t rowWidth = (buttonW * 2) + gap;
    const int16_t startX = x + ((w - rowWidth) / 2);
    const int16_t row0 = y + 82;
    const int16_t row1 = row0 + buttonH + gap;

    _titleLabel.SetBounds(x + 36, y + 28, w - 72, 24);
    _currentButton.SetBounds(startX, row0, buttonW, buttonH);
    _voltageButton.SetBounds(startX + buttonW + gap, row0, buttonW, buttonH);
    _powerButton.SetBounds(startX, row1, buttonW, buttonH);
    _resistanceButton.SetBounds(startX + buttonW + gap, row1, buttonW, buttonH);
    _cancelButton.SetBounds(x + ((w - 160) / 2), y + 236, 160, 48);
}

Button* MeasureModePopup::GetSelectedButton() const
{
    if (_system == nullptr)
    {
        return nullptr;
    }

    switch (_system->GetMeasureMode())
    {
        case MeasureMode::Current: return const_cast<Button*>(&_currentButton);
        case MeasureMode::Voltage: return const_cast<Button*>(&_voltageButton);
        case MeasureMode::Power: return const_cast<Button*>(&_powerButton);
        case MeasureMode::Resistance: return const_cast<Button*>(&_resistanceButton);
        default: return const_cast<Button*>(&_voltageButton);
    }
}

void MeasureModePopup::DrawSelectedButton() const
{
    Button* selected = GetSelectedButton();
    if (selected == nullptr || !selected->IsVisible())
    {
        return;
    }

    const int16_t x = selected->GetX() + 1;
    const int16_t y = selected->GetY() + 1;
    const int16_t x1 = selected->GetX() + selected->GetWidth() - 1;
    const int16_t y1 = selected->GetY() + selected->GetHeight() - 1;

    GD.ColorRGB(Theme::FocusYellow);
    GD.LineWidth(16);
    GD.Begin(LINE_STRIP);
    GD.Vertex2f(x, y);
    GD.Vertex2f(x1, y);
    GD.Vertex2f(x1, y1);
    GD.Vertex2f(x, y1);
    GD.Vertex2f(x, y);
}

int16_t MeasureModePopup::GetWindowX() const
{
    return GetX() + ((GetWidth() - 560) / 2);
}

int16_t MeasureModePopup::GetWindowY() const
{
    return GetY() + ((GetHeight() - 330) / 2);
}

int16_t MeasureModePopup::GetWindowWidth() const
{
    return 560;
}

int16_t MeasureModePopup::GetWindowHeight() const
{
    return 330;
}

void MeasureModePopup::OnOptionClick(Button* sender)
{
    if (sender == nullptr || g_activeMeasureModePopup == nullptr)
    {
        return;
    }

    g_activeMeasureModePopup->HandleOptionClick(sender);
}

void MeasureModePopup::OnCancelClick(Button* sender)
{
    if (sender == nullptr || g_activeMeasureModePopup == nullptr)
    {
        return;
    }

    g_activeMeasureModePopup->HandleCancelClick(sender);
}

void MeasureModePopup::HandleOptionClick(Button* sender)
{
    if (sender == nullptr || _system == nullptr)
    {
        return;
    }

    if (sender == &_currentButton)
    {
        _system->SetMeasureMode(MeasureMode::Current);
    }
    else if (sender == &_voltageButton)
    {
        _system->SetMeasureMode(MeasureMode::Voltage);
    }
    else if (sender == &_powerButton)
    {
        _system->SetMeasureMode(MeasureMode::Power);
    }
    else if (sender == &_resistanceButton)
    {
        _system->SetMeasureMode(MeasureMode::Resistance);
    }
    else
    {
        return;
    }

    Close();
    if (_onClose != nullptr)
    {
        _onClose(this, true);
    }
}

void MeasureModePopup::HandleCancelClick(Button* sender)
{
    if (sender != &_cancelButton)
    {
        return;
    }

    Close();
    if (_onClose != nullptr)
    {
        _onClose(this, false);
    }
}
