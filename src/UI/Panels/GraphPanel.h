#ifndef _GRAPH_PANEL_H_INCLUDED
#define _GRAPH_PANEL_H_INCLUDED

#include "../Core/Panel.h"
#include "../Core/GraphState.h"
#include "../Core/TouchFrame.h"
#include "../../Core/System.h"

// Compact live view using the same data preparation and GD renderer as GraphPage.
// Navigation and buttons remain the responsibility of the parent MainPage.
class GraphPanel : public Panel
{
public:
    GraphPanel();
    GraphPanel(int16_t x,int16_t y,int16_t w,int16_t h);
    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;
    void Draw() override;
    bool InPlot(int x,int y) const;
    void TouchStart(int16_t x,int16_t y) override;
    void TouchMove(int16_t x,int16_t y) override;
    void TouchEnd(int16_t x,int16_t y) override;
    void CancelTouch();
    bool HandleMultiTouch(const TouchFrame& frame,bool allowStart=true);
    const GraphState& GetGraphState() const {return _state;}

private:
    SystemClass* _system;
    GraphState _state;
    bool _drag=false,_moved=false,_multiBlocked=false,_hasTap=false;
    int16_t _lastX=0,_lastY=0,_tapX=0,_tapY=0;
    uint32_t _lastTapMs=0;
};
#endif
