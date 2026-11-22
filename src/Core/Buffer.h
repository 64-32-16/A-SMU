#ifndef _BUFFER_H_INCLUDED
#define _BUFFER_H_INCLUDED

#include <Arduino.h>

#include "Hardware/HardwareBackend.h"
using BufferSample = Hardware::Measurement;

enum class BufferQuantity : uint8_t { Voltage, Current, Power, Resistance };

struct BufferStatistics
{
    uint16_t Count = 0;
    float Minimum = 0;
    float Maximum = 0;
    float Average = 0;
    float PeakToPeak = 0;
    // Population standard deviation of the valid samples currently retained.
    float StandardDeviation = 0;
};

class BufferClass
{
public:
    static const uint16_t Capacity = 1024;

    BufferClass();

    void Reset();
    void AddSample(uint32_t timeMs, float voltage, float current);
    void AddSample(const BufferSample& sample);

    BufferStatistics GetStatistics(BufferQuantity quantity) const;

    bool IsEmpty() const;
    uint16_t GetCount() const;
    uint16_t GetCapacity() const;
    // Stable reading indices and elapsed-time origin, retained across ring overwrite.
    uint64_t GetTotalSamples() const { return _totalSamples; }
    uint32_t GetStartTimeMs() const { return _startTimeMs; }

    const BufferSample* GetLatest() const;
    uint16_t CopyLatest(BufferSample* dest, uint16_t maxCount) const;

    float GetVoltage() const;
    float GetCurrent() const;
    float GetPower() const;
    float GetResistance() const;
    bool HasValidResistance() const;
    float GetVoltageMin() const;
    float GetVoltageMax() const;
    float GetVoltageAverage() const;
    float GetVoltagePeakToPeak() const;
    float GetCurrentMin() const;
    float GetCurrentMax() const;
    float GetCurrentAverage() const;
    float GetCurrentPeakToPeak() const;

private:
    BufferSample _samples[Capacity];
    uint16_t _head;
    uint16_t _count;
    uint64_t _totalSamples=0;
    uint32_t _startTimeMs=0;
};

#endif
