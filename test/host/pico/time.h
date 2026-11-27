#pragma once
#include <stdint.h>
#include <stddef.h>
#include <vector>
using alarm_id_t=int32_t;
using alarm_callback_t=int64_t (*)(alarm_id_t,void*);
struct repeating_timer_t {
    void* user_data=nullptr;
    int64_t delay_us=0;
    bool (*callback)(repeating_timer_t*)=nullptr;
    uint64_t due=0;
};
namespace FakePicoTime {
struct Alarm {alarm_id_t id;uint64_t due;alarm_callback_t callback;void* context;};
inline std::vector<Alarm>& Alarms() {static std::vector<Alarm> value;return value;}
inline uint64_t& Now() {static uint64_t value=0;return value;}
inline bool& Fail() {static bool value=false;return value;}
inline alarm_id_t Next() {static alarm_id_t value=0;return ++value;}
inline repeating_timer_t*& Repeating() {static repeating_timer_t* value=nullptr;return value;}
inline void Advance(unsigned ms) {
    const uint64_t end=Now()+uint64_t(ms)*1000;
    while(Repeating() && Repeating()->due<=end) {
        auto* timer=Repeating();Now()=timer->due;
        timer->due+=uint64_t(-timer->delay_us);
        if(!timer->callback(timer)) Repeating()=nullptr;
    }
    Now()=end;
    for(size_t i=0;i<Alarms().size();) {
        if(Alarms()[i].due>Now()) {++i;continue;}
        auto alarm=Alarms()[i];Alarms().erase(Alarms().begin()+i);
        alarm.callback(alarm.id,alarm.context);
    }
}
}
inline bool add_repeating_timer_us(int64_t delay,bool (*callback)(repeating_timer_t*),void* context,repeating_timer_t* timer) {
    if(FakePicoTime::Fail()) return false;
    timer->user_data=context;timer->delay_us=delay;timer->callback=callback;
    timer->due=FakePicoTime::Now()+uint64_t(-delay);
    FakePicoTime::Repeating()=timer;return true;
}
inline alarm_id_t add_alarm_in_ms(uint32_t ms,alarm_callback_t callback,void* context,bool) {
    if(FakePicoTime::Fail()) return -1;
    auto id=FakePicoTime::Next();
    FakePicoTime::Alarms().push_back({id,FakePicoTime::Now()+uint64_t(ms)*1000,callback,context});
    return id;
}
inline bool cancel_alarm(alarm_id_t id) {
    auto& alarms=FakePicoTime::Alarms();
    for(size_t i=0;i<alarms.size();++i) if(alarms[i].id==id) {
        alarms.erase(alarms.begin()+i);return true;
    }
    return false;
}
