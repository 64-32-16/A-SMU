#ifndef _SYSTEM_H_INCLUDED
#define _SYSTEM_H_INCLUDED

#include <Arduino.h>
#include "Buffer.h"
#include "DacOutput.h"

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

    void Reset();
    void Execute();
    void SetDacOutput(DacOutput* dacOutput);

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
    void SetVoltage(float value);

    float GetCurrent() const;
    void SetCurrent(float value);

    float GetResistance() const;
    void SetResistance(float value);

    float GetPower() const;
    void SetPower(float value);

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
    void UpdateDerivedValues();
    void ExecuteSimulation();
    float ScaleToDacControl(float value, float fullScale) const;
    float ResolveDacVSetControl() const;
    float ResolveDacILimitControl() const;
    void MarkDacDirty();
    void ApplyDacOutputs();

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
    DacOutput* _dacOutput;
    bool _dacDirty;

    float _voltage;
    float _current;
    float _resistance;
    float _power;
    float _voltageSourceValue;
    float _voltageLimitValue;
    float _currentSourceValue;
    float _currentLimitValue;
    uint32_t _lastSimulationMs;
    float _simulationPhase;
};

extern SystemClass System;

#endif
