// ArloMotorController.ino

#include "project.h"
#include "Commands.h"

//https://forum.arduino.cc/index.php?topic=566042.0
HardwareSerial Serial(USART1);
HardwareSerial Serial2(USART2);

int led = 33;

void setup()
{
   // initialize the digital pin as an output.
   pinMode(led, OUTPUT);

   // LET OP: het is blijkbaar van belang ook de 2e poort te openen aan het begin
   // van het programma. Anders lijkt serial2 toch usart1 te worden....
   Serial.begin(115200);   // Console
   Serial2.begin(115200);  // interface naar OpenCM.

   //  Serial.println("Serial _ ready.");
   Serial2.println("Serial2 ready.");

   EncodersInit();
   MotorsInit();

   printf("ArloMotorController ready.\n");
}

void loop() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(100);               // wait for a second
//  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
//  delay(300);               // wait for a second
//  Serial.print(Timer2.getCount());
//  Serial.print(" ");
//  Serial.println(Timer4.getCount());


   Command.Takt(Serial);  // Drive command interpreter
}

void MyCommands(int Number[])
{
   if (Command.Match("?", 0)) {
      printf("ArloMotorController - see source for help on commands ;)\n");
   }

   if (Command.Match("motors", 2)) {
      Motors(Number[0], Number[1]);
   }

   if (Command.Match("encoders", 0)) {
      int Left, Right;
      EncodersRead (Left, Right);
      printf("Encoders: %d %d\n", Left, Right);
   }
}
