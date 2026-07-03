#include "StatisticsPanel.h"

#include <stdio.h>
#include "../Core/Theme.h"

StatisticsPanel::StatisticsPanel()
    : Panel(),
      _system(&System)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);

    auto setupLegend = [](Label& label, const char* text)
    {
        label.SetFont(Theme::FontLabel);
        label.SetTextColor(Theme::SourceBlue);
        label.SetText(text);
    };

    auto setupValue = [](Label& label)
    {
        label.SetFont(Theme::FontLabel);
        label.SetTextColor(Theme::HeaderText);
    };

    setupLegend(_legendValue, "Value:");
    setupLegend(_legendPeak, "Peak-Peak:");
    setupLegend(_legendAverage, "Average:");
    setupLegend(_legendMin, "Min:");
    setupLegend(_legendMax, "Max:");

    _currentHeader.SetFont(Theme::FontLabel);
    _currentHeader.SetTextColor(Theme::LabelGreen);
    _currentHeader.SetText("Current");

    _voltageHeader.SetFont(Theme::FontLabel);
    _voltageHeader.SetTextColor(Theme::SourceBlue);
    _voltageHeader.SetText("Voltage");

    setupValue(_currentValue);
    setupValue(_currentPeak);
    setupValue(_currentAverage);
    setupValue(_currentMin);
    setupValue(_currentMax);
    setupValue(_voltageValue);
    setupValue(_voltagePeak);
    setupValue(_voltageAverage);
    setupValue(_voltageMin);
    setupValue(_voltageMax);

    setupLegend(_bufferInfoLabel, "Readings:");
    setupValue(_bufferInfoValue);

    _clearButton.SetFont(Theme::FontButton);
    _clearButton.SetTextOptions(OPT_CENTER);
    _clearButton.SetText("Clear Buffer");
    _clearButton.SetOnClick(OnClearClick);

    AddChild(&_legendValue);
    AddChild(&_legendPeak);
    AddChild(&_legendAverage);
    AddChild(&_legendMin);
    AddChild(&_legendMax);
    AddChild(&_currentHeader);
    AddChild(&_currentValue);
    AddChild(&_currentPeak);
    AddChild(&_currentAverage);
    AddChild(&_currentMin);
    AddChild(&_currentMax);
    AddChild(&_voltageHeader);
    AddChild(&_voltageValue);
    AddChild(&_voltagePeak);
    AddChild(&_voltageAverage);
    AddChild(&_voltageMin);
    AddChild(&_voltageMax);
    AddChild(&_bufferInfoLabel);
    AddChild(&_bufferInfoValue);
    AddChild(&_clearButton);

    UpdateLayout();
    UpdateTexts();
}

StatisticsPanel::StatisticsPanel(int16_t x, int16_t y, int16_t w, int16_t h)
    : StatisticsPanel()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void StatisticsPanel::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
    UpdateTexts();
}

SystemClass* StatisticsPanel::GetSystem() const
{
    return _system;
}

void StatisticsPanel::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    UpdateLayout();
    UpdateTexts();
    Panel::Draw();
}

void StatisticsPanel::UpdateLayout()
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t leftX = x + 18;
    const int16_t voltageX = x + 170;
    const int16_t currentX = x + 400;
    const int16_t row0 = y + 16;
    const int16_t rowStep = 30;

    _voltageHeader.SetBounds(voltageX, row0, 160, 24);
    _currentHeader.SetBounds(currentX, row0, 160, 24);

    _legendValue.SetBounds(leftX, row0 + rowStep, 110, 24);
    _legendPeak.SetBounds(leftX, row0 + (rowStep * 2), 110, 24);
    _legendAverage.SetBounds(leftX, row0 + (rowStep * 3), 110, 24);
    _legendMin.SetBounds(leftX, row0 + (rowStep * 4), 110, 24);
    _legendMax.SetBounds(leftX, row0 + (rowStep * 5), 110, 24);

    _voltageValue.SetBounds(voltageX, row0 + rowStep, 190, 24);
    _voltagePeak.SetBounds(voltageX, row0 + (rowStep * 2), 190, 24);
    _voltageAverage.SetBounds(voltageX, row0 + (rowStep * 3), 190, 24);
    _voltageMin.SetBounds(voltageX, row0 + (rowStep * 4), 190, 24);
    _voltageMax.SetBounds(voltageX, row0 + (rowStep * 5), 190, 24);

    _currentValue.SetBounds(currentX, row0 + rowStep, 190, 24);
    _currentPeak.SetBounds(currentX, row0 + (rowStep * 2), 190, 24);
    _currentAverage.SetBounds(currentX, row0 + (rowStep * 3), 190, 24);
    _currentMin.SetBounds(currentX, row0 + (rowStep * 4), 190, 24);
    _currentMax.SetBounds(currentX, row0 + (rowStep * 5), 190, 24);

    _bufferInfoLabel.SetBounds(x + 560, row0 + 2, 100, 24);
    _bufferInfoValue.SetBounds(x + 660, row0 + 2, 80, 24);
    _clearButton.SetBounds(x + 586, row0 + 96, 144, 42);
}

void StatisticsPanel::UpdateTexts()
{
    if (_system == nullptr)
    {
        return;
    }

    snprintf(_bufferCountText, sizeof(_bufferCountText), "%u", _system->Buffer.GetCount());
    _bufferInfoValue.SetText(_bufferCountText);

    FormatStat(DisplayQuantity::Current, _system->Buffer.GetCurrent(), true, _currentValueState, _currentValueText, sizeof(_currentValueText));
    FormatStat(DisplayQuantity::Current, _system->Buffer.GetCurrentPeakToPeak(), false, _currentPeakState, _currentPeakText, sizeof(_currentPeakText));
    FormatStat(DisplayQuantity::Current, _system->Buffer.GetCurrentAverage(), true, _currentAverageState, _currentAverageText, sizeof(_currentAverageText));
    FormatStat(DisplayQuantity::Current, _system->Buffer.GetCurrentMin(), true, _currentMinState, _currentMinText, sizeof(_currentMinText));
    FormatStat(DisplayQuantity::Current, _system->Buffer.GetCurrentMax(), true, _currentMaxState, _currentMaxText, sizeof(_currentMaxText));

    FormatStat(DisplayQuantity::Voltage, _system->Buffer.GetVoltage(), true, _voltageValueState, _voltageValueText, sizeof(_voltageValueText));
    FormatStat(DisplayQuantity::Voltage, _system->Buffer.GetVoltagePeakToPeak(), false, _voltagePeakState, _voltagePeakText, sizeof(_voltagePeakText));
    FormatStat(DisplayQuantity::Voltage, _system->Buffer.GetVoltageAverage(), true, _voltageAverageState, _voltageAverageText, sizeof(_voltageAverageText));
    FormatStat(DisplayQuantity::Voltage, _system->Buffer.GetVoltageMin(), true, _voltageMinState, _voltageMinText, sizeof(_voltageMinText));
    FormatStat(DisplayQuantity::Voltage, _system->Buffer.GetVoltageMax(), true, _voltageMaxState, _voltageMaxText, sizeof(_voltageMaxText));

    _currentValue.SetText(_currentValueText);
    _currentPeak.SetText(_currentPeakText);
    _currentAverage.SetText(_currentAverageText);
    _currentMin.SetText(_currentMinText);
    _currentMax.SetText(_currentMaxText);

    _voltageValue.SetText(_voltageValueText);
    _voltagePeak.SetText(_voltagePeakText);
    _voltageAverage.SetText(_voltageAverageText);
    _voltageMin.SetText(_voltageMinText);
    _voltageMax.SetText(_voltageMaxText);
}

void StatisticsPanel::FormatStat(DisplayQuantity quantity, float value, bool showSign,
                                 DisplayFormatState& state, char* buffer, size_t bufferSize)
{
    char valueText[24];
    char unitText[12];
    DisplayFormat::FormatValue(quantity, value, state, showSign, valueText, sizeof(valueText), unitText, sizeof(unitText));
    snprintf(buffer, bufferSize, "%s %s", valueText, unitText);
}

void StatisticsPanel::OnClearClick(Button* sender)
{
    (void)sender;
    System.Buffer.Reset();
}
