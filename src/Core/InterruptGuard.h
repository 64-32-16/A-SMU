#pragma once
#include <hardware/sync.h>

// Core-0 main/IRQ exclusion only. This is NOT an inter-core mutex.
// Restore the previous state so nesting and calls from IRQs remain safe.
class InterruptGuard {
public:
    InterruptGuard() : _state(save_and_disable_interrupts()) {}
    ~InterruptGuard() {restore_interrupts(_state);}
    InterruptGuard(const InterruptGuard&) = delete;
    InterruptGuard& operator=(const InterruptGuard&) = delete;
private:
    uint32_t _state;
};
