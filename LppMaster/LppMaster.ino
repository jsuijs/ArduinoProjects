#include <Wire.h>
#include "LppMaster.h"
TLpp Lpp;

int PeriodicInterval = 500;     // 0 = alleen toolkit, anders periodiek uitlezen printen.

void setup() {
   Serial.begin(115200);      // start serial
   fdevopen( &my_putc, 0);    // Koppel printf uitvoer (stdout) via my_putc() aan de serial port.

   PrintTkMsg();              // print helptekst van I2C Master Toolkit

   Wire.begin();              // start I2C

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
      printf("LPP I2C error.\n");
   }
}

void loop()
{

   CGet();  // this drives the I2CmTk

   if (PeriodicInterval) {

      if (Lpp.IsRunning()) {
         Lpp.ReadSensors();
         printf("Sensor 0, graden: %d, afstand: %d\n", Lpp.Sensor[0].Degrees32/32, Lpp.Sensor[0].Distance);

         Lpp.ReadArray();
         printf("Array: ");
         for (int i=0; i<9; i++) {
            printf("%5d ", Lpp.Array[i].Distance);
         }
         printf("\n");
      }

      delay(PeriodicInterval);
   }
}
