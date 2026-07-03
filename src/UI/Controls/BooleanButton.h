#ifndef _BOOLEAN_BUTTON_H_INCLUDED
#define _BOOLEAN_BUTTON_H_INCLUDED

#include <Arduino.h>
#include "../Core/Control.h"
#include "../Core/GDClass.h"

class BooleanButton;

typedef void (*BooleanButtonChangeHandler)(BooleanButton* sender, bool value);

class BooleanButton : public Control
{
public:
    BooleanButton();
    BooleanButton(int16_t x, int16_t y, int16_t w, int16_t h, const char* caption);

    virtual void Draw() override;
    virtual bool IsFocusable() const override;

    virtual void TouchStart(int16_t x, int16_t y) override;
    virtual void TouchMove(int16_t x, int16_t y) override;
    virtual void TouchEnd(int16_t x, int16_t y) override;

    void SetCaption(const char* caption);
    const char* GetCaption() const;

    void SetValue(bool value);
    bool GetValue() const;

    void SetOnChange(BooleanButtonChangeHandler handler);

    void SetBeepOnChange(bool enabled);
    bool GetBeepOnChange() const;

private:
    void Toggle();
    void DrawTrack(int16_t trackX, int16_t trackY, int16_t trackW, int16_t trackH) const;

private:
    const char* _caption;
    bool _value;
    bool _pressed;
    bool _trackingTouch;
    bool _beepOnChange;
    BooleanButtonChangeHandler _onChange;
};

#endif
