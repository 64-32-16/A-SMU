#pragma once
#include <stdint.h>
#include <stddef.h>
#include "pico/time.h"
using byte = uint8_t;
inline uint32_t millis() { return static_cast<uint32_t>(FakePicoTime::Now()/1000); }
namespace FakeArduino {
inline void (*&BeforeMicros())() {static void (*hook)()=nullptr;return hook;}
}
inline uint32_t micros() {
    auto hook=FakeArduino::BeforeMicros();
    if(hook) {FakeArduino::BeforeMicros()=nullptr;hook();}
    return static_cast<uint32_t>(FakePicoTime::Now());
}

constexpr int OUTPUT=1,HIGH=1,LOW=0;
void pinMode(uint8_t pin,int mode);
void digitalWrite(uint8_t pin,int value);
