#include "XYGraphPopup.h"

#include <math.h>
#include <stdio.h>
#include "../Core/GDClass.h"
#include "../Core/Theme.h"

namespace
{
XYGraphPopup* g_activeXYGraphPopup = nullptr;
}

XYGraphPopup::XYGraphPopup()
    : Panel(),
      _system(&System),
      _xAxis(XYGraphAxis::Voltage),
      _yAxis(XYGraphAxis::Current),
      _onClose(nullptr)
{
    SetShowBackground(false);
    SetShowBorder(false);
    SetShowTitle(false);
    SetVisible(false);

    _titleLabel.SetFont(Theme::FontPopupTitle);
    _titleLabel.SetTextColor(Theme::HeaderText);
    _titleLabel.SetTextOptions(OPT_CENTERX);
    _titleLabel.SetText("XY GRAPH");

    _xAxisButton.SetFont(Theme::FontButton);
    _xAxisButton.SetTextOptions(OPT_CENTER);
    _xAxisButton.SetOnClick(OnAxisClick);

    _yAxisButton.SetFont(Theme::FontButton);
    _yAxisButton.SetTextOptions(OPT_CENTER);
    _yAxisButton.SetOnClick(OnAxisClick);

    _closeButton.SetFont(Theme::FontButton);
    _closeButton.SetTextOptions(OPT_CENTER);
    _closeButton.SetText("Close");
    _closeButton.SetOnClick(OnCloseClick);

    AddChild(&_titleLabel);
    AddChild(&_xAxisButton);
    AddChild(&_yAxisButton);
    AddChild(&_closeButton);
}

XYGraphPopup::XYGraphPopup(int16_t x, int16_t y, int16_t w, int16_t h)
    : XYGraphPopup()
{
    SetBounds(x, y, w, h);
}

void XYGraphPopup::SetSystem(SystemClass* system)
{
    _system = (system != nullptr) ? system : &System;
}

void XYGraphPopup::Open()
{
    g_activeXYGraphPopup = this;
    SetVisible(true);
    ClearFocus();
}

void XYGraphPopup::Close()
{
    SetVisible(false);
    if (g_activeXYGraphPopup == this)
    {
        g_activeXYGraphPopup = nullptr;
    }

    if (_onClose != nullptr)
    {
        _onClose(this);
    }
}

bool XYGraphPopup::IsOpen() const
{
    return IsVisible();
}

void XYGraphPopup::SetOnClose(XYGraphPopupCloseHandler handler)
{
    _onClose = handler;
}

void XYGraphPopup::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    _titleLabel.SetBounds(GetX() + (GetWidth() / 2), GetY() + 12, 0, 24);
    snprintf(_textBuffer, sizeof(_textBuffer), "X: %s", GetAxisText(_xAxis));
    _xAxisButton.SetText(_textBuffer);
    _xAxisButton.SetBounds(GetX() + 20, GetY() + 10, 180, 40);

    snprintf(_textBuffer, sizeof(_textBuffer), "Y: %s", GetAxisText(_yAxis));
    _yAxisButton.SetText(_textBuffer);
    _yAxisButton.SetBounds(GetX() + 210, GetY() + 10, 180, 40);
    _closeButton.SetBounds(GetX() + GetWidth() - 140, GetY() + 10, 120, 40);

    GD.ColorRGB(0x05080B);
    GD.Begin(RECTS);
    GD.Vertex2f(GetX(), GetY());
    GD.Vertex2f(GetX() + GetWidth(), GetY() + GetHeight());

    const int16_t plotX = GetX() + 92;
    const int16_t plotY = GetY() + 64;
    const int16_t plotW = GetWidth() - 120;
    const int16_t plotH = GetHeight() - 118;

    DrawGrid(plotX, plotY, plotW, plotH);

    float minX = 0.0f;
    float maxX = 1.0f;
    float minY = 0.0f;
    float maxY = 1.0f;
    const bool haveX = GetAxisRange(_xAxis, &minX, &maxX);
    const bool haveY = GetAxisRange(_yAxis, &minY, &maxY);

    DrawXAxisLabels(plotX, plotY, plotW, plotH, minX, maxX);
    DrawYAxisLabels(plotX, plotY, plotH, minY, maxY);

    if (haveX && haveY)
    {
        DrawTrace(plotX, plotY, plotW, plotH, minX, maxX, minY, maxY);
    }

    GD.ColorRGB(Theme::HeaderText);
    GD.cmd_text(plotX + (plotW / 2), GetY() + GetHeight() - 18, Theme::FontButton, OPT_CENTERX, GetAxisText(_xAxis));
    GD.cmd_text(GetX() + 18, plotY - 10, Theme::FontButton, 0, GetAxisText(_yAxis));

    Container::Draw();
}

void XYGraphPopup::OnAxisClick(Button* sender)
{
    if (sender == nullptr || g_activeXYGraphPopup == nullptr)
    {
        return;
    }

    g_activeXYGraphPopup->HandleAxisClick(sender);
}

void XYGraphPopup::OnCloseClick(Button* sender)
{
    if (sender == nullptr || g_activeXYGraphPopup == nullptr)
    {
        return;
    }

    g_activeXYGraphPopup->HandleCloseClick(sender);
}

void XYGraphPopup::HandleAxisClick(Button* sender)
{
    if (sender == &_xAxisButton)
    {
        _xAxis = GetNextAxis(_xAxis);
    }
    else if (sender == &_yAxisButton)
    {
        _yAxis = GetNextAxis(_yAxis);
    }
}

void XYGraphPopup::HandleCloseClick(Button* sender)
{
    if (sender == &_closeButton)
    {
        Close();
    }
}

void XYGraphPopup::DrawGrid(int16_t plotX, int16_t plotY, int16_t plotW, int16_t plotH) const
{
    const int16_t rows = 6;
    const int16_t cols = 8;

    GD.ColorRGB(0x4E5C68);
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

void XYGraphPopup::DrawTrace(int16_t plotX, int16_t plotY, int16_t plotW, int16_t plotH,
                             float minX, float maxX, float minY, float maxY)
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

    const float rangeX = maxX - minX;
    const float rangeY = maxY - minY;
    if (rangeX <= 0.0f || rangeY <= 0.0f)
    {
        return;
    }

    GD.ColorRGB(Theme::SourceValueBlue);
    GD.LineWidth(10);
    GD.Begin(LINE_STRIP);

    for (uint16_t i = 0; i < count; ++i)
    {
        bool validX = false;
        bool validY = false;
        const float sampleX = GetAxisValue(_plotSamples[i], _xAxis, &validX);
        const float sampleY = GetAxisValue(_plotSamples[i], _yAxis, &validY);
        if (!validX || !validY)
        {
            continue;
        }

        const float normalizedX = (sampleX - minX) / rangeX;
        const float normalizedY = (sampleY - minY) / rangeY;
        const int16_t x = plotX + static_cast<int16_t>(normalizedX * static_cast<float>(plotW));
        const int16_t y = plotY + plotH - static_cast<int16_t>(normalizedY * static_cast<float>(plotH));
        GD.Vertex2f(x, y);
    }
}

void XYGraphPopup::DrawXAxisLabels(int16_t plotX, int16_t plotY, int16_t plotW, int16_t plotH, float minX, float maxX)
{
    const int16_t cols = 4;
    const float step = (maxX - minX) / static_cast<float>(cols);
    DisplayFormatState state;
    state.Reset();

    for (int16_t i = 0; i <= cols; ++i)
    {
        const float value = minX + (step * static_cast<float>(i));
        char buffer[24];
        FormatAxisLabel(_xAxis, value, buffer, sizeof(buffer), GetShowSign(_xAxis), &state);
        GD.ColorRGB(Theme::HeaderText);
        GD.cmd_text(plotX + ((plotW * i) / cols), plotY + plotH + 10, Theme::FontButton, OPT_CENTERX, buffer);
    }
}

void XYGraphPopup::DrawYAxisLabels(int16_t plotX, int16_t plotY, int16_t plotH, float minY, float maxY)
{
    const int16_t rows = 6;
    const float step = (maxY - minY) / static_cast<float>(rows);
    DisplayFormatState state;
    state.Reset();

    for (int16_t i = 0; i <= rows; ++i)
    {
        const float value = maxY - (step * static_cast<float>(i));
        char buffer[24];
        FormatAxisLabel(_yAxis, value, buffer, sizeof(buffer), GetShowSign(_yAxis), &state);
        GD.ColorRGB(Theme::HeaderText);
        GD.cmd_text(plotX - 82, plotY + ((plotH * i) / rows), Theme::FontButton, OPT_CENTERY, buffer);
    }
}

bool XYGraphPopup::GetAxisRange(XYGraphAxis axis, float* minValue, float* maxValue)
{
    if (_system == nullptr || minValue == nullptr || maxValue == nullptr || _system->Buffer.IsEmpty())
    {
        *minValue = 0.0f;
        *maxValue = 1.0f;
        return false;
    }

    const uint16_t count = _system->Buffer.CopyLatest(_plotSamples, MaxPlotSamples);
    bool haveValue = false;

    for (uint16_t i = 0; i < count; ++i)
    {
        bool valid = false;
        const float value = GetAxisValue(_plotSamples[i], axis, &valid);
        if (!valid)
        {
            continue;
        }

        if (!haveValue)
        {
            *minValue = value;
            *maxValue = value;
            haveValue = true;
        }
        else
        {
            if (value < *minValue) *minValue = value;
            if (value > *maxValue) *maxValue = value;
        }
    }

    if (!haveValue)
    {
        *minValue = 0.0f;
        *maxValue = 1.0f;
        return false;
    }

    if (fabsf(*maxValue - *minValue) < 1e-9f)
    {
        *maxValue += 0.001f;
        *minValue -= 0.001f;
    }

    return true;
}

float XYGraphPopup::GetAxisValue(const BufferSample& sample, XYGraphAxis axis, bool* valid) const
{
    bool isValid = true;
    float value = 0.0f;

    switch (axis)
    {
        case XYGraphAxis::Time:
            value = static_cast<float>(sample.TimeMs);
            break;
        case XYGraphAxis::Voltage:
            value = sample.Voltage;
            break;
        case XYGraphAxis::Current:
            value = sample.Current;
            break;
        case XYGraphAxis::Resistance:
            isValid = sample.ResistanceValid;
            value = sample.Resistance;
            break;
        case XYGraphAxis::Power:
            value = sample.Power;
            break;
        default:
            value = 0.0f;
            break;
    }

    if (valid != nullptr)
    {
        *valid = isValid;
    }

    return value;
}

void XYGraphPopup::FormatAxisLabel(XYGraphAxis axis, float value, char* buffer, size_t bufferSize,
                                   bool showSign, DisplayFormatState* state)
{
    if (axis == XYGraphAxis::Time)
    {
        if (fabsf(value) >= 1000.0f)
        {
            snprintf(buffer, bufferSize, "%.2f s", value / 1000.0f);
        }
        else
        {
            snprintf(buffer, bufferSize, "%.0f ms", value);
        }
        return;
    }

    char valueText[16];
    char unitText[8];
    DisplayQuantity quantity = GetDisplayQuantity(axis);
    DisplayFormat::FormatValue(quantity, value, *state, showSign, valueText, sizeof(valueText), unitText, sizeof(unitText));
    snprintf(buffer, bufferSize, "%s %s", valueText, unitText);
}

const char* XYGraphPopup::GetAxisText(XYGraphAxis axis) const
{
    switch (axis)
    {
        case XYGraphAxis::Time: return "Time";
        case XYGraphAxis::Voltage: return "Voltage";
        case XYGraphAxis::Current: return "Current";
        case XYGraphAxis::Resistance: return "Resistance";
        case XYGraphAxis::Power: return "Power";
        default: return "--";
    }
}

XYGraphAxis XYGraphPopup::GetNextAxis(XYGraphAxis axis) const
{
    switch (axis)
    {
        case XYGraphAxis::Time: return XYGraphAxis::Voltage;
        case XYGraphAxis::Voltage: return XYGraphAxis::Current;
        case XYGraphAxis::Current: return XYGraphAxis::Resistance;
        case XYGraphAxis::Resistance: return XYGraphAxis::Power;
        case XYGraphAxis::Power:
        default:
            return XYGraphAxis::Time;
    }
}

DisplayQuantity XYGraphPopup::GetDisplayQuantity(XYGraphAxis axis) const
{
    switch (axis)
    {
        case XYGraphAxis::Voltage: return DisplayQuantity::Voltage;
        case XYGraphAxis::Current: return DisplayQuantity::Current;
        case XYGraphAxis::Resistance: return DisplayQuantity::Resistance;
        case XYGraphAxis::Power: return DisplayQuantity::Power;
        case XYGraphAxis::Time:
        default:
            return DisplayQuantity::Voltage;
    }
}

bool XYGraphPopup::GetShowSign(XYGraphAxis axis) const
{
    return axis != XYGraphAxis::Resistance && axis != XYGraphAxis::Time;
}
