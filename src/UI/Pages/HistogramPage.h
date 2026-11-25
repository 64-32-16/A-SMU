#pragma once
#include "../Core/Panel.h"
#include "../Core/TouchFrame.h"
#include "../Controls/TabControl.h"
#include "../Controls/Button.h"
#include "../Panels/NumericEntryDialog.h"
#include "../../Core/System.h"

class HistogramPage : public Panel {
public:
    HistogramPage(int16_t x,int16_t y,int16_t w,int16_t h);
    void SetSystem(SystemClass* system);
    void SetOnBack(ButtonClickHandler handler);
    void SetOnHome(ButtonClickHandler handler);
    void Open();
    void Draw() override;
    void TouchStart(int16_t x,int16_t y) override;
    void TouchMove(int16_t x,int16_t y) override;
    void TouchEnd(int16_t x,int16_t y) override;
    bool HandleMultiTouch(const TouchFrame& frame,bool allowStart=true);
    TabControl& GetTabs() {return _tabs;}
    const HistogramResult& GetHistogram() const {return _result;}
private:
    void Refresh();
    void Layout();
    void Configure(unsigned id,const char* text,int x,int y,int w,int h=44);
    void Action(unsigned id);
    void DrawHistogram();
    void DrawSettings();
    void DrawPopup();
    void Edit(unsigned field);
    static bool AcceptValue(void* context,double value);
    int BinAtPoint(int x,int y) const;
    SystemClass* _system;
    Button _buttons[20];
    char _captions[20][64]={};
    TabControl _tabs;
    NumericEntryDialog _editor;
    HistogramResult _result;
    unsigned _field=0;
    int _popup=0,_selectedBin=-1,_pressedBin=-1;
    int16_t _touchX=0,_touchY=0;
    bool _moved=false,_multiBlocked=false;
};
