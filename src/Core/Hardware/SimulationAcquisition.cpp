#include "SimulationAcquisition.h"
#include <Arduino.h>
namespace Hardware {
namespace {
class Lock {
public:
    explicit Lock(mutex_t& mutex):_mutex(mutex) {mutex_enter_blocking(&_mutex);}
    ~Lock() {mutex_exit(&_mutex);}
private:
    mutex_t& _mutex;
};
}
SimulationAcquisition::SimulationAcquisition() {mutex_init(&_mutex);}
void SimulationAcquisition::WorkerReady() {Lock lock(_mutex);_ready=true;}
bool SimulationAcquisition::Start() {
    Lock lock(_mutex);
    _started=_ready;
    return _started;
}
void SimulationAcquisition::RunOnce() {
    uint32_t revision;
    {
        Lock lock(_mutex);
        if(!_started) return;
        revision=_revision;
        if(_appliedRevision!=revision) {
            // Simulation Apply/Configure only copy fixed-size records. Future
            // hardware I/O must be outside this lock, with generation checking.
            if(_offPending) {
                Request off=_request;off.output=false;
                _core.Configure(false,off,_settings,micros());
                _offPending=false;
            }
            _core.Configure(_enabled,_request,_settings,micros());
            _appliedRevision=revision;
        }
    }
    Measurement sample;uint32_t missed=0;
    // No mutex while simulating, and no GUI/SPI/USB access on this core.
    if(!_core.Produce(millis(),micros(),sample,missed)) return;
    {
        Lock lock(_mutex);
        if(revision==_revision) _core.Publish(sample,missed);
    }
}
void SimulationAcquisition::Configure(bool enabled,const Request& request,const SimulationSettings& settings) {
    Lock lock(_mutex);
    enabled=enabled && _started && request.nplcProfile<MeasurementTiming::Count;
    if(enabled==_enabled && SameRequest(request,_request) && SameSimulation(settings,_settings)) return;
    if(!enabled || !request.output) _offPending=true;
    _enabled=enabled;_request=request;_settings=settings;
    ++_revision;
    // Discard old data immediately, before Core 0 starts the next frame.
    // Preserve loss counters across ordinary setpoint changes.
    _core.Discard();
}
bool SimulationAcquisition::Read(Measurement& sample) {
    Lock lock(_mutex);return _core.Read(sample);
}
void SimulationAcquisition::Clear() {
    Lock lock(_mutex);++_revision;_core.Clear();
}
AcquisitionStats SimulationAcquisition::Stats() {
    Lock lock(_mutex);auto stats=_core.Stats();
    stats.requestedRevision=_revision;stats.appliedRevision=_appliedRevision;
    return stats;
}
}
