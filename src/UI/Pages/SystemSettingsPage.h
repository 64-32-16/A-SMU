#pragma once
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../Panels/NumericEntryDialog.h"
#include "../../Core/System.h"
class SystemSettingsPage : public Panel {
public:
    SystemSettingsPage(int16_t x,int16_t y,int16_t w,int16_t h);
    void SetSystem(SystemClass* system) {_system=system?system:&System;}
    void SetOnBack(ButtonClickHandler h) {_buttons[0].SetOnClick(h);}
    void SetOnHome(ButtonClickHandler h) {_buttons[1].SetOnClick(h);}
    void Open();
    void Draw() override;
    void TouchStart(int16_t x,int16_t y) override;
    void TouchMove(int16_t x,int16_t y) override;
    void TouchEnd(int16_t x,int16_t y) override;
private:
    void Layout();
    void MoveSlider(int x);
    static bool AcceptDimmer(void*,double);
    SystemClass* _system=&System;
    Button _buttons[8];
    char _backlightText[24]={},_dimmerText[24]={};
    NumericEntryDialog _number;
    Button _dateTime; // unavailable until the RTC backend is connected
    bool _sliderOpen=false,_dragging=false;
};
