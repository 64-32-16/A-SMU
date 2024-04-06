#ifndef BEEPERCLASS_h
#define BEEPERCLASS_h

#include <Arduino.h>



class BeeperClass
{
    public:
        
        BeeperClass();

        void Begin(int cs_pin);

        void KeyPress();
        void Error();
        
    protected:
        int CSPin;

};

extern BeeperClass Beeper;

#endif