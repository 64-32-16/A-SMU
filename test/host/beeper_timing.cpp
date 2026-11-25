#include <assert.h>
#include <stdio.h>
#include "Core/Beeper.h"
static int pins[256]={}, writes=0;
static bool clicksAllowed=true;
static bool AllowClick() {return clicksAllowed;}
void pinMode(uint8_t,int) {}
void digitalWrite(uint8_t pin,int value) {pins[pin]=value;++writes;}
int main() {
    Beeper.Click();assert(writes==0 && !Beeper.IsActive());
    Beeper.Begin(9);assert(pins[9]==LOW);
    Beeper.Click();assert(Beeper.IsActive() && pins[9]==HIGH);
    mbed::Timeout::Advance(19);assert(Beeper.IsActive());
    mbed::Timeout::Advance(1);assert(!Beeper.IsActive() && pins[9]==LOW);
    Beeper.Error();mbed::Timeout::Advance(79);assert(Beeper.IsActive());
    mbed::Timeout::Advance(1);assert(!Beeper.IsActive());
    Beeper.Click();mbed::Timeout::Advance(10);Beeper.Error();
    mbed::Timeout::Advance(10);assert(Beeper.IsActive());
    mbed::Timeout::Advance(69);assert(Beeper.IsActive());
    mbed::Timeout::Advance(1);assert(!Beeper.IsActive());
    Beeper.Error();Beeper.Stop();Beeper.Click();
    mbed::Timeout::Advance(20);assert(!Beeper.IsActive());
    Beeper.Error();Beeper.Beep(0);assert(!Beeper.IsActive() && pins[9]==LOW);
    Beeper.Error();Beeper.Begin(10,false);assert(pins[9]==LOW && pins[10]==HIGH);
    Beeper.Click();assert(pins[10]==LOW);mbed::Timeout::Advance(20);assert(pins[10]==HIGH);
    Beeper.Beep(65535);mbed::Timeout::Advance(65534);assert(Beeper.IsActive());
    mbed::Timeout::Advance(1);assert(!Beeper.IsActive() && pins[10]==HIGH);
    Beeper.SetClickAllowed(AllowClick);clicksAllowed=false;
    Beeper.Click();assert(!Beeper.IsActive());
    Beeper.Error();assert(Beeper.IsActive());mbed::Timeout::Advance(80);
    clicksAllowed=true;Beeper.Click();assert(Beeper.IsActive());mbed::Timeout::Advance(20);
    puts("PASS: timer-only shutdown, 20/80 ms, retrigger, cancellation, zero, reinit, active-low, maximum duration");
}
