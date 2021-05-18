#define UNO

#ifdef UNO
   #define CSprintf printf
   #define CSerial Serial

   #include <Wire.h>
   #define MyWire Wire

   int my_putc(char c, FILE *t) { t = t;  return CSerial.write(c); } // to support printf

#else
   HardwareSerial Serial2 (PA3, PA2);
   #define CSprintf Serial2.printf
   #define CSerial Serial2

   #include <Wire.h>
   TwoWire Wire99(PB11, PB10);
   #define MyWire Wire99
#endif

#include "LppMaster.h"
TLpp Lpp;

int PeriodicInterval = 0;     // 0 = alleen toolkit, anders periodiek uitlezen printen.

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void setup() {
   CSerial.begin(115200);      // start serial

#ifdef UNO
   fdevopen( &my_putc, 0);  // device 0 (stdout) output naar my_putc()
#endif

   PrintTkMsg();              // print helptekst van I2C Master Toolkit

   MyWire.begin();              // start I2C

   delay(500);  // allow Lpp to start up.

   if (Lpp.begin()) {
      // init success
      Lpp.SetOffsetDegrees(180);       // Align lidar with robotlib coordinate system
      Lpp.SetReverse(1);

      Lpp.ArraySetup(-90, 20, 9);      // Setup array with 9 segments of 20 degrees
      Lpp.SensorSetup(0, -60, 120);    // Setup Sensor 0 to detect forward objects

      // Lees en print status (ter informatie)
      Lpp.ReadStatus();
      Lpp.PrintStatus();

      Lpp.Start();
   } else {
      CSprintf("LPP I2C error.\n");
   }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void loop()
{

   CGet();  // this drives the I2CmTk

   if (PeriodicInterval) {

      if (Lpp.IsRunning()) {
         Lpp.ReadSensors();
         CSprintf("Sensor 0, graden: %d, afstand: %d\n", Lpp.Sensor[0].Degrees32/32, Lpp.Sensor[0].Distance);

         Lpp.ReadArray();
         CSprintf("Array: ");
         for (int i=0; i<9; i++) {
            CSprintf("%5d ", Lpp.Array[i].Distance);
         }
         CSprintf("\n");
      }

      delay(PeriodicInterval);
   }
}
