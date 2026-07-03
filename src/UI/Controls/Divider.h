#ifndef _DIVIDER_H_INCLUDED
#define _DIVIDER_H_INCLUDED

#include <Arduino.h>
#include "../Core/Control.h"

class Divider : public Control
{
public:
    Divider();
    Divider(int16_t x, int16_t y, int16_t w, uint32_t color = 0x3FCF72);

    virtual void Draw() override;

    void SetColor(uint32_t color);
    uint32_t GetColor() const;

    void SetThickness(uint16_t thickness);
    uint16_t GetThickness() const;

private:
    uint32_t _color;
    uint16_t _thickness;
};

#endif
