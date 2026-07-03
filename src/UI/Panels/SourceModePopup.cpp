#include "SourceModePopup.h"

#include "../Core/GDClass.h"
#include "../Core/Theme.h"

namespace
{
SourceModePopup* g_activeSourceModePopup = nullptr;

void ApplyDialogButtonTheme(Button& button)
{
    button.SetBackgroundColor(Theme::ToolbarButtonBackground);
    button.SetTextColor(Theme::ToolbarButtonText);
}
}

SourceModePopup::SourceModePopup()
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
    _titleLabel.SetText("SELECT SOURCE MODE");

    _voltageButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_voltageButton);
    _voltageButton.SetTextOptions(OPT_CENTER);
    _voltageButton.SetText("Voltage");
    _voltageButton.SetOnClick(OnOptionClick);

    _currentButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_currentButton);
    _currentButton.SetTextOptions(OPT_CENTER);
    _currentButton.SetText("Current");
    _currentButton.SetOnClick(OnOptionClick);

    _cancelButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_cancelButton);
    _cancelButton.SetTextOptions(OPT_CENTER);
    _cancelButton.SetText("Cancel");
    _cancelButton.SetOnClick(OnCancelClick);

    AddChild(&_titleLabel);
    AddChild(&_voltageButton);
    AddChild(&_currentButton);
    AddChild(&_cancelButton);
}

SourceModePopup::SourceModePopup(int16_t x, int16_t y, int16_t w, int16_t h)
    : SourceModePopup()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void SourceModePopup::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

void SourceModePopup::Open()
{
    g_activeSourceModePopup = this;

    SetVisible(true);
    UpdateLayout();
    ClearFocus();
}

void SourceModePopup::Close()
{
    SetVisible(false);
    if (g_activeSourceModePopup == this)
    {
        g_activeSourceModePopup = nullptr;
    }
}

bool SourceModePopup::IsOpen() const
{
    return IsVisible();
}

void SourceModePopup::SetOnClose(SourceModePopupCloseHandler handler)
{
    _onClose = handler;
}

void SourceModePopup::Draw()
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
    _voltageButton.SetTextColor(selected == &_voltageButton ? Theme::FocusYellow : Theme::ButtonText);
    _currentButton.SetTextColor(selected == &_currentButton ? Theme::FocusYellow : Theme::ButtonText);
    Container::Draw();
    DrawSelectedButton();
}

void SourceModePopup::UpdateLayout()
{
    const int16_t x = GetWindowX();
    const int16_t y = GetWindowY();
    const int16_t w = GetWindowWidth();
    const int16_t buttonW = 160;
    const int16_t buttonH = 58;
    const int16_t gap = 18;
    const int16_t rowWidth = (buttonW * 2) + gap;
    const int16_t startX = x + ((w - rowWidth) / 2);

    _titleLabel.SetBounds(x + 36, y + 28, w - 72, 24);
    _voltageButton.SetBounds(startX, y + 86, buttonW, buttonH);
    _currentButton.SetBounds(startX + buttonW + gap, y + 86, buttonW, buttonH);
    _cancelButton.SetBounds(x + ((w - 160) / 2), y + 198, 160, 48);
}

Button* SourceModePopup::GetSelectedButton() const
{
    if (_system == nullptr)
    {
        return nullptr;
    }

    return (_system->GetSourceMode() == SourceMode::Voltage)
        ? const_cast<Button*>(&_voltageButton)
        : const_cast<Button*>(&_currentButton);
}

void SourceModePopup::DrawSelectedButton() const
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

int16_t SourceModePopup::GetWindowX() const
{
    return GetX() + ((GetWidth() - 520) / 2);
}

int16_t SourceModePopup::GetWindowY() const
{
    return GetY() + ((GetHeight() - 300) / 2);
}

int16_t SourceModePopup::GetWindowWidth() const
{
    return 520;
}

int16_t SourceModePopup::GetWindowHeight() const
{
    return 300;
}

void SourceModePopup::OnOptionClick(Button* sender)
{
    if (sender == nullptr || g_activeSourceModePopup == nullptr)
    {
        return;
    }

    g_activeSourceModePopup->HandleOptionClick(sender);
}

void SourceModePopup::OnCancelClick(Button* sender)
{
    if (sender == nullptr || g_activeSourceModePopup == nullptr)
    {
        return;
    }

    g_activeSourceModePopup->HandleCancelClick(sender);
}

void SourceModePopup::HandleOptionClick(Button* sender)
{
    if (sender == nullptr || _system == nullptr)
    {
        return;
    }

    if (sender == &_voltageButton)
    {
        _system->SetSourceMode(SourceMode::Voltage);
    }
    else if (sender == &_currentButton)
    {
        _system->SetSourceMode(SourceMode::Current);
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

void SourceModePopup::HandleCancelClick(Button* sender)
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
