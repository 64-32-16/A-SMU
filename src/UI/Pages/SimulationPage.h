#pragma once
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../Panels/NumericEntryDialog.h"
#include "../../Core/System.h"
class SimulationPage : public Panel {
public:
    SimulationPage(int16_t x,int16_t y,int16_t w,int16_t h);
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
    void Configure(unsigned id,const char* text,int x,int y,int w,int h=42);
    void Action(unsigned id);
    static bool AcceptResistance(void* context,double value);
    SystemClass* _system=&System;
    Button _buttons[16];
    char _captions[16][48]={};
    NumericEntryDialog _editor;
    unsigned _unit=0; // entry/display unit only; resistance belongs to SystemClass
};
