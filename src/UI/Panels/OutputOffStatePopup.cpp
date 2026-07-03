#include "OutputOffStatePopup.h"

#include "../Core/GDClass.h"
#include "../Core/Theme.h"

namespace
{
OutputOffStatePopup* g_activeOutputOffStatePopup = nullptr;

void ApplyDialogButtonTheme(Button& button)
{
    button.SetBackgroundColor(Theme::ToolbarButtonBackground);
    button.SetTextColor(Theme::ToolbarButtonText);
}
}

OutputOffStatePopup::OutputOffStatePopup()
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
    _titleLabel.SetText("SELECT OUTPUT OFF STATE");

    _highZButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_highZButton);
    _highZButton.SetTextOptions(OPT_CENTER);
    _highZButton.SetText("Z-High");
    _highZButton.SetOnClick(OnOptionClick);

    _normalButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_normalButton);
    _normalButton.SetTextOptions(OPT_CENTER);
    _normalButton.SetText("Standard");
    _normalButton.SetOnClick(OnOptionClick);

    _zeroButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_zeroButton);
    _zeroButton.SetTextOptions(OPT_CENTER);
    _zeroButton.SetText("Zero");
    _zeroButton.SetOnClick(OnOptionClick);

    _cancelButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_cancelButton);
    _cancelButton.SetTextOptions(OPT_CENTER);
    _cancelButton.SetText("Cancel");
    _cancelButton.SetOnClick(OnCancelClick);

    AddChild(&_titleLabel);
    AddChild(&_highZButton);
    AddChild(&_normalButton);
    AddChild(&_zeroButton);
    AddChild(&_cancelButton);
}

OutputOffStatePopup::OutputOffStatePopup(int16_t x, int16_t y, int16_t w, int16_t h)
    : OutputOffStatePopup()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void OutputOffStatePopup::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

void OutputOffStatePopup::Open()
{
    g_activeOutputOffStatePopup = this;
    SetVisible(true);
    UpdateLayout();
    ClearFocus();
}

void OutputOffStatePopup::Close()
{
    SetVisible(false);
    if (g_activeOutputOffStatePopup == this)
    {
        g_activeOutputOffStatePopup = nullptr;
    }
}

bool OutputOffStatePopup::IsOpen() const
{
    return IsVisible();
}

void OutputOffStatePopup::SetOnClose(OutputOffStatePopupCloseHandler handler)
{
    _onClose = handler;
}

void OutputOffStatePopup::Draw()
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
    _highZButton.SetTextColor(selected == &_highZButton ? Theme::FocusYellow : Theme::ButtonText);
    _normalButton.SetTextColor(selected == &_normalButton ? Theme::FocusYellow : Theme::ButtonText);
    _zeroButton.SetTextColor(selected == &_zeroButton ? Theme::FocusYellow : Theme::ButtonText);
    Container::Draw();
    DrawSelectedButton();
}

void OutputOffStatePopup::UpdateLayout()
{
    const int16_t x = GetWindowX();
    const int16_t y = GetWindowY();
    const int16_t w = GetWindowWidth();
    const int16_t buttonW = 130;
    const int16_t buttonH = 56;
    const int16_t gap = 14;
    const int16_t rowWidth = (buttonW * 3) + (gap * 2);
    const int16_t startX = x + ((w - rowWidth) / 2);

    _titleLabel.SetBounds(x + 36, y + 28, w - 72, 24);
    _highZButton.SetBounds(startX, y + 86, buttonW, buttonH);
    _normalButton.SetBounds(startX + buttonW + gap, y + 86, buttonW, buttonH);
    _zeroButton.SetBounds(startX + ((buttonW + gap) * 2), y + 86, buttonW, buttonH);
    _cancelButton.SetBounds(x + ((w - 160) / 2), y + 192, 160, 48);
}

Button* OutputOffStatePopup::GetSelectedButton() const
{
    if (_system == nullptr)
    {
        return nullptr;
    }

    switch (_system->GetOutputOffState())
    {
        case OutputOffState::ZHigh: return const_cast<Button*>(&_highZButton);
        case OutputOffState::Standard: return const_cast<Button*>(&_normalButton);
        case OutputOffState::Zero: return const_cast<Button*>(&_zeroButton);
        default: return nullptr;
    }
}

void OutputOffStatePopup::DrawSelectedButton() const
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

void OutputOffStatePopup::OnOptionClick(Button* sender)
{
    if (sender == nullptr || g_activeOutputOffStatePopup == nullptr)
    {
        return;
    }

    g_activeOutputOffStatePopup->HandleOptionClick(sender);
}

void OutputOffStatePopup::OnCancelClick(Button* sender)
{
    if (sender == nullptr || g_activeOutputOffStatePopup == nullptr)
    {
        return;
    }

    g_activeOutputOffStatePopup->HandleCancelClick(sender);
}

void OutputOffStatePopup::HandleOptionClick(Button* sender)
{
    if (_system == nullptr || sender == nullptr)
    {
        return;
    }

    if (sender == &_highZButton)
    {
        _system->SetOutputOffState(OutputOffState::ZHigh);
    }
    else if (sender == &_normalButton)
    {
        _system->SetOutputOffState(OutputOffState::Standard);
    }
    else if (sender == &_zeroButton)
    {
        _system->SetOutputOffState(OutputOffState::Zero);
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

void OutputOffStatePopup::HandleCancelClick(Button* sender)
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

int16_t OutputOffStatePopup::GetWindowX() const
{
    return GetX() + ((GetWidth() - 600) / 2);
}

int16_t OutputOffStatePopup::GetWindowY() const
{
    return GetY() + ((GetHeight() - 280) / 2);
}

int16_t OutputOffStatePopup::GetWindowWidth() const
{
    return 600;
}

int16_t OutputOffStatePopup::GetWindowHeight() const
{
    return 280;
}
