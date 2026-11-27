#pragma once
#include <stdint.h>
namespace FakeIrq {
inline uint32_t& Disabled() {static uint32_t state=0;return state;}
}
inline uint32_t save_and_disable_interrupts() {
    uint32_t old=FakeIrq::Disabled();FakeIrq::Disabled()=1;return old;
}
inline void restore_interrupts(uint32_t state) {FakeIrq::Disabled()=state;}
