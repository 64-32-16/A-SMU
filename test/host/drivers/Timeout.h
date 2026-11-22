#pragma once
#include <chrono>
#include <functional>
namespace mbed {
class Timeout {
    std::function<void()> callback_;
    long long due_=0;
    static Timeout*& current() {static Timeout* value=nullptr;return value;}
    static long long& now() {static long long value=0;return value;}
public:
    void detach() {callback_=nullptr;if(current()==this) current()=nullptr;}
    void attach(std::function<void()> callback,std::chrono::microseconds duration) {
        callback_=callback;due_=now()+duration.count();current()=this;
    }
    static void Advance(unsigned milliseconds) {
        now()+=milliseconds*1000LL;
        auto* timer=current();
        if(timer && timer->due_<=now()) {
            auto callback=timer->callback_;timer->detach();callback();
        }
    }
};
}
