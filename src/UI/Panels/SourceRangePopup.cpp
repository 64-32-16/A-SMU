#include "SourceRangePopup.h"

#include "../Core/GDClass.h"
#include "../Core/Theme.h"

namespace
{
SourceRangePopup* g_activeSourceRangePopup = nullptr;

void ApplyDialogButtonTheme(Button& button)
{
    button.SetBackgroundColor(Theme::ToolbarButtonBackground);
    button.SetTextColor(Theme::ToolbarButtonText);
}
}

SourceRangePopup::SourceRangePopup()
    : Panel(),
      _system(&System),
      _mode(SourceMode::Voltage),
      _onClose(nullptr)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);
    SetVisible(false);

    _titleLabel.SetFont(Theme::FontPopupTitle);
    _titleLabel.SetTextColor(Theme::HeaderText);

    _autoButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_autoButton);
    _autoButton.SetTextOptions(OPT_CENTER);
    _autoButton.SetOnClick(OnOptionClick);

    _option1Button.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_option1Button);
    _option1Button.SetTextOptions(OPT_CENTER);
    _option1Button.SetOnClick(OnOptionClick);

    _option2Button.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_option2Button);
    _option2Button.SetTextOptions(OPT_CENTER);
    _option2Button.SetOnClick(OnOptionClick);

    _cancelButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_cancelButton);
    _cancelButton.SetTextOptions(OPT_CENTER);
    _cancelButton.SetText("Cancel");
    _cancelButton.SetOnClick(OnCancelClick);

    AddChild(&_titleLabel);
    AddChild(&_autoButton);
    AddChild(&_option1Button);
    AddChild(&_option2Button);
    AddChild(&_cancelButton);
}

SourceRangePopup::SourceRangePopup(int16_t x, int16_t y, int16_t w, int16_t h)
    : SourceRangePopup()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void SourceRangePopup::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

void SourceRangePopup::OpenForMode(SourceMode mode)
{
    _mode = mode;
    g_activeSourceRangePopup = this;

    UpdateTexts();
    SetVisible(true);
    UpdateLayout();
    ClearFocus();
}

void SourceRangePopup::Close()
{
    SetVisible(false);
    if (g_activeSourceRangePopup == this)
    {
        g_activeSourceRangePopup = nullptr;
    }
}

bool SourceRangePopup::IsOpen() const
{
    return IsVisible();
}

void SourceRangePopup::SetOnClose(SourceRangePopupCloseHandler handler)
{
    _onClose = handler;
}

void SourceRangePopup::Draw()
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
    _autoButton.SetTextColor(selected == &_autoButton ? Theme::FocusYellow : Theme::ButtonText);
    _option1Button.SetTextColor(selected == &_option1Button ? Theme::FocusYellow : Theme::ButtonText);
    _option2Button.SetTextColor(selected == &_option2Button ? Theme::FocusYellow : Theme::ButtonText);
    Container::Draw();
    DrawSelectedButton();
}

void SourceRangePopup::UpdateLayout()
{
    const int16_t x = GetWindowX();
    const int16_t y = GetWindowY();
    const int16_t w = GetWindowWidth();
    const int16_t buttonW = 120;
    const int16_t buttonH = 58;
    const int16_t gap = 14;
    const int16_t rowWidth = (buttonW * 3) + (gap * 2);
    const int16_t startX = x + ((w - rowWidth) / 2);

    _titleLabel.SetBounds(x + 36, y + 28, w - 72, 24);
    _autoButton.SetBounds(startX, y + 86, buttonW, buttonH);
    _option1Button.SetBounds(startX + buttonW + gap, y + 86, buttonW, buttonH);
    _option2Button.SetBounds(startX + ((buttonW + gap) * 2), y + 86, buttonW, buttonH);
    _cancelButton.SetBounds(x + ((w - 160) / 2), y + 198, 160, 48);
}

Button* SourceRangePopup::GetSelectedButton() const
{
    if (_system == nullptr)
    {
        return nullptr;
    }

    if (_mode == SourceMode::Voltage)
    {
        const VoltageRange range = _system->GetVoltageRange();
        if (range == VoltageRange::Auto) return const_cast<Button*>(&_autoButton);
        if (range == VoltageRange::Range5V) return const_cast<Button*>(&_option1Button);
        return const_cast<Button*>(&_option2Button);
    }

    const CurrentRange range = _system->GetCurrentRange();
    if (range == CurrentRange::Auto) return const_cast<Button*>(&_autoButton);
    if (range == CurrentRange::Range100mA) return const_cast<Button*>(&_option1Button);
    return const_cast<Button*>(&_option2Button);
}

void SourceRangePopup::DrawSelectedButton() const
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

int16_t SourceRangePopup::GetWindowX() const
{
    return GetX() + ((GetWidth() - 600) / 2);
}

int16_t SourceRangePopup::GetWindowY() const
{
    return GetY() + ((GetHeight() - 300) / 2);
}

int16_t SourceRangePopup::GetWindowWidth() const
{
    return 600;
}

int16_t SourceRangePopup::GetWindowHeight() const
{
    return 300;
}

void SourceRangePopup::OnOptionClick(Button* sender)
{
    if (sender == nullptr || g_activeSourceRangePopup == nullptr)
    {
        return;
    }

    g_activeSourceRangePopup->HandleOptionClick(sender);
}

void SourceRangePopup::OnCancelClick(Button* sender)
{
    if (sender == nullptr || g_activeSourceRangePopup == nullptr)
    {
        return;
    }

    g_activeSourceRangePopup->HandleCancelClick(sender);
}

void SourceRangePopup::HandleOptionClick(Button* sender)
{
    if (sender == nullptr || _system == nullptr)
    {
        return;
    }

    if (_mode == SourceMode::Voltage)
    {
        if (sender == &_autoButton)
        {
            _system->SetVoltageRange(VoltageRange::Auto);
        }
        else if (sender == &_option1Button)
        {
            _system->SetVoltageRange(VoltageRange::Range5V);
        }
        else if (sender == &_option2Button)
        {
            _system->SetVoltageRange(VoltageRange::Range30V);
        }
    }
    else
    {
        if (sender == &_autoButton)
        {
            _system->SetCurrentRange(CurrentRange::Auto);
        }
        else if (sender == &_option1Button)
        {
            _system->SetCurrentRange(CurrentRange::Range100mA);
        }
        else if (sender == &_option2Button)
        {
            _system->SetCurrentRange(CurrentRange::Range1A);
        }
    }

    Close();
    if (_onClose != nullptr)
    {
        _onClose(this, true);
    }
}

void SourceRangePopup::HandleCancelClick(Button* sender)
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

void SourceRangePopup::UpdateTexts()
{
    if (_mode == SourceMode::Voltage)
    {
        _titleLabel.SetText("SELECT SOURCE VOLTAGE RANGE");
        _autoButton.SetText("Auto");
        _option1Button.SetText("5V");
        _option2Button.SetText("30V");
    }
    else
    {
        _titleLabel.SetText("SELECT SOURCE CURRENT RANGE");
        _autoButton.SetText("Auto");
        _option1Button.SetText("100mA");
        _option2Button.SetText("1A");
    }
}
