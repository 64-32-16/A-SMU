
#include "LTC1655.h"


LTC1655Class::LTC1655Class( SPIClass *spi) 
{
    SPI1 = spi;
} 


void LTC1655Class::Begin(int cs_pin) 
{
    CSPin = cs_pin;
    pinMode( CSPin, OUTPUT );
    digitalWrite(CSPin, HIGH);
} 



void LTC1655Class::SetCode(uint16_t code) 
{

    //SPI1->beginTransaction(SPISettings( 1000000, )
    digitalWrite( CSPin, LOW);
    SPI1->transfer16( code);    

    digitalWrite( CSPin, HIGH);


} 

