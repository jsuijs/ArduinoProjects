// I2cToolkit.ino -

//-----------------------------------------------------------------------------
// printf for microbit:
extern "C" int _write(int __attribute__ ((unused)) file, char *buffer, int size)
   { return Serial.write(buffer, size); }
// #define MyPrintf CSerial.printf

//-----------------------------------------------------------------------------
// printf for STM32:
extern "C" void putch(unsigned char c)
   { Serial.print((char)c); }
// #define MyPrintf CSerial.printf

//-----------------------------------------------------------------------------
// printf for AVR:
#ifdef __AVR__
   int my_putc(char c, FILE __attribute__ ((unused)) *t) {
     if (c == '\n') Serial.write('\r');
     return Serial.write(c);
   }

   #define MyPrintf printf
#endif

//-----------------------------------------------------------------------------
// Toolkit & commands setup.
#include <Wire.h>

#define MAIN
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

#ifdef __AVR__
  fdevopen( &my_putc, 0);    // Link printf output (stdout) to serial port via my_putc().
#endif

  MyPrintf("I2c toolkit, compiled at %s %s\n", __DATE__, __TIME__ );

  Wire.begin();              // start I2C
}

//-----------------------------------------------------------------------------
// loop -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void loop()
{
   Command.Takt(Serial);
}
