#include "MeasurePanel.h"

#include <math.h>
#include <stdio.h>
#include "../Core/DisplayFormat.h"
#include "../Core/Theme.h"

MeasurePanel::MeasurePanel()
    : Panel(),
      _system(&System),
      _caption("VOLTAGE")
{
    _titleTextBuffer[0] = '\0';
    _readingTextBuffer[0] = '\0';

    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    _topDivider.SetColor(Theme::LabelGreen);
    _topDivider.SetThickness(16);

    _titleLabel.SetFont(Theme::FontLabel);
    _titleLabel.SetTextColor(Theme::LabelGreen);

    _valueLabel.SetFont(Theme::FontValue);
    _valueLabel.SetTextColor(Theme::ValueGreen);
    _valueLabel.SetTextOptions(OPT_MONO);

    _rangeLabel.SetFont(Theme::FontLabel);
    _rangeLabel.SetTextColor(Theme::LabelGreen);
    _rangeLabel.SetText("Range");

    _rangeValueLabel.SetFont(Theme::FontLabel);
    _rangeValueLabel.SetTextColor(Theme::LabelGreen);

    _rangeButton.SetFont(Theme::FontButton);
    _rangeButton.SetTextOptions(OPT_CENTER);
    _rangeButton.SetText("--");

    _functionButton.SetFont(Theme::FontButton);
    _functionButton.SetTextOptions(OPT_CENTER);
    _functionButton.SetText("Function");

    AddChild(&_topDivider);
    AddChild(&_titleLabel);
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
      _system(&System),
      _caption("VOLTAGE")
{
    _titleTextBuffer[0] = '\0';
    _readingTextBuffer[0] = '\0';

    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    _topDivider.SetColor(Theme::LabelGreen);
    _topDivider.SetThickness(16);

    _titleLabel.SetFont(Theme::FontLabel);
    _titleLabel.SetTextColor(Theme::LabelGreen);

    _valueLabel.SetFont(Theme::FontValue);
    _valueLabel.SetTextColor(Theme::ValueGreen);
    _valueLabel.SetTextOptions(OPT_MONO);

    _rangeLabel.SetFont(Theme::FontLabel);
    _rangeLabel.SetTextColor(Theme::LabelGreen);
    _rangeLabel.SetText("Range");

    _rangeValueLabel.SetFont(Theme::FontLabel);
    _rangeValueLabel.SetTextColor(Theme::LabelGreen);

    _rangeButton.SetFont(Theme::FontButton);
    _rangeButton.SetTextOptions(OPT_CENTER);
    _rangeButton.SetText("--");

    _functionButton.SetFont(Theme::FontButton);
    _functionButton.SetTextOptions(OPT_CENTER);
    _functionButton.SetText("Function");

    AddChild(&_topDivider);
    AddChild(&_titleLabel);
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
    _caption = (caption != nullptr) ? caption : "";
    UpdateTexts();
}

const char* MeasurePanel::GetCaption() const
{
    return _caption;
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
    const int16_t valueTop = y + Theme::PanelValueTopY;
    const int16_t controlsTop = y + Theme::PanelControlsTopY;

    _topDivider.SetBounds(x + 8, y + 8, w - 16, 1);
    _titleLabel.SetBounds(x + 12, y + 16, 300, 28);
    _valueLabel.SetBounds(x + 14, valueTop, 600, 56);

    _rangeLabel.SetBounds(x + 14, controlsTop + 2, 80, 24);
    _rangeValueLabel.SetBounds(x + 14, controlsTop + 26, 120, 24);
    _rangeButton.SetBounds(x + 112, controlsTop, 116, 42);
    _functionButton.SetBounds(x + w - 170, controlsTop, 156, 42);

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
    snprintf(_titleTextBuffer, sizeof(_titleTextBuffer), "MEASURE %s", _caption);

    _titleLabel.SetText(_titleTextBuffer);
    _valueLabel.SetText(_readingTextBuffer);

    if (_system == nullptr)
    {
        _rangeValueLabel.SetText("--");
        _rangeButton.SetText("--");
        _functionButton.SetText("Measure");
        return;
    }

    _rangeValueLabel.SetText(_system->GetRangeText());
    _rangeButton.SetText(_system->GetRangeText());
    _functionButton.SetText(_caption);
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
            DisplayFormat::FormatValue(DisplayQuantity::Voltage, _system->GetVoltage(),
                                       _voltageFormatState,
                                       true,
                                       valueBuffer, valueBufferSize, unitBuffer, unitBufferSize);
            break;
        case MeasureMode::Current:
            DisplayFormat::FormatValue(DisplayQuantity::Current, _system->GetCurrent(),
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
                DisplayFormat::FormatValue(DisplayQuantity::Resistance, _system->GetResistance(),
                                           _resistanceFormatState,
                                           false,
                                           valueBuffer, valueBufferSize, unitBuffer, unitBufferSize);
            }
            break;
        case MeasureMode::Power:
            DisplayFormat::FormatValue(DisplayQuantity::Power, _system->GetPower(),
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
