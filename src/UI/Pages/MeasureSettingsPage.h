#pragma once
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../../Core/System.h"

class MeasureSettingsPage : public Panel {
public:
    MeasureSettingsPage(int16_t x,int16_t y,int16_t w,int16_t h);
    void SetSystem(SystemClass* system) {_system=system?system:&System;}
    void SetOnBack(ButtonClickHandler h) {_back.SetOnClick(h);}
    void SetOnHome(ButtonClickHandler h) {_home.SetOnClick(h);}
    void SetOnRange(void (*handler)(SourceMode)) {_onRange=handler;}
    void Open();
    void Draw() override;
    void TouchEnd(int16_t x,int16_t y) override;
private:
    void Layout();
    void Select(unsigned option);
    enum class Editor {None,Nplc};
    Editor _editor=Editor::None; // presentation only
    SystemClass* _system=&System;
    void (*_onRange)(SourceMode)=nullptr;
    Button _back,_home;
    Button _values[3],_cancel;
    char _valueCaptions[3][32]={};
    Button _choices[MeasurementTiming::Count];
    char _captions[MeasurementTiming::Count][24]={};
};
