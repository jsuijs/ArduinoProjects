// Biped-h - specific commands to interact with the biped robot
const unsigned char FRAME_END   = 0xC0;    /* indicates end of packet */
const unsigned char FRAME_START = 0xC1;    /* indicates start of packet */

//-----------------------------------------------------------------------------
// BipedSet - set 5 servos
//-----------------------------------------------------------------------------
// 6 Params time, s1 ... s5
//-----------------------------------------------------------------------------
void BipedSet(int Param[])
{
   int Time = Param[0];

   for (int Servo=1; Servo<6; Servo++) {
      UbtServo.setServoAngle(Servo, Param[Servo], Time);
   }
}

//-----------------------------------------------------------------------------
// BipedRead - read 5 servos
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void BipedRead()
{
   CSerial.printf("%cREAD", FRAME_START);
   for (int Servo=1; Servo<6; Servo++) {
      CSerial.printf(" %d", UbtServo.readServoAnglePD(Servo));
   }
   CSerial.printf("%c\n", FRAME_END);
}
