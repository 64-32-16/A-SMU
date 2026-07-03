#ifndef _GRAPH_PANEL_H_INCLUDED
#define _GRAPH_PANEL_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Core/DisplayFormat.h"
#include "../../Core/System.h"

class GraphPanel : public Panel
{
public:
    GraphPanel();
    GraphPanel(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    SystemClass* GetSystem() const;

    virtual void Draw() override;

private:
    void DrawGrid(int16_t plotX, int16_t plotY, int16_t plotW, int16_t plotH) const;
    void DrawLabels(int16_t plotX, int16_t plotY, int16_t plotH, float minValue, float maxValue, DisplayQuantity quantity, bool showSign);
    void DrawTrace(int16_t plotX, int16_t plotY, int16_t plotW, int16_t plotH, float minValue, float maxValue, MeasureMode mode);
    float GetSampleValue(const BufferSample& sample, MeasureMode mode, bool* valid = nullptr) const;
    DisplayQuantity GetDisplayQuantity(MeasureMode mode) const;
    bool GetShowSign(MeasureMode mode) const;

private:
    static const uint16_t MaxPlotSamples = 200;

    SystemClass* _system;
    BufferSample _plotSamples[MaxPlotSamples];
    DisplayFormatState _voltageAxisState;
    DisplayFormatState _currentAxisState;
    DisplayFormatState _resistanceAxisState;
    DisplayFormatState _powerAxisState;
    char _labelBuffers[7][24];
};

#endif
