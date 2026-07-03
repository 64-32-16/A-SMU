#include "MeasureFunctionPopup.h"

#include "../Core/GDClass.h"
#include "../Core/Theme.h"

namespace
{
MeasureFunctionPopup* g_activeMeasureFunctionPopup = nullptr;
const char* kMeasureTexts[4] = {"Current", "Voltage", "Resistance", "Power"};
const MeasureMode kMeasureModes[4] = {
    MeasureMode::Current,
    MeasureMode::Voltage,
    MeasureMode::Resistance,
    MeasureMode::Power
};

void ApplyDialogButtonTheme(Button& button)
{
    button.SetBackgroundColor(Theme::ToolbarButtonBackground);
    button.SetTextColor(Theme::ToolbarButtonText);
}
}

MeasureFunctionPopup::MeasureFunctionPopup()
    : Panel(),
      _system(&System),
      _onClose(nullptr)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);
    SetVisible(false);

    _leftTitleTopLabel.SetFont(Theme::FontPopupTitle);
    _leftTitleTopLabel.SetTextColor(Theme::HeaderText);
    _leftTitleTopLabel.SetTextOptions(OPT_CENTERX);
    _leftTitleTopLabel.SetText("Source Voltage");

    _leftTitleBottomLabel.SetFont(Theme::FontPopupTitle);
    _leftTitleBottomLabel.SetTextColor(Theme::HeaderText);
    _leftTitleBottomLabel.SetTextOptions(OPT_CENTERX);
    _leftTitleBottomLabel.SetText("and Measure:");

    _rightTitleTopLabel.SetFont(Theme::FontPopupTitle);
    _rightTitleTopLabel.SetTextColor(Theme::HeaderText);
    _rightTitleTopLabel.SetTextOptions(OPT_CENTERX);
    _rightTitleTopLabel.SetText("Source Current");

    _rightTitleBottomLabel.SetFont(Theme::FontPopupTitle);
    _rightTitleBottomLabel.SetTextColor(Theme::HeaderText);
    _rightTitleBottomLabel.SetTextOptions(OPT_CENTERX);
    _rightTitleBottomLabel.SetText("and Measure:");

    for (uint8_t i = 0; i < 4; ++i)
    {
        _leftButtons[i].SetFont(Theme::FontButton);
        ApplyDialogButtonTheme(_leftButtons[i]);
        _leftButtons[i].SetTextOptions(OPT_CENTER);
        _leftButtons[i].SetText(kMeasureTexts[i]);
        _leftButtons[i].SetOnClick(OnOptionClick);
        AddChild(&_leftButtons[i]);

        _rightButtons[i].SetFont(Theme::FontButton);
        ApplyDialogButtonTheme(_rightButtons[i]);
        _rightButtons[i].SetTextOptions(OPT_CENTER);
        _rightButtons[i].SetText(kMeasureTexts[i]);
        _rightButtons[i].SetOnClick(OnOptionClick);
        AddChild(&_rightButtons[i]);
    }

    _cancelButton.SetFont(Theme::FontButton);
    ApplyDialogButtonTheme(_cancelButton);
    _cancelButton.SetTextOptions(OPT_CENTER);
    _cancelButton.SetText("Cancel");
    _cancelButton.SetOnClick(OnCancelClick);

    AddChild(&_leftTitleTopLabel);
    AddChild(&_leftTitleBottomLabel);
    AddChild(&_rightTitleTopLabel);
    AddChild(&_rightTitleBottomLabel);
    AddChild(&_cancelButton);
}

MeasureFunctionPopup::MeasureFunctionPopup(int16_t x, int16_t y, int16_t w, int16_t h)
    : MeasureFunctionPopup()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void MeasureFunctionPopup::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

void MeasureFunctionPopup::Open()
{
    g_activeMeasureFunctionPopup = this;
    SetVisible(true);
    UpdateLayout();
    SyncSelection();
}

void MeasureFunctionPopup::Close()
{
    SetVisible(false);
    if (g_activeMeasureFunctionPopup == this)
    {
        g_activeMeasureFunctionPopup = nullptr;
    }
}

bool MeasureFunctionPopup::IsOpen() const
{
    return IsVisible();
}

void MeasureFunctionPopup::SetOnClose(MeasureFunctionPopupCloseHandler handler)
{
    _onClose = handler;
}

void MeasureFunctionPopup::Draw()
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

    GD.ColorRGB(0x000000);
    GD.LineWidth(32);
    GD.Begin(LINES);
    GD.Vertex2f(GetWindowX() + (GetWindowWidth() / 2), GetWindowY() + 24);
    GD.Vertex2f(GetWindowX() + (GetWindowWidth() / 2), GetWindowY() + GetWindowHeight() - 92);

    UpdateLayout();
    const SourceMode selectedSourceMode = (_system != nullptr) ? _system->GetSourceMode() : SourceMode::Voltage;
    uint8_t selectedIndex = 0;
    if (_system != nullptr)
    {
        switch (_system->GetMeasureMode())
        {
            case MeasureMode::Current: selectedIndex = 0; break;
            case MeasureMode::Voltage: selectedIndex = 1; break;
            case MeasureMode::Resistance: selectedIndex = 2; break;
            case MeasureMode::Power: selectedIndex = 3; break;
            default: selectedIndex = 0; break;
        }
    }

    for (uint8_t i = 0; i < 4; ++i)
    {
        _leftButtons[i].SetTextColor((selectedSourceMode == SourceMode::Voltage && selectedIndex == i) ? Theme::FocusYellow : Theme::ButtonText);
        _rightButtons[i].SetTextColor((selectedSourceMode == SourceMode::Current && selectedIndex == i) ? Theme::FocusYellow : Theme::ButtonText);
    }
    Container::Draw();
    DrawSelectedButton();
}

void MeasureFunctionPopup::UpdateLayout()
{
    const int16_t x = GetWindowX();
    const int16_t y = GetWindowY();
    const int16_t w = GetWindowWidth();
    const int16_t buttonW = 260;
    const int16_t buttonH = 48;
    const int16_t rowGap = 14;
    const int16_t leftX = x + 34;
    const int16_t rightX = x + w - 34 - buttonW;
    const int16_t row0 = y + 90;

    _leftTitleTopLabel.SetBounds(leftX + (buttonW / 2), y + 18, 0, 24);
    _leftTitleBottomLabel.SetBounds(leftX + (buttonW / 2), y + 50, 0, 24);
    _rightTitleTopLabel.SetBounds(rightX + (buttonW / 2), y + 18, 0, 24);
    _rightTitleBottomLabel.SetBounds(rightX + (buttonW / 2), y + 50, 0, 24);

    for (uint8_t i = 0; i < 4; ++i)
    {
        const int16_t rowY = row0 + (i * (buttonH + rowGap));
        _leftButtons[i].SetBounds(leftX, rowY, buttonW, buttonH);
        _rightButtons[i].SetBounds(rightX, rowY, buttonW, buttonH);
    }

    _cancelButton.SetBounds(x + ((w - 180) / 2), y + GetWindowHeight() - 62, 180, 44);
}

void MeasureFunctionPopup::OnOptionClick(Button* sender)
{
    if (sender == nullptr || g_activeMeasureFunctionPopup == nullptr)
    {
        return;
    }

    g_activeMeasureFunctionPopup->HandleOptionClick(sender);
}

void MeasureFunctionPopup::OnCancelClick(Button* sender)
{
    if (sender == nullptr || g_activeMeasureFunctionPopup == nullptr)
    {
        return;
    }

    g_activeMeasureFunctionPopup->HandleCancelClick(sender);
}

void MeasureFunctionPopup::HandleOptionClick(Button* sender)
{
    if (_system == nullptr || sender == nullptr)
    {
        return;
    }

    for (uint8_t i = 0; i < 4; ++i)
    {
        if (sender == &_leftButtons[i])
        {
            _system->SetSourceMode(SourceMode::Voltage);
            _system->SetMeasureMode(kMeasureModes[i]);
            SyncSelection();
            Close();
            if (_onClose != nullptr)
            {
                _onClose(this, true);
            }
            return;
        }

        if (sender == &_rightButtons[i])
        {
            _system->SetSourceMode(SourceMode::Current);
            _system->SetMeasureMode(kMeasureModes[i]);
            SyncSelection();
            Close();
            if (_onClose != nullptr)
            {
                _onClose(this, true);
            }
            return;
        }
    }
}

void MeasureFunctionPopup::HandleCancelClick(Button* sender)
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

void MeasureFunctionPopup::SyncSelection()
{
    ClearFocus();
}

void MeasureFunctionPopup::DrawSelectedButton() const
{
    if (_system == nullptr)
    {
        return;
    }

    uint8_t index = 0;
    switch (_system->GetMeasureMode())
    {
        case MeasureMode::Current: index = 0; break;
        case MeasureMode::Voltage: index = 1; break;
        case MeasureMode::Resistance: index = 2; break;
        case MeasureMode::Power: index = 3; break;
        default: index = 0; break;
    }

    const Button* selected = (_system->GetSourceMode() == SourceMode::Voltage) ? &_leftButtons[index] : &_rightButtons[index];
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

int16_t MeasureFunctionPopup::GetWindowX() const
{
    return GetX() + ((GetWidth() - 720) / 2);
}

int16_t MeasureFunctionPopup::GetWindowY() const
{
    return GetY() + ((GetHeight() - 410) / 2);
}

int16_t MeasureFunctionPopup::GetWindowWidth() const
{
    return 720;
}

int16_t MeasureFunctionPopup::GetWindowHeight() const
{
    return 410;
}
