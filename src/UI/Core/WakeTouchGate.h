#pragma once
// Suppress the entire wake gesture, including release and additional fingers.
class WakeTouchGate {
public:
    bool Consume(bool touching,bool dimmed) {
        if(touching && dimmed) _waking=true;
        const bool consume=_waking;
        if(!touching) _waking=false;
        return consume;
    }
private:
    bool _waking=false;
};
