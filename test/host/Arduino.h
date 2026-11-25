#pragma once
#include <stdint.h>
#include <stddef.h>
using byte = uint8_t;
inline uint32_t millis() { return 0; }
inline uint32_t micros() { return 0; }

constexpr int OUTPUT=1,HIGH=1,LOW=0;
void pinMode(uint8_t pin,int mode);
void digitalWrite(uint8_t pin,int value);
