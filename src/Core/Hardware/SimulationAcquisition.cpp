#include "SimulationAcquisition.h"
#include <Arduino.h>
#include <chrono>
namespace Hardware {
SimulationAcquisition::SimulationAcquisition():_thread(osPriorityAboveNormal,sizeof(_stack),_stack,"measurement") {}
bool SimulationAcquisition::Start() {
    if(!_started) _started=_thread.start(mbed::callback(this,&SimulationAcquisition::Run))==osOK;
    return _started;
}
void SimulationAcquisition::Signal() {_events.set(1);}
void SimulationAcquisition::Run() {
    for(;;) {
        _events.wait_any(1);
        _mutex.lock();_core.Tick(millis(),micros());_mutex.unlock();
    }
}
void SimulationAcquisition::Configure(bool enabled,const Request& request,const SimulationSettings& settings) {
    _mutex.lock();
    if(_core.Configure(enabled && _started,request,settings,micros())) {
        _ticker.detach();
        if(enabled && _started && request.nplcProfile<MeasurementTiming::Count)
            _ticker.attach(mbed::callback(this,&SimulationAcquisition::Signal),
                           std::chrono::microseconds(MeasurementTiming::PairTimeUs(request.nplcProfile)));
    }
    _mutex.unlock();
}
bool SimulationAcquisition::Read(Measurement& sample) {
    _mutex.lock();const bool result=_core.Read(sample);_mutex.unlock();return result;
}
void SimulationAcquisition::Clear() {_mutex.lock();_core.Clear();_mutex.unlock();}
AcquisitionStats SimulationAcquisition::Stats() {_mutex.lock();auto stats=_core.Stats();_mutex.unlock();return stats;}
}
