#include "ResistanceRangePopup.h"

#include "../Core/GDClass.h"
#include "../Core/Theme.h"

namespace
{
ResistanceRangePopup* g_activeResistanceRangePopup = nullptr;

void ApplyDialogButtonTheme(Button& button)
{
    button.SetBackgroundColor(Theme::ToolbarButtonBackground);
    button.SetTextColor(Theme::ToolbarButtonText);
}
}

ResistanceRangePopup::ResistanceRangePopup()
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
    _titleLabel.SetText("SELECT RESISTANCE RANGE");

    _autoButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_autoButton);
    _autoButton.SetTextOptions(OPT_CENTER);
    _autoButton.SetText("Auto");
    _autoButton.SetOnClick(OnOptionClick);

    _range10Button.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_range10Button);
    _range10Button.SetTextOptions(OPT_CENTER);
    _range10Button.SetText("10 Ohm");
    _range10Button.SetOnClick(OnOptionClick);

    _range100Button.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_range100Button);
    _range100Button.SetTextOptions(OPT_CENTER);
    _range100Button.SetText("100 Ohm");
    _range100Button.SetOnClick(OnOptionClick);

    _range1kButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_range1kButton);
    _range1kButton.SetTextOptions(OPT_CENTER);
    _range1kButton.SetText("1 kOhm");
    _range1kButton.SetOnClick(OnOptionClick);

    _range100kButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_range100kButton);
    _range100kButton.SetTextOptions(OPT_CENTER);
    _range100kButton.SetText("100 kOhm");
    _range100kButton.SetOnClick(OnOptionClick);

    _cancelButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_cancelButton);
    _cancelButton.SetTextOptions(OPT_CENTER);
    _cancelButton.SetText("Cancel");
    _cancelButton.SetOnClick(OnCancelClick);

    AddChild(&_titleLabel);
    AddChild(&_autoButton);
    AddChild(&_range10Button);
    AddChild(&_range100Button);
    AddChild(&_range1kButton);
    AddChild(&_range100kButton);
    AddChild(&_cancelButton);
}

ResistanceRangePopup::ResistanceRangePopup(int16_t x, int16_t y, int16_t w, int16_t h)
    : ResistanceRangePopup()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void ResistanceRangePopup::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

void ResistanceRangePopup::Open()
{
    g_activeResistanceRangePopup = this;
    SetVisible(true);
    UpdateLayout();
    ClearFocus();
}

void ResistanceRangePopup::Close()
{
    SetVisible(false);
    if (g_activeResistanceRangePopup == this)
    {
        g_activeResistanceRangePopup = nullptr;
    }
}

bool ResistanceRangePopup::IsOpen() const
{
    return IsVisible();
}

void ResistanceRangePopup::SetOnClose(ResistanceRangePopupCloseHandler handler)
{
    _onClose = handler;
}

void ResistanceRangePopup::Draw()
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
    _range10Button.SetTextColor(selected == &_range10Button ? Theme::FocusYellow : Theme::ButtonText);
    _range100Button.SetTextColor(selected == &_range100Button ? Theme::FocusYellow : Theme::ButtonText);
    _range1kButton.SetTextColor(selected == &_range1kButton ? Theme::FocusYellow : Theme::ButtonText);
    _range100kButton.SetTextColor(selected == &_range100kButton ? Theme::FocusYellow : Theme::ButtonText);
    Container::Draw();
    DrawSelectedButton();
}

void ResistanceRangePopup::UpdateLayout()
{
    const int16_t x = GetWindowX();
    const int16_t y = GetWindowY();
    const int16_t w = GetWindowWidth();
    const int16_t buttonW = 150;
    const int16_t buttonH = 54;
    const int16_t gap = 12;
    const int16_t row0 = y + 86;
    const int16_t row1 = row0 + buttonH + gap;
    const int16_t row2 = row1 + buttonH + gap;

    _titleLabel.SetBounds(x + 36, y + 28, w - 72, 24);
    _autoButton.SetBounds(x + 48, row0, buttonW, buttonH);
    _range10Button.SetBounds(x + 48 + buttonW + gap, row0, buttonW, buttonH);
    _range100Button.SetBounds(x + 48 + ((buttonW + gap) * 2), row0, buttonW, buttonH);
    _range1kButton.SetBounds(x + 126, row1, buttonW, buttonH);
    _range100kButton.SetBounds(x + 126 + buttonW + gap, row1, buttonW, buttonH);
    _cancelButton.SetBounds(x + ((w - 160) / 2), row2 + 8, 160, 48);
}

Button* ResistanceRangePopup::GetSelectedButton() const
{
    if (_system == nullptr)
    {
        return nullptr;
    }

    switch (_system->GetResistanceRange())
    {
        case ResistanceRange::Auto: return const_cast<Button*>(&_autoButton);
        case ResistanceRange::Range10Ohm: return const_cast<Button*>(&_range10Button);
        case ResistanceRange::Range100Ohm: return const_cast<Button*>(&_range100Button);
        case ResistanceRange::Range1kOhm: return const_cast<Button*>(&_range1kButton);
        case ResistanceRange::Range100kOhm: return const_cast<Button*>(&_range100kButton);
        default: return nullptr;
    }
}

void ResistanceRangePopup::DrawSelectedButton() const
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

void ResistanceRangePopup::OnOptionClick(Button* sender)
{
    if (sender == nullptr || g_activeResistanceRangePopup == nullptr)
    {
        return;
    }

    g_activeResistanceRangePopup->HandleOptionClick(sender);
}

void ResistanceRangePopup::OnCancelClick(Button* sender)
{
    if (sender == nullptr || g_activeResistanceRangePopup == nullptr)
    {
        return;
    }

    g_activeResistanceRangePopup->HandleCancelClick(sender);
}

void ResistanceRangePopup::HandleOptionClick(Button* sender)
{
    if (_system == nullptr || sender == nullptr)
    {
        return;
    }

    if (sender == &_autoButton)
    {
        _system->SetResistanceRange(ResistanceRange::Auto);
    }
    else if (sender == &_range10Button)
    {
        _system->SetResistanceRange(ResistanceRange::Range10Ohm);
    }
    else if (sender == &_range100Button)
    {
        _system->SetResistanceRange(ResistanceRange::Range100Ohm);
    }
    else if (sender == &_range1kButton)
    {
        _system->SetResistanceRange(ResistanceRange::Range1kOhm);
    }
    else if (sender == &_range100kButton)
    {
        _system->SetResistanceRange(ResistanceRange::Range100kOhm);
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

void ResistanceRangePopup::HandleCancelClick(Button* sender)
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

int16_t ResistanceRangePopup::GetWindowX() const
{
    return GetX() + ((GetWidth() - 600) / 2);
}

int16_t ResistanceRangePopup::GetWindowY() const
{
    return GetY() + ((GetHeight() - 280) / 2);
}

int16_t ResistanceRangePopup::GetWindowWidth() const
{
    return 600;
}

int16_t ResistanceRangePopup::GetWindowHeight() const
{
    return 280;
}
