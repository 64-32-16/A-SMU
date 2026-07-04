#include "StatisticsPanel.h"

#include <stdio.h>
#include "../Core/GDClass.h"
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
        label.SetFont(Theme::FontPopupTitle);
        label.SetTextColor(Theme::SourceBlue);
        label.SetTextOptions(OPT_CENTERY);
        label.SetText(text);
    };

    auto setupValue = [](Label& label)
    {
        label.SetFont(Theme::FontPopupTitle);
        label.SetTextColor(Theme::HeaderText);
        label.SetTextOptions(OPT_CENTERY);
    };

    setupLegend(_legendValue, "Value:");
    setupLegend(_legendPeak, "Peak-Peak:");
    setupLegend(_legendAverage, "Average:");
    setupLegend(_legendMin, "Min:");
    setupLegend(_legendMax, "Max:");

    _currentHeader.SetFont(Theme::FontPopupTitle);
    _currentHeader.SetTextColor(Theme::LabelGreen);
    _currentHeader.SetTextOptions(OPT_CENTERY);
    _currentHeader.SetText("Current");

    _voltageHeader.SetFont(Theme::FontPopupTitle);
    _voltageHeader.SetTextColor(Theme::SourceBlue);
    _voltageHeader.SetTextOptions(OPT_CENTERY);
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
    DrawTable();
    Panel::Draw();
}

void StatisticsPanel::UpdateLayout()
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t tableX = x + 20;
    const int16_t tableY = y + 24;
    const int16_t rowH = 42;
    const int16_t statX = tableX + 16;
    const int16_t voltageX = tableX + 178;
    const int16_t currentX = tableX + 404;
    const int16_t labelH = 34;
    const int16_t headerCenterY = tableY + (rowH / 2);
    const int16_t statusX = x + GetWidth() - 160;

    _voltageHeader.SetBounds(voltageX, headerCenterY, 190, labelH);
    _currentHeader.SetBounds(currentX, headerCenterY, 190, labelH);

    _legendValue.SetBounds(statX, tableY + rowH + (rowH / 2), 140, labelH);
    _legendPeak.SetBounds(statX, tableY + (rowH * 2) + (rowH / 2), 140, labelH);
    _legendAverage.SetBounds(statX, tableY + (rowH * 3) + (rowH / 2), 140, labelH);
    _legendMin.SetBounds(statX, tableY + (rowH * 4) + (rowH / 2), 140, labelH);
    _legendMax.SetBounds(statX, tableY + (rowH * 5) + (rowH / 2), 140, labelH);

    _voltageValue.SetBounds(voltageX, tableY + rowH + (rowH / 2), 210, labelH);
    _voltagePeak.SetBounds(voltageX, tableY + (rowH * 2) + (rowH / 2), 210, labelH);
    _voltageAverage.SetBounds(voltageX, tableY + (rowH * 3) + (rowH / 2), 210, labelH);
    _voltageMin.SetBounds(voltageX, tableY + (rowH * 4) + (rowH / 2), 210, labelH);
    _voltageMax.SetBounds(voltageX, tableY + (rowH * 5) + (rowH / 2), 210, labelH);

    _currentValue.SetBounds(currentX, tableY + rowH + (rowH / 2), 210, labelH);
    _currentPeak.SetBounds(currentX, tableY + (rowH * 2) + (rowH / 2), 210, labelH);
    _currentAverage.SetBounds(currentX, tableY + (rowH * 3) + (rowH / 2), 210, labelH);
    _currentMin.SetBounds(currentX, tableY + (rowH * 4) + (rowH / 2), 210, labelH);
    _currentMax.SetBounds(currentX, tableY + (rowH * 5) + (rowH / 2), 210, labelH);

    _bufferInfoLabel.SetBounds(statusX, tableY + 34, 116, labelH);
    _bufferInfoValue.SetBounds(statusX + 108, tableY + 34, 48, labelH);
    _clearButton.SetBounds(statusX, tableY + 92, 144, 46);
}

void StatisticsPanel::DrawTable() const
{
    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t tableX = x + 20;
    const int16_t tableY = y + 24;
    const int16_t tableW = GetWidth() - 200;
    const int16_t rowH = 42;
    const int16_t rows = 6;
    const int16_t tableH = rowH * rows;
    const int16_t statColW = 162;
    const int16_t voltageColW = 226;
    const int16_t statusX = x + GetWidth() - 174;
    const int16_t statusW = 154;

    GD.SaveContext();

    GD.ColorRGB(Theme::PanelBackground);
    GD.Begin(RECTS);
    GD.Vertex2f(tableX, tableY);
    GD.Vertex2f(tableX + tableW, tableY + tableH);
    GD.Vertex2f(statusX, tableY);
    GD.Vertex2f(statusX + statusW, tableY + tableH);

    GD.ColorRGB(Theme::ToolbarButtonBackground);
    GD.Begin(RECTS);
    GD.Vertex2f(tableX, tableY);
    GD.Vertex2f(tableX + tableW, tableY + rowH);

    GD.ColorRGB(Theme::PanelBorder);
    GD.LineWidth(8);
    GD.Begin(LINES);

    for (int16_t i = 0; i <= rows; ++i)
    {
        const int16_t lineY = tableY + (rowH * i);
        GD.Vertex2f(tableX, lineY);
        GD.Vertex2f(tableX + tableW, lineY);
    }

    const int16_t voltageDividerX = tableX + statColW;
    const int16_t currentDividerX = voltageDividerX + voltageColW;

    GD.Vertex2f(tableX, tableY);
    GD.Vertex2f(tableX, tableY + tableH);
    GD.Vertex2f(voltageDividerX, tableY);
    GD.Vertex2f(voltageDividerX, tableY + tableH);
    GD.Vertex2f(currentDividerX, tableY);
    GD.Vertex2f(currentDividerX, tableY + tableH);
    GD.Vertex2f(tableX + tableW, tableY);
    GD.Vertex2f(tableX + tableW, tableY + tableH);

    GD.Vertex2f(statusX, tableY);
    GD.Vertex2f(statusX + statusW, tableY);
    GD.Vertex2f(statusX + statusW, tableY);
    GD.Vertex2f(statusX + statusW, tableY + tableH);
    GD.Vertex2f(statusX + statusW, tableY + tableH);
    GD.Vertex2f(statusX, tableY + tableH);
    GD.Vertex2f(statusX, tableY + tableH);
    GD.Vertex2f(statusX, tableY);

    GD.RestoreContext();
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
