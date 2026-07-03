#ifndef _DISPLAY_FORMAT_H_INCLUDED
#define _DISPLAY_FORMAT_H_INCLUDED

#include <Arduino.h>

enum class DisplayQuantity : uint8_t
{
    Voltage,
    Current,
    Resistance,
    Power
};

class DisplayFormatState
{
public:
    DisplayFormatState();
    void Reset();

private:
    friend class DisplayFormat;
    int8_t _unitIndex;
};

class DisplayFormat
{
public:
    static void FormatValue(DisplayQuantity quantity, float value,
                            DisplayFormatState& state,
                            bool showSign,
                            char* valueBuffer, size_t valueBufferSize,
                            char* unitBuffer, size_t unitBufferSize);
    static uint8_t GetDecimalsFor(DisplayQuantity quantity, int8_t unitIndex);

private:
    static int8_t ResolveUnitIndex(DisplayQuantity quantity, float absValue, int8_t currentIndex);
    static float ScaleValue(DisplayQuantity quantity, float value, int8_t unitIndex);
    static const char* GetUnitText(DisplayQuantity quantity, int8_t unitIndex);
};

#endif
