#pragma once
#include <stdint.h>

// Calibrated screen coordinates; id identifies the controller slot.
struct TouchPoint { int16_t x=0, y=0; uint8_t id=0; };
struct TouchFrame {
    TouchPoint points[5];
    uint8_t count=0;
    void Add(uint32_t xy, uint8_t id) {
        const int16_t x=static_cast<int16_t>(xy>>16), y=static_cast<int16_t>(xy);
        if(x!=-32768 && y!=-32768 && count<5) points[count++]={x,y,id};
    }
};
