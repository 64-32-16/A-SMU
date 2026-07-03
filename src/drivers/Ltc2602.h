#ifndef _LTC2602_H_INCLUDED
#define _LTC2602_H_INCLUDED

#include <Arduino.h>
#include "hardware/spi.h"
#include "../board/PrototypeV1Pins.h"
#include "../Core/DacOutput.h"

enum class Ltc2602Channel : uint8_t
{
    A = 0x0,
    B = 0x1,
    All = 0xF
};

class Ltc2602 : public DacOutput
{
public:
    static constexpr float DefaultReferenceVoltage = 2.5f;
    static constexpr uint32_t DefaultSpiClockHz = 500000UL;

    explicit Ltc2602(
        uint8_t csPin = PrototypeV1Pins::DacCs,
        uint8_t sckPin = PrototypeV1Pins::SpiClk,
        uint8_t mosiPin = PrototypeV1Pins::SpiMosi,
        uint8_t misoPin = PrototypeV1Pins::SpiMiso,
        uint32_t spiClockHz = DefaultSpiClockHz);

    explicit Ltc2602(
        spi_inst_t* spi,
        uint8_t csPin = PrototypeV1Pins::DacCs,
        uint8_t sckPin = PrototypeV1Pins::SpiClk,
        uint8_t mosiPin = PrototypeV1Pins::SpiMosi,
        uint8_t misoPin = PrototypeV1Pins::SpiMiso,
        uint32_t spiClockHz = DefaultSpiClockHz);

    virtual void Begin() override;

    void WriteInput(Ltc2602Channel channel, uint16_t code);
    void Update(Ltc2602Channel channel);
    void WriteAndUpdate(Ltc2602Channel channel, uint16_t code);
    void WriteAndUpdateAll(Ltc2602Channel channel, uint16_t code);
    void PowerDown(Ltc2602Channel channel);
    void Nop();

    virtual void SetVSet(float volts) override;
    virtual void SetILimit(float volts) override;
    virtual void ZeroOutputs() override;

    void SetVSet(float volts, float referenceVoltage);
    void SetILimit(float volts, float referenceVoltage);
    void ZeroOutputs(float referenceVoltage);

    static uint16_t CodeFromUnipolarVoltage(float volts, float referenceVoltage = DefaultReferenceVoltage);
    static uint16_t CodeFromBipolarLevelShiftedVoltage(float volts, float referenceVoltage = DefaultReferenceVoltage);

private:
    enum class Command : uint8_t
    {
        WriteInput = 0x0,
        Update = 0x1,
        WriteInputUpdateAll = 0x2,
        WriteAndUpdate = 0x3,
        PowerDown = 0x4,
        NoOperation = 0xF
    };

    void Transfer(Command command, Ltc2602Channel channel, uint16_t code);

    spi_inst_t* _spi;
    uint8_t _csPin;
    uint8_t _sckPin;
    uint8_t _mosiPin;
    uint8_t _misoPin;
    uint32_t _spiClockHz;
    bool _begun;
};

#endif
