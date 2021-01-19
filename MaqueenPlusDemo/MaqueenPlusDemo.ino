
// Create the robot
//#define MAQUEEN_USE_USART3 // USART3 is on I2C pins, default USART1 on servo pins S1 & S2
#include "MaqueenPlus.h"
TMaqueenPlus Robot;

#include "StateMachine.h"

void setup()
{
   delay(100);  // allow IDE to activate terminal after programming...
   Serial.begin(115200);
   printf("Maqueen-plus ready.\n");
}

void loop()
{  static int NextMainTakt;

   // Main takt interval
   int ms = millis();
   if ((ms - NextMainTakt) > 0) {
      NextMainTakt = ms + 50;  // zet tijd voor volgende interval

      DriveTakt();    // stuur motoren aan
   }
}
