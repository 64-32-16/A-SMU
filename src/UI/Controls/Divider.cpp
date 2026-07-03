#include "Divider.h"
#include "../Core/GDClass.h"
#include "../Core/Theme.h"

Divider::Divider()
    : Control(),
      _color(Theme::LabelGreen),
      _thickness(16)
{
    SetHeight(1);
}

Divider::Divider(int16_t x, int16_t y, int16_t w, uint32_t color)
    : Control(x, y, w, 1),
      _color(color),
      _thickness(16)
{
}

void Divider::Draw()
{
    if (!IsVisible())
    {
        return;
    }

    GD.ColorRGB(_color);
    GD.Begin(LINES);
    GD.LineWidth(_thickness);
    GD.Vertex2f(GetX(), GetY());
    GD.Vertex2f(GetX() + GetWidth(), GetY());
}

void Divider::SetColor(uint32_t color)
{
    _color = color;
}

uint32_t Divider::GetColor() const
{
    return _color;
}

void Divider::SetThickness(uint16_t thickness)
{
    _thickness = thickness;
}

uint16_t Divider::GetThickness() const
{
    return _thickness;
}
