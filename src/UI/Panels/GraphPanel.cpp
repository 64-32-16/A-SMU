#include "GraphPanel.h"

#include <math.h>
#include <stdio.h>
#include "../Core/GDClass.h"
#include "../Core/Theme.h"

GraphPanel::GraphPanel()
    : Panel(),
      _system(&System)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);
}

GraphPanel::GraphPanel(int16_t x, int16_t y, int16_t w, int16_t h)
    : GraphPanel()
{
    SetBounds(x, y, w, h);
}

void GraphPanel::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

SystemClass* GraphPanel::GetSystem() const
{
    return _system;
}

void GraphPanel::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    const int16_t x = GetX();
    const int16_t y = GetY();
    const int16_t w = GetWidth();
    const int16_t h = GetHeight();
    const int16_t plotX = x + 96;
    const int16_t plotY = y + 12;
    const int16_t plotW = w - 114;
    const int16_t plotH = h - 24;
    const MeasureMode mode = (_system != nullptr) ? _system->GetMeasureMode() : MeasureMode::Current;
    const DisplayQuantity quantity = GetDisplayQuantity(mode);
    const bool showSign = GetShowSign(mode);

    DrawGrid(plotX, plotY, plotW, plotH);

    float minValue = -1.0f;
    float maxValue = 1.0f;
    if (_system != nullptr && !_system->Buffer.IsEmpty())
    {
        const uint16_t count = _system->Buffer.CopyLatest(_plotSamples, MaxPlotSamples);
        bool haveValue = false;

        for (uint16_t i = 0; i < count; ++i)
        {
            bool valid = false;
            const float value = GetSampleValue(_plotSamples[i], mode, &valid);
            if (!valid)
            {
                continue;
            }

            if (!haveValue)
            {
                minValue = value;
                maxValue = value;
                haveValue = true;
            }
            else
            {
                if (value < minValue) minValue = value;
                if (value > maxValue) maxValue = value;
            }
        }

        if (!haveValue)
        {
            minValue = -1.0f;
            maxValue = 1.0f;
        }

        if (fabsf(maxValue - minValue) < 1e-9f)
        {
            maxValue += 0.001f;
            minValue -= 0.001f;
        }
    }

    DrawLabels(plotX, plotY, plotH, minValue, maxValue, quantity, showSign);
    DrawTrace(plotX, plotY, plotW, plotH, minValue, maxValue, mode);
}

void GraphPanel::DrawGrid(int16_t plotX, int16_t plotY, int16_t plotW, int16_t plotH) const
{
    const int16_t rows = 6;
    const int16_t cols = 10;

    GD.ColorRGB(0x8A8A8A);
    GD.LineWidth(4);
    GD.Begin(LINES);

    for (int16_t r = 0; r <= rows; ++r)
    {
        const int16_t y = plotY + ((plotH * r) / rows);
        GD.Vertex2f(plotX, y);
        GD.Vertex2f(plotX + plotW, y);
    }

    for (int16_t c = 0; c <= cols; ++c)
    {
        const int16_t x = plotX + ((plotW * c) / cols);
        GD.Vertex2f(x, plotY);
        GD.Vertex2f(x, plotY + plotH);
    }
}

void GraphPanel::DrawLabels(int16_t plotX, int16_t plotY, int16_t plotH, float minValue, float maxValue,
                            DisplayQuantity quantity, bool showSign)
{
    const int16_t rows = 6;
    const float step = (maxValue - minValue) / static_cast<float>(rows);
    DisplayFormatState* axisState = &_currentAxisState;

    switch (quantity)
    {
        case DisplayQuantity::Voltage:
            axisState = &_voltageAxisState;
            break;
        case DisplayQuantity::Current:
            axisState = &_currentAxisState;
            break;
        case DisplayQuantity::Resistance:
            axisState = &_resistanceAxisState;
            break;
        case DisplayQuantity::Power:
            axisState = &_powerAxisState;
            break;
        default:
            break;
    }

    axisState->Reset();

    for (int16_t i = 0; i <= rows; ++i)
    {
        const float value = maxValue - (step * static_cast<float>(i));
        char valueText[16];
        char unitText[8];
        DisplayFormat::FormatValue(quantity, value, *axisState, showSign,
                                   valueText, sizeof(valueText), unitText, sizeof(unitText));
        snprintf(_labelBuffers[i], sizeof(_labelBuffers[i]), "%s %s", valueText, unitText);
        GD.ColorRGB(Theme::HeaderText);
        GD.cmd_text(plotX - 88, plotY + ((plotH * i) / rows), Theme::FontButton, OPT_CENTERY, _labelBuffers[i]);
    }
}

void GraphPanel::DrawTrace(int16_t plotX, int16_t plotY, int16_t plotW, int16_t plotH,
                           float minValue, float maxValue, MeasureMode mode)
{
    if (_system == nullptr)
    {
        return;
    }

    const uint16_t count = _system->Buffer.CopyLatest(_plotSamples, MaxPlotSamples);
    if (count < 2)
    {
        return;
    }

    const float range = maxValue - minValue;
    if (range <= 0.0f)
    {
        return;
    }

    GD.ColorRGB(Theme::LabelGreen);
    GD.LineWidth(12);
    GD.Begin(LINE_STRIP);

    for (uint16_t i = 0; i < count; ++i)
    {
        bool valid = false;
        const float sampleValue = GetSampleValue(_plotSamples[i], mode, &valid);
        if (!valid)
        {
            continue;
        }

        const int16_t x = plotX + static_cast<int16_t>((static_cast<int32_t>(plotW) * i) / (count - 1));
        const float normalized = (sampleValue - minValue) / range;
        const int16_t y = plotY + plotH - static_cast<int16_t>(normalized * static_cast<float>(plotH));
        GD.Vertex2f(x, y);
    }
}

float GraphPanel::GetSampleValue(const BufferSample& sample, MeasureMode mode, bool* valid) const
{
    bool isValid = true;
    float value = 0.0f;

    switch (mode)
    {
        case MeasureMode::Voltage:
            value = sample.Voltage;
            break;
        case MeasureMode::Current:
            value = sample.Current;
            break;
        case MeasureMode::Resistance:
            isValid = sample.ResistanceValid;
            value = sample.Resistance;
            break;
        case MeasureMode::Power:
            value = sample.Power;
            break;
        default:
            value = sample.Current;
            break;
    }

    if (valid != nullptr)
    {
        *valid = isValid;
    }

    return value;
}

DisplayQuantity GraphPanel::GetDisplayQuantity(MeasureMode mode) const
{
    switch (mode)
    {
        case MeasureMode::Voltage: return DisplayQuantity::Voltage;
        case MeasureMode::Current: return DisplayQuantity::Current;
        case MeasureMode::Resistance: return DisplayQuantity::Resistance;
        case MeasureMode::Power: return DisplayQuantity::Power;
        default: return DisplayQuantity::Current;
    }
}

bool GraphPanel::GetShowSign(MeasureMode mode) const
{
    return mode != MeasureMode::Resistance;
}
