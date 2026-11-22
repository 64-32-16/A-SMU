#pragma once
#include <stdint.h>
#include <math.h>

namespace Hardware {
enum class Regulation : uint8_t { Off, CV, CC };
enum class Error : uint8_t { None, Unavailable, UnsupportedMode, InvalidConfiguration, NumericOverflow };
enum SampleFlags : uint8_t { OutputOn=1, VoltageOverrange=2, CurrentOverrange=4 };
// Compact shared record: derived quantities never occupy buffer RAM.
struct Measurement {
    float Voltage=0, Current=0;
    uint32_t TimeMs=0;
    Regulation Mode=Regulation::Off;
    Error Fault=Error::None;
    uint8_t Flags=0;
    float GetPower() const {return Voltage*Current;}
    bool HasValidResistance() const {
        return isfinite(Voltage) && isfinite(Current) && fabsf(Current)>=1e-9f &&
               isfinite(Voltage/Current);
    }
    float GetResistance() const {return HasValidResistance()?Voltage/Current:0.0f;}
};
static_assert(sizeof(Measurement)==16,"Keep measurement buffers bounded");
struct Request {
    float voltage=0, compliance=0;
    float voltageRange=5, currentRange=0.1f;
    bool output=false, voltageMode=true;
};
enum class LoadType : uint8_t { Resistor, Open, Short };
struct SimulationSettings {
    LoadType load=LoadType::Resistor;
    double resistance=1000;
    uint8_t voltageRanges=3, currentRanges=15;
};
// Called by backends, never by GUI. Legacy test injection uses this same conversion.
inline Measurement MakeMeasurement(uint32_t time,float voltage,float current) {
    Measurement m; m.TimeMs=time;m.Voltage=voltage;m.Current=current;
    return m;
}
class Backend {
public:
    virtual ~Backend()=default;
    // Apply must accept output=false before a backend is detached.
    virtual void Apply(const Request& request)=0;
    // now is monotonic milliseconds; false means a real ADC has no new sample yet.
    virtual bool Read(uint32_t now,Measurement& measurement)=0;
};
class SimulationBackend : public Backend {
public:
    void Configure(const SimulationSettings& settings) {_settings=settings;}
    void Apply(const Request& request) override {_request=request;}
    bool Read(uint32_t now,Measurement& measurement) override;
private:
    Request _request;
    SimulationSettings _settings;
};
}
