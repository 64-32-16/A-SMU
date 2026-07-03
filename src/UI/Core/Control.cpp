#include "Control.h"

/**
 * @brief Erstellt ein leeres Control mit Defaultwerten.
 */
Control::Control()
    : _x(0),
      _y(0),
      _w(0),
      _h(0),
      _visible(true),
      _enabled(true),
      _focused(false)
{
}

/**
 * @brief Erstellt ein Control mit Position und Größe.
 */
Control::Control(int16_t x, int16_t y, int16_t w, int16_t h)
    : _x(x),
      _y(y),
      _w(w),
      _h(h),
      _visible(true),
      _enabled(true),
      _focused(false)
{
}

/**
 * @brief Standardbehandlung für TouchStart.
 */
void Control::TouchStart(int16_t x, int16_t y)
{
    (void)x;
    (void)y;
}

/**
 * @brief Standardbehandlung für TouchMove.
 */
void Control::TouchMove(int16_t x, int16_t y)
{
    (void)x;
    (void)y;
}

/**
 * @brief Standardbehandlung für TouchEnd.
 */
void Control::TouchEnd(int16_t x, int16_t y)
{
    (void)x;
    (void)y;
}

bool Control::IsFocusable() const
{
    return false;
}

void Control::SetFocused(bool focused)
{
    _focused = focused;
}

bool Control::IsFocused() const
{
    return _focused;
}

/**
 * @brief Prüft, ob ein Punkt innerhalb des Controls liegt.
 */
bool Control::Contains(int16_t px, int16_t py) const
{
    return (px >= _x) &&
           (py >= _y) &&
           (px < (_x + _w)) &&
           (py < (_y + _h));
}

/**
 * @brief Setzt Position und Größe.
 */
void Control::SetBounds(int16_t x, int16_t y, int16_t w, int16_t h)
{
    _x = x;
    _y = y;
    _w = w;
    _h = h;
}

/**
 * @brief Setzt die Position.
 */
void Control::SetPosition(int16_t x, int16_t y)
{
    _x = x;
    _y = y;
}

/**
 * @brief Setzt die Größe.
 */
void Control::SetSize(int16_t w, int16_t h)
{
    _w = w;
    _h = h;
}

/**
 * @brief Setzt die X-Position.
 */
void Control::SetX(int16_t x)
{
    _x = x;
}

/**
 * @brief Setzt die Y-Position.
 */
void Control::SetY(int16_t y)
{
    _y = y;
}

/**
 * @brief Setzt die Breite.
 */
void Control::SetWidth(int16_t w)
{
    _w = w;
}

/**
 * @brief Setzt die Höhe.
 */
void Control::SetHeight(int16_t h)
{
    _h = h;
}

/**
 * @brief Setzt den Aktiv-Zustand.
 */
void Control::SetEnabled(bool enabled)
{
    _enabled = enabled;
}

/**
 * @brief Setzt die Sichtbarkeit.
 */
void Control::SetVisible(bool visible)
{
    _visible = visible;
}

/**
 * @brief Liefert die X-Position.
 */
int16_t Control::GetX() const
{
    return _x;
}

/**
 * @brief Liefert die Y-Position.
 */
int16_t Control::GetY() const
{
    return _y;
}

/**
 * @brief Liefert die Breite.
 */
int16_t Control::GetWidth() const
{
    return _w;
}

/**
 * @brief Liefert die Höhe.
 */
int16_t Control::GetHeight() const
{
    return _h;
}

/**
 * @brief Liefert den Aktiv-Zustand.
 */
bool Control::IsEnabled() const
{
    return _enabled;
}

/**
 * @brief Liefert den Sichtbarkeits-Zustand.
 */
bool Control::IsVisible() const
{
    return _visible;
}
