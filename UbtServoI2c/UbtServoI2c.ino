//-----------------------------------------------------------------------------
// UbtServoDemo.ino - demo program for UbTech servo control
//-----------------------------------------------------------------------------
// See Execute() function for supported commands.
//-----------------------------------------------------------------------------

#define MAIN
#include "Project.h"

HardwareSerial Serial2 (PA3, PA2);  // console serial

const int IDENTIFICATION = 185;
#include "I2cInterface.h"
#include "I2cSlaveRegisters.h"

// Create UBT serial port
// * for now stm32f1 specific
// * hardcoded to Serial1
// * connect pin 15 & 16 (PB7 & PB6) to each other & servo data pins
// * single wire half duplex supported by setting TX pin to input
//   when done transmitting.
#include "UbTechSerial.h"

// Servo class
#include "UbTechServo.h"
uKitServo UbtServo;

// command-parser
void Execute(int Param[]);
TCommand  Command(Execute);

//---------------------------------------------------------------------------------------
// setup -
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void setup() {
   // start serial
   CSerial.begin(115200);
   CSerial.printf("UbtServoI2c starup.\n");

   UbtSetup();
   UbtServo.getServoId(9);    // first message always fails, so have that failure now
                              // (works when servo # exists and when it doesn't...).

   pinMode(PB1, OUTPUT);      //Led on Maple-Mini


   I2cSlaveRegistersInit(0x98/2);
   CSerial.printf("Startup done.\n");
}

//---------------------------------------------------------------------------------------
// loop -
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void loop() {
   static int PrevMs;

   // once per milisecond
   int ms = millis();
   if (ms != PrevMs) {
      PrevMs = ms;
      BlinkTakt();
   }

   if (I2cNewDataAvailable()) {
      // NewData, so probably a command to process.
      CSerial.printf("I2c command %d [%d] (%d %d %d %d)\n",
        RegisterGetByte(R_CMD), RegisterGetByte(R_SERVO_ID),
        RegisterGetByte(R_PARAM0), RegisterGetByte(R_PARAM1),
        RegisterGetByte(R_PARAM2), RegisterGetByte(R_PARAM3)
      );
      int Cmd = RegisterGetByte(R_CMD);
      RegisterSetByte(R_CMD, 0); // clear command

      // execute command
      switch(Cmd) {
         case 1 : {  // ServoSetTurn
            UbtServo.setServoTurn(RegisterGetByte(R_SERVO_ID), RegisterGetByte(R_PARAM0), RegisterGetByte(R_PARAM1));  // id dir speed
         }
         break;

         case 2 : {  // ServoSetAngle
            UbtServo.setServoAngle(RegisterGetByte(R_SERVO_ID), RegisterGetByte(R_PARAM0), 20 * RegisterGetByte(R_PARAM1));
         }
         break;

         case 3 : { // ServoSetStop
            UbtServo.setServoStop(RegisterGetByte(R_SERVO_ID));
         }
         break;

         case 4 : {  // ServoSetId
            UbtServo.setServoId(RegisterGetByte(R_SERVO_ID), RegisterGetByte(R_PARAM0));
         }
         break;

         case 5 : { // ServoReadAngle
            int r = UbtServo.readServoAngleNPD(RegisterGetByte(R_SERVO_ID));
            RegisterSetWord(R_RET0, r);
         }
         break;

         case 6 : { // ServoReadAnglePD
            int r = UbtServo.readServoAnglePD(RegisterGetByte(R_SERVO_ID));
            RegisterSetWord(R_RET0, r);
         }
         break;

         case 7 : { // ScanNext
            RegisterSetWord(R_RET0, 257);
            int r = UbtServo.ScanNext(RegisterGetByte(R_SERVO_ID));
            RegisterSetWord(R_RET0, r);
         }
         break;
      }
   }

   Command.Takt(CSerial);  // Console command interpreter
}

//-----------------------------------------------------------------------------
// Execute - execute commando
//-----------------------------------------------------------------------------
// Called via CmdTakt() when a command is received from the serial port.
//-----------------------------------------------------------------------------
void Execute(int Param[])
{
   if (Command.Match("?",                 0)) Command.Help("ArduinoPlanck command parser.");

   if (Command.Match("ServoSetTurn",      3)) UbtServo.setServoTurn(Param[0], Param[1], Param[2]);  // id dir speed
//   if (Command.Match("ServoSetStif",      2)) UbtServo.setServoStiffness(Param[0], Param[1]);
   if (Command.Match("ServoSetAngle",     2)) UbtServo.setServoAngle(Param[0], Param[1], 200);
   if (Command.Match("ServoSetAngle",     3)) UbtServo.setServoAngle(Param[0], Param[1], Param[2]);

   if (Command.Match("ServoSetStop",      1)) UbtServo.setServoStop(Param[0]);

   if (Command.Match("Scan",              0)) UbtServo.Scan(); // scan for servo's & print result

   if (Command.Match("ServoGetId",        1)) CSerial.printf("R: %d\n", (int)UbtServo.getServoId(Param[0]));

   if (Command.Match("ServoSetId",        2)) CSerial.printf("R: %d\n", (int)UbtServo.setServoId(Param[0], Param[1]));

   if (Command.Match("ServoReadAngle",    1)) CSerial.printf("Degrees: %d\n", UbtServo.readServoAngleNPD(Param[0]));
   if (Command.Match("ServoReadAnglePD",  1)) CSerial.printf("Degrees: %d\n", UbtServo.readServoAnglePD(Param[0]));

   if (Command.Match("I2cDump",        0)) {
      CSerial.printf("Events Rx: %d, Tx: %d\n", RxEventCounter, TxEventCounter);
      RegDump();
   }
}

//-----------------------------------------------------------------------------
// _write - link printf to CSerial
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
extern "C" int _write(int file, char *ptr, int len)
{
    int i;
    file = file;
    for (i = 0; i < len; i++) {
       CSerial.print(*ptr++);
    }
    return len;
}

//-----------------------------------------------------------------------------
// BlinkTakt -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void BlinkTakt()
{  static int Count;

   if (Count > 100) {
      digitalWrite(PB1, ! digitalRead(PB1));
      Count = 0;
   }
   Count ++;
}
