#include "DisplayFormat.h"

#include <math.h>
#include <stdio.h>

namespace
{
constexpr float kUpThreshold = 1.05f;
constexpr float kDownThreshold = 0.95f;

constexpr uint8_t kVoltageCurrentMicroDecimals = 1;
constexpr uint8_t kVoltageCurrentMilliDecimals = 2;
constexpr uint8_t kVoltageCurrentBaseDecimals = 3;

constexpr uint8_t kResistanceMilliDecimals = 1;
constexpr uint8_t kResistanceOhmDecimals = 1;
constexpr uint8_t kResistanceKiloDecimals = 3;
constexpr uint8_t kResistanceMegaDecimals = 3;

constexpr uint8_t kPowerMicroDecimals = 1;
constexpr uint8_t kPowerMilliDecimals = 2;
constexpr uint8_t kPowerBaseDecimals = 3;
}

DisplayFormatState::DisplayFormatState()
    : _unitIndex(0)
{
}

void DisplayFormatState::Reset()
{
    _unitIndex = 0;
}

void DisplayFormat::FormatValue(DisplayQuantity quantity, float value,
                                DisplayFormatState& state,
                                bool showSign,
                                char* valueBuffer, size_t valueBufferSize,
                                char* unitBuffer, size_t unitBufferSize)
{
    const float absValue = fabsf(value);
    state._unitIndex = ResolveUnitIndex(quantity, absValue, state._unitIndex);

    const float scaledValue = ScaleValue(quantity, value, state._unitIndex);
    const uint8_t decimals = GetDecimalsFor(quantity, state._unitIndex);

    snprintf(valueBuffer, valueBufferSize, showSign ? "%+.*f" : "%.*f", decimals, scaledValue);
    snprintf(unitBuffer, unitBufferSize, "%s", GetUnitText(quantity, state._unitIndex));
}

int8_t DisplayFormat::ResolveUnitIndex(DisplayQuantity quantity, float absValue, int8_t currentIndex)
{
    switch (quantity)
    {
        case DisplayQuantity::Voltage:
            if (currentIndex == 2)
            {
                return (absValue <= 0.95f) ? 1 : 2;
            }
            if (currentIndex == 1)
            {
                if (absValue >= 1.05f) return 2;
                if (absValue <= 0.00095f) return 0;
                return 1;
            }
            if (absValue >= 1.05f) return 2;
            if (absValue >= 0.00105f) return 1;
            return 0;

        case DisplayQuantity::Current:
            if (currentIndex == 2)
            {
                return (absValue <= 0.95f) ? 1 : 2;
            }
            if (currentIndex == 1)
            {
                if (absValue >= 1.05f) return 2;
                if (absValue <= 0.00095f) return 0;
                return 1;
            }
            if (absValue >= 1.05f) return 2;
            if (absValue >= 0.00105f) return 1;
            return 0;

        case DisplayQuantity::Resistance:
            if (currentIndex == 3)
            {
                return (absValue <= 950000.0f) ? 2 : 3;
            }
            if (currentIndex == 2)
            {
                if (absValue >= 1050000.0f) return 3;
                if (absValue <= 950.0f) return 1;
                return 2;
            }
            if (currentIndex == 1)
            {
                if (absValue >= 1050.0f) return 2;
                if (absValue <= 0.95f) return 0;
                return 1;
            }
            if (absValue >= 1050000.0f) return 3;
            if (absValue >= 1050.0f) return 2;
            if (absValue >= 1.05f) return 1;
            return 0;

        case DisplayQuantity::Power:
        default:
            if (currentIndex == 2)
            {
                return (absValue <= 0.95f) ? 1 : 2;
            }
            if (currentIndex == 1)
            {
                if (absValue >= 1.05f) return 2;
                if (absValue <= 0.00095f) return 0;
                return 1;
            }
            if (absValue >= 1.05f) return 2;
            if (absValue >= 0.00105f) return 1;
            return 0;
    }
}

float DisplayFormat::ScaleValue(DisplayQuantity quantity, float value, int8_t unitIndex)
{
    switch (quantity)
    {
        case DisplayQuantity::Voltage:
            return (unitIndex == 0) ? value * 1000000.0f :
                   (unitIndex == 1) ? value * 1000.0f : value;

        case DisplayQuantity::Current:
            return (unitIndex == 0) ? value * 1000000.0f :
                   (unitIndex == 1) ? value * 1000.0f : value;

        case DisplayQuantity::Resistance:
            return (unitIndex == 0) ? value * 1000.0f :
                   (unitIndex == 1) ? value :
                   (unitIndex == 2) ? value / 1000.0f :
                                      value / 1000000.0f;

        case DisplayQuantity::Power:
        default:
            return (unitIndex == 0) ? value * 1000000.0f :
                   (unitIndex == 1) ? value * 1000.0f : value;
    }
}

const char* DisplayFormat::GetUnitText(DisplayQuantity quantity, int8_t unitIndex)
{
    switch (quantity)
    {
        case DisplayQuantity::Voltage:
            return (unitIndex == 0) ? "uV" : (unitIndex == 1) ? "mV" : "V";
        case DisplayQuantity::Current:
            return (unitIndex == 0) ? "uA" : (unitIndex == 1) ? "mA" : "A";
        case DisplayQuantity::Resistance:
            return (unitIndex == 0) ? "mOhm" : (unitIndex == 1) ? "Ohm" : (unitIndex == 2) ? "kOhm" : "MOhm";
        case DisplayQuantity::Power:
        default:
            return (unitIndex == 0) ? "uW" : (unitIndex == 1) ? "mW" : "W";
    }
}

uint8_t DisplayFormat::GetDecimalsFor(DisplayQuantity quantity, int8_t unitIndex)
{
    switch (quantity)
    {
        case DisplayQuantity::Voltage:
        case DisplayQuantity::Current:
            return (unitIndex == 2) ? kVoltageCurrentBaseDecimals :
                   (unitIndex == 1) ? kVoltageCurrentMilliDecimals :
                                      kVoltageCurrentMicroDecimals;
        case DisplayQuantity::Resistance:
            return (unitIndex == 3) ? kResistanceMegaDecimals :
                   (unitIndex == 2) ? kResistanceKiloDecimals :
                   (unitIndex == 1) ? kResistanceOhmDecimals :
                                      kResistanceMilliDecimals;
        case DisplayQuantity::Power:
        default:
            return (unitIndex == 2) ? kPowerBaseDecimals :
                   (unitIndex == 1) ? kPowerMilliDecimals :
                                      kPowerMicroDecimals;
    }
}
