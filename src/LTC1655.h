#ifndef LTC1655CLASS_h
#define LTC1655CLASS_h

#include <Arduino.h>
#include "SPI.h"



class LTC1655Class
{
    public:
        
        LTC1655Class( SPIClass *spi);

        void Begin(int cs_pin);

        virtual void SetCode( uint16_t code);


    protected:
        int CSPin;
        SPIClass *SPI1;

};


#endif