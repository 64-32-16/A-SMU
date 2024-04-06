
#include "Beeper.h"


BeeperClass::BeeperClass() 
{

} 


void BeeperClass::Begin(int cs_pin) 
{
    CSPin = cs_pin;
    pinMode( CSPin, OUTPUT );
    digitalWrite(CSPin, LOW);
} 



void BeeperClass::KeyPress() 
{

   
    digitalWrite( CSPin, HIGH);
    delay(5);
    digitalWrite( CSPin, LOW);

} 

void BeeperClass::Error() 
{
    KeyPress();
    delay(5);
    KeyPress();


} 
