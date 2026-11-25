#include "System.h"

#include <math.h>

namespace
{
struct VoltageRangeDefinition { VoltageRange range; float maximum; const char* label; };
constexpr VoltageRangeDefinition kVoltageRanges[] = {
    {VoltageRange::Range5V, 5.0f, "5 V"},
    {VoltageRange::Range30V, 30.0f, "30 V"}
};
static_assert(sizeof(kVoltageRanges)/sizeof(kVoltageRanges[0]) == SystemClass::VoltageRangeCount,
              "Voltage range metadata must cover all GUI options");
struct CurrentRangeDefinition
{
    CurrentRange range;
    float maximum;
    const char* label;
};
// User-confirmed ranges, 2026-11-20; supersedes the stale I_Shunt note.
constexpr CurrentRangeDefinition kCurrentRanges[] = {
    {CurrentRange::Range10uA, 10e-6f, "10 uA"},
    {CurrentRange::Range1mA, 1e-3f, "1 mA"},
    {CurrentRange::Range100mA, 100e-3f, "100 mA"},
    {CurrentRange::Range1A, 1.0f, "1 A"}
};
static_assert(sizeof(kCurrentRanges) / sizeof(kCurrentRanges[0]) == SystemClass::CurrentRangeCount,
              "Current range metadata must cover all GUI options");
}

SystemClass::SystemClass()
{
    Reset();
}

void SystemClass::Reset()
{
    if(_acquisition) {_acquisition->Configure(false,Hardware::Request(),Hardware::SimulationSettings());_acquisition->Clear();}
    _acquisitionStats=Hardware::AcquisitionStats();
    if(_backend) {Hardware::Request off;_backend->Apply(off);}
    _uiSettings=UserInterfaceSettings();RegisterUserActivity(millis());
    _graphSettings=GraphSettings();
    _histogramSettings=HistogramSettings();
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

    _measurement.Voltage = 0.0f;
    _measurement.Current = 0.0f;
    _voltageSourceValue = 0.0f;
    _voltageLimitValue = 0.0f;
    _currentSourceValue = 0.0f;
    _currentLimitValue = 0.0f;
    _lastSimulationUs = 0;
    _nplcProfile=MeasurementTiming::Default;
    _simulationSettings=Hardware::SimulationSettings();
    _backend=nullptr;_measurement=Hardware::Measurement();
    _measurement.Fault=Hardware::Error::Unavailable;_hardwareDirty=true;

    Buffer.Reset();
}

bool SystemClass::SetBacklightPercent(unsigned percent) {
    if(percent<UserInterfaceSettings::MinBacklight || percent>UserInterfaceSettings::MaxBacklight) return false;
    _uiSettings.backlightPercent=percent;RegisterUserActivity(millis());return true;
}
bool SystemClass::SetDimmerMinutes(unsigned minutes) {
    if(minutes>UserInterfaceSettings::MaxDimmerMinutes) return false;
    _uiSettings.dimmerMinutes=minutes;RegisterUserActivity(millis());return true;
}
void SystemClass::UpdateUserInterface(uint32_t now) {
    if(_uiSettings.dimmerMinutes && uint32_t(now-_lastUserActivityMs)>=uint32_t(_uiSettings.dimmerMinutes)*60000u)
        _backlightDimmed=true;
}

bool SystemClass::SetNplcProfile(uint8_t index)
{
    if(index>=MeasurementTiming::Count) return false;
    _nplcProfile=index;
    return true;
}
void SystemClass::SetAcquisition(Hardware::Acquisition* acquisition) {
    if(_acquisition) _acquisition->Configure(false,MakeHardwareRequest(),_simulationSettings);
    _acquisition=acquisition;_acquisitionStats=Hardware::AcquisitionStats();
    if(_acquisition) _acquisition->Clear();
    _hardwareDirty=true;
}
void SystemClass::Execute() {ExecuteAtTime(millis(),micros());}
void SystemClass::ExecuteAt(uint32_t now) {ExecuteAtTime(now,now*1000u);}
void SystemClass::ExecuteAtTime(uint32_t now,uint32_t nowUs)
{
    if(_acquisition) {
        const bool enabled=_backend==&_simulator;
        _acquisition->Configure(enabled,MakeHardwareRequest(),_simulationSettings);
        _hardwareDirty=false;
        if(enabled) {
            Hardware::Measurement sample;bool received=false;
            // Bounded draining; the worker never touches the history or GUI.
            for(unsigned i=0;i<Hardware::AcquisitionCore::Capacity && _acquisition->Read(sample);++i) {
                _measurement=sample;Buffer.AddSample(sample);received=true;
            }
            if(received) {
                AutoAdjustVoltageRangeForValue(fmaxf(fabsf(sample.Voltage),fabsf(_voltageSourceValue)));
                AutoAdjustCurrentRangeForValue(fabsf(sample.Current));
                _acquisition->Configure(true,MakeHardwareRequest(),_simulationSettings);
            }
        }
        _acquisitionStats=_acquisition->Stats();
        if(enabled) return;
    }
    // Polling never invents missed samples after slow rendering. Real backends
    // own ADC timing and return false until a complete U/I pair is available.
    if(!_hardwareDirty && _backend==&_simulator &&
       uint32_t(nowUs-_lastSimulationUs)<MeasurementTiming::PairTimeUs(_nplcProfile)) return;
    _lastSimulationUs=nowUs;_hardwareDirty=false;
    if(!_backend) {
        _measurement=Hardware::Measurement();_measurement.TimeMs=now;
        _measurement.Fault=Hardware::Error::Unavailable;
        _measurement.Voltage=_measurement.Current=0;return;
    }
    _simulator.Configure(_simulationSettings);
    _backend->Apply(MakeHardwareRequest());
    Hardware::Measurement sample;
    if(!_backend->Read(now,sample)) return;
    // Auto range uses the actual shared measurement; it never changes compliance.
    AutoAdjustVoltageRangeForValue(fmaxf(fabsf(sample.Voltage),fabsf(_voltageSourceValue)));
    AutoAdjustCurrentRangeForValue(fabsf(sample.Current));
    // The ideal simulator can immediately re-evaluate after an auto-range change.
    // A real ADC must retain its first completed sample and use the new range next cycle.
    if(_backend==&_simulator) {_backend->Apply(MakeHardwareRequest());_backend->Read(now,sample);}
    _measurement=sample;
    Buffer.AddSample(sample);
}
Hardware::Request SystemClass::MakeHardwareRequest() const
{
    Hardware::Request request;
    request.nplcProfile=_nplcProfile;
    request.voltage=_voltageSourceValue;request.compliance=_currentLimitValue;
    request.voltageRange=GetVoltageRangeMax(_voltageRange);request.currentRange=GetCurrentRangeMax(_currentRange);
    request.output=_outputOn;request.voltageMode=_sourceMode==SourceMode::Voltage;
    return request;
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
    _hardwareDirty=true;
    _voltageRangeMode = mode;
    if (_voltageRangeMode == RangeMode::Auto)
    {
        RecalculateAutoVoltageRange();
    }
}

VoltageRange SystemClass::GetVoltageRange() const
{
    return _voltageRange;
}

void SystemClass::SetVoltageRange(VoltageRange range)
{
    if(!IsVoltageRangeAvailable(range)) return;
    _hardwareDirty=true;
    _voltageRange = range;
    if (_voltageRangeMode == RangeMode::Auto)
    {
        RecalculateAutoVoltageRange();
    }

    if (_voltageSourceValue < GetVoltageSourceMin() ||
        _voltageSourceValue > GetVoltageSourceMax())
    {
        _voltageSourceValue = 0.0f;
    }

    if (_voltageLimitValue < GetVoltageLimitMin() ||
        _voltageLimitValue > GetVoltageLimitMax())
    {
        _voltageLimitValue = 0.0f;
    }
}

RangeMode SystemClass::GetCurrentRangeMode() const
{
    return _currentRangeMode;
}

void SystemClass::SetCurrentRangeMode(RangeMode mode)
{
    _hardwareDirty=true;
    _currentRangeMode = mode;
    if (_currentRangeMode == RangeMode::Auto)
    {
        RecalculateAutoCurrentRange();
    }
}

CurrentRange SystemClass::GetCurrentRange() const
{
    return _currentRange;
}

void SystemClass::SetCurrentRange(CurrentRange range)
{
    if(!IsCurrentRangeAvailable(range)) return;
    _hardwareDirty=true;
    _currentRange = range;
    if (_currentRangeMode == RangeMode::Auto)
    {
        RecalculateAutoCurrentRange();
    }

    if (_currentSourceValue < GetCurrentSourceMin() ||
        _currentSourceValue > GetCurrentSourceMax())
    {
        _currentSourceValue = 0.0f;
    }


}

ResistanceRange SystemClass::GetResistanceRange() const
{
    return _resistanceRange;
}

void SystemClass::SetResistanceRange(ResistanceRange range)
{
    _hardwareDirty=true;
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
    _hardwareDirty=true;
    _sourceMode = mode;
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
    return _outputOn && (_measurement.Flags&Hardware::OutputOn);
}

void SystemClass::SetOutputOn(bool enabled)
{
    _outputOn = enabled && _backend!=nullptr;
    _hardwareDirty=true;
}

bool SystemClass::GetSimulationEnabled() const
{
    return _simulationEnabled;
}

void SystemClass::SetSimulationEnabled(bool enabled)
{
    if(enabled==_simulationEnabled) return;
    // Disable the previous backend before changing ownership; no real driver yet.
    _outputOn=false;
    if(_backend) _backend->Apply(MakeHardwareRequest());
    _backend=nullptr;_simulationEnabled=enabled;
    _measurement=Hardware::Measurement();_measurement.Voltage=_measurement.Current=0;
    if(enabled) {_backend=&_simulator;_outputOn=true;}
    else _measurement.Fault=Hardware::Error::Unavailable;
    _hardwareDirty=true;
    Execute();
}

float SystemClass::GetVoltage() const
{
    return _measurement.Voltage;
}

void SystemClass::SetVoltage(float value)
{
    _measurement.Voltage = value;
    AutoAdjustVoltageRangeForValue(value);
}

float SystemClass::GetCurrent() const
{
    return _measurement.Current;
}

void SystemClass::SetCurrent(float value)
{
    _measurement.Current = value;
    RecalculateAutoCurrentRange();
}

float SystemClass::GetResistance() const
{
    return _measurement.GetResistance();
}


float SystemClass::GetPower() const
{
    return _measurement.GetPower();
}


float SystemClass::GetVoltageSourceValue() const
{
    return _voltageSourceValue;
}

void SystemClass::SetVoltageSourceValue(float value)
{
    if(!isfinite(value)) return;
    _hardwareDirty=true;
    if (value < GetVoltageSourceMin() || value > GetVoltageSourceMax())
    {
        return;
    }

    AutoAdjustVoltageRangeForValue(value);
    _voltageSourceValue = value;
}

float SystemClass::GetVoltageSourceMin() const
{
    return -GetVoltageSourceMax();
}

float SystemClass::GetVoltageSourceMax() const
{
    if (_voltageRangeMode == RangeMode::Auto)
    {
        return IsVoltageRangeAvailable(VoltageRange::Range30V)?30.0f:5.0f;
    }

    switch (_voltageRange)
    {
        case VoltageRange::Range5V:
            return GetVoltageRangeMax(VoltageRange::Range5V);
        case VoltageRange::Range30V:
        default:
            return IsVoltageRangeAvailable(VoltageRange::Range30V)?30.0f:5.0f;
    }
}

float SystemClass::GetVoltageLimitValue() const
{
    return _voltageLimitValue;
}

void SystemClass::SetVoltageLimitValue(float value)
{
    if(!isfinite(value)) return;
    _hardwareDirty=true;
    if (value < GetVoltageLimitMin() || value > GetVoltageLimitMax())
    {
        return;
    }

    AutoAdjustVoltageRangeForValue(value);
    _voltageLimitValue = value;
}

float SystemClass::GetVoltageLimitMin() const
{
    return 0.0f;
}

float SystemClass::GetVoltageLimitMax() const
{
    if (_voltageRangeMode == RangeMode::Auto)
    {
        return IsVoltageRangeAvailable(VoltageRange::Range30V)?30.0f:5.0f;
    }

    switch (_voltageRange)
    {
        case VoltageRange::Range5V:
            return GetVoltageRangeMax(VoltageRange::Range5V);
        case VoltageRange::Range30V:
        default:
            return IsVoltageRangeAvailable(VoltageRange::Range30V)?30.0f:5.0f;
    }
}

float SystemClass::GetCurrentSourceValue() const
{
    return _currentSourceValue;
}

void SystemClass::SetCurrentSourceValue(float value)
{
    if(!isfinite(value)) return;
    _hardwareDirty=true;
    if (value < GetCurrentSourceMin() || value > GetCurrentSourceMax())
    {
        return;
    }

    _currentSourceValue = value;
    RecalculateAutoCurrentRange();
}

float SystemClass::GetCurrentSourceMin() const
{
    return -GetCurrentSourceMax();
}

float SystemClass::GetCurrentSourceMax() const
{
    if (_currentRangeMode == RangeMode::Auto)
    {
        for(int i=CurrentRangeCount-1;i>=0;--i)
            if(IsCurrentRangeAvailable(GetCurrentRangeAt(i))) return GetCurrentRangeMax(GetCurrentRangeAt(i));
        return 0;
    }

    return GetCurrentRangeMax(_currentRange);
}

float SystemClass::GetCurrentLimitValue() const
{
    return _currentLimitValue;
}

void SystemClass::SetCurrentLimitValue(float value)
{
    if(!isfinite(value)) return;
    _hardwareDirty=true;
    if (value < GetCurrentLimitMin() || value > GetCurrentLimitMax())
    {
        return;
    }

    _currentLimitValue = value;
    RecalculateAutoCurrentRange();
}

float SystemClass::GetCurrentLimitMin() const
{
    return 0.0f;
}

float SystemClass::GetCurrentLimitMax() const {return 1.0f;}

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
    return GetVoltageRangeText(_voltageRange);
}
VoltageRange SystemClass::GetVoltageRangeAt(uint8_t index)
{
    return kVoltageRanges[index < VoltageRangeCount ? index : 0].range;
}
const char* SystemClass::GetVoltageRangeText(VoltageRange range)
{
    for (const auto& definition : kVoltageRanges)
        if (definition.range == range) return definition.label;
    return "--";
}

CurrentRange SystemClass::GetCurrentRangeAt(uint8_t index)
{
    return kCurrentRanges[index < CurrentRangeCount ? index : 0].range;
}

const char* SystemClass::GetCurrentRangeText(CurrentRange range)
{
    for (const auto& definition : kCurrentRanges)
    {
        if (definition.range == range) return definition.label;
    }
    return "--";
}

const char* SystemClass::GetCurrentRangeText() const
{
    return GetCurrentRangeText(_currentRange);
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
    const auto vr=range==ResistanceRange::Range100kOhm?VoltageRange::Range30V:VoltageRange::Range5V;
    const auto ir=(range==ResistanceRange::Range10Ohm || range==ResistanceRange::Range100Ohm)?CurrentRange::Range100mA:CurrentRange::Range1mA;
    if(!IsVoltageRangeAvailable(vr) || !IsCurrentRangeAvailable(ir)) return;
    _hardwareDirty=true;
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
            break;

        case ResistanceRange::Range100Ohm:
            _voltageRange = VoltageRange::Range5V;
            _currentRange = CurrentRange::Range100mA;
            _voltageSourceValue = 1.0f;
            _currentLimitValue = 0.01f;
            break;

        case ResistanceRange::Range1kOhm:
            _voltageRange = VoltageRange::Range5V;
            _currentRange = CurrentRange::Range1mA;
            _voltageSourceValue = 1.0f;
            _currentLimitValue = 0.001f;
            break;

        case ResistanceRange::Range100kOhm:
        default:
            _voltageRange = VoltageRange::Range30V;
            _currentRange = CurrentRange::Range1mA;
            _voltageSourceValue = 10.0f;
            _currentLimitValue = 0.0001f;
            break;
    }
}

ResistanceRange SystemClass::ResolveAutoResistanceRange() const
{
    const float absResistance = (_measurement.GetResistance() < 0.0f) ? -_measurement.GetResistance() : _measurement.GetResistance();

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


void SystemClass::AutoAdjustVoltageRangeForValue(float value)
{
    if (_voltageRangeMode != RangeMode::Auto)
    {
        return;
    }

    const VoltageRange range = IsVoltageRangeAvailable(VoltageRange::Range5V) &&
        (fabsf(value)<=5 || !IsVoltageRangeAvailable(VoltageRange::Range30V))
        ? VoltageRange::Range5V : VoltageRange::Range30V;

    if (_voltageRange != range)
    {
        _voltageRange = range;
    }
}

void SystemClass::AutoAdjustCurrentRangeForValue(float value)
{
    if (_currentRangeMode != RangeMode::Auto)
    {
        return;
    }

    CurrentRange range = CurrentRange::Range1A;
    for (const auto& definition : kCurrentRanges)
    {
        if (!IsCurrentRangeAvailable(definition.range)) continue;
        range=definition.range;
        if (fabsf(value) <= definition.maximum)
        {
            range = definition.range;
            break;
        }
    }
    _currentRange = range;
}

void SystemClass::RecalculateAutoVoltageRange()
{
    if (_voltageRangeMode != RangeMode::Auto)
    {
        return;
    }

    const float maxValue = fmaxf(fmaxf(fabsf(_measurement.Voltage), fabsf(_voltageSourceValue)),
                                fabsf(_voltageLimitValue));
    AutoAdjustVoltageRangeForValue(maxValue);
}

void SystemClass::RecalculateAutoCurrentRange()
{
    if (_currentRangeMode != RangeMode::Auto)
    {
        return;
    }

    const float maxValue = fmaxf(fmaxf(fabsf(_measurement.Current), fabsf(_currentSourceValue)),
                                fabsf(_currentLimitValue));
    AutoAdjustCurrentRangeForValue(maxValue);
}

float SystemClass::GetVoltageRangeMax(VoltageRange range) const
{
    for (const auto& definition : kVoltageRanges)
        if (definition.range == range) return definition.maximum;
    return kVoltageRanges[VoltageRangeCount-1].maximum;
}

float SystemClass::GetCurrentRangeMax(CurrentRange range) const
{
    for (const auto& definition : kCurrentRanges)
    {
        if (definition.range == range) return definition.maximum;
    }
    return 0.0f;
}

SystemClass System;

BufferStatistics SystemClass::GetMeasurementStatistics() const
{
    switch (GetMeasureMode())
    {
        case MeasureMode::Voltage: return Buffer.GetStatistics(BufferQuantity::Voltage);
        case MeasureMode::Current: return Buffer.GetStatistics(BufferQuantity::Current);
        case MeasureMode::Power: return Buffer.GetStatistics(BufferQuantity::Power);
        case MeasureMode::Resistance: return Buffer.GetStatistics(BufferQuantity::Resistance);
        default: return BufferStatistics{};
    }
}

void SystemClass::ClearMeasurementBuffer()
{
    if(_acquisition) _acquisition->Clear();
    _acquisitionStats=Hardware::AcquisitionStats();
    Buffer.Reset();
}

bool SystemClass::SetGraphStyle(GraphStyle style)
{
    if(style!=GraphStyle::Lines && style!=GraphStyle::Points && style!=GraphStyle::Both) return false;
    _graphSettings.style=style;return true;
}

bool SystemClass::SetHistogramSettings(const HistogramSettings& settings)
{
    if(!ValidHistogramSettings(settings)) return false;
    _histogramSettings=settings;return true;
}

bool SystemClass::IsVoltageRangeAvailable(VoltageRange range) const {
    const auto i=static_cast<unsigned>(range);return i<2 && (_simulationSettings.voltageRanges&(1u<<i));
}
bool SystemClass::IsCurrentRangeAvailable(CurrentRange range) const {
    const auto i=static_cast<unsigned>(range);return i<4 && (_simulationSettings.currentRanges&(1u<<i));
}
bool SystemClass::SetVoltageRangeAvailable(VoltageRange range,bool enabled) {
    const auto i=static_cast<unsigned>(range);if(i>=2 || (!enabled && range==_voltageRange)) return false;
    if(enabled) _simulationSettings.voltageRanges|=1u<<i;else _simulationSettings.voltageRanges&=~(1u<<i);
    _hardwareDirty=true;return true;
}
bool SystemClass::SetCurrentRangeAvailable(CurrentRange range,bool enabled) {
    const auto i=static_cast<unsigned>(range);if(i>=4 || (!enabled && range==_currentRange)) return false;
    if(enabled) _simulationSettings.currentRanges|=1u<<i;else _simulationSettings.currentRanges&=~(1u<<i);
    _hardwareDirty=true;return true;
}
bool SystemClass::SetSimulationLoad(Hardware::LoadType load) {
    if(static_cast<unsigned>(load)>2) return false;
    _simulationSettings.load=load;_hardwareDirty=true;return true;
}
bool SystemClass::SetSimulationResistance(double ohms) {
    if(!isfinite(ohms) || ohms<=0) return false;
    _simulationSettings.resistance=ohms;_hardwareDirty=true;return true;
}
const char* SystemClass::GetBackendText() const {return _simulationEnabled?"SIM":"NO HW";}
const char* SystemClass::GetHardwareStatusText() const {
    switch(_measurement.Fault) {
        case Hardware::Error::Unavailable:return "NO HARDWARE";
        case Hardware::Error::UnsupportedMode:return "MODE UNSUPPORTED";
        case Hardware::Error::InvalidConfiguration:return "CONFIG ERROR";
        case Hardware::Error::NumericOverflow:return "NUMERIC ERROR";
        default:break;
    }
    if(_acquisitionStats.queueOverruns || _acquisitionStats.missedDeadlines) return "ACQ OVERRUN";
    if(_measurement.Flags&(Hardware::VoltageOverrange|Hardware::CurrentOverrange)) return "OVERRANGE";
    if(!(_measurement.Flags&Hardware::OutputOn)) return "OUTPUT OFF";
    return _measurement.Mode==Hardware::Regulation::CC?"CC":"CV";
}
