#include "HardwareBackend.h"
#include <float.h>
namespace Hardware {
namespace {
// Stateless timestamp hash: repeated reads during auto-ranging use the same noise.
// Separate channel salts keep voltage/current noise independent, with no RAM buffer.
double NoiseFactor(uint32_t time,uint32_t salt) {
    uint32_t value=time^salt;
    value^=value>>16;value*=0x7feb352du;
    value^=value>>15;value*=0x846ca68bu;value^=value>>16;
    return 1.0+0.0001*(2.0*(value>>8)/16777215.0-1.0);
}
}

bool SimulationBackend::Read(uint32_t now,Measurement& result) {
    result=Measurement();result.TimeMs=now;
    const auto& r=_request;
    if(!isfinite(r.voltage) || !isfinite(r.compliance) || r.compliance<0 ||
       !isfinite(r.voltageRange) || r.voltageRange<=0 || !isfinite(r.currentRange) || r.currentRange<=0 ||
       !isfinite(_settings.resistance) || _settings.resistance<=0 ||
       static_cast<unsigned>(_settings.load)>static_cast<unsigned>(LoadType::Short)) {
        result.Fault=Error::InvalidConfiguration;return true;
    }
    if(!r.output) return true;
    if(!r.voltageMode) {result.Fault=Error::UnsupportedMode;return true;}
    double u=r.voltage,i=0;Regulation mode=Regulation::CV;
    const double sign=r.voltage>0?1:r.voltage<0?-1:0;
    if(_settings.load==LoadType::Short) {u=0;i=sign*r.compliance;mode=sign?Regulation::CC:Regulation::CV;}
    else if(_settings.load==LoadType::Resistor) {
        i=u/_settings.resistance;
        // Compare at the precision of the float setpoint (equality remains CV).
        if(fabs(i)>r.compliance && fabs(i)-r.compliance>r.compliance*FLT_EPSILON) {i=sign*r.compliance;u=i*_settings.resistance;mode=Regulation::CC;}
    }
    // Measurement noise follows the ideal source/compliance model. Zero stays zero.
    u*=NoiseFactor(now,0xa511e9b3u);
    i*=NoiseFactor(now,0x63d83595u);
    if(!isfinite(u) || !isfinite(i) || fabs(u)>FLT_MAX || fabs(i)>FLT_MAX || fabs(u*i)>FLT_MAX) {
        result.Fault=Error::NumericOverflow;return true;
    }
    result=MakeMeasurement(now,static_cast<float>(u),static_cast<float>(i));
    result.Mode=mode;result.Flags=OutputOn;
    if(fabsf(result.Voltage)>r.voltageRange) result.Flags|=VoltageOverrange;
    if(fabsf(result.Current)>r.currentRange) result.Flags|=CurrentOverrange;
    return true;
}
}
