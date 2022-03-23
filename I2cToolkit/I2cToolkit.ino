// I2cToolkit.ino -
#define MAIN
#include <Wire.h>

#ifdef ARDUINO_ARCH_STM32
//-----------------------------------------------------------------------------
// printf for STM32:

   //https://forum.arduino.cc/index.php?topic=566042.0
   //HardwareSerial Serial(PA10, PA9);   // RX, TX
   HardwareSerial Serial(PA3, PA2);   // RX, TX

   extern "C" int _write(int __attribute__ ((unused)) file, char *buffer, int size)
      { return Serial.write(buffer, size); }

   // Use alternative I2C pins
   TwoWire TK_WIRE(PB11, PB10);   // sda, scl
//---
#endif

//-----------------------------------------------------------------------------
// printf for microbit:
//-extern "C" int _write(int __attribute__ ((unused)) file, char *buffer, int size)
//-   { return Serial.write(buffer, size); }
//-#define TK_WIRE Wire

//-----------------------------------------------------------------------------
// printf for AVR:
#ifdef __AVR__
   int my_putc(char c, FILE __attribute__ ((unused)) *t) {
     if (c == '\n') Serial.write('\r');
     return Serial.write(c);
   }
   #define TK_WIRE Wire
#endif

#define MyPrintf printf

//-----------------------------------------------------------------------------
// Toolkit & commands setup.
#include "Commands.h"
void TkExecute(int Param[]);
TCommand Command(TkExecute);
#define I2CMTK_INTERACTIVE
#include "I2CmTk.h"

//-----------------------------------------------------------------------------
// setup -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);      // start serial
  Serial.print("-------------------------------------\n");

#ifdef __AVR__
  fdevopen( &my_putc, 0);    // Link printf output (stdout) to serial port via my_putc().
#endif

  MyPrintf("I2c toolkit, compiled at %s %s\n", __DATE__, __TIME__ );

  TK_WIRE.begin();              // start I2C
}

//-----------------------------------------------------------------------------
// loop -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void loop()
{
   Command.Takt(Serial);
}
