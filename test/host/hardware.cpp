#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "System.h"
#include "../../src/UI/Core/WakeTouchGate.h"
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
    // NPLC profiles control actual simulator cadence, including sub-ms and wrap.
    SystemClass timed;timed.SetSimulationEnabled(true);
    assert(timed.GetNplcProfile()==MeasurementTiming::Default);
    assert(!timed.SetNplcProfile(MeasurementTiming::Count));
    for(uint8_t profile=0;profile<MeasurementTiming::Count;++profile) {
        assert(timed.SetNplcProfile(profile));
        timed.ExecuteAtTime(1000,1000000);
        const auto before=timed.Buffer.GetTotalSamples();
        const auto period=MeasurementTiming::PairTimeUs(profile);
        timed.ExecuteAtTime(1000+(period-1)/1000,1000000+period-1);
        assert(timed.Buffer.GetTotalSamples()==before);
        timed.ExecuteAtTime(1000+period/1000,1000000+period);
        assert(timed.Buffer.GetTotalSamples()==before+1);
        // No fabricated catch-up readings after a slow frame.
        timed.ExecuteAtTime(2000,2000000);
        assert(timed.Buffer.GetTotalSamples()==before+2);
    }
    timed.SetNplcProfile(0);timed.ExecuteAtTime(3000,0xffffff00u);
    auto beforeWrap=timed.Buffer.GetTotalSamples();
    timed.ExecuteAtTime(3000,0x8fu);assert(timed.Buffer.GetTotalSamples()==beforeWrap);
    timed.ExecuteAtTime(3000,0x90u);assert(timed.Buffer.GetTotalSamples()==beforeWrap+1);
    timed.Reset();assert(timed.GetNplcProfile()==MeasurementTiming::Default);
    SystemClass preferences;
    assert(preferences.GetUserInterfaceSettings().keyClick && preferences.GetUserInterfaceSettings().dimmerMinutes==30);
    assert(!preferences.SetBacklightPercent(UserInterfaceSettings::MinBacklight-1));
    assert(!preferences.SetBacklightPercent(UserInterfaceSettings::MaxBacklight+1));
    assert(preferences.SetBacklightPercent(80));
    preferences.RegisterUserActivity(100);preferences.UpdateUserInterface(100+30*60000-1);
    assert(!preferences.IsBacklightDimmed() && preferences.GetEffectiveBacklightPercent()==80);
    preferences.UpdateUserInterface(100+30*60000);assert(preferences.IsBacklightDimmed());
    assert(preferences.GetEffectiveBacklightPercent()==UserInterfaceSettings::MinBacklight);
    preferences.UpdateUserInterface(0);assert(preferences.IsBacklightDimmed()); // dim state latches across clock wrap
    WakeTouchGate gate;assert(gate.Consume(true,preferences.IsBacklightDimmed()));
    preferences.RegisterUserActivity(2000000);assert(preferences.GetEffectiveBacklightPercent()==80);
    assert(gate.Consume(true,false));assert(gate.Consume(false,false));assert(!gate.Consume(true,false));
    assert(preferences.SetDimmerMinutes(1));preferences.RegisterUserActivity(0xfffffff0);
    preferences.UpdateUserInterface(0xfffffff0u+59999u);assert(!preferences.IsBacklightDimmed());
    preferences.UpdateUserInterface(0xfffffff0u+60000u);assert(preferences.IsBacklightDimmed());
    assert(preferences.SetDimmerMinutes(0));preferences.UpdateUserInterface(0xffffffff);assert(!preferences.IsBacklightDimmed());
    assert(!preferences.SetDimmerMinutes(UserInterfaceSettings::MaxDimmerMinutes+1));
    puts("PASS: bounded/balanced 0.01% noise, repeatable channels, derived values, ideal loads, bipolar CV/CC, zero/off, overrange, invalid input, shared samples, timing, range masks, backend switch");
}
