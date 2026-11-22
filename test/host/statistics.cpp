#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "System.h"

static void Near(float actual, float expected)
{
    assert(fabsf(actual - expected) <= 1e-5f * (1.0f + fabsf(expected)));
}

int main()
{
    static_assert(sizeof(BufferSample)==16,"Compact samples must remain 16 bytes");
    auto derived=Hardware::MakeMeasurement(123,6,2);
    Near(derived.GetPower(),12);Near(derived.GetResistance(),3);
    derived.Current=-3;Near(derived.GetPower(),-18);Near(derived.GetResistance(),-2);
    derived.Current=0;assert(!derived.HasValidResistance());Near(derived.GetPower(),0);
    derived.Current=1e-10f;assert(!derived.HasValidResistance());
    derived.Current=1;derived.Voltage=INFINITY;assert(!derived.HasValidResistance());
    SystemClass system;
    system.Reset();
    assert(system.GetMeasurementStatistics().Count == 0);
    system.Buffer.AddSample(0, 2, 1);
    system.Buffer.AddSample(1, 4, 2);
    system.Buffer.AddSample(2, 6, 0);
    system.SetMeasureMode(MeasureMode::Voltage);
    auto stats = system.GetMeasurementStatistics();
    assert(stats.Count == 3);
    Near(stats.Minimum, 2); Near(stats.Maximum, 6); Near(stats.Average, 4);
    Near(stats.PeakToPeak, 4); Near(stats.StandardDeviation, sqrtf(8.0f / 3));
    system.SetMeasureMode(MeasureMode::Current);
    stats = system.GetMeasurementStatistics();
    Near(stats.Average, 1); Near(stats.StandardDeviation, sqrtf(2.0f / 3));
    system.SetMeasureMode(MeasureMode::Power);
    stats = system.GetMeasurementStatistics();
    Near(stats.Minimum, 0); Near(stats.Maximum, 8);
    Near(stats.Average, 10.0f / 3); Near(stats.StandardDeviation, sqrtf(104.0f / 9));
    system.SetMeasureMode(MeasureMode::Resistance);
    stats = system.GetMeasurementStatistics();
    assert(stats.Count == 2);
    Near(stats.Average, 2); Near(stats.PeakToPeak, 0); Near(stats.StandardDeviation, 0);
    system.ClearMeasurementBuffer();
    assert(system.Buffer.IsEmpty());
    assert(system.GetMeasurementStatistics().Count == 0);
    system.Buffer.AddSample(0, 3, 0);
    assert(system.GetMeasurementStatistics().Count == 0);
    system.Buffer.AddSample(1, -6, 2);
    stats = system.GetMeasurementStatistics();
    assert(stats.Count == 1); Near(stats.Average, -3); Near(stats.StandardDeviation, 0);
    system.Buffer.AddSample(2, NAN, 1);
    system.Buffer.AddSample(3, INFINITY, 1);
    assert(system.GetMeasurementStatistics().Count == 1);
    system.ClearMeasurementBuffer();
    system.SetMeasureMode(MeasureMode::Voltage);
    for (unsigned i = 0; i < BufferClass::Capacity + 10; ++i)
        system.Buffer.AddSample(i, static_cast<float>(i), 1);
    stats = system.GetMeasurementStatistics();
    assert(stats.Count == BufferClass::Capacity);
    Near(stats.Minimum, 10); Near(stats.Maximum, BufferClass::Capacity + 9);
    Near(stats.Average, (BufferClass::Capacity + 19) / 2.0f);
    Near(stats.StandardDeviation, sqrtf((BufferClass::Capacity * BufferClass::Capacity - 1.0f) / 12));
    puts("PASS: V/I/P/R, population deviation, invalid resistance, finite values, reset, ring overwrite");
}
