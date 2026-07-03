#ifndef _BUFFER_H_INCLUDED
#define _BUFFER_H_INCLUDED

#include <Arduino.h>

struct BufferSample
{
    float Voltage;
    float Current;
    float Power;
    float Resistance;
    uint32_t TimeMs;
    bool ResistanceValid;
};

class BufferClass
{
public:
    static const uint16_t Capacity = 1024;

    BufferClass();

    void Reset();
    void AddSample(uint32_t timeMs, float voltage, float current);

    bool IsEmpty() const;
    uint16_t GetCount() const;
    uint16_t GetCapacity() const;

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
};

#endif
