#pragma once
#include "HardwareBackend.h"

namespace Hardware {
struct AcquisitionStats {uint32_t queueOverruns=0, missedDeadlines=0;};
// A bounded transport queue, not a second history buffer. Caller serializes
// Configure/Tick/Read/Clear. No SystemClass or GUI access on the producer side.
class AcquisitionCore {
public:
    static constexpr unsigned Capacity=256;
    bool Configure(bool enabled,const Request& request,const SimulationSettings& settings,uint32_t nowUs);
    void Tick(uint32_t nowMs,uint32_t nowUs);
    bool Read(Measurement& sample);
    void Clear() {_head=_count=0;_stats=AcquisitionStats();}
    AcquisitionStats Stats() const {return _stats;}
private:
    bool _enabled=false;
    Request _request;
    SimulationSettings _settings;
    SimulationBackend _backend;
    Measurement _pending[Capacity];
    unsigned _head=0,_count=0;
    uint32_t _nextUs=0;
    AcquisitionStats _stats;
};
class Acquisition {
public:
    virtual ~Acquisition()=default;
    virtual void Configure(bool enabled,const Request&,const SimulationSettings&)=0;
    virtual bool Read(Measurement&)=0;
    virtual void Clear()=0;
    virtual AcquisitionStats Stats()=0;
};
}
