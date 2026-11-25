#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "System.h"
using namespace Hardware;
class TestAcquisition : public Acquisition {
public:
    AcquisitionCore core;uint32_t us=0;
    void Configure(bool enabled,const Request& r,const SimulationSettings& s) override {core.Configure(enabled,r,s,us);}
    bool Read(Measurement& m) override {return core.Read(m);}
    void Clear() override {core.Clear();}
    AcquisitionStats Stats() override {return core.Stats();}
    void Tick(uint32_t now) {us=now;core.Tick(now/1000,now);}
};
int main() {
    Request r;r.output=true;r.voltage=5;r.compliance=.02f;
    SimulationSettings settings;Measurement m;
    for(unsigned profile=0;profile<MeasurementTiming::Count;++profile) {
        AcquisitionCore core;r.nplcProfile=profile;
        assert(core.Configure(true,r,settings,0));
        const auto period=MeasurementTiming::PairTimeUs(profile);
        core.Tick((period-1)/1000,period-1);assert(!core.Read(m));
        // Rendering does not drain the queue, but acquisition still runs.
        for(unsigned n=1;n<=100;++n) core.Tick(n*period/1000,n*period);
        for(unsigned n=1;n<=100;++n) {assert(core.Read(m));assert(m.TimeMs==n*period/1000);}
        assert(!core.Read(m));assert(core.Stats().queueOverruns==0 && core.Stats().missedDeadlines==0);
    }
    AcquisitionCore core;r.nplcProfile=0;const auto period=MeasurementTiming::PairTimeUs(0);
    core.Configure(true,r,settings,0);
    for(unsigned n=1;n<=AcquisitionCore::Capacity+5;++n) core.Tick(n*period/1000,n*period);
    assert(core.Stats().queueOverruns==5);assert(core.Read(m) && m.TimeMs==6*period/1000);
    core.Clear();assert(!core.Read(m) && core.Stats().queueOverruns==0);
    core.Configure(false,r,settings,0);core.Configure(true,r,settings,0);
    core.Tick(2,5*period);assert(core.Stats().missedDeadlines==4);
    assert(core.Read(m) && !core.Read(m)); // no fabricated catch-up values
    core.Configure(false,r,settings,5*period);core.Tick(3,6*period);assert(!core.Read(m));
    core.Configure(true,r,settings,0xffffff00u);
    core.Tick(0,0x8f);assert(!core.Read(m));core.Tick(0,0x90);assert(core.Read(m));
    core.Configure(false,r,settings,0);core.Configure(true,r,settings,0);
    core.Tick(1,period);r.voltage=2;core.Configure(true,r,settings,period);
    assert(!core.Read(m));core.Tick(1,2*period);assert(core.Read(m) && m.Voltage<2.001f);

    // System history stays unchanged during a frame; all produced pairs arrive
    // afterward, and the displayed value is the last complete pair.
    TestAcquisition transport;SystemClass system;
    system.SetSimulationEnabled(true);system.SetVoltageSourceValue(5);system.SetCurrentLimitValue(.02f);
    system.SetNplcProfile(0);system.SetAcquisition(&transport);system.ClearMeasurementBuffer();system.ExecuteAtTime(0,0);
    for(unsigned n=1;n<=200;++n) transport.Tick(n*period);
    assert(system.Buffer.IsEmpty());
    system.ExecuteAtTime(80,80000);assert(system.Buffer.GetCount()==200);
    assert(system.GetMeasurement().TimeMs==80 && system.Buffer.GetLatest()->TimeMs==80);
    const auto view=system.Buffer.View();const float first=view[0].Voltage;
    transport.Tick(201*period);assert(view.Count()==200 && view[0].Voltage==first);
    system.ClearMeasurementBuffer();system.ExecuteAtTime(81,81000);assert(system.Buffer.IsEmpty());
    for(unsigned n=202;n<202+AcquisitionCore::Capacity+2;++n) transport.Tick(n*period);
    system.ExecuteAtTime(200,200000);assert(system.GetAcquisitionStats().queueOverruns>0);
    assert(strcmp(system.GetHardwareStatusText(),"ACQ OVERRUN")==0);
    system.ClearMeasurementBuffer();assert(system.GetAcquisitionStats().queueOverruns==0);
    transport.Tick(500*period);system.SetSimulationEnabled(false);
    system.ExecuteAtTime(201,201000);assert(system.Buffer.IsEmpty());
    assert(!system.GetOutputOn() && strcmp(system.GetHardwareStatusText(),"NO HARDWARE")==0);
    puts("PASS: all NPLC cadences, acquisition during rendering, bounded FIFO, overflow/deadline counters, wrap, OFF/reconfigure/clear, stable zero-copy views, newest display value");
}
