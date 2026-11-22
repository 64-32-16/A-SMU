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
    _rangeButton.SetBackgroundColor(Theme::MainButtonBackground);
    _rangeButton.SetCornerRadius(0);
    _rangeButton.SetTextColor(Theme::MainButtonText);
    _rangeButton.SetTextOptions(OPT_CENTER);
    _rangeButton.SetText("--");

    _functionButton.SetFont(Theme::FontButton);
    _functionButton.SetBackgroundColor(Theme::MainButtonBackground);
    _functionButton.SetCornerRadius(0);
    _functionButton.SetTextColor(Theme::MainButtonText);
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
    _rangeButton.SetBackgroundColor(Theme::MainButtonBackground);
    _rangeButton.SetCornerRadius(0);
    _rangeButton.SetTextColor(Theme::MainButtonText);
    _rangeButton.SetTextOptions(OPT_CENTER);
    _rangeButton.SetText("--");

    _functionButton.SetFont(Theme::FontButton);
    _functionButton.SetBackgroundColor(Theme::MainButtonBackground);
    _functionButton.SetCornerRadius(0);
    _functionButton.SetTextColor(Theme::MainButtonText);
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
    _topDivider.SetVisible(false);
    _titleLabel.SetBounds(x + 12, y + 4, 150, 28);
    _modeLabel.SetBounds(x + 162, y + 4, 200, 28);
    _valueLabel.SetBounds(x + 20, y + 38, w - 40, 72);
    _rangeLabel.SetVisible(true);
    _rangeLabel.SetBounds(x + 12, y + 140, 80, 24);
    _rangeValueLabel.SetVisible(false);
    _rangeButton.SetBounds(x + 100, y + 128, 150, 47);
    _functionButton.SetBounds(x + w - 170, y + 128, 158, 47);
    const bool showRange = _system->GetMeasureMode() != MeasureMode::Power;
    _rangeButton.SetVisible(showRange);
    _rangeLabel.SetVisible(showRange);
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
                snprintf(buffer, bufferSize, "RANGE\n%s", _system->GetCurrentRangeText());
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

    if(_system->GetMeasurement().Fault!=Hardware::Error::None) {
        snprintf(valueBuffer,valueBufferSize,"---");if(unitBufferSize) unitBuffer[0]='\0';return;
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
            if (!_system->GetMeasurement().HasValidResistance())
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
