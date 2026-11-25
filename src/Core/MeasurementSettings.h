#pragma once
#include <stdint.h>

// Nominal AD7177-2 sinc5+sinc1 settling times at 16 MHz, 50 Hz only.
// NPLC is an equivalent filter-time label, not a rectangular aperture.
namespace MeasurementTiming {
struct Profile { float nplc; uint32_t channelTimeUs; };
constexpr Profile Profiles[]={{0.01f,200},{0.1f,2000},{0.5f,10000},{1,20016},{5,100000},{10,200000}};
constexpr uint8_t Count=sizeof(Profiles)/sizeof(Profiles[0]);
constexpr uint8_t Default=3;
constexpr unsigned LineFrequencyHz=50;
inline uint32_t PairTimeUs(uint8_t index) {return 2*Profiles[index].channelTimeUs;}
inline double PairsPerSecond(uint8_t index) {return 1000000.0/PairTimeUs(index);}
}
