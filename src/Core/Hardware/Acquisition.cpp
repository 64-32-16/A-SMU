#include "Acquisition.h"
namespace Hardware {
namespace {
bool Same(const Request& a,const Request& b) {
    return a.nplcProfile==b.nplcProfile && a.voltage==b.voltage && a.compliance==b.compliance &&
        a.voltageRange==b.voltageRange && a.currentRange==b.currentRange && a.output==b.output && a.voltageMode==b.voltageMode;
}
bool Same(const SimulationSettings& a,const SimulationSettings& b) {
    return a.load==b.load && a.resistance==b.resistance && a.voltageRanges==b.voltageRanges && a.currentRanges==b.currentRanges;
}
void AddSaturating(uint32_t& value,uint32_t add) {value=UINT32_MAX-value<add?UINT32_MAX:value+add;}
}
bool AcquisitionCore::Configure(bool enabled,const Request& request,const SimulationSettings& settings,uint32_t nowUs) {
    enabled=enabled && request.nplcProfile<MeasurementTiming::Count;
    if(enabled==_enabled && Same(request,_request) && Same(settings,_settings)) return false;
    _enabled=enabled;_request=request;_settings=settings;
    // Old queued readings must not appear after OFF or a configuration change.
    _head=_count=0;
    _backend.Configure(settings);_backend.Apply(request);
    if(enabled) _nextUs=nowUs+MeasurementTiming::PairTimeUs(request.nplcProfile);
    return true;
}
void AcquisitionCore::Tick(uint32_t nowMs,uint32_t nowUs) {
    if(!_enabled || int32_t(nowUs-_nextUs)<0) return;
    const uint32_t period=MeasurementTiming::PairTimeUs(_request.nplcProfile);
    const uint32_t missed=uint32_t(nowUs-_nextUs)/period;
    AddSaturating(_stats.missedDeadlines,missed);
    _nextUs+=(missed+1)*period;
    Measurement sample;
    if(!_backend.Read(nowMs,sample)) return;
    if(_count==Capacity) {
        // Keep the newest actual reading, with an explicit loss counter.
        _head=(_head+1)%Capacity;--_count;AddSaturating(_stats.queueOverruns,1);
    }
    _pending[(_head+_count)%Capacity]=sample;++_count;
}
bool AcquisitionCore::Read(Measurement& sample) {
    if(!_count) return false;
    sample=_pending[_head];_head=(_head+1)%Capacity;--_count;return true;
}
}
