#include "MeasurePanel.h"

#include <math.h>
#include <stdio.h>
#include "../Core/DisplayFormat.h"
#include "../Core/Theme.h"

MeasurePanel::MeasurePanel()
    : Panel(),
      _system(&System)
{
    _readingTextBuffer[0] = '\0';
    _rangeTextBuffer[0] = '\0';

    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    _topDivider.SetColor(Theme::LabelGreen);
    _topDivider.SetThickness(16);

    _titleLabel.SetFont(Theme::FontLabel);
    _titleLabel.SetTextColor(Theme::LabelGreen);
    _titleLabel.SetText("MEASURE");

    _modeLabel.SetFont(Theme::FontLabel);
    _modeLabel.SetTextColor(Theme::LabelGreen);
    _modeLabel.SetText("VOLTAGE");

    _valueLabel.SetFont(Theme::FontValue);
    _valueLabel.SetTextColor(Theme::ValueGreen);
    _valueLabel.SetTextOptions(OPT_MONO);

    _rangeLabel.SetFont(Theme::FontLabel);
    _rangeLabel.SetTextColor(Theme::LabelGreen);
    _rangeLabel.SetText("Range");

    _rangeValueLabel.SetFont(Theme::FontLabel);
    _rangeValueLabel.SetTextColor(Theme::LabelGreen);

    _rangeButton.SetFont(Theme::FontButton);
    _rangeButton.SetBackgroundColor(Theme::ButtonBackground);
    _rangeButton.SetTextColor(Theme::ButtonTextColor);
    _rangeButton.SetTextOptions(OPT_CENTER);
    _rangeButton.SetText("--");

    _functionButton.SetFont(Theme::FontButton);
    _functionButton.SetBackgroundColor(Theme::ButtonBackground);
    _functionButton.SetTextColor(Theme::ButtonTextColor);
    _functionButton.SetTextOptions(OPT_CENTER);
    _functionButton.SetText("Function");

    AddChild(&_topDivider);
    AddChild(&_titleLabel);
    AddChild(&_modeLabel);
    AddChild(&_valueLabel);
    AddChild(&_rangeLabel);
    AddChild(&_rangeValueLabel);
    AddChild(&_rangeButton);
    AddChild(&_functionButton);

    UpdateLayout();
    UpdateTexts();
}

MeasurePanel::MeasurePanel(int16_t x, int16_t y, int16_t w, int16_t h)
    : Panel(x, y, w, h),
      _system(&System)
{
    _readingTextBuffer[0] = '\0';
    _rangeTextBuffer[0] = '\0';

    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    _topDivider.SetColor(Theme::LabelGreen);
    _topDivider.SetThickness(16);

    _titleLabel.SetFont(Theme::FontLabel);
    _titleLabel.SetTextColor(Theme::LabelGreen);
    _titleLabel.SetText("MEASURE");

    _modeLabel.SetFont(Theme::FontLabel);
    _modeLabel.SetTextColor(Theme::LabelGreen);
    _modeLabel.SetText("VOLTAGE");

    _valueLabel.SetFont(Theme::FontValue);
    _valueLabel.SetTextColor(Theme::ValueGreen);
    _valueLabel.SetTextOptions(OPT_MONO);

    _rangeLabel.SetFont(Theme::FontLabel);
    _rangeLabel.SetTextColor(Theme::LabelGreen);
    _rangeLabel.SetText("Range");

    _rangeValueLabel.SetFont(Theme::FontLabel);
    _rangeValueLabel.SetTextColor(Theme::LabelGreen);

    _rangeButton.SetFont(Theme::FontButton);
    _rangeButton.SetBackgroundColor(Theme::ButtonBackground);
    _rangeButton.SetTextColor(Theme::ButtonTextColor);
    _rangeButton.SetTextOptions(OPT_CENTER);
    _rangeButton.SetText("--");

    _functionButton.SetFont(Theme::FontButton);
    _functionButton.SetBackgroundColor(Theme::ButtonBackground);
    _functionButton.SetTextColor(Theme::ButtonTextColor);
    _functionButton.SetTextOptions(OPT_CENTER);
    _functionButton.SetText("Function");

    AddChild(&_topDivider);
    AddChild(&_titleLabel);
    AddChild(&_modeLabel);
    AddChild(&_valueLabel);
    AddChild(&_rangeLabel);
    AddChild(&_rangeValueLabel);
    AddChild(&_rangeButton);
    AddChild(&_functionButton);

    UpdateLayout();
    UpdateTexts();
}

void MeasurePanel::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
    UpdateTexts();
}

SystemClass* MeasurePanel::GetSystem() const
{
    return _system;
}

void MeasurePanel::SetCaption(const char* caption)
{
    (void)caption;
    UpdateTexts();
}

const char* MeasurePanel::GetCaption() const
{
    return GetModeCaption();
}

Button& MeasurePanel::GetRangeButton()
{
    return _rangeButton;
}

Button& MeasurePanel::GetFunctionButton()
{
    return _functionButton;
}

void MeasurePanel::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateLayout();
    UpdateTexts();
    GD.cmd_fgcolor(Theme::ButtonGray);
    Panel::Draw();
}

void MeasurePanel::UpdateLayout()
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t w = GetWidth();
    const int16_t controlOffsetY = -10;
    const int16_t valueTop = y + 92 + controlOffsetY;
    const int16_t controlsTop = y + 34 + controlOffsetY;
    const int16_t buttonH = Theme::ButtonHeight;
    const int16_t labelOffsetY = (buttonH - 24) / 2;
    const int16_t setupButtonX = x + w - 170;
    const int16_t measureModeButtonX = setupButtonX - 170;
    const int16_t rangeButtonX = x + 176;

    _topDivider.SetBounds(x + 8, y + 8, w - 16, 1);
    _titleLabel.SetBounds(x + 14, y + 38 + controlOffsetY, 150, 28);
    _modeLabel.SetBounds(x + 14, y + 66 + controlOffsetY, 150, 28);
    _valueLabel.SetBounds(x + 42, valueTop, 430, 64);

    _rangeLabel.SetVisible(false);
    _rangeValueLabel.SetVisible(false);
    _rangeLabel.SetBounds(rangeButtonX - 88, controlsTop + labelOffsetY, 80, 24);
    _rangeValueLabel.SetBounds(x + 14, controlsTop + 26, 120, 24);
    _rangeButton.SetBounds(rangeButtonX, controlsTop, 150, buttonH);
    _functionButton.SetBounds(measureModeButtonX, controlsTop, 154, buttonH);

    const bool showRangeButton = (_system != nullptr) &&
                                 (_system->GetMeasureMode() != MeasureMode::Power);
    _rangeButton.SetVisible(showRangeButton);
}

void MeasurePanel::UpdateTexts()
{
    char valueText[32];
    char unitText[16];
    FormatMainValue(valueText, sizeof(valueText), unitText, sizeof(unitText));
    if (unitText[0] != '\0')
    {
        snprintf(_readingTextBuffer, sizeof(_readingTextBuffer), "%s %s", valueText, unitText);
    }
    else
    {
        snprintf(_readingTextBuffer, sizeof(_readingTextBuffer), "%s", valueText);
    }
    FormatRangeButtonText(_rangeTextBuffer, sizeof(_rangeTextBuffer));

    _titleLabel.SetText("MEASURE");
    _modeLabel.SetText(GetModeCaption());
    _valueLabel.SetText(_readingTextBuffer);

    if (_system == nullptr)
    {
        _rangeValueLabel.SetText("--");
        _rangeButton.SetText("--");
        _functionButton.SetText("Measure");
        return;
    }

    _rangeValueLabel.SetText(_system->GetRangeText());
    _rangeButton.SetText(_rangeTextBuffer);
    _functionButton.SetText(GetModeButtonText());
}

const char* MeasurePanel::GetModeCaption() const
{
    if (_system == nullptr)
    {
        return "MEASURE";
    }

    switch (_system->GetMeasureMode())
    {
        case MeasureMode::Voltage: return "VOLTAGE";
        case MeasureMode::Current: return "CURRENT";
        case MeasureMode::Resistance: return "RESISTANCE";
        case MeasureMode::Power: return "POWER";
        default: return "MEASURE";
    }
}

const char* MeasurePanel::GetModeButtonText() const
{
    if (_system == nullptr)
    {
        return "Measure:\n--";
    }

    switch (_system->GetMeasureMode())
    {
        case MeasureMode::Voltage: return "Measure:\nVOLTS(V)";
        case MeasureMode::Current: return "Measure:\nCURRENT(I)";
        case MeasureMode::Resistance: return "Measure:\nRES(Ohm)";
        case MeasureMode::Power: return "Measure:\nPOWER(W)";
        default: return "Measure:\n--";
    }
}

void MeasurePanel::FormatRangeButtonText(char* buffer, size_t bufferSize)
{
    if (_system == nullptr)
    {
        snprintf(buffer, bufferSize, "--");
        return;
    }

    switch (_system->GetMeasureMode())
    {
        case MeasureMode::Voltage:
            if (_system->GetVoltageRangeMode() == RangeMode::Auto)
            {
                snprintf(buffer, bufferSize, "AUTO\n%s", _system->GetVoltageRangeText());
            }
            else
            {
                switch (_system->GetVoltageRange())
                {

                    case VoltageRange::Range5V:
                        snprintf(buffer, bufferSize, "RANGE\n5 V");
                        break;

                    case VoltageRange::Range30V:
                        snprintf(buffer, bufferSize, "RANGE\n30 V");
                        break;

                    default:
                        snprintf(buffer, bufferSize, "--");
                        break;
                }
            }
            break;

        case MeasureMode::Current:
            if (_system->GetCurrentRangeMode() == RangeMode::Auto)
            {
                snprintf(buffer, bufferSize, "AUTO\n%s", _system->GetCurrentRangeText());
            }
            else
            {
                switch (_system->GetCurrentRange())
                {

                    case CurrentRange::Range100mA:
                        snprintf(buffer, bufferSize, "RANGE\n100 mA");
                        break;

                    case CurrentRange::Range1A:
                        snprintf(buffer, bufferSize, "RANGE\n1 A");
                        break;

                    default:
                        snprintf(buffer, bufferSize, "--");
                        break;
                }
            }
            break;

        case MeasureMode::Resistance:
            switch (_system->GetResistanceRange())
            {
                case ResistanceRange::Auto:
                    snprintf(buffer, bufferSize, "AUTO\nOhm");
                    break;

                case ResistanceRange::Range10Ohm:
                    snprintf(buffer, bufferSize, "RANGE\n10 Ohm");
                    break;

                case ResistanceRange::Range100Ohm:
                    snprintf(buffer, bufferSize, "RANGE\n100 Ohm");
                    break;

                case ResistanceRange::Range1kOhm:
                    snprintf(buffer, bufferSize, "RANGE\n1 kOhm");
                    break;

                case ResistanceRange::Range100kOhm:
                    snprintf(buffer, bufferSize, "RANGE\n100 kOhm");
                    break;

                default:
                    snprintf(buffer, bufferSize, "--");
                    break;
            }
            break;

        case MeasureMode::Power:
            snprintf(buffer, bufferSize, "AUTO\nW");
            break;

        default:
            snprintf(buffer, bufferSize, "--");
            break;
    }
}

void MeasurePanel::FormatMainValue(char* valueBuffer, size_t valueBufferSize,
                                   char* unitBuffer, size_t unitBufferSize)
{
    if (_system == nullptr)
    {
        snprintf(valueBuffer, valueBufferSize, "+0.000");
        snprintf(unitBuffer, unitBufferSize, "-");
        return;
    }

    switch (_system->GetMeasureMode())
    {
        case MeasureMode::Voltage:
            DisplayFormat::FormatFixedValue(DisplayQuantity::Voltage, _system->GetVoltage(),
                                            _voltageFormatState,
                                            true,
                                            valueBuffer, valueBufferSize, unitBuffer, unitBufferSize);
            break;
        case MeasureMode::Current:
            DisplayFormat::FormatFixedValue(DisplayQuantity::Current, _system->GetCurrent(),
                                            _currentFormatState,
                                            true,
                                            valueBuffer, valueBufferSize, unitBuffer, unitBufferSize);
            break;
        case MeasureMode::Resistance:
            if (fabsf(_system->GetCurrent()) < 1e-9f)
            {
                snprintf(valueBuffer, valueBufferSize, "---");
                if (unitBufferSize > 0)
                {
                    unitBuffer[0] = '\0';
                }
            }
            else
            {
                DisplayFormat::FormatFixedValue(DisplayQuantity::Resistance, _system->GetResistance(),
                                                _resistanceFormatState,
                                                true,
                                                valueBuffer, valueBufferSize, unitBuffer, unitBufferSize);
            }
            break;
        case MeasureMode::Power:
            DisplayFormat::FormatFixedValue(DisplayQuantity::Power, _system->GetPower(),
                                            _powerFormatState,
                                            true,
                                            valueBuffer, valueBufferSize, unitBuffer, unitBufferSize);
            break;
        default:
            snprintf(valueBuffer, valueBufferSize, "+0.000");
            snprintf(unitBuffer, unitBufferSize, "-");
            break;
    }
}
