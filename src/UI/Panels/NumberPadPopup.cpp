#include "NumberPadPopup.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Core/GDClass.h"
#include "../Core/Theme.h"

namespace
{
NumberPadPopup* g_activeNumberPadPopup = nullptr;
const char* kDigitTexts[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

void ApplyDialogButtonTheme(Button& button)
{
    button.SetBackgroundColor(Theme::ToolbarButtonBackground);
    button.SetTextColor(Theme::ToolbarButtonText);
}

void SafeCopy(char* dest, size_t destSize, const char* src)
{
    if (dest == nullptr || destSize == 0)
    {
        return;
    }

    if (src == nullptr)
    {
        dest[0] = '\0';
        return;
    }

    strncpy(dest, src, destSize - 1);
    dest[destSize - 1] = '\0';
}
}

NumberPadPopup::NumberPadPopup()
    : Panel(),
      _system(&System),
      _mode(SourceMode::Voltage),
      _targetType(NumberPadTargetType::SourceValue),
      _unitScale(NumberPadUnitScale::Base),
      _inputValid(true),
      _onClose(nullptr)
{
    _inputBuffer[0] = '\0';
    _displayBuffer[0] = '\0';
    _titleBuffer[0] = '\0';

    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);
    SetVisible(false);

    _titleLabel.SetFont(Theme::FontPopupTitle);
    _titleLabel.SetTextColor(Theme::HeaderText);
    _titleLabel.SetTextOptions(OPT_CENTERX);

    _inputLabel.SetFont(Theme::FontPopupInput);
    _inputLabel.SetTextColor(Theme::ValueGreen);
    _inputLabel.SetTextOptions(OPT_CENTERY);

    for (int i = 0; i < 10; ++i)
    {
        _digitButtons[i].SetFont(Theme::FontPopupButton);
        ApplyDialogButtonTheme(_digitButtons[i]);
        _digitButtons[i].SetTextOptions(OPT_CENTER);
        _digitButtons[i].SetText(kDigitTexts[i]);
        _digitButtons[i].SetOnClick(OnDigitClick);
        AddChild(&_digitButtons[i]);
    }

    _decimalButton.SetFont(Theme::FontPopupButton);
    ApplyDialogButtonTheme(_decimalButton);
    _decimalButton.SetText(".");
    _decimalButton.SetOnClick(OnActionClick);

    _signButton.SetFont(Theme::FontPopupButton);
    ApplyDialogButtonTheme(_signButton);
    _signButton.SetText("+/-");
    _signButton.SetOnClick(OnActionClick);

    _backButton.SetFont(Theme::FontPopupButton);
    ApplyDialogButtonTheme(_backButton);
    _backButton.SetText("Back");
    _backButton.SetOnClick(OnActionClick);

    _clearButton.SetFont(Theme::FontPopupButton);
    ApplyDialogButtonTheme(_clearButton);
    _clearButton.SetText("Clear");
    _clearButton.SetOnClick(OnActionClick);

    _okButton.SetFont(Theme::FontPopupButton);
    ApplyDialogButtonTheme(_okButton);
    _okButton.SetText("OK");
    _okButton.SetOnClick(OnActionClick);

    _cancelButton.SetFont(Theme::FontPopupButton);
    ApplyDialogButtonTheme(_cancelButton);
    _cancelButton.SetText("Cancel");
    _cancelButton.SetOnClick(OnActionClick);

    _unitBaseButton.SetFont(Theme::FontPopupButton);
    ApplyDialogButtonTheme(_unitBaseButton);
    _unitBaseButton.SetOnClick(OnActionClick);

    _unitMilliButton.SetFont(Theme::FontPopupButton);
    ApplyDialogButtonTheme(_unitMilliButton);
    _unitMilliButton.SetOnClick(OnActionClick);

    _unitMicroButton.SetFont(Theme::FontPopupButton);
    ApplyDialogButtonTheme(_unitMicroButton);
    _unitMicroButton.SetOnClick(OnActionClick);

    AddChild(&_titleLabel);
    AddChild(&_inputLabel);
    AddChild(&_decimalButton);
    AddChild(&_signButton);
    AddChild(&_backButton);
    AddChild(&_clearButton);
    AddChild(&_okButton);
    AddChild(&_cancelButton);
    AddChild(&_unitBaseButton);
    AddChild(&_unitMilliButton);
    AddChild(&_unitMicroButton);
}

NumberPadPopup::NumberPadPopup(int16_t x, int16_t y, int16_t w, int16_t h)
    : NumberPadPopup()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void NumberPadPopup::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

void NumberPadPopup::Open(SourceMode mode, NumberPadTargetType targetType)
{
    _mode = mode;
    _targetType = targetType;
    g_activeNumberPadPopup = this;

    snprintf(_titleBuffer, sizeof(_titleBuffer), "SET %s %s",
             mode == SourceMode::Voltage ? "VOLTAGE" : "CURRENT",
             targetType == NumberPadTargetType::SourceValue ? "SOURCE" : "LIMIT");
    _titleLabel.SetText(_titleBuffer);

    _unitScale = GetSuggestedUnitScale(GetCurrentValue());

    _unitBaseButton.SetText(GetUnitText(NumberPadUnitScale::Base));
    _unitMilliButton.SetText(GetUnitText(NumberPadUnitScale::Milli));
    _unitMicroButton.SetText(GetUnitText(NumberPadUnitScale::Micro));
    _signButton.SetEnabled(AllowSigned());

    LoadCurrentValue();
    SetVisible(true);
    UpdateLayout();
    ClearFocus();
}

void NumberPadPopup::Close()
{
    SetVisible(false);
    if (g_activeNumberPadPopup == this)
    {
        g_activeNumberPadPopup = nullptr;
    }
}

bool NumberPadPopup::IsOpen() const
{
    return IsVisible();
}

void NumberPadPopup::SetOnClose(NumberPadPopupCloseHandler handler)
{
    _onClose = handler;
}

void NumberPadPopup::Draw()
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

    GD.ColorRGB(0xF0F0E0);
    GD.Begin(RECTS);
    GD.Vertex2f(GetWindowX() + 28, GetWindowY() + 60);
    GD.Vertex2f(GetWindowX() + GetWindowWidth() - 28, GetWindowY() + 116);

    GD.ColorRGB(0x0A0A0A);
    GD.Begin(RECTS);
    GD.Vertex2f(GetWindowX() + 34, GetWindowY() + 66);
    GD.Vertex2f(GetWindowX() + GetWindowWidth() - 34, GetWindowY() + 110);

    UpdateLayout();
    ValidateInput();

    Container::Draw();
}

void NumberPadPopup::UpdateLayout()
{
    const int16_t x = GetWindowX();
    const int16_t y = GetWindowY();
    const int16_t buttonW = 86;
    const int16_t buttonH = 58;
    const int16_t gap = 8;
    const int16_t col0 = x + 30;
    const int16_t col1 = col0 + buttonW + gap;
    const int16_t col2 = col1 + buttonW + gap;
    const int16_t col3 = col2 + buttonW + gap;
    const int16_t rightWideX = x + GetWindowWidth() - 170;
    const int16_t row0 = y + 124;
    const int16_t row1 = row0 + buttonH + gap;
    const int16_t row2 = row1 + buttonH + gap;
    const int16_t row3 = row2 + buttonH + gap;
    const int16_t doubleButtonW = (buttonW * 2) + gap;

    _titleLabel.SetBounds(x + (GetWindowWidth() / 2), y + 28, 0, 28);
    _inputLabel.SetBounds(x + 46, y + 88, 0, 32);

    _digitButtons[1].SetBounds(col0, row0, buttonW, buttonH);
    _digitButtons[2].SetBounds(col1, row0, buttonW, buttonH);
    _digitButtons[3].SetBounds(col2, row0, buttonW, buttonH);
    _unitBaseButton.SetBounds(col3, row0, buttonW, buttonH);
    _backButton.SetBounds(rightWideX, row0, 140, buttonH);

    _digitButtons[4].SetBounds(col0, row1, buttonW, buttonH);
    _digitButtons[5].SetBounds(col1, row1, buttonW, buttonH);
    _digitButtons[6].SetBounds(col2, row1, buttonW, buttonH);
    _unitMilliButton.SetBounds(col3, row1, buttonW, buttonH);
    _clearButton.SetBounds(rightWideX, row1, 140, buttonH);

    _digitButtons[7].SetBounds(col0, row2, buttonW, buttonH);
    _digitButtons[8].SetBounds(col1, row2, buttonW, buttonH);
    _digitButtons[9].SetBounds(col2, row2, buttonW, buttonH);
    _unitMicroButton.SetBounds(col3, row2, buttonW, buttonH);

    _cancelButton.SetBounds(rightWideX, row2, 140, buttonH);
    _digitButtons[0].SetBounds(col0, row3, doubleButtonW, buttonH);
    _signButton.SetBounds(col2, row3, buttonW, buttonH);
    _decimalButton.SetBounds(col3, row3, buttonW, buttonH);
    _okButton.SetBounds(rightWideX, row3, 140, buttonH);
}

int16_t NumberPadPopup::GetWindowX() const
{
    return GetX() + ((GetWidth() - 600) / 2);
}

int16_t NumberPadPopup::GetWindowY() const
{
    return GetY() + ((GetHeight() - 430) / 2);
}

int16_t NumberPadPopup::GetWindowWidth() const
{
    return 600;
}

int16_t NumberPadPopup::GetWindowHeight() const
{
    return 430;
}

Button* NumberPadPopup::GetSelectedUnitButton() const
{
    if (_unitScale == NumberPadUnitScale::Milli)
    {
        return const_cast<Button*>(&_unitMilliButton);
    }

    if (_unitScale == NumberPadUnitScale::Micro)
    {
        return const_cast<Button*>(&_unitMicroButton);
    }

    return const_cast<Button*>(&_unitBaseButton);
}

DisplayQuantity NumberPadPopup::GetDisplayQuantity() const
{
    if (_mode == SourceMode::Voltage)
    {
        return (_targetType == NumberPadTargetType::SourceValue)
            ? DisplayQuantity::Voltage
            : DisplayQuantity::Current;
    }

    return (_targetType == NumberPadTargetType::SourceValue)
        ? DisplayQuantity::Current
        : DisplayQuantity::Voltage;
}

void NumberPadPopup::LoadCurrentValue()
{
    const float value = GetCurrentValue();
    float displayValue = value;

    if (_unitScale == NumberPadUnitScale::Milli)
    {
        displayValue *= 1000.0f;
    }
    else if (_unitScale == NumberPadUnitScale::Micro)
    {
        displayValue *= 1000000.0f;
    }

    SetInputFromDisplayValue(displayValue);
    BufferToDisplay();
}

void NumberPadPopup::SetInputFromDisplayValue(float displayValue)
{
    snprintf(_inputBuffer, sizeof(_inputBuffer), "%.*f",
             GetDisplayDecimals(_unitScale), displayValue);
}

uint8_t NumberPadPopup::GetDisplayDecimals(NumberPadUnitScale scale) const
{
    int8_t unitIndex = 0;

    if (scale == NumberPadUnitScale::Milli)
    {
        unitIndex = 1;
    }
    else if (scale == NumberPadUnitScale::Base)
    {
        unitIndex = 2;
    }

    return DisplayFormat::GetDecimalsFor(GetDisplayQuantity(), unitIndex);
}

void NumberPadPopup::BufferToDisplay()
{
    snprintf(_displayBuffer, sizeof(_displayBuffer), "%s %s", _inputBuffer, GetUnitText(_unitScale));
    _inputLabel.SetText(_displayBuffer);
    ValidateInput();
}

void NumberPadPopup::ValidateInput()
{
    const float value = GetAsFloat();
    _inputValid = (value >= GetMinValue() && value <= GetMaxValue());
    _inputLabel.SetTextColor(_inputValid ? Theme::ValueGreen : Theme::ErrorRed);
    _okButton.SetEnabled(_inputValid);
}

float NumberPadPopup::GetAsFloat() const
{
    float value = static_cast<float>(atof(_inputBuffer));

    if (_unitScale == NumberPadUnitScale::Milli)
    {
        value /= 1000.0f;
    }
    else if (_unitScale == NumberPadUnitScale::Micro)
    {
        value /= 1000000.0f;
    }

    return value;
}

float NumberPadPopup::GetCurrentValue() const
{
    if (_system == nullptr)
    {
        return 0.0f;
    }

    if (_mode == SourceMode::Voltage)
    {
        return (_targetType == NumberPadTargetType::SourceValue)
            ? _system->GetVoltageSourceValue()
            : _system->GetCurrentLimitValue();
    }

    return (_targetType == NumberPadTargetType::SourceValue)
        ? _system->GetCurrentSourceValue()
        : _system->GetVoltageLimitValue();
}

float NumberPadPopup::GetMinValue() const
{
    if (_system == nullptr)
    {
        return 0.0f;
    }

    if (_mode == SourceMode::Voltage)
    {
        return (_targetType == NumberPadTargetType::SourceValue)
            ? _system->GetVoltageSourceMin()
            : _system->GetCurrentLimitMin();
    }

    return (_targetType == NumberPadTargetType::SourceValue)
        ? _system->GetCurrentSourceMin()
        : _system->GetVoltageLimitMin();
}

float NumberPadPopup::GetMaxValue() const
{
    if (_system == nullptr)
    {
        return 0.0f;
    }

    if (_mode == SourceMode::Voltage)
    {
        return (_targetType == NumberPadTargetType::SourceValue)
            ? _system->GetVoltageSourceMax()
            : _system->GetCurrentLimitMax();
    }

    return (_targetType == NumberPadTargetType::SourceValue)
        ? _system->GetCurrentSourceMax()
        : _system->GetVoltageLimitMax();
}

void NumberPadPopup::ApplyValue(float value)
{
    if (_system == nullptr)
    {
        return;
    }

    if (_mode == SourceMode::Voltage)
    {
        if (_targetType == NumberPadTargetType::SourceValue)
        {
            _system->SetVoltageSourceValue(value);
        }
        else
        {
            _system->SetCurrentLimitValue(value);
        }
    }
    else
    {
        if (_targetType == NumberPadTargetType::SourceValue)
        {
            _system->SetCurrentSourceValue(value);
        }
        else
        {
            _system->SetVoltageLimitValue(value);
        }
    }
}

bool NumberPadPopup::AllowSigned() const
{
    return _targetType == NumberPadTargetType::SourceValue;
}

NumberPadUnitKind NumberPadPopup::GetUnitKind() const
{
    if (_mode == SourceMode::Voltage)
    {
        return (_targetType == NumberPadTargetType::SourceValue)
            ? NumberPadUnitKind::Voltage
            : NumberPadUnitKind::Current;
    }

    return (_targetType == NumberPadTargetType::SourceValue)
        ? NumberPadUnitKind::Current
        : NumberPadUnitKind::Voltage;
}

const char* NumberPadPopup::GetUnitText(NumberPadUnitScale scale) const
{
    const NumberPadUnitKind kind = GetUnitKind();

    if (kind == NumberPadUnitKind::Voltage)
    {
        if (scale == NumberPadUnitScale::Milli) return "mV";
        if (scale == NumberPadUnitScale::Micro) return "uV";
        return "V";
    }

    if (scale == NumberPadUnitScale::Milli) return "mA";
    if (scale == NumberPadUnitScale::Micro) return "uA";
    return "A";
}

NumberPadUnitScale NumberPadPopup::GetSuggestedUnitScale(float value) const
{
    const float absValue = fabsf(value);

    if (absValue < 0.000000001f)
    {
        return NumberPadUnitScale::Base;
    }

    if (absValue >= 1.0f)
    {
        return NumberPadUnitScale::Base;
    }

    if (absValue >= 0.001f)
    {
        return NumberPadUnitScale::Milli;
    }

    return NumberPadUnitScale::Micro;
}

void NumberPadPopup::SetUnitScale(NumberPadUnitScale scale)
{
    _unitScale = scale;
    BufferToDisplay();
}

void NumberPadPopup::AddDigit(char c)
{
    const size_t len = strlen(_inputBuffer);
    if (len < (sizeof(_inputBuffer) - 1))
    {
        _inputBuffer[len] = c;
        _inputBuffer[len + 1] = '\0';
        BufferToDisplay();
    }
}

void NumberPadPopup::AddDecimalPoint()
{
    if (strchr(_inputBuffer, '.') == nullptr)
    {
        AddDigit('.');
    }
}

void NumberPadPopup::ToggleSign()
{
    if (!AllowSigned())
    {
        return;
    }

    if (_inputBuffer[0] == '-')
    {
        _inputBuffer[0] = '+';
    }
    else if (_inputBuffer[0] == '+')
    {
        _inputBuffer[0] = '-';
    }
    else
    {
        const size_t len = strlen(_inputBuffer);
        if (len < (sizeof(_inputBuffer) - 1))
        {
            memmove(_inputBuffer + 1, _inputBuffer, len + 1);
            _inputBuffer[0] = '-';
        }
    }

    BufferToDisplay();
}

void NumberPadPopup::Backspace()
{
    size_t len = strlen(_inputBuffer);
    if (len > 1)
    {
        _inputBuffer[len - 1] = '\0';
    }
    else
    {
        SafeCopy(_inputBuffer, sizeof(_inputBuffer), AllowSigned() ? "+" : "");
    }

    BufferToDisplay();
}

void NumberPadPopup::ClearInput()
{
    SafeCopy(_inputBuffer, sizeof(_inputBuffer), AllowSigned() ? "+" : "");
    BufferToDisplay();
}

void NumberPadPopup::Submit()
{
    ValidateInput();
    if (!_inputValid)
    {
        return;
    }

    ApplyValue(GetAsFloat());
    Close();
    if (_onClose != nullptr)
    {
        _onClose(this, true);
    }
}

void NumberPadPopup::OnDigitClick(Button* sender)
{
    if (sender == nullptr || g_activeNumberPadPopup == nullptr)
    {
        return;
    }

    for (int i = 0; i < 10; ++i)
    {
        if (sender == &g_activeNumberPadPopup->_digitButtons[i])
        {
            g_activeNumberPadPopup->AddDigit(static_cast<char>('0' + i));
            return;
        }
    }
}

void NumberPadPopup::OnActionClick(Button* sender)
{
    if (sender == nullptr || g_activeNumberPadPopup == nullptr)
    {
        return;
    }

    NumberPadPopup* popup = g_activeNumberPadPopup;

    if (sender == &popup->_decimalButton)
    {
        popup->AddDecimalPoint();
    }
    else if (sender == &popup->_signButton)
    {
        popup->ToggleSign();
    }
    else if (sender == &popup->_backButton)
    {
        popup->Backspace();
    }
    else if (sender == &popup->_clearButton)
    {
        popup->ClearInput();
    }
    else if (sender == &popup->_okButton)
    {
        popup->Submit();
    }
    else if (sender == &popup->_cancelButton)
    {
        popup->Close();
        if (popup->_onClose != nullptr)
        {
            popup->_onClose(popup, false);
        }
    }
    else if (sender == &popup->_unitBaseButton)
    {
        popup->SetUnitScale(NumberPadUnitScale::Base);
    }
    else if (sender == &popup->_unitMilliButton)
    {
        popup->SetUnitScale(NumberPadUnitScale::Milli);
    }
    else if (sender == &popup->_unitMicroButton)
    {
        popup->SetUnitScale(NumberPadUnitScale::Micro);
    }
}
