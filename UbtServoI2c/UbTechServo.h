//-----------------------------------------------------------------------------
// UbTechServo.cpp
//-----------------------------------------------------------------------------
// The code in this file is significantly modified subset, extraced from
// various files of:
//    https://github.com/UBTEDU/uKitExplore-library
//-----------------------------------------------------------------------------
#ifndef UBTECH_SERVO_H
#define UBTECH_SERVO_H

#include <Arduino.h>

class SemiduplexSerial
{
public:

    unsigned short   ubtServoProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data);
    unsigned char    ubtServoIdProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data);
    unsigned char    ubtServoActionProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data);

    unsigned char    CheckSum(unsigned char len, unsigned char *buf);

private:
   void TrxSetup(unsigned char Head,unsigned char ServoNO,unsigned char CMD);

   unsigned char RxBuf[120];
   unsigned char TxBuf[10];
};

class uKitServo : public SemiduplexSerial
{
public:

   unsigned char setServoId(char oldid,char newid);
   unsigned char getServoId(char id);

   unsigned char setServoId1M(char oldid,char newid);
   unsigned char getServoId1M();
   unsigned char getServoId1M(char id);

   void Scan();
   int  ScanNext(int InId);

   void setServoTurn(unsigned char id,int dir, int speed);      // The steering gear wheel mode control, id is the number of the steering gear, dir is the direction (0 clockwise, 1 counterclockwise), speed is the speed (0-255)）
   //void setServoStiffness(unsigned char id,unsigned char stiffness);
   void setServoAngle(unsigned char id,int angle, int times);   // Servo mode, id is the number of the servo, angle is the angle (-118°~118°), and times is the running time (300-5000ms)

   void setServoStop(unsigned char id);                         // Servo stop function
   int  readServoAnglePD(unsigned char id);                     // Servo read back, return the servo angle value (return after power off)
   int  readServoAngleNPD(unsigned char id);                    // Servo read back, return the servo angle value (read back without power down)
};

// prototype of interface to hardware
int UbtWrite(unsigned char *TxBuf, int TxLength, unsigned char *RxBuf, int RxLength);

#endif
