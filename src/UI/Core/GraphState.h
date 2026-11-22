#pragma once
#include "PlotGeometry.h"
#include "../../Core/Buffer.h"

// Graph presentation settings, not copies of SystemClass domain settings.
class GraphState
{
public:
    enum Quantity : uint8_t { Voltage, Current, Power, Resistance };
    enum ZoomAxis : uint8_t { ZoomXY, ZoomX, ZoomY };
    enum CursorMode : uint8_t { Off, Vertical, Horizontal, Cross };
    static constexpr uint8_t XDivisions=10, YDivisions=6;
    static constexpr uint16_t MaxPoints=96;
    bool enabled[4] = {true,false,false,false};
    Quantity selected = Voltage;
    bool iv = false;
    bool ivEnabled = true;
    bool IsCurveVisible(unsigned quantity) const
    { return iv ? quantity==Current && ivEnabled : enabled[quantity]; }
    bool tightYAxis = false; // compact views: fit without rounding the range outward
    bool scrolling = true;
    double windowSeconds = 100; // 10 divisions at 10 seconds/division
    PlotRange x[2]; // independent time and voltage axes
    PlotRange y[4];
    CursorMode cursorMode = Off;
    uint8_t cursor = 0;
    double cursorX[2] = {0.25,0.75};
    double cursorY[2] = {0.25,0.75};

    PlotRange& X() { return x[iv ? 1 : 0]; }
    const PlotRange& X() const { return x[iv ? 1 : 0]; }
    void SetIV(bool value);
    void AutoScale();
    void Resolve(const BufferSample* samples, uint16_t count);
    bool SetManual(bool xAxis, bool division, double value);
    bool Zoom(double factor, double anchorX=0.5, double anchorY=0.5, ZoomAxis axis=ZoomXY);
    bool Pan(double xFraction, double yFraction);
    static bool ValidRange(double start, double division, unsigned divisions);
    static void FormatValue(double value,const char* unit,char* text,size_t size,unsigned precision=4);
    static void FormatTime(double seconds,double tickSpacing,char* text,size_t size);
    static void FormatAxis(const PlotRange& range,unsigned divisions,unsigned tick,const char* unit,char* text,size_t size);
    static bool Value(const BufferSample& sample, Quantity quantity, double& value);
    static double Time(const BufferSample& sample, uint32_t origin);
    uint16_t MakePoints(const BufferSample* samples, uint16_t count, Quantity quantity,
                        PlotPoint* points,uint8_t bucketLimit=32) const;
    struct Trace {
        const GraphState* state;
        const BufferSample* samples;
        Quantity quantity;
        static PlotPoint Read(const void* context,uint16_t index) {
            const auto& trace=*static_cast<const Trace*>(context);
            const auto& sample=trace.samples[index];double value=0;
            const bool valid=Value(sample,trace.quantity,value);
            const double x=trace.state->iv?sample.Voltage:Time(sample,trace.state->timeOrigin);
            return {x,value,valid && isfinite(x)};
        }
        PlotPointSource Source(uint16_t count) const {return {this,Read,count};}
    };
    uint32_t timeOrigin = 0;
    bool hasOrigin = false;
private:
    bool _autoYFitted[4] = {};
    Quantity _timeSelected = Voltage;
    PlotRange _timeY[4];
    PlotRange _ivY;
};
