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
    _totalSamples=0;_startTimeMs=0;
    _head = 0;
    _count = 0;
}

void BufferClass::AddSample(uint32_t timeMs, float voltage, float current)
{
    AddSample(Hardware::MakeMeasurement(timeMs,voltage,current));
}

void BufferClass::AddSample(const BufferSample& sample)
{
    if(_totalSamples==0) _startTimeMs=sample.TimeMs;
    ++_totalSamples;
    _samples[_head]=sample;

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
    return (sample != nullptr) ? sample->GetPower() : 0.0f;
}

float BufferClass::GetResistance() const
{
    const BufferSample* sample = GetLatest();
    if (sample == nullptr || !sample->HasValidResistance())
    {
        return 0.0f;
    }

    return sample->GetResistance();
}

bool BufferClass::HasValidResistance() const
{
    const BufferSample* sample = GetLatest();
    return (sample != nullptr) && sample->HasValidResistance();
}

float BufferClass::GetVoltageMin() const
{
    return GetStatistics(BufferQuantity::Voltage).Minimum;
}

float BufferClass::GetVoltageMax() const
{
    return GetStatistics(BufferQuantity::Voltage).Maximum;
}

float BufferClass::GetVoltageAverage() const
{
    return GetStatistics(BufferQuantity::Voltage).Average;
}

float BufferClass::GetVoltagePeakToPeak() const
{
    return GetStatistics(BufferQuantity::Voltage).PeakToPeak;
}

float BufferClass::GetCurrentMin() const
{
    return GetStatistics(BufferQuantity::Current).Minimum;
}

float BufferClass::GetCurrentMax() const
{
    return GetStatistics(BufferQuantity::Current).Maximum;
}

float BufferClass::GetCurrentAverage() const
{
    return GetStatistics(BufferQuantity::Current).Average;
}

float BufferClass::GetCurrentPeakToPeak() const
{
    return GetStatistics(BufferQuantity::Current).PeakToPeak;
}

BufferStatistics BufferClass::GetStatistics(BufferQuantity quantity) const
{
    BufferStatistics result;
    double mean = 0;
    double m2 = 0;
    for (uint16_t i = 0; i < _count; ++i)
    {
        const BufferSample& sample = _samples[i];
        if(sample.Fault!=Hardware::Error::None) continue;
        float value = 0;
        switch (quantity)
        {
            case BufferQuantity::Voltage: value = sample.Voltage; break;
            case BufferQuantity::Current: value = sample.Current; break;
            case BufferQuantity::Power: value = sample.GetPower(); break;
            case BufferQuantity::Resistance:
                if (!sample.HasValidResistance()) continue;
                value = sample.GetResistance();
                break;
        }
        if (!isfinite(value)) continue;
        if (result.Count == 0) result.Minimum = result.Maximum = value;
        if (value < result.Minimum) result.Minimum = value;
        if (value > result.Maximum) result.Maximum = value;
        ++result.Count;
        const double delta = value - mean;
        mean += delta / result.Count;
        m2 += delta * (value - mean);
    }
    if (result.Count > 0)
    {
        result.Average = static_cast<float>(mean);
        result.PeakToPeak = result.Maximum - result.Minimum;
        result.StandardDeviation = static_cast<float>(sqrt(m2 > 0 ? m2 / result.Count : 0));
    }
    return result;
}
