#include "Buffer.h"

#include <math.h>

namespace
{
constexpr float kMinResistanceCurrent = 1e-9f;
}

BufferClass::BufferClass()
    : _head(0),
      _count(0)
{
}

void BufferClass::Reset()
{
    _head = 0;
    _count = 0;
}

void BufferClass::AddSample(uint32_t timeMs, float voltage, float current)
{
    BufferSample& sample = _samples[_head];
    sample.Voltage = voltage;
    sample.Current = current;
    sample.Power = voltage * current;
    sample.TimeMs = timeMs;
    sample.ResistanceValid = (fabsf(current) >= kMinResistanceCurrent);
    sample.Resistance = sample.ResistanceValid ? (voltage / current) : 0.0f;

    _head = static_cast<uint16_t>((_head + 1) % Capacity);
    if (_count < Capacity)
    {
        ++_count;
    }
}

bool BufferClass::IsEmpty() const
{
    return _count == 0;
}

uint16_t BufferClass::GetCount() const
{
    return _count;
}

uint16_t BufferClass::GetCapacity() const
{
    return Capacity;
}

const BufferSample* BufferClass::GetLatest() const
{
    if (IsEmpty())
    {
        return nullptr;
    }

    const uint16_t latestIndex = (_head == 0) ? (Capacity - 1) : (_head - 1);
    return &_samples[latestIndex];
}

uint16_t BufferClass::CopyLatest(BufferSample* dest, uint16_t maxCount) const
{
    if (dest == nullptr || maxCount == 0 || IsEmpty())
    {
        return 0;
    }

    const uint16_t copyCount = (_count < maxCount) ? _count : maxCount;
    const uint16_t start = (_count < maxCount) ? 0 : (_count - maxCount);

    for (uint16_t i = 0; i < copyCount; ++i)
    {
        const uint16_t logicalIndex = static_cast<uint16_t>(start + i);
        const uint16_t physicalIndex = static_cast<uint16_t>((_head + Capacity - _count + logicalIndex) % Capacity);
        dest[i] = _samples[physicalIndex];
    }

    return copyCount;
}

float BufferClass::GetVoltage() const
{
    const BufferSample* sample = GetLatest();
    return (sample != nullptr) ? sample->Voltage : 0.0f;
}

float BufferClass::GetCurrent() const
{
    const BufferSample* sample = GetLatest();
    return (sample != nullptr) ? sample->Current : 0.0f;
}

float BufferClass::GetPower() const
{
    const BufferSample* sample = GetLatest();
    return (sample != nullptr) ? sample->Power : 0.0f;
}

float BufferClass::GetResistance() const
{
    const BufferSample* sample = GetLatest();
    if (sample == nullptr || !sample->ResistanceValid)
    {
        return 0.0f;
    }

    return sample->Resistance;
}

bool BufferClass::HasValidResistance() const
{
    const BufferSample* sample = GetLatest();
    return (sample != nullptr) && sample->ResistanceValid;
}

float BufferClass::GetVoltageMin() const
{
    if (IsEmpty())
    {
        return 0.0f;
    }

    float value = _samples[0].Voltage;
    for (uint16_t i = 1; i < _count; ++i)
    {
        if (_samples[i].Voltage < value)
        {
            value = _samples[i].Voltage;
        }
    }
    return value;
}

float BufferClass::GetVoltageMax() const
{
    if (IsEmpty())
    {
        return 0.0f;
    }

    float value = _samples[0].Voltage;
    for (uint16_t i = 1; i < _count; ++i)
    {
        if (_samples[i].Voltage > value)
        {
            value = _samples[i].Voltage;
        }
    }
    return value;
}

float BufferClass::GetVoltageAverage() const
{
    if (IsEmpty())
    {
        return 0.0f;
    }

    float sum = 0.0f;
    for (uint16_t i = 0; i < _count; ++i)
    {
        sum += _samples[i].Voltage;
    }
    return sum / static_cast<float>(_count);
}

float BufferClass::GetVoltagePeakToPeak() const
{
    return GetVoltageMax() - GetVoltageMin();
}

float BufferClass::GetCurrentMin() const
{
    if (IsEmpty())
    {
        return 0.0f;
    }

    float value = _samples[0].Current;
    for (uint16_t i = 1; i < _count; ++i)
    {
        if (_samples[i].Current < value)
        {
            value = _samples[i].Current;
        }
    }
    return value;
}

float BufferClass::GetCurrentMax() const
{
    if (IsEmpty())
    {
        return 0.0f;
    }

    float value = _samples[0].Current;
    for (uint16_t i = 1; i < _count; ++i)
    {
        if (_samples[i].Current > value)
        {
            value = _samples[i].Current;
        }
    }
    return value;
}

float BufferClass::GetCurrentAverage() const
{
    if (IsEmpty())
    {
        return 0.0f;
    }

    float sum = 0.0f;
    for (uint16_t i = 0; i < _count; ++i)
    {
        sum += _samples[i].Current;
    }
    return sum / static_cast<float>(_count);
}

float BufferClass::GetCurrentPeakToPeak() const
{
    return GetCurrentMax() - GetCurrentMin();
}
