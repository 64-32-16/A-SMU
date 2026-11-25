#pragma once
#include "Acquisition.h"
#include <mbed.h>
#include <rtos.h>

namespace Hardware {
// Simulator only: deliberately no SPI calls in this worker. A future ADC
// implementation must provide transaction-level arbitration with EVE first.
class SimulationAcquisition : public Acquisition {
public:
    SimulationAcquisition();
    bool Start();
    void Configure(bool enabled,const Request&,const SimulationSettings&) override;
    bool Read(Measurement&) override;
    void Clear() override;
    AcquisitionStats Stats() override;
private:
    void Signal();
    void Run();
    alignas(8) unsigned char _stack[4096];
    rtos::Thread _thread;
    rtos::Mutex _mutex;
    rtos::EventFlags _events;
    mbed::Ticker _ticker;
    AcquisitionCore _core;
    bool _started=false;
};
}
