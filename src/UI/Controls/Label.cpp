#include "Label.h"
#include "../Core/GDClass.h"

/**
 * @brief Erstellt ein leeres Label.
 */
Label::Label()
    : Control(),
      _text(""),
      _font(27),
      _textOptions(0),
      _textColor(0xFFFFFF)
{
}

/**
 * @brief Erstellt ein Label mit Position, Größe und Text.
 */
Label::Label(int16_t x, int16_t y, int16_t w, int16_t h, const char* text)
    : Control(x, y, w, h),
      _text(text),
      _font(27),
      _textOptions(0),
      _textColor(0xFFFFFF)
{
}

/**
 * @brief Zeichnet das Label.
 */
void Label::Draw()
{
    if (!_visible)
    {
        return;
    }

    GD.ColorRGB(_textColor);
    GD.cmd_text(_x, _y, _font, _textOptions, _text);
}

/**
 * @brief Setzt den Label-Text.
 */
void Label::SetText(const char* text)
{
    _text = text;
}

/**
 * @brief Liefert den Label-Text.
 */
const char* Label::GetText() const
{
    return _text;
}

/**
 * @brief Setzt den Font.
 */
void Label::SetFont(uint8_t font)
{
    _font = font;
}

/**
 * @brief Liefert den Font.
 */
uint8_t Label::GetFont() const
{
    return _font;
}

/**
 * @brief Setzt die Textoptionen.
 */
void Label::SetTextOptions(uint16_t options)
{
    _textOptions = options;
}

/**
 * @brief Liefert die Textoptionen.
 */
uint16_t Label::GetTextOptions() const
{
    return _textOptions;
}

/**
 * @brief Setzt die Textfarbe.
 */
void Label::SetTextColor(uint32_t color)
{
    _textColor = color;
}

/**
 * @brief Liefert die Textfarbe.
 */
uint32_t Label::GetTextColor() const
{
    return _textColor;
}