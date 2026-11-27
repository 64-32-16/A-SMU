#pragma once
#include "Acquisition.h"
#include <pico/mutex.h>

namespace Hardware {
// Core 0 owns the consumer/configuration API; Core 1 owns RunOnce and backend.
// One transport FIFO lives in _core. Never expose it or SystemClass to Core 1.
class SimulationAcquisition : public Acquisition {
public:
    SimulationAcquisition();
    SimulationAcquisition(const SimulationAcquisition&)=delete;
    SimulationAcquisition& operator=(const SimulationAcquisition&)=delete;
    bool Start();
    void WorkerReady();
    void RunOnce();
    void Configure(bool enabled,const Request&,const SimulationSettings&) override;
    bool Read(Measurement&) override;
    void Clear() override;
    AcquisitionStats Stats() override;
private:
    mutex_t _mutex;
    AcquisitionCore _core;
    Request _request;
    SimulationSettings _settings;
    bool _enabled=false, _started=false, _ready=false;
    // Latest complete configuration; OFF is sticky until consumed, even if
    // followed immediately by ON. Clear/config revisions reject in-flight data.
    bool _offPending=false;
    uint32_t _revision=0, _appliedRevision=0;
};
}
