#pragma once
#include "../Core/Control.h"

class TabControl;
using TabChangedHandler = void (*)(TabControl*, uint8_t);

// Presentation-only tab selection. Captions must outlive the control.
class TabControl : public Control
{
public:
    static constexpr uint8_t MaxTabs = 8;
    TabControl();
    bool AddTab(const char* caption);
    uint8_t GetCount() const { return _count; }
    uint8_t GetSelectedIndex() const { return _selected; }
    void SetSelectedIndex(uint8_t index);
    void SetOnChanged(TabChangedHandler handler) { _onChanged=handler; }
    void SetFont(uint8_t font) { _font=font; }
    void SetColors(uint32_t active,uint32_t inactive,uint32_t pressed,uint32_t text);
    void Draw() override;
    void TouchStart(int16_t x,int16_t y) override;
    void TouchMove(int16_t x,int16_t y) override;
    void TouchEnd(int16_t x,int16_t y) override;
private:
    int HitTab(int16_t x,int16_t y) const;
    const char* _captions[MaxTabs] = {};
    uint8_t _count=0, _selected=0, _font=28;
    int _tracking=-1, _pressed=-1;
    uint32_t _active, _inactive, _pressedColor, _text;
    TabChangedHandler _onChanged=nullptr;
};
