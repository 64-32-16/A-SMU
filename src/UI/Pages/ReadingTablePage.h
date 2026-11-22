#pragma once
#include "../Core/Panel.h"
#include "../Core/ReadingTableState.h"
#include "../Core/GraphState.h"
#include "../Core/TouchFrame.h"
#include "../Controls/Button.h"
#include "../../Core/System.h"
class ReadingTablePage : public Panel {
public:
    ReadingTablePage(int16_t x,int16_t y,int16_t w,int16_t h);
    void SetSystem(SystemClass* system);
    void SetOnBack(ButtonClickHandler handler);
    void SetOnHome(ButtonClickHandler handler);
    void Open();
    void Draw() override;
    void TouchStart(int16_t x,int16_t y) override;
    void TouchMove(int16_t x,int16_t y) override;
    void TouchEnd(int16_t x,int16_t y) override;
    bool HandleMultiTouch(const TouchFrame& frame,bool allowStart=true);
    const ReadingTableState& GetTable() const {return _table;}
private:
    void Layout();
    void Configure(unsigned id,const char* text,int x,int y,int w,int h=38);
    void Action(unsigned id);
    void DrawRows();
    void DrawPreview();
    void DrawPopup();
    int RowAt(int x,int y) const;
    bool InPreview(int x,int y) const;
    static void Value(const BufferSample& sample,GraphState::Quantity quantity,char* text,size_t size);
    static void Time(uint32_t ms,char* text,size_t size);
    SystemClass* _system;
    ReadingTableState _table;
    Button _buttons[10];
    int _popup=0,_pressedRow=-1;
    bool _previewPressed=false,_moved=false,_multiBlocked=false;
    int16_t _touchX=0,_touchY=0;
};
