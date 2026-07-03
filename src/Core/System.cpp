#include "System.h"

#include <math.h>

namespace
{
constexpr uint32_t kSimulationPeriodMs = 100;
constexpr float kMinResistanceCurrent = 1e-9f;
constexpr float kDacControlFullScaleVolts = 2.5f;
}

SystemClass::SystemClass()
{
    Reset();
}

void SystemClass::Reset()
{
    _measureMode = MeasureMode::Voltage;
    _senseMode = SenseMode::Local;

    _voltageRangeMode = RangeMode::Manual;
    _voltageRange = VoltageRange::Range5V;
    _currentRangeMode = RangeMode::Manual;
    _currentRange = CurrentRange::Range100mA;
    _resistanceRange = ResistanceRange::Auto;
    _sourceMode = SourceMode::Voltage;
    _outputOffState = OutputOffState::Standard;
    _ovpSetting = OvpSetting::Off;
    _ocpSetting = OcpSetting::Off;
    _sweepEnabled = false;

    _measureAutoUnit = true;
    _outputOn = false;
    _simulationEnabled = false;
    _dacOutput = nullptr;
    _dacDirty = true;

    _voltage = 0.0f;
    _current = 0.0f;
    _resistance = 0.0f;
    _power = 0.0f;
    _voltageSourceValue = 0.0f;
    _voltageLimitValue = 0.0f;
    _currentSourceValue = 0.0f;
    _currentLimitValue = 0.0f;
    _lastSimulationMs = 0;
    _simulationPhase = 0.0f;

    Buffer.Reset();
    UpdateDerivedValues();
}

void SystemClass::Execute()
{
    ApplyDacOutputs();

    if (_simulationEnabled)
    {
        ExecuteSimulation();
    }
}

void SystemClass::SetDacOutput(DacOutput* dacOutput)
{
    _dacOutput = dacOutput;
    _dacDirty = true;

    if (_dacOutput != nullptr)
    {
        _dacOutput->Begin();
        ApplyDacOutputs();
    }
}

MeasureMode SystemClass::GetMeasureMode() const
{
    return _measureMode;
}

void SystemClass::SetMeasureMode(MeasureMode mode)
{
    _measureMode = mode;

    if (_measureMode == MeasureMode::Resistance)
    {
        ApplyResolvedResistanceRange(
            _resistanceRange == ResistanceRange::Auto ? ResolveAutoResistanceRange() : _resistanceRange);
    }
}

SenseMode SystemClass::GetSenseMode() const
{
    return _senseMode;
}

void SystemClass::SetSenseMode(SenseMode mode)
{
    _senseMode = mode;
}

RangeMode SystemClass::GetVoltageRangeMode() const
{
    return _voltageRangeMode;
}

void SystemClass::SetVoltageRangeMode(RangeMode mode)
{
    _voltageRangeMode = mode;
    if (_voltageRangeMode == RangeMode::Auto)
    {
        RecalculateAutoVoltageRange();
    }
    MarkDacDirty();
}

VoltageRange SystemClass::GetVoltageRange() const
{
    return _voltageRange;
}

void SystemClass::SetVoltageRange(VoltageRange range)
{
    _voltageRange = range;
    if (_voltageRangeMode == RangeMode::Auto)
    {
        RecalculateAutoVoltageRange();
    }
    MarkDacDirty();

    if (_voltageSourceValue < GetVoltageSourceMin() ||
        _voltageSourceValue > GetVoltageSourceMax())
    {
        _voltageSourceValue = 0.0f;
        MarkDacDirty();
    }

    if (_voltageLimitValue < GetVoltageLimitMin() ||
        _voltageLimitValue > GetVoltageLimitMax())
    {
        _voltageLimitValue = 0.0f;
        MarkDacDirty();
    }
}

RangeMode SystemClass::GetCurrentRangeMode() const
{
    return _currentRangeMode;
}

void SystemClass::SetCurrentRangeMode(RangeMode mode)
{
    _currentRangeMode = mode;
    if (_currentRangeMode == RangeMode::Auto)
    {
        RecalculateAutoCurrentRange();
    }
    MarkDacDirty();
}

CurrentRange SystemClass::GetCurrentRange() const
{
    return _currentRange;
}

void SystemClass::SetCurrentRange(CurrentRange range)
{
    _currentRange = range;
    if (_currentRangeMode == RangeMode::Auto)
    {
        RecalculateAutoCurrentRange();
    }
    MarkDacDirty();

    if (_currentSourceValue < GetCurrentSourceMin() ||
        _currentSourceValue > GetCurrentSourceMax())
    {
        _currentSourceValue = 0.0f;
        MarkDacDirty();
    }

    if (_currentLimitValue < GetCurrentLimitMin() ||
        _currentLimitValue > GetCurrentLimitMax())
    {
        _currentLimitValue = 0.0f;
        MarkDacDirty();
    }
}

ResistanceRange SystemClass::GetResistanceRange() const
{
    return _resistanceRange;
}

void SystemClass::SetResistanceRange(ResistanceRange range)
{
    _resistanceRange = range;
    ApplyResolvedResistanceRange(
        _resistanceRange == ResistanceRange::Auto ? ResolveAutoResistanceRange() : _resistanceRange);
}

SourceMode SystemClass::GetSourceMode() const
{
    return _sourceMode;
}

void SystemClass::SetSourceMode(SourceMode mode)
{
    _sourceMode = mode;
    MarkDacDirty();
}

OutputOffState SystemClass::GetOutputOffState() const
{
    return _outputOffState;
}

void SystemClass::SetOutputOffState(OutputOffState state)
{
    _outputOffState = state;
}

OvpSetting SystemClass::GetOvpSetting() const
{
    return _ovpSetting;
}

void SystemClass::SetOvpSetting(OvpSetting setting)
{
    _ovpSetting = setting;
}

OcpSetting SystemClass::GetOcpSetting() const
{
    return _ocpSetting;
}

void SystemClass::SetOcpSetting(OcpSetting setting)
{
    _ocpSetting = setting;
}

bool SystemClass::GetSweepEnabled() const
{
    return _sweepEnabled;
}

void SystemClass::SetSweepEnabled(bool enabled)
{
    _sweepEnabled = enabled;
}

bool SystemClass::GetMeasureAutoUnit() const
{
    return _measureAutoUnit;
}

void SystemClass::SetMeasureAutoUnit(bool enabled)
{
    _measureAutoUnit = enabled;
}

bool SystemClass::GetOutputOn() const
{
    return _outputOn;
}

void SystemClass::SetOutputOn(bool enabled)
{
    _outputOn = enabled;
}

bool SystemClass::GetSimulationEnabled() const
{
    return _simulationEnabled;
}

void SystemClass::SetSimulationEnabled(bool enabled)
{
    _simulationEnabled = enabled;
    _lastSimulationMs = 0;
}

float SystemClass::GetVoltage() const
{
    return _voltage;
}

void SystemClass::SetVoltage(float value)
{
    _voltage = value;
    AutoAdjustVoltageRangeForValue(value);
    UpdateDerivedValues();
}

float SystemClass::GetCurrent() const
{
    return _current;
}

void SystemClass::SetCurrent(float value)
{
    _current = value;
    AutoAdjustCurrentRangeForValue(value);
    UpdateDerivedValues();
}

float SystemClass::GetResistance() const
{
    return _resistance;
}

void SystemClass::SetResistance(float value)
{
    _resistance = value;

    if (_resistanceRange == ResistanceRange::Auto)
    {
        ApplyResolvedResistanceRange(ResolveAutoResistanceRange());
    }
}

float SystemClass::GetPower() const
{
    return _power;
}

void SystemClass::SetPower(float value)
{
    _power = value;
}

float SystemClass::GetVoltageSourceValue() const
{
    return _voltageSourceValue;
}

void SystemClass::SetVoltageSourceValue(float value)
{
    if (value < GetVoltageSourceMin() || value > GetVoltageSourceMax())
    {
        return;
    }

    AutoAdjustVoltageRangeForValue(value);
    _voltageSourceValue = value;
    MarkDacDirty();
}

float SystemClass::GetVoltageSourceMin() const
{
    return -GetVoltageSourceMax();
}

float SystemClass::GetVoltageSourceMax() const
{
    if (_voltageRangeMode == RangeMode::Auto)
    {
        return GetVoltageRangeMax(VoltageRange::Range30V);
    }

    switch (_voltageRange)
    {
        case VoltageRange::Range5V:
            return GetVoltageRangeMax(VoltageRange::Range5V);
        case VoltageRange::Range30V:
        default:
            return GetVoltageRangeMax(VoltageRange::Range30V);
    }
}

float SystemClass::GetVoltageLimitValue() const
{
    return _voltageLimitValue;
}

void SystemClass::SetVoltageLimitValue(float value)
{
    if (value < GetVoltageLimitMin() || value > GetVoltageLimitMax())
    {
        return;
    }

    AutoAdjustVoltageRangeForValue(value);
    _voltageLimitValue = value;
    MarkDacDirty();
}

float SystemClass::GetVoltageLimitMin() const
{
    return 0.0f;
}

float SystemClass::GetVoltageLimitMax() const
{
    if (_voltageRangeMode == RangeMode::Auto)
    {
        return GetVoltageRangeMax(VoltageRange::Range30V);
    }

    switch (_voltageRange)
    {
        case VoltageRange::Range5V:
            return GetVoltageRangeMax(VoltageRange::Range5V);
        case VoltageRange::Range30V:
        default:
            return GetVoltageRangeMax(VoltageRange::Range30V);
    }
}

float SystemClass::GetCurrentSourceValue() const
{
    return _currentSourceValue;
}

void SystemClass::SetCurrentSourceValue(float value)
{
    if (value < GetCurrentSourceMin() || value > GetCurrentSourceMax())
    {
        return;
    }

    AutoAdjustCurrentRangeForValue(value);
    _currentSourceValue = value;
    MarkDacDirty();
}

float SystemClass::GetCurrentSourceMin() const
{
    return -GetCurrentSourceMax();
}

float SystemClass::GetCurrentSourceMax() const
{
    if (_currentRangeMode == RangeMode::Auto)
    {
        return GetCurrentRangeMax(CurrentRange::Range1A);
    }

    switch (_currentRange)
    {
        case CurrentRange::Range100mA:
            return GetCurrentRangeMax(CurrentRange::Range100mA);
        case CurrentRange::Range1A:
        default:
            return GetCurrentRangeMax(CurrentRange::Range1A);
    }
}

float SystemClass::GetCurrentLimitValue() const
{
    return _currentLimitValue;
}

void SystemClass::SetCurrentLimitValue(float value)
{
    if (value < GetCurrentLimitMin() || value > GetCurrentLimitMax())
    {
        return;
    }

    AutoAdjustCurrentRangeForValue(value);
    _currentLimitValue = value;
    MarkDacDirty();
}

float SystemClass::GetCurrentLimitMin() const
{
    return 0.0f;
}

float SystemClass::GetCurrentLimitMax() const
{
    if (_currentRangeMode == RangeMode::Auto)
    {
        return GetCurrentRangeMax(CurrentRange::Range1A);
    }

    switch (_currentRange)
    {
        case CurrentRange::Range100mA:
            return GetCurrentRangeMax(CurrentRange::Range100mA);
        case CurrentRange::Range1A:
        default:
            return GetCurrentRangeMax(CurrentRange::Range1A);
    }
}

const char* SystemClass::GetMeasureModeText() const
{
    switch (_measureMode)
    {
        case MeasureMode::Voltage: return "VOLT";
        case MeasureMode::Current: return "CURR";
        case MeasureMode::Resistance: return "RES";
        case MeasureMode::Power: return "POWER";
        default: return "--";
    }
}

const char* SystemClass::GetSenseModeText() const
{
    switch (_senseMode)
    {
        case SenseMode::Local: return "LOCAL";
        case SenseMode::Remote: return "REMOTE";
        default: return "--";
    }
}

const char* SystemClass::GetRangeText() const
{
    switch (_measureMode)
    {
        case MeasureMode::Voltage:
            return GetVoltageRangeText();

        case MeasureMode::Current:
            return GetCurrentRangeText();

        case MeasureMode::Resistance:
            return GetResistanceRangeText();

        case MeasureMode::Power:
            return "AUTO";

        default:
            return "--";
    }
}

const char* SystemClass::GetVoltageRangeText() const
{
    switch (_voltageRange)
    {
        case VoltageRange::Range5V: return "5 V";
        case VoltageRange::Range30V: return "30 V";
        default: return "--";
    }
}

const char* SystemClass::GetCurrentRangeText() const
{
    switch (_currentRange)
    {
        case CurrentRange::Range100mA: return "100 mA";
        case CurrentRange::Range1A: return "1 A";
        default: return "--";
    }
}

const char* SystemClass::GetResistanceRangeText() const
{
    switch (_resistanceRange)
    {
        case ResistanceRange::Auto: return "AUTO";
        case ResistanceRange::Range10Ohm: return "10 Ohm";
        case ResistanceRange::Range100Ohm: return "100 Ohm";
        case ResistanceRange::Range1kOhm: return "1 kOhm";
        case ResistanceRange::Range100kOhm: return "100 kOhm";
        default: return "--";
    }
}

const char* SystemClass::GetOutputOffStateText() const
{
    switch (_outputOffState)
    {
        case OutputOffState::ZHigh: return "Z-High";
        case OutputOffState::Standard: return "Standard";
        case OutputOffState::Zero: return "Zero";
        default: return "--";
    }
}

const char* SystemClass::GetOvpSettingText() const
{
    switch (_ovpSetting)
    {
        case OvpSetting::Off: return "OVP Off";
        case OvpSetting::Range5V: return "OVP 5 V";
        case OvpSetting::Range30V: return "OVP 30 V";
        default: return "--";
    }
}

const char* SystemClass::GetOcpSettingText() const
{
    switch (_ocpSetting)
    {
        case OcpSetting::Off: return "OCP Off";
        case OcpSetting::Range100mA: return "OCP 100 mA";
        case OcpSetting::Range1A: return "OCP 1 A";
        default: return "--";
    }
}

const char* SystemClass::GetSweepSettingText() const
{
    return _sweepEnabled ? "Sweep On" : "Sweep Off";
}

void SystemClass::ApplyResolvedResistanceRange(ResistanceRange range)
{
    _sourceMode = SourceMode::Voltage;
    _voltageRangeMode = RangeMode::Manual;
    _currentRangeMode = RangeMode::Manual;

    switch (range)
    {
        case ResistanceRange::Range10Ohm:
            _voltageRange = VoltageRange::Range5V;
            _currentRange = CurrentRange::Range100mA;
            _voltageSourceValue = 1.0f;
            _currentLimitValue = 0.1f;
            MarkDacDirty();
            break;

        case ResistanceRange::Range100Ohm:
            _voltageRange = VoltageRange::Range5V;
            _currentRange = CurrentRange::Range100mA;
            _voltageSourceValue = 1.0f;
            _currentLimitValue = 0.01f;
            MarkDacDirty();
            break;

        case ResistanceRange::Range1kOhm:
            _voltageRange = VoltageRange::Range5V;
            _currentRange = CurrentRange::Range100mA;
            _voltageSourceValue = 1.0f;
            _currentLimitValue = 0.001f;
            MarkDacDirty();
            break;

        case ResistanceRange::Range100kOhm:
        default:
            _voltageRange = VoltageRange::Range30V;
            _currentRange = CurrentRange::Range100mA;
            _voltageSourceValue = 10.0f;
            _currentLimitValue = 0.0001f;
            MarkDacDirty();
            break;
    }
}

ResistanceRange SystemClass::ResolveAutoResistanceRange() const
{
    const float absResistance = (_resistance < 0.0f) ? -_resistance : _resistance;

    if (absResistance <= 12.0f)
    {
        return ResistanceRange::Range10Ohm;
    }

    if (absResistance <= 120.0f)
    {
        return ResistanceRange::Range100Ohm;
    }

    if (absResistance <= 1200.0f)
    {
        return ResistanceRange::Range1kOhm;
    }

    return ResistanceRange::Range100kOhm;
}

void SystemClass::UpdateDerivedValues()
{
    _power = _voltage * _current;

    if (fabsf(_current) >= kMinResistanceCurrent)
    {
        _resistance = _voltage / _current;
    }
    else
    {
        _resistance = 0.0f;
    }
}

void SystemClass::AutoAdjustVoltageRangeForValue(float value)
{
    if (_voltageRangeMode != RangeMode::Auto)
    {
        return;
    }

    const VoltageRange range = (fabsf(value) <= GetVoltageRangeMax(VoltageRange::Range5V))
        ? VoltageRange::Range5V
        : VoltageRange::Range30V;

    if (_voltageRange != range)
    {
        _voltageRange = range;
        MarkDacDirty();
    }
}

void SystemClass::AutoAdjustCurrentRangeForValue(float value)
{
    if (_currentRangeMode != RangeMode::Auto)
    {
        return;
    }

    const CurrentRange range = (fabsf(value) <= GetCurrentRangeMax(CurrentRange::Range100mA))
        ? CurrentRange::Range100mA
        : CurrentRange::Range1A;

    if (_currentRange != range)
    {
        _currentRange = range;
        MarkDacDirty();
    }
}

void SystemClass::RecalculateAutoVoltageRange()
{
    if (_voltageRangeMode != RangeMode::Auto)
    {
        return;
    }

    const float maxValue = fmaxf(fmaxf(fabsf(_voltage), fabsf(_voltageSourceValue)),
                                fabsf(_voltageLimitValue));
    AutoAdjustVoltageRangeForValue(maxValue);
}

void SystemClass::RecalculateAutoCurrentRange()
{
    if (_currentRangeMode != RangeMode::Auto)
    {
        return;
    }

    const float maxValue = fmaxf(fmaxf(fabsf(_current), fabsf(_currentSourceValue)),
                                fabsf(_currentLimitValue));
    AutoAdjustCurrentRangeForValue(maxValue);
}

float SystemClass::GetVoltageRangeMax(VoltageRange range) const
{
    switch (range)
    {
        case VoltageRange::Range5V:
            return 5.0f;

        case VoltageRange::Range30V:
        default:
            return 30.0f;
    }
}

float SystemClass::GetCurrentRangeMax(CurrentRange range) const
{
    switch (range)
    {
        case CurrentRange::Range100mA:
            return 0.1f;

        case CurrentRange::Range1A:
        default:
            return 1.0f;
    }
}

void SystemClass::ExecuteSimulation()
{
    const uint32_t now = millis();
    if ((now - _lastSimulationMs) < kSimulationPeriodMs)
    {
        return;
    }

    _lastSimulationMs = now;
    _simulationPhase += 0.2f;

    const float resistance = 47.0f + (18.0f * sinf(_simulationPhase * 0.35f));

    float sourceVoltage = (_sourceMode == SourceMode::Voltage) ? _voltageSourceValue : 1.2f;
    if (fabsf(sourceVoltage) < 0.001f)
    {
        sourceVoltage = 1.2f;
    }

    const float ripple = 0.004f * sinf(_simulationPhase * 1.7f);
    const float measuredVoltage = sourceVoltage + ripple;
    const float measuredCurrent = measuredVoltage / resistance;

    _voltage = measuredVoltage;
    _current = measuredCurrent;
    UpdateDerivedValues();

    Buffer.AddSample(now, _voltage, _current);
}

float SystemClass::ScaleToDacControl(float value, float fullScale) const
{
    if (fullScale <= 0.0f)
    {
        return 0.0f;
    }

    float control = (value / fullScale) * kDacControlFullScaleVolts;

    if (control < -kDacControlFullScaleVolts)
    {
        return -kDacControlFullScaleVolts;
    }

    if (control > kDacControlFullScaleVolts)
    {
        return kDacControlFullScaleVolts;
    }

    return control;
}

float SystemClass::ResolveDacVSetControl() const
{
    if (_sourceMode == SourceMode::Current)
    {
        if (fabsf(_currentSourceValue) < kMinResistanceCurrent)
        {
            return 0.0f;
        }

        const float signedVoltageLimit = (_currentSourceValue < 0.0f)
            ? -_voltageLimitValue
            : _voltageLimitValue;
        return ScaleToDacControl(signedVoltageLimit, GetVoltageRangeMax(_voltageRange));
    }

    return ScaleToDacControl(_voltageSourceValue, GetVoltageRangeMax(_voltageRange));
}

float SystemClass::ResolveDacILimitControl() const
{
    if (_sourceMode == SourceMode::Current)
    {
        return ScaleToDacControl(fabsf(_currentSourceValue), GetCurrentRangeMax(_currentRange));
    }

    return ScaleToDacControl(_currentLimitValue, GetCurrentRangeMax(_currentRange));
}

void SystemClass::MarkDacDirty()
{
    _dacDirty = true;
}

void SystemClass::ApplyDacOutputs()
{
    if (!_dacDirty || _dacOutput == nullptr)
    {
        return;
    }

    const float vSetControl = ResolveDacVSetControl();
    const float iLimitControl = ResolveDacILimitControl();

    _dacOutput->SetVSet(vSetControl);
    _dacOutput->SetILimit(iLimitControl);

    Serial.print("DAC V_SET_CTRL=");
    Serial.print(vSetControl, 6);
    Serial.print(" V, I_LIMIT_CTRL=");
    Serial.print(iLimitControl, 6);
    Serial.println(" V");

    _dacDirty = false;
}

SystemClass System;
