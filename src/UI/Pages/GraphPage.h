#pragma once
#include "../Core/Panel.h"
#include "../Core/TouchFrame.h"
#include "../Core/GraphState.h"
#include "../Controls/Button.h"
#include "../Controls/TabControl.h"
#include "../../Core/System.h"

class GraphPage : public Panel
{
public:
    GraphPage(int16_t x,int16_t y,int16_t w,int16_t h);
    void SetSystem(SystemClass* system);
    void SetOnBack(ButtonClickHandler handler);
    void SetOnHome(ButtonClickHandler handler);
    void Open();
    void Draw() override;
    // Consumes multitouch and all remaining contacts until full release.
    bool HandleMultiTouch(const TouchFrame& frame,bool allowStart=true);
    void TouchStart(int16_t x,int16_t y) override;
    void TouchMove(int16_t x,int16_t y) override;
    void TouchEnd(int16_t x,int16_t y) override;
    TabControl& GetTabs() { return _tabs; }
    const GraphState& GetGraphState() const {return _state;}
private:
    void Layout();
    void Action(unsigned id);
    void DrawGraph();
    uint32_t CurveColor(unsigned quantity) const;
    void DrawInfo();
    void DrawSettings();
    void Edit(unsigned target);
    void EditorKey(unsigned key);
    void DrawEditor();
    void Configure(unsigned id,const char* text,int x,int y,int w,int h=38);
    void ReadBuffer();
    bool InPlot(int x,int y) const;
    void MoveCursor(int x,int y);
    static void Format(double value,const char* unit,char* text,size_t size);
    SystemClass* _system;
    GraphState _state;
    Button _buttons[29];
    char _captions[29][56] = {};
    Panel _editor;
    Button _keys[17];
    char _input[32] = {};
    const char* _error = "";
    int _editing = -1;
    bool _replaceInput = false;
    bool _initialized = false;
    uint8_t _firstCurve = 0;
    TabControl _tabs;
    uint8_t _info = 0;
    GraphState::ZoomAxis _zoomAxis=GraphState::ZoomXY;
    bool _pinchAxisChosen=false;
    double _pinchStartDx=0,_pinchStartDy=0;
    double PinchDistance(const TouchFrame& frame) const;
    bool _drag = false;
    bool _multiBlocked = false, _pinching = false;
    uint8_t _pinchIds[2] = {};
    double _pinchDistance=0, _pinchX=0.5, _pinchY=0.5;
    int16_t _lastX = 0, _lastY = 0;
    uint16_t _count = 0;
    BufferSample _samples[BufferClass::Capacity];
};
