

#include <GD2.h>
#include "Beeper.h"
#include "Display.h"
#include "System.h"

#include "Arduino.h"

/*
  ESP32 IP
   192.168.178.87:5553



W I C H T I G   B I T T E   B E A C H T E N


SPI1 problem mit Großschreibung !!!

NEUE LIBRARY ADS1220

https://github.com/wollewald/ADS1220_WE


*/

/*
Pin-Belegung ESP32

* INPUT
  OUTPUT-ON-OFF Button
  SYSTEM-ON-OFF Button

*DISPLAY
  SCLK = GPIO18 (18)
  MISO = GPIO19 (19)
  MOSI = GPIO23 (23)
  CS   = GPIO25 (25)

*ADS1220
  SCLK = GPIO14 (14)
  MISO = GPIO12 (12)
  MOSI = GPIO13 (13)
  CS   = GPIO05 (5)
  DARY = GPIO27 (27) => SWITCH TO 35

*DAC856X
  SCLK = GPIO14 (14)
  MOSI = GPIO13 (13)
  CS   = GPIO17 (17)

*FAN PWM =


*Beeper 
  CS = GPIO 16


*OUTPUT 74HC595

  -Bepper
  -OUTPUT-ON-OFF Relais
  -SYSTEM-ON-OFF Relais
  -U-Range 5V
  -U-Range 1V
  -I-Range 200mA
  -I-Range 2mA
  -U/I-Umschalter
  -Remote Sense





*/

uint32_t micros_end = 0;
uint32_t current_millis = 0;

#define PIN_SHIFT 13 // connected to CLOCK
#define PIN_STORE 12 // connected to STROBE
#define PIN_DATA 14  // connected to DS (14)
#define PIN_BEEPER 16  //


// Die Reihenfolge des Erzeugung der Komponenten ist wichtig.
// Das Display greift auf Funktionen vom System zurück.

BeeperClass Beeper;

SystemClass System;
DisplayClass Display;


void setup()
{

  Serial.begin(9600);

  Serial.println("A-SMU setup");
  btStop();
  setCpuFrequencyMhz(80);
  Beeper.Begin(PIN_BEEPER);

  Serial.println("EVE2 setup");

  GD.begin(0);
  GD.wr(REG_PWM_DUTY, 50);


  System.IsSimulation = true;

  System.Begin();

  Beeper.KeyPress();

}

void loop()
{

  {

    current_millis = millis();

    // Update System
    System.Execute();

    

    // Update display
    if ((current_millis - micros_end) > 20)
    {
      micros_end = millis();

      

      GD.cmd_dlstart();

      
      GD.cmd_romfont(1, 34);
      GD.cmd_romfont(2, 33);
      GD.ClearColorRGB(0x000000);
      GD.Clear();
      GD.VertexFormat(0);

      Display.ReadInput();
      Display.Render();



      GD.swap();
    }


 

  }
}
