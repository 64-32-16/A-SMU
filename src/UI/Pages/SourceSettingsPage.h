#pragma once
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../../Core/System.h"

class SourceSettingsPage : public Panel {
public:
    enum class Setting {Function,Range,Limit,OutputOff};
    SourceSettingsPage(int16_t x,int16_t y,int16_t w,int16_t h);
    void SetSystem(SystemClass* system) {_system=system?system:&System;}
    void SetOnBack(ButtonClickHandler h) {_buttons[0].SetOnClick(h);}
    void SetOnHome(ButtonClickHandler h) {_buttons[1].SetOnClick(h);}
    void SetOnSetting(void (*handler)(Setting)) {_onSetting=handler;}
    void Open();
    void Draw() override;
    void TouchEnd(int16_t x,int16_t y) override;
private:
    void Layout();
    SystemClass* _system=&System;
    void (*_onSetting)(Setting)=nullptr;
    Button _buttons[6];
    char _limitCaption[48]={}; // formatted presentation only
};
