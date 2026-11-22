#ifndef _SYSTEM_H_INCLUDED
#define _SYSTEM_H_INCLUDED

#include <Arduino.h>
#include "Buffer.h"
#include "GraphSettings.h"
#include "Histogram.h"

enum class MeasureMode : uint8_t
{
    Voltage,
    Current,
    Resistance,
    Power
};

enum class SenseMode : uint8_t
{
    Local,
    Remote
};

enum class RangeMode : uint8_t
{
    Auto,
    Manual
};

enum class VoltageRange : uint8_t
{
    Range5V,
    Range30V
};

enum class CurrentRange : uint8_t
{
    Range10uA,
    Range1mA,
    Range100mA,
    Range1A
};

enum class ResistanceRange : uint8_t
{
    Auto,
    Range10Ohm,
    Range100Ohm,
    Range1kOhm,
    Range100kOhm
};

enum class SourceMode : uint8_t
{
    Voltage,
    Current
};

enum class OutputOffState : uint8_t
{
    ZHigh,
    Standard,
    Zero
};

enum class OvpSetting : uint8_t
{
    Off,
    Range5V,
    Range30V
};

enum class OcpSetting : uint8_t
{
    Off,
    Range100mA,
    Range1A
};

class SystemClass
{
public:
    SystemClass();
    SystemClass(const SystemClass&)=delete;
    SystemClass& operator=(const SystemClass&)=delete;

    void Reset();
    void Execute();
    void ExecuteAt(uint32_t now);
    const Hardware::Measurement& GetMeasurement() const { return _measurement; }
    const Hardware::SimulationSettings& GetSimulationSettings() const {return _simulationSettings;}
    bool SetSimulationLoad(Hardware::LoadType load);
    bool SetSimulationResistance(double ohms);
    bool IsVoltageRangeAvailable(VoltageRange range) const;
    bool IsCurrentRangeAvailable(CurrentRange range) const;
    bool SetVoltageRangeAvailable(VoltageRange range,bool enabled);
    bool SetCurrentRangeAvailable(CurrentRange range,bool enabled);
    const char* GetBackendText() const;
    const char* GetHardwareStatusText() const;

    BufferStatistics GetMeasurementStatistics() const;
    void ClearMeasurementBuffer();
    const GraphSettings& GetGraphSettings() const { return _graphSettings; }
    bool SetGraphStyle(GraphStyle style);
    const HistogramSettings& GetHistogramSettings() const { return _histogramSettings; }
    bool SetHistogramSettings(const HistogramSettings& settings);
    MeasureMode GetMeasureMode() const;
    void SetMeasureMode(MeasureMode mode);

    SenseMode GetSenseMode() const;
    void SetSenseMode(SenseMode mode);

    RangeMode GetVoltageRangeMode() const;
    void SetVoltageRangeMode(RangeMode mode);
    VoltageRange GetVoltageRange() const;
    void SetVoltageRange(VoltageRange range);

    RangeMode GetCurrentRangeMode() const;
    void SetCurrentRangeMode(RangeMode mode);
    CurrentRange GetCurrentRange() const;
    void SetCurrentRange(CurrentRange range);

    ResistanceRange GetResistanceRange() const;
    void SetResistanceRange(ResistanceRange range);

    SourceMode GetSourceMode() const;
    void SetSourceMode(SourceMode mode);

    OutputOffState GetOutputOffState() const;
    void SetOutputOffState(OutputOffState state);

    OvpSetting GetOvpSetting() const;
    void SetOvpSetting(OvpSetting setting);

    OcpSetting GetOcpSetting() const;
    void SetOcpSetting(OcpSetting setting);

    bool GetSweepEnabled() const;
    void SetSweepEnabled(bool enabled);

    bool GetMeasureAutoUnit() const;
    void SetMeasureAutoUnit(bool enabled);

    bool GetOutputOn() const;
    void SetOutputOn(bool enabled);

    bool GetSimulationEnabled() const;
    void SetSimulationEnabled(bool enabled);

    float GetVoltage() const;
    // Legacy host-test injection; acquisition uses the backend record atomically.
    void SetVoltage(float value);

    float GetCurrent() const;
    void SetCurrent(float value);

    float GetResistance() const;

    float GetPower() const;

    float GetVoltageSourceValue() const;
    void SetVoltageSourceValue(float value);
    float GetVoltageSourceMin() const;
    float GetVoltageSourceMax() const;

    float GetVoltageLimitValue() const;
    void SetVoltageLimitValue(float value);
    float GetVoltageLimitMin() const;
    float GetVoltageLimitMax() const;

    float GetCurrentSourceValue() const;
    void SetCurrentSourceValue(float value);
    float GetCurrentSourceMin() const;
    float GetCurrentSourceMax() const;

    float GetCurrentLimitValue() const;
    void SetCurrentLimitValue(float value);
    float GetCurrentLimitMin() const;
    float GetCurrentLimitMax() const;

    const char* GetMeasureModeText() const;
    const char* GetSenseModeText() const;
    const char* GetRangeText() const;
    const char* GetVoltageRangeText() const;
    const char* GetCurrentRangeText() const;
    static constexpr uint8_t CurrentRangeCount = 4;
    static CurrentRange GetCurrentRangeAt(uint8_t index);
    static const char* GetCurrentRangeText(CurrentRange range);
    const char* GetResistanceRangeText() const;
    const char* GetOutputOffStateText() const;
    const char* GetOvpSettingText() const;
    const char* GetOcpSettingText() const;
    const char* GetSweepSettingText() const;

public:
    BufferClass Buffer;

private:
    void ApplyResolvedResistanceRange(ResistanceRange range);
    ResistanceRange ResolveAutoResistanceRange() const;
    void AutoAdjustVoltageRangeForValue(float value);
    void AutoAdjustCurrentRangeForValue(float value);
    void RecalculateAutoVoltageRange();
    void RecalculateAutoCurrentRange();
    float GetVoltageRangeMax(VoltageRange range) const;
    float GetCurrentRangeMax(CurrentRange range) const;
    Hardware::Request MakeHardwareRequest() const;

private:
    MeasureMode _measureMode;
    SenseMode _senseMode;

    VoltageRange _voltageRange;
    RangeMode _voltageRangeMode;
    CurrentRange _currentRange;
    RangeMode _currentRangeMode;
    ResistanceRange _resistanceRange;
    SourceMode _sourceMode;
    OutputOffState _outputOffState;
    OvpSetting _ovpSetting;
    OcpSetting _ocpSetting;
    bool _sweepEnabled;

    bool _measureAutoUnit;
    bool _outputOn;
    bool _simulationEnabled;

    float _voltageSourceValue;
    float _voltageLimitValue;
    float _currentSourceValue;
    float _currentLimitValue;
    uint32_t _lastSimulationMs;
    Hardware::SimulationSettings _simulationSettings;
    Hardware::SimulationBackend _simulator;
    Hardware::Backend* _backend=nullptr;
    Hardware::Measurement _measurement;
    bool _hardwareDirty=true;
    GraphSettings _graphSettings;
    HistogramSettings _histogramSettings;
};

extern SystemClass System;

#endif
