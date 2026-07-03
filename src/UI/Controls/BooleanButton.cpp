#include "BooleanButton.h"
#include "../Core/Theme.h"
#include "../../Core/Beeper.h"

BooleanButton::BooleanButton()
    : Control(),
      _caption("OUTPUT"),
      _value(false),
      _pressed(false),
      _trackingTouch(false),
      _beepOnChange(true),
      _onChange(nullptr)
{
}

BooleanButton::BooleanButton(int16_t x, int16_t y, int16_t w, int16_t h, const char* caption)
    : Control(x, y, w, h),
      _caption(caption),
      _value(false),
      _pressed(false),
      _trackingTouch(false),
      _beepOnChange(true),
      _onChange(nullptr)
{
}

void BooleanButton::Draw()
{
    if (!_visible)
    {
        return;
    }

    GD.SaveContext();

    const uint32_t textColor = _enabled ? Theme::ToolbarButtonText : Theme::ButtonTextDisabled;
    const bool hasCaption = (_caption != nullptr && _caption[0] != '\0');
    const int16_t labelW = hasCaption ? 68 : 0;
    const int16_t trackH = (_h > 32) ? 26 : (_h - 8);
    const int16_t trackGap = hasCaption ? 8 : 0;
    const int16_t trackW = _w - labelW - trackGap;
    const int16_t trackX = _x + labelW;
    const int16_t trackY = _y + ((_h - trackH) / 2);

    if (hasCaption)
    {
        GD.ColorRGB(textColor);
        GD.cmd_text(_x + 8, _y + (_h / 2), Theme::FontButton, OPT_CENTERY, _caption);
    }

    DrawTrack(trackX, trackY, trackW, trackH);

    GD.RestoreContext();
}

bool BooleanButton::IsFocusable() const
{
    return false;
}

void BooleanButton::TouchStart(int16_t x, int16_t y)
{
    if (!_enabled || !_visible)
    {
        return;
    }

    if (Contains(x, y))
    {
        _pressed = true;
        _trackingTouch = true;
    }
}

void BooleanButton::TouchMove(int16_t x, int16_t y)
{
    if (!_enabled || !_visible || !_trackingTouch)
    {
        return;
    }

    _pressed = Contains(x, y);
}

void BooleanButton::TouchEnd(int16_t x, int16_t y)
{
    if (!_enabled || !_visible)
    {
        _pressed = false;
        _trackingTouch = false;
        return;
    }

    const bool wasTracking = _trackingTouch;
    const bool inside = Contains(x, y);

    _pressed = false;
    _trackingTouch = false;

    if (wasTracking && inside)
    {
        Toggle();
    }
}

void BooleanButton::SetCaption(const char* caption)
{
    _caption = (caption != nullptr) ? caption : "";
}

const char* BooleanButton::GetCaption() const
{
    return _caption;
}

void BooleanButton::SetValue(bool value)
{
    _value = value;
}

bool BooleanButton::GetValue() const
{
    return _value;
}

void BooleanButton::SetOnChange(BooleanButtonChangeHandler handler)
{
    _onChange = handler;
}

void BooleanButton::SetBeepOnChange(bool enabled)
{
    _beepOnChange = enabled;
}

bool BooleanButton::GetBeepOnChange() const
{
    return _beepOnChange;
}

void BooleanButton::Toggle()
{
    _value = !_value;

    if (_beepOnChange)
    {
        Beeper.Click();
    }

    if (_onChange != nullptr)
    {
        _onChange(this, _value);
    }
}

void BooleanButton::DrawTrack(int16_t trackX, int16_t trackY, int16_t trackW, int16_t trackH) const
{
    const int16_t radius = trackH / 2;
    const int16_t centerY = trackY + radius;
    const int16_t thumbRadius = radius - 3;
    const int16_t thumbX = _value
        ? (trackX + trackW - radius)
        : (trackX + radius);
    const int16_t stateTextX = _value
        ? (trackX + radius + 10)
        : (trackX + trackW - radius - 10);

    const uint32_t trackColor = !_enabled
        ? Theme::ButtonGrayDisabled
        : (_value ? Theme::BooleanOnTrack : Theme::BooleanOffTrack);
    const uint32_t thumbColor = !_enabled
        ? Theme::ButtonTextDisabled
        : Theme::BooleanThumb;
    const uint32_t stateTextColor = _value
        ? Theme::BooleanOnText
        : Theme::BooleanOffText;

    GD.ColorRGB(trackColor);
    GD.LineWidth(trackH * 16);
    GD.Begin(LINES);
    GD.Vertex2f(trackX + radius, centerY);
    GD.Vertex2f(trackX + trackW - radius, centerY);

    GD.PointSize(radius * 16);
    GD.Begin(POINTS);
    GD.Vertex2f(trackX + radius, centerY);
    GD.Vertex2f(trackX + trackW - radius, centerY);

    GD.ColorRGB(stateTextColor);
    GD.cmd_text(stateTextX, centerY, Theme::FontButton, OPT_CENTER, _value ? "ON" : "OFF");

    GD.ColorRGB(_pressed ? Theme::BooleanThumbPressed : thumbColor);
    GD.PointSize(thumbRadius * 16);
    GD.Begin(POINTS);
    GD.Vertex2f(thumbX, centerY);
}
