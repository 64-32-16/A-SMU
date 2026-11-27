#include <assert.h>
#include <stdio.h>
#include <atomic>
#include <thread>
#include "Core/Hardware/SimulationAcquisition.h"
#include <Arduino.h>
using namespace Hardware;
static SimulationAcquisition* inFlight=nullptr;
static void ClearInFlight() {inFlight->Clear();}
int main() {
    SimulationAcquisition acquisition;
    assert(!acquisition.Start());
    acquisition.WorkerReady();assert(acquisition.Start());
    Request request;request.output=true;request.voltage=1;request.compliance=1;request.nplcProfile=0;
    SimulationSettings settings;
    acquisition.Configure(true,request,settings);
    assert(acquisition.Stats().requestedRevision!=acquisition.Stats().appliedRevision);
    acquisition.RunOnce();
    const auto period=MeasurementTiming::PairTimeUs(0);
    for(unsigned i=0;i<AcquisitionCore::Capacity+5;++i) {
        FakePicoTime::Now()+=period;acquisition.RunOnce();
    }
    assert(acquisition.Stats().queueOverruns==5);
    Measurement sample;unsigned count=0;
    while(acquisition.Read(sample)) {assert(sample.Voltage>0.999f && sample.Voltage<1.001f);++count;}
    assert(count==AcquisitionCore::Capacity);
    acquisition.Clear();assert(!acquisition.Read(sample));assert(acquisition.Stats().queueOverruns==0);
    acquisition.Configure(false,request,settings);acquisition.RunOnce();
    FakePicoTime::Now()+=period;acquisition.RunOnce();assert(!acquisition.Read(sample));
    acquisition.Configure(true,request,settings);acquisition.RunOnce();
    FakePicoTime::Now()+=period;acquisition.RunOnce();assert(acquisition.Read(sample));
    // OFF then ON before worker service must restart the acquisition deadline.
    request.output=false;acquisition.Configure(true,request,settings);
    request.output=true;acquisition.Configure(true,request,settings);
    FakePicoTime::Now()+=period;acquisition.RunOnce();assert(!acquisition.Read(sample));
    FakePicoTime::Now()+=period;acquisition.RunOnce();assert(acquisition.Read(sample));
    assert(acquisition.Stats().requestedRevision==acquisition.Stats().appliedRevision);

    // Deterministically clear after the worker captured the revision but before
    // it publishes. The old in-flight result must never reappear after CLEAR.
    inFlight=&acquisition;FakePicoTime::Now()+=period;
    FakeArduino::BeforeMicros()=ClearInFlight;
    acquisition.RunOnce();assert(!acquisition.Read(sample));
    acquisition.RunOnce();FakePicoTime::Now()+=period;acquisition.RunOnce();
    assert(acquisition.Read(sample));

    // Real concurrent producer/consumer, including reconfiguration while a
    // producer calculation may be in flight. Only worker advances the clock.
    std::atomic<bool> stop(false);
    std::thread producer([&] {
        while(!stop.load()) {
            FakePicoTime::Now()+=period;acquisition.RunOnce();std::this_thread::yield();
        }
    });
    for(unsigned n=0;n<2000;++n) {
        request.voltage=(n%4)+1.0f;
        acquisition.Configure(true,request,settings);
        if(n%7==0) acquisition.Clear();
        for(unsigned i=0;i<16;++i) {
            if(acquisition.Read(sample)) {
                assert(sample.Voltage>request.voltage*0.999f && sample.Voltage<request.voltage*1.001f);
                assert(sample.Current>sample.Voltage/1000*0.999f && sample.Current<sample.Voltage/1000*1.001f);
            }
            std::this_thread::yield();
        }
    }
    stop=true;producer.join();
    acquisition.RunOnce();FakePicoTime::Now()+=period;acquisition.RunOnce();
    assert(acquisition.Read(sample));
    assert(acquisition.Stats().requestedRevision==acquisition.Stats().appliedRevision);
    puts("PASS: Core-1 handshake, bounded FIFO/drop-oldest, OFF/ON barrier, CLEAR, acknowledgements, concurrent settings/read/publication");
}
