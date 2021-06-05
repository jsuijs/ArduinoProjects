
#include <Wire.h>


#define MyPrintf printf

#define MAIN
#include "Commands.h"

void Execute(int Param[]);
TCommand       Command(Execute);

#include "I2CmTk.h"


// For microbit:
extern "C" int _write(int __attribute__ ((unused)) file, char *buffer, int size)
   { return Serial.write(buffer, size); }

// For STM32:
extern "C" void putch(unsigned char c)
   { Serial.print((char)c); }

// for AVR:
int my_putc(char c, FILE __attribute__ ((unused)) *t) {
  if (c == '\n') Serial.write('\r');
  return Serial.write(c);
}

void setup() {
  Serial.begin(115200);      // start serial

#ifdef __AVR__
  fdevopen( &my_putc, 0);    // Link printf output (stdout) to serial port via my_putc().
#endif

  PrintTkMsg();              // print helptekst van I2C Master Toolkit

  Wire.begin();              // start I2C
}

void loop()
{
  CGet();  // this drives the I2CmTk
}
