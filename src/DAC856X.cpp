
#include "DAC856X.h"





DAC856XClass::DAC856XClass( SPIClass *spi) 
{
    SPI1 = spi;
} 


void DAC856XClass::Begin(int cs_pin) 
{
    CSPin = cs_pin;
    pinMode( CSPin, OUTPUT );
    digitalWrite(CSPin, HIGH);

    Initialize();

    Serial.println( "DAC::Begin");
} 




 

void DAC856XClass::WriteValue(uint8_t cmd_byte, uint16_t code) {
  

  
  digitalWrite(CSPin, LOW);
  SPI1->transfer(cmd_byte);
  SPI1->transfer16(code);
  


  digitalWrite(CSPin, HIGH);
};


void DAC856XClass::DAC_WR_REG(uint8_t cmd_byte, uint16_t data_byte) {
  digitalWrite(CSPin, LOW);
  SPI1->transfer(cmd_byte);
  SPI1->transfer16(data_byte);
  digitalWrite(CSPin, HIGH);
};


void DAC856XClass::Initialize() {
  DAC_WR_REG(CMD_RESET_ALL_REG, DATA_RESET_ALL_REG);        // reset
  DAC_WR_REG(CMD_PWR_UP_A_B, DATA_PWR_UP_A_B);              // power up
  DAC_WR_REG(CMD_INTERNAL_REF_EN, DATA_INTERNAL_REF_EN);    // enable internal reference 2V5
  DAC_WR_REG(CMD_GAIN, DATA_GAIN_B1_A1);                    // Channel A and B Gain 1
  DAC_WR_REG(CMD_LDAC_DIS, DATA_LDAC_DIS);                  // update the caches
};
