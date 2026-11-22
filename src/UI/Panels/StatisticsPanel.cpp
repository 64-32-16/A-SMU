#include "StatisticsPanel.h"

#include <stdio.h>
#include "../Core/GDClass.h"
#include "../Core/Theme.h"

namespace
{
constexpr uint8_t kStatisticsFont = 28;
constexpr uint8_t kFooterFont = kStatisticsFont;
void FormatValue(DisplayQuantity quantity, float value, bool signedValue, char* text, size_t size)
{
    DisplayFormatState state;
    char number[24];
    char unit[12];
    DisplayFormat::FormatValue(quantity, value, state, signedValue,
                              number, sizeof(number), unit, sizeof(unit));
    snprintf(text, size, "%s %s", number, unit);
}
}

StatisticsPanel::StatisticsPanel() : Panel(), _system(&System)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);
    _clearButton.SetFont(Theme::FontButton);
    _clearButton.SetText("Clear Active Buffer");
    _clearButton.SetTextColor(Theme::MainButtonText);
    _clearButton.SetBackgroundColor(Theme::MainButtonBackground);
    _clearButton.SetCornerRadius(0);
    AddChild(&_clearButton);
}

StatisticsPanel::StatisticsPanel(int16_t x, int16_t y, int16_t w, int16_t h) : StatisticsPanel()
{
    SetBounds(x, y, w, h);
    UpdateLayout();
}

void StatisticsPanel::SetSystem(SystemClass* system)
{
    _system = system != nullptr ? system : &System;
}

SystemClass* StatisticsPanel::GetSystem() const { return _system; }

void StatisticsPanel::UpdateLayout()
{
    _clearButton.SetBounds(GetX() + GetWidth() / 2 + 12, GetY() + 106,
                          GetWidth() / 2 - 24, 47);
}

void StatisticsPanel::TouchEnd(int16_t x, int16_t y)
{
    const bool clear = IsVisible() && IsEnabled() && _clearButton.IsEnabled() &&
                       _clearButton.IsPressed() && _clearButton.Contains(x, y);
    Panel::TouchEnd(x, y);
    if (clear) _system->ClearMeasurementBuffer();
}

void StatisticsPanel::DrawStatistic(const char* label, float value, bool valid, bool signedValue,
                                   DisplayQuantity quantity, int16_t x, int16_t valueX, int16_t y)
{
    char text[40];
    if (valid) FormatValue(quantity, value, signedValue, text, sizeof(text));
    else snprintf(text, sizeof(text), "---");
    GD.ColorRGB(Theme::SourceBlue);
    GD.cmd_text(x, y, kStatisticsFont, OPT_CENTERY, label);
    GD.ColorRGB(Theme::ToolbarText);
    GD.cmd_text(valueX, y, kStatisticsFont, OPT_CENTERY, text);
}

void StatisticsPanel::Draw()
{
    if (!IsVisible()) return;
    UpdateLayout();
    const BufferStatistics stats = _system->GetMeasurementStatistics();
    DisplayQuantity quantity = DisplayQuantity::Voltage;
    switch (_system->GetMeasureMode())
    {
        case MeasureMode::Current: quantity = DisplayQuantity::Current; break;
        case MeasureMode::Power: quantity = DisplayQuantity::Power; break;
        case MeasureMode::Resistance: quantity = DisplayQuantity::Resistance; break;
        default: break;
    }
    const int16_t x = GetX() + 12;
    const int16_t valueX = GetX() + 196;
    const int16_t right = GetX() + GetWidth() / 2 + 12;
    const int16_t rightValue = right + 128;
    const int16_t y = GetY() + 22;
    const bool valid = stats.Count != 0;
    DrawStatistic("Peak to Peak:", stats.PeakToPeak, valid, false, quantity, x, valueX, y);
    DrawStatistic("Average:", stats.Average, valid, true, quantity, x, valueX, y + 34);
    DrawStatistic("Standard Dev:", stats.StandardDeviation, valid, false, quantity, x, valueX, y + 68);
    DrawStatistic("Minimum:", stats.Minimum, valid, true, quantity, right, rightValue, y);
    DrawStatistic("Maximum:", stats.Maximum, valid, true, quantity, right, rightValue, y + 34);
    GD.ColorRGB(Theme::SourceBlue);
    GD.cmd_text(x, y + 102, kStatisticsFont, OPT_CENTERY, "Span:");
    char span[24];
    snprintf(span, sizeof(span), "%u rdgs", static_cast<unsigned int>(stats.Count));
    GD.ColorRGB(Theme::ToolbarText);
    GD.cmd_text(valueX, y + 102, kStatisticsFont, OPT_CENTERY, span);
    DrawFooter();
    Panel::Draw();
}

void StatisticsPanel::DrawFooter()
{
    const int16_t x = GetX() + 12;
    const int16_t y = GetY() + GetHeight() - 20;
    const bool voltageSource = _system->GetSourceMode() == SourceMode::Voltage;
    char source[40];
    char limit[40];
    FormatValue(voltageSource ? DisplayQuantity::Voltage : DisplayQuantity::Current,
                voltageSource ? _system->GetVoltageSourceValue() : _system->GetCurrentSourceValue(),
                true, source, sizeof(source));
    FormatValue(voltageSource ? DisplayQuantity::Current : DisplayQuantity::Voltage,
                voltageSource ? _system->GetCurrentLimitValue() : _system->GetVoltageLimitValue(),
                false, limit, sizeof(limit));
    const char* range = _system->GetRangeText();
    const char* labels[] = {"Range:", "Source:", "Limit:"};
    const char* values[] = {range, source, limit};
    for (uint8_t i = 0; i < 3; ++i)
    {
        const int16_t columnX = x + i * (GetWidth() / 3);
        GD.ColorRGB(Theme::SourceBlue);
        GD.cmd_text(columnX, y, kFooterFont, OPT_CENTERY, labels[i]);
        GD.ColorRGB(0xB0B0B0);
        GD.cmd_text(columnX + 92, y, kFooterFont, OPT_CENTERY, values[i]);
    }
}
