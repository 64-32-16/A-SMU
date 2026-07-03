#ifndef _CONTAINER_H_INCLUDED
#define _CONTAINER_H_INCLUDED

#include <Arduino.h>
#include "Control.h"

class Container : public Control
{
public:
    static constexpr uint8_t MaxChildren = 32;

    Container();
    Container(int16_t x, int16_t y, int16_t w, int16_t h);

    virtual void Draw() override;
    virtual void TouchStart(int16_t x, int16_t y) override;
    virtual void TouchMove(int16_t x, int16_t y) override;
    virtual void TouchEnd(int16_t x, int16_t y) override;

    bool AddChild(Control* child);
    void SetFocus(Control* control);
    Control* GetFocus() const;
    void ClearFocus();
    void ClearChildren();
    uint8_t GetChildCount() const;
    Control* GetChild(uint8_t index) const;

protected:
    void DrawFocus() const;

    static Control* _globalFocus;
    static const Container* _focusOwner;

    Control* _children[MaxChildren];
    uint8_t _childCount;
};

#endif
