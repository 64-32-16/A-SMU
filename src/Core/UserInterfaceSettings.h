#pragma once
#include <stdint.h>
struct UserInterfaceSettings {
    static constexpr unsigned MinBacklight=5, MaxBacklight=100;
    static constexpr unsigned MaxDimmerMinutes=1440;
    bool keyClick=true;
    uint8_t backlightPercent=40;
    uint16_t dimmerMinutes=30; // zero disables inactivity dimming
};
