#pragma once
#include "HardwareBackend.h"

namespace Hardware {
bool SameRequest(const Request&,const Request&);
bool SameSimulation(const SimulationSettings&,const SimulationSettings&);
struct AcquisitionStats {
    uint32_t queueOverruns=0, missedDeadlines=0;
    uint32_t requestedRevision=0, appliedRevision=0;
};
// A bounded transport queue, not a second history buffer. Caller serializes
// Configure/Tick/Read/Clear. No SystemClass or GUI access on the producer side.
class AcquisitionCore {
public:
    static constexpr unsigned Capacity=256;
    bool Configure(bool enabled,const Request& request,const SimulationSettings& settings,uint32_t nowUs);
    void Tick(uint32_t nowMs,uint32_t nowUs);
    // Producer-only calculation; no queue or shared statistics access.
    bool Produce(uint32_t nowMs,uint32_t nowUs,Measurement&,uint32_t& missed);
    // Caller protects publication together with Read/Clear/Stats.
    void Publish(const Measurement&,uint32_t missed);
    bool Read(Measurement& sample);
    void Discard() {_head=_count=0;}
    void Clear() {Discard();_stats=AcquisitionStats();}
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
