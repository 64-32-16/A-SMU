#include "Ltc2602.h"

#include "hardware/gpio.h"

namespace
{
    constexpr uint16_t DacMidscaleCode = 0x8000;

    spi_inst_t* GetDefaultSpi()
    {
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#endif
        return spi1;
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
    }

    float ClampFloat(float value, float minValue, float maxValue)
    {
        if (value < minValue)
        {
            return minValue;
        }

        if (value > maxValue)
        {
            return maxValue;
        }

        return value;
    }
}

Ltc2602::Ltc2602(uint8_t csPin, uint8_t sckPin, uint8_t mosiPin, uint8_t misoPin, uint32_t spiClockHz)
    : Ltc2602(GetDefaultSpi(), csPin, sckPin, mosiPin, misoPin, spiClockHz)
{
}

Ltc2602::Ltc2602(spi_inst_t* spi, uint8_t csPin, uint8_t sckPin, uint8_t mosiPin, uint8_t misoPin, uint32_t spiClockHz)
    : _spi(spi),
      _csPin(csPin),
      _sckPin(sckPin),
      _mosiPin(mosiPin),
      _misoPin(misoPin),
      _spiClockHz(spiClockHz),
      _begun(false)
{
}

void Ltc2602::Begin()
{
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);

    gpio_set_function(_sckPin, GPIO_FUNC_SPI);
    gpio_set_function(_mosiPin, GPIO_FUNC_SPI);
    gpio_set_function(_misoPin, GPIO_FUNC_SPI);

#if defined(ARDUINO_RASPBERRY_PI_PICO)
    _spi_init(_spi, _spiClockHz);
#else
    spi_init(_spi, _spiClockHz);
#endif

    spi_set_format(_spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    _begun = true;

    Serial.println("LTC2602 begin");
}

void Ltc2602::WriteInput(Ltc2602Channel channel, uint16_t code)
{
    Transfer(Command::WriteInput, channel, code);
}

void Ltc2602::Update(Ltc2602Channel channel)
{
    Transfer(Command::Update, channel, 0);
}

void Ltc2602::WriteAndUpdate(Ltc2602Channel channel, uint16_t code)
{
    Transfer(Command::WriteAndUpdate, channel, code);
}

void Ltc2602::WriteAndUpdateAll(Ltc2602Channel channel, uint16_t code)
{
    Transfer(Command::WriteInputUpdateAll, channel, code);
}

void Ltc2602::PowerDown(Ltc2602Channel channel)
{
    Transfer(Command::PowerDown, channel, 0);
}

void Ltc2602::Nop()
{
    Transfer(Command::NoOperation, Ltc2602Channel::All, 0);
}

void Ltc2602::SetVSet(float volts, float referenceVoltage)
{
    const uint16_t code = CodeFromBipolarLevelShiftedVoltage(volts, referenceVoltage);

    Serial.print("LTC2602 V_SET code=");
    Serial.print(code);
    Serial.print(" ctrl=");
    Serial.print(volts, 6);
    Serial.println(" V");

    WriteAndUpdate(Ltc2602Channel::A, code);
}

void Ltc2602::SetVSet(float volts)
{
    SetVSet(volts, DefaultReferenceVoltage);
}

void Ltc2602::SetILimit(float volts, float referenceVoltage)
{
    const uint16_t code = CodeFromBipolarLevelShiftedVoltage(volts, referenceVoltage);

    Serial.print("LTC2602 I_LIMIT code=");
    Serial.print(code);
    Serial.print(" ctrl=");
    Serial.print(volts, 6);
    Serial.println(" V");

    WriteAndUpdate(Ltc2602Channel::B, code);
}

void Ltc2602::SetILimit(float volts)
{
    SetILimit(volts, DefaultReferenceVoltage);
}

void Ltc2602::ZeroOutputs(float referenceVoltage)
{
    (void)referenceVoltage;

    WriteInput(Ltc2602Channel::A, DacMidscaleCode);
    WriteAndUpdateAll(Ltc2602Channel::B, DacMidscaleCode);
}

void Ltc2602::ZeroOutputs()
{
    ZeroOutputs(DefaultReferenceVoltage);
}

uint16_t Ltc2602::CodeFromUnipolarVoltage(float volts, float referenceVoltage)
{
    if (referenceVoltage <= 0.0f)
    {
        return 0;
    }

    const float clamped = ClampFloat(volts, 0.0f, referenceVoltage);
    const float scaled = (clamped / referenceVoltage) * 65535.0f;
    return static_cast<uint16_t>(scaled + 0.5f);
}

uint16_t Ltc2602::CodeFromBipolarLevelShiftedVoltage(float volts, float referenceVoltage)
{
    if (referenceVoltage <= 0.0f)
    {
        return DacMidscaleCode;
    }

    const float clamped = ClampFloat(volts, -referenceVoltage, referenceVoltage);
    const float scaled = ((clamped + referenceVoltage) / (2.0f * referenceVoltage)) * 65535.0f;
    return static_cast<uint16_t>(scaled + 0.5f);
}

void Ltc2602::Transfer(Command command, Ltc2602Channel channel, uint16_t code)
{
    if (!_begun)
    {
        Begin();
    }

    const uint8_t commandAddress = (static_cast<uint8_t>(command) << 4) | (static_cast<uint8_t>(channel) & 0x0F);
    const uint8_t buffer[3] = {
        commandAddress,
        static_cast<uint8_t>(code >> 8),
        static_cast<uint8_t>(code & 0xFF)
    };

    digitalWrite(_csPin, LOW);
    spi_write_blocking(_spi, buffer, sizeof(buffer));
    digitalWrite(_csPin, HIGH);
}
