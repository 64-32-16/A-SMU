#include "Button.h"
#include "../Core/GDClass.h"
#include "../Core/Theme.h"
#include "../../Core/Beeper.h"

#include <string.h>

/**
 * @brief Erstellt einen leeren Button.
 */
Button::Button()
    : Control(),
      _text(""),
      _font(27),
      _textOptions(OPT_CENTER),
      _pressed(false),
      _trackingTouch(false),
      _beepOnClick(true),
      _backgroundColor(Theme::ButtonGray),
      _disabledBackgroundColor(Theme::ButtonGrayDisabled),
      _textColor(Theme::ButtonText),
      _disabledTextColor(Theme::ButtonTextDisabled),
      _onClick(nullptr)
{
}

/**
 * @brief Erstellt einen Button mit Position, Größe und Text.
 */
Button::Button(int16_t x, int16_t y, int16_t w, int16_t h, const char* text)
    : Control(x, y, w, h),
      _text(text),
      _font(27),
      _textOptions(OPT_CENTER),
      _pressed(false),
      _trackingTouch(false),
      _beepOnClick(true),
      _backgroundColor(Theme::ButtonGray),
      _disabledBackgroundColor(Theme::ButtonGrayDisabled),
      _textColor(Theme::ButtonText),
      _disabledTextColor(Theme::ButtonTextDisabled),
      _onClick(nullptr)
{
}

/**
 * @brief Zeichnet den Button.
 */
void Button::Draw()
{
    if (!_visible)
    {
        return;
    }

    uint16_t options = _textOptions;

    if (_pressed)
    {
        options |= OPT_FLAT;
    }

    GD.cmd_fgcolor(_enabled ? _backgroundColor : _disabledBackgroundColor);
    GD.ColorRGB(_enabled ? _textColor : _disabledTextColor);

    const char* newline = strchr(_text, '\n');
    if (newline != nullptr)
    {
        char line1[24];
        char line2[24];
        const size_t line1Len = static_cast<size_t>(newline - _text);
        const size_t copyLine1Len = (line1Len < (sizeof(line1) - 1)) ? line1Len : (sizeof(line1) - 1);

        memcpy(line1, _text, copyLine1Len);
        line1[copyLine1Len] = '\0';

        strncpy(line2, newline + 1, sizeof(line2) - 1);
        line2[sizeof(line2) - 1] = '\0';

        GD.cmd_button(_x, _y, _w, _h, _font, options, "");
        GD.ColorRGB(_enabled ? _textColor : _disabledTextColor);
        GD.cmd_text(_x + (_w / 2), _y + (_h / 2) - 12, _font, OPT_CENTER, line1);
        GD.cmd_text(_x + (_w / 2), _y + (_h / 2) + 12, _font, OPT_CENTER, line2);
        return;
    }

    GD.cmd_button(_x, _y, _w, _h, _font, options, _text);
}

bool Button::IsFocusable() const
{
    return false;
}

/**
 * @brief Verarbeitet den Start eines Touchs innerhalb des Buttons.
 */
void Button::TouchStart(int16_t x, int16_t y)
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

/**
 * @brief Aktualisiert den Pressed-Zustand bei Touch-Bewegung.
 */
void Button::TouchMove(int16_t x, int16_t y)
{
    if (!_enabled || !_visible)
    {
        return;
    }

    if (!_trackingTouch)
    {
        return;
    }

    _pressed = Contains(x, y);
}

/**
 * @brief Verarbeitet das Ende des Touchs und löst ggf. den Click aus.
 */
void Button::TouchEnd(int16_t x, int16_t y)
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
        Click();
    }
}

/**
 * @brief Setzt den Button-Text.
 */
void Button::SetText(const char* text)
{
    _text = text;
}

/**
 * @brief Liefert den Button-Text.
 */
const char* Button::GetText() const
{
    return _text;
}

/**
 * @brief Setzt den Font.
 */
void Button::SetFont(uint8_t font)
{
    _font = font;
}

/**
 * @brief Liefert den Font.
 */
uint8_t Button::GetFont() const
{
    return _font;
}

/**
 * @brief Setzt die Textoptionen.
 */
void Button::SetTextOptions(uint16_t options)
{
    _textOptions = options;
}

/**
 * @brief Liefert die Textoptionen.
 */
uint16_t Button::GetTextOptions() const
{
    return _textOptions;
}

/**
 * @brief Setzt den Klick-Callback.
 */
void Button::SetOnClick(ButtonClickHandler handler)
{
    _onClick = handler;
}

void Button::SetTextColor(uint32_t color)
{
    _textColor = color;
}

uint32_t Button::GetTextColor() const
{
    return _textColor;
}

void Button::SetBackgroundColor(uint32_t color)
{
    _backgroundColor = color;
}

uint32_t Button::GetBackgroundColor() const
{
    return _backgroundColor;
}

void Button::SetDisabledBackgroundColor(uint32_t color)
{
    _disabledBackgroundColor = color;
}

uint32_t Button::GetDisabledBackgroundColor() const
{
    return _disabledBackgroundColor;
}

void Button::SetDisabledTextColor(uint32_t color)
{
    _disabledTextColor = color;
}

uint32_t Button::GetDisabledTextColor() const
{
    return _disabledTextColor;
}

/**
 * @brief Liefert true, wenn der Button aktuell gedrückt ist.
 */
bool Button::IsPressed() const
{
    return _pressed;
}

/**
 * @brief Aktiviert oder deaktiviert den Beep bei Click.
 */
void Button::SetBeepOnClick(bool enabled)
{
    _beepOnClick = enabled;
}

/**
 * @brief Liefert den Beep-On-Click-Zustand.
 */
bool Button::GetBeepOnClick() const
{
    return _beepOnClick;
}

/**
 * @brief Löst den Click aus.
 */
void Button::Click()
{
    if (_beepOnClick)
    {
        Beeper.Click();
    }

    if (_onClick != nullptr)
    {
        _onClick(this);
    }
}
