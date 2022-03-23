// I2cToolkit.ino -
#define MAIN
#include <Wire.h>

#ifdef ARDUINO_ARCH_STM32
//-----------------------------------------------------------------------------
// printf for STM32:

   //https://forum.arduino.cc/index.php?topic=566042.0
   //HardwareSerial Serial(PA10, PA9);   // RX, TX
   HardwareSerial Serial(PA3, PA2);   // RX, TX
   #define TK_SERIAL Serial
   
   extern "C" int _write(int __attribute__ ((unused)) file, char *buffer, int size)
      { return Serial.write(buffer, size); }

   // Use alternative I2C pins
   TwoWire TK_WIRE(PB11, PB10);   // sda, scl
//---
#endif

#ifdef ARDUINO_ARCH_RP2040
   #define TK_SERIAL Serial1 // Note: for printf, select this serial port in tools->'Debug Port'
   #define TK_WIRE Wire
#endif

//-----------------------------------------------------------------------------
// printf for microbit:
//-extern "C" int _write(int __attribute__ ((unused)) file, char *buffer, int size)
//-   { return Serial.write(buffer, size); }
//-#define TK_WIRE Wire

//-----------------------------------------------------------------------------
// printf for AVR:
#ifdef __AVR__
   #define TK_SERIAL Serial
   #define TK_WIRE Wire

   int my_putc(char c, FILE __attribute__ ((unused)) *t) {
     if (c == '\n') TK_SERIAL.write('\r');
     return TK_SERIAL.write(c);
   }
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
  TK_SERIAL.begin(115200);      // start serial
  TK_SERIAL.print("-------------------------------------\n");

#ifdef __AVR__
  fdevopen( &my_putc, 0);    // Link printf output (stdout) to serial port via my_putc().
#endif

#ifdef ARDUINO_ARCH_RP2040
    TK_WIRE.setSDA(8);
    TK_WIRE.setSCL(9);
#endif
  MyPrintf("I2c toolkit, compiled at %s %s\n", __DATE__, __TIME__ );

  TK_WIRE.begin();              // start I2C
  MyPrintf("Wire started\n");
}

//-----------------------------------------------------------------------------
// loop -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void loop()
{
   Command.Takt(TK_SERIAL);
}
