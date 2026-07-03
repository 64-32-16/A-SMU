#include "Panel.h"
#include "GDClass.h"
#include "Theme.h"

/**
 * @brief Erstellt ein leeres Panel.
 */
Panel::Panel()
    : Container(),
      _title(""),
      _backgroundColor(Theme::PanelBackground),
      _borderColor(Theme::PanelBorder),
      _titleColor(Theme::HeaderText),
      _titleFont(Theme::FontButton),
      _showBackground(true),
      _showBorder(true),
      _showTitle(false)
{
}

/**
 * @brief Erstellt ein Panel mit Position und Größe.
 */
Panel::Panel(int16_t x, int16_t y, int16_t w, int16_t h)
    : Container(x, y, w, h),
      _title(""),
      _backgroundColor(Theme::PanelBackground),
      _borderColor(Theme::PanelBorder),
      _titleColor(Theme::HeaderText),
      _titleFont(Theme::FontButton),
      _showBackground(true),
      _showBorder(true),
      _showTitle(false)
{
}

/**
 * @brief Zeichnet Hintergrund, Rahmen, Titel und anschließend alle Kinder.
 */
void Panel::Draw()
{
    if (!_visible)
    {
        return;
    }

    if (_showBackground)
    {
        DrawBackground();
    }

    if (_showBorder)
    {
        DrawBorder();
    }

    if (_showTitle)
    {
        DrawTitle();
    }

    Container::Draw();
}

/**
 * @brief Setzt den Titel.
 */
void Panel::SetTitle(const char* title)
{
    _title = title;
}

/**
 * @brief Liefert den Titel.
 */
const char* Panel::GetTitle() const
{
    return _title;
}

/**
 * @brief Setzt die Hintergrundfarbe.
 */
void Panel::SetBackgroundColor(uint32_t color)
{
    _backgroundColor = color;
}

/**
 * @brief Liefert die Hintergrundfarbe.
 */
uint32_t Panel::GetBackgroundColor() const
{
    return _backgroundColor;
}

/**
 * @brief Setzt die Rahmenfarbe.
 */
void Panel::SetBorderColor(uint32_t color)
{
    _borderColor = color;
}

/**
 * @brief Liefert die Rahmenfarbe.
 */
uint32_t Panel::GetBorderColor() const
{
    return _borderColor;
}

/**
 * @brief Setzt die Titelfarbe.
 */
void Panel::SetTitleColor(uint32_t color)
{
    _titleColor = color;
}

/**
 * @brief Liefert die Titelfarbe.
 */
uint32_t Panel::GetTitleColor() const
{
    return _titleColor;
}

/**
 * @brief Setzt den Font für den Titel.
 */
void Panel::SetTitleFont(uint8_t font)
{
    _titleFont = font;
}

/**
 * @brief Liefert den Font für den Titel.
 */
uint8_t Panel::GetTitleFont() const
{
    return _titleFont;
}

/**
 * @brief Aktiviert oder deaktiviert die Hintergrundzeichnung.
 */
void Panel::SetShowBackground(bool show)
{
    _showBackground = show;
}

/**
 * @brief Aktiviert oder deaktiviert die Rahmendarstellung.
 */
void Panel::SetShowBorder(bool show)
{
    _showBorder = show;
}

/**
 * @brief Aktiviert oder deaktiviert die Titeldarstellung.
 */
void Panel::SetShowTitle(bool show)
{
    _showTitle = show;
}

/**
 * @brief Liefert, ob der Hintergrund gezeichnet wird.
 */
bool Panel::GetShowBackground() const
{
    return _showBackground;
}

/**
 * @brief Liefert, ob der Rahmen gezeichnet wird.
 */
bool Panel::GetShowBorder() const
{
    return _showBorder;
}

/**
 * @brief Liefert, ob der Titel gezeichnet wird.
 */
bool Panel::GetShowTitle() const
{
    return _showTitle;
}

/**
 * @brief Zeichnet den Hintergrund des Panels.
 */
void Panel::DrawBackground()
{
    GD.ColorRGB(_backgroundColor);
    GD.Begin(RECTS);
    GD.Vertex2f(_x, _y);
    GD.Vertex2f(_x + _w, _y + _h);
}

/**
 * @brief Zeichnet den Rahmen des Panels.
 */
void Panel::DrawBorder()
{
    GD.ColorRGB(_borderColor);
    GD.LineWidth(16);

    GD.Begin(LINE_STRIP);
    GD.Vertex2f(_x, _y);
    GD.Vertex2f(_x + _w, _y);
    GD.Vertex2f(_x + _w, _y + _h);
    GD.Vertex2f(_x, _y + _h);
    GD.Vertex2f(_x, _y);
}

/**
 * @brief Zeichnet den Titel des Panels.
 */
void Panel::DrawTitle()
{
    GD.ColorRGB(_titleColor);
    GD.cmd_text(_x + 8, _y + 6, _titleFont, 0, _title);
}
