#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "System.h"
static void Near(double a,double b) {assert(fabs(a-b)<=fmax(1e-12,fabs(b)*1e-6));}
static void NoisyNear(double a,double b) {assert(fabs(a-b)<=fmax(1e-12,fabs(b)*0.000201));}
int main() {
    using namespace Hardware;
    SimulationBackend backend;Request r;r.voltage=5;r.compliance=0.02f;r.output=true;
    SimulationSettings settings;Measurement m;
    auto read=[&]() {backend.Configure(settings);backend.Apply(r);assert(backend.Read(123,m));assert(m.TimeMs==123);};
    read();NoisyNear(m.Voltage,5);NoisyNear(m.Current,.005);NoisyNear(m.GetPower(),.025);assert(m.Mode==Regulation::CV);
    settings.resistance=250;read();assert(m.Mode==Regulation::CV);NoisyNear(m.Current,.02);
    settings.resistance=100;read();NoisyNear(m.Voltage,2);NoisyNear(m.Current,.02);NoisyNear(m.GetPower(),.04);assert(m.Mode==Regulation::CC);
    settings.load=LoadType::Open;read();NoisyNear(m.Voltage,5);NoisyNear(m.Current,0);assert(m.Mode==Regulation::CV && !m.HasValidResistance());
    settings.load=LoadType::Short;read();NoisyNear(m.Voltage,0);NoisyNear(m.Current,.02);assert(m.Mode==Regulation::CC);
    r.voltage=-5;read();NoisyNear(m.Current,-.02);NoisyNear(m.Voltage,0);
    settings.load=LoadType::Resistor;read();NoisyNear(m.Voltage,-2);NoisyNear(m.Current,-.02);NoisyNear(m.GetPower(),.04);
    r.voltage=0;read();NoisyNear(m.Voltage,0);NoisyNear(m.Current,0);assert(m.Mode==Regulation::CV);
    settings.load=LoadType::Short;read();NoisyNear(m.Current,0);assert(m.Mode==Regulation::CV);
    r.voltage=5;r.compliance=0;read();NoisyNear(m.Current,0);assert(m.Mode==Regulation::CC);
    settings.load=LoadType::Resistor;read();NoisyNear(m.Voltage,0);assert(m.Mode==Regulation::CC);
    settings.load=LoadType::Open;read();NoisyNear(m.Voltage,5);assert(m.Mode==Regulation::CV);
    r.output=false;read();NoisyNear(m.Voltage,0);NoisyNear(m.Current,0);assert(m.Mode==Regulation::Off && !(m.Flags&OutputOn));
    r.output=true;r.compliance=.02f;settings.load=LoadType::Resistor;settings.resistance=1000;
    r.voltageRange=1;r.currentRange=.001f;read();assert((m.Flags&(VoltageOverrange|CurrentOverrange))==(VoltageOverrange|CurrentOverrange));NoisyNear(m.Current,.005);Near(r.compliance,.02);
    r.voltageMode=false;read();assert(m.Fault==Error::UnsupportedMode && !(m.Flags&OutputOn));
    r.voltageMode=true;r.voltage=NAN;read();assert(m.Fault==Error::InvalidConfiguration);
    r.voltage=5;settings.resistance=0;read();assert(m.Fault==Error::InvalidConfiguration);
    settings.resistance=1e-300;read();NoisyNear(m.Current,.02);assert(m.Fault==Error::None);
    settings.resistance=1e300;read();assert(m.Fault==Error::None && !m.HasValidResistance());

    // Noise is bounded per channel, balanced and repeatable within one timestamp.
    r=Request();r.output=true;r.voltage=5;r.compliance=.02f;settings=SimulationSettings();
    double sumU=0,sumI=0,sumCross=0;float minU=6,maxU=0;
    backend.Configure(settings);backend.Apply(r);
    for(uint32_t t=0;t<10000;++t) {
        backend.Read(t*100,m);Measurement again;backend.Read(t*100,again);
        assert(m.Voltage==again.Voltage && m.Current==again.Current);
        assert(m.Voltage>=4.9995f && m.Voltage<=5.0005f);
        assert(m.Current>=.0049995f && m.Current<=.0050005f);
        assert(m.Mode==Regulation::CV && m.Fault==Error::None);
        Near(m.GetPower(),m.Voltage*m.Current);Near(m.GetResistance(),m.Voltage/m.Current);
        assert(bool(m.Flags&VoltageOverrange)==(m.Voltage>r.voltageRange));
        double du=m.Voltage/5.0-1,di=m.Current/.005-1;sumU+=du;sumI+=di;sumCross+=du*di;
        minU=fminf(minU,m.Voltage);maxU=fmaxf(maxU,m.Voltage);
    }
    assert(minU<4.9996f && maxU>5.0004f);
    assert(fabs(sumU/10000)<.000003 && fabs(sumI/10000)<.000003 && fabs(sumCross/10000)<.0000000002);

    SystemClass system;assert(!system.GetOutputOn());system.SetOutputOn(true);assert(!system.GetOutputOn());
    system.ExecuteAt(10);assert(system.GetMeasurement().Fault==Error::Unavailable && system.Buffer.IsEmpty());
    system.SetSimulationEnabled(true);assert(system.GetOutputOn());
    system.SetVoltageSourceValue(5);system.SetCurrentLimitValue(.02f);system.ExecuteAt(100);
    NoisyNear(system.GetVoltage(),5);NoisyNear(system.GetCurrent(),.005);NoisyNear(system.GetPower(),.025);
    const auto* sample=system.Buffer.GetLatest();assert(sample && sample->TimeMs==100);
    Near(sample->GetPower(),system.GetMeasurement().GetPower());assert(sample->Flags==system.GetMeasurement().Flags);
    auto count=system.Buffer.GetCount();system.ExecuteAt(101);assert(system.Buffer.GetCount()==count);
    system.SetSimulationResistance(100);system.ExecuteAt(102);assert(system.Buffer.GetCount()==count+1);NoisyNear(system.GetVoltage(),2);
    system.SetCurrentRange(CurrentRange::Range1mA);system.ExecuteAt(103);Near(system.GetCurrentLimitValue(),.02);
    assert(system.GetMeasurement().Flags&CurrentOverrange);NoisyNear(system.GetCurrent(),.02);
    assert(!system.SetCurrentRangeAvailable(CurrentRange::Range1mA,false));
    assert(system.SetCurrentRangeAvailable(CurrentRange::Range100mA,false));
    system.SetCurrentRange(CurrentRange::Range100mA);assert(system.GetCurrentRange()==CurrentRange::Range1mA);
    system.SetCurrentRangeMode(RangeMode::Auto);system.ExecuteAt(104);assert(system.GetCurrentRange()==CurrentRange::Range1A);
    assert(!(system.GetMeasurement().Flags&CurrentOverrange));
    assert(!system.SetVoltageRangeAvailable(VoltageRange::Range5V,false));
    assert(system.SetVoltageRangeAvailable(VoltageRange::Range30V,false));system.SetVoltageRangeMode(RangeMode::Auto);
    system.SetVoltageSourceValue(6);Near(system.GetVoltageSourceValue(),5);system.SetVoltageRange(VoltageRange::Range30V);
    assert(system.GetVoltageRange()==VoltageRange::Range5V);
    assert(!system.SetSimulationResistance(0) && !system.SetSimulationResistance(-1) && !system.SetSimulationResistance(INFINITY));
    assert(!system.SetSimulationLoad(static_cast<LoadType>(99)));
    system.SetVoltageSourceValue(NAN);system.SetCurrentLimitValue(INFINITY);Near(system.GetVoltageSourceValue(),5);Near(system.GetCurrentLimitValue(),.02);
    system.SetOutputOn(false);system.ExecuteAt(105);NoisyNear(system.GetVoltage(),0);NoisyNear(system.GetCurrent(),0);assert(system.GetMeasurement().Mode==Regulation::Off);
    system.SetSimulationEnabled(false);assert(!system.GetOutputOn());count=system.Buffer.GetCount();system.ExecuteAt(1000);assert(system.Buffer.GetCount()==count);
    system.SetSimulationEnabled(true);system.SetSourceMode(SourceMode::Current);system.ExecuteAt(1001);assert(system.GetMeasurement().Fault==Error::UnsupportedMode);
    assert(system.Buffer.GetLatest()->Fault==Error::UnsupportedMode);
    system.SetSourceMode(SourceMode::Voltage);system.ExecuteAt(1002);NoisyNear(system.GetCurrent(),.02);
    system.ExecuteAt(0xfffffff0);count=system.Buffer.GetCount();system.ExecuteAt(0x10);assert(system.Buffer.GetCount()==count);system.ExecuteAt(0x100);assert(system.Buffer.GetCount()==count+1);
    puts("PASS: bounded/balanced 0.01% noise, repeatable channels, derived values, ideal loads, bipolar CV/CC, zero/off, overrange, invalid input, shared samples, timing, range masks, backend switch");
}
