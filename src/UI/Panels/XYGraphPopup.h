#ifndef _XY_GRAPH_POPUP_H_INCLUDED
#define _XY_GRAPH_POPUP_H_INCLUDED

#include <Arduino.h>
#include "../Core/Panel.h"
#include "../Controls/Button.h"
#include "../Controls/Label.h"
#include "../Core/DisplayFormat.h"
#include "../../Core/System.h"

enum class XYGraphAxis : uint8_t
{
    Time,
    Voltage,
    Current,
    Resistance,
    Power
};

class XYGraphPopup;
typedef void (*XYGraphPopupCloseHandler)(XYGraphPopup* sender);

class XYGraphPopup : public Panel
{
public:
    XYGraphPopup();
    XYGraphPopup(int16_t x, int16_t y, int16_t w, int16_t h);

    void SetSystem(SystemClass* system);
    void Open();
    void Close();
    bool IsOpen() const;
    void SetOnClose(XYGraphPopupCloseHandler handler);

    virtual void Draw() override;

private:
    static void OnAxisClick(Button* sender);
    static void OnCloseClick(Button* sender);

    void HandleAxisClick(Button* sender);
    void HandleCloseClick(Button* sender);
    void DrawGrid(int16_t plotX, int16_t plotY, int16_t plotW, int16_t plotH) const;
    void DrawTrace(int16_t plotX, int16_t plotY, int16_t plotW, int16_t plotH,
                   float minX, float maxX, float minY, float maxY);
    void DrawXAxisLabels(int16_t plotX, int16_t plotY, int16_t plotW, int16_t plotH, float minX, float maxX);
    void DrawYAxisLabels(int16_t plotX, int16_t plotY, int16_t plotH, float minY, float maxY);
    bool GetAxisRange(XYGraphAxis axis, float* minValue, float* maxValue);
    float GetAxisValue(const BufferSample& sample, XYGraphAxis axis, bool* valid = nullptr) const;
    void FormatAxisLabel(XYGraphAxis axis, float value, char* buffer, size_t bufferSize, bool showSign, DisplayFormatState* state);
    const char* GetAxisText(XYGraphAxis axis) const;
    XYGraphAxis GetNextAxis(XYGraphAxis axis) const;
    DisplayQuantity GetDisplayQuantity(XYGraphAxis axis) const;
    bool GetShowSign(XYGraphAxis axis) const;

private:
    static const uint16_t MaxPlotSamples = 256;

    SystemClass* _system;
    XYGraphAxis _xAxis;
    XYGraphAxis _yAxis;
    XYGraphPopupCloseHandler _onClose;

    Button _xAxisButton;
    Button _yAxisButton;
    Button _closeButton;
    Label _titleLabel;
    BufferSample _plotSamples[MaxPlotSamples];
    char _textBuffer[32];
};

#endif
