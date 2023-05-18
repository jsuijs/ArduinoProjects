//-----------------------------------------------------------------------------
// UbTechServo.cpp
//-----------------------------------------------------------------------------
// The code in this file is significantly modified subset, extraced from
// various files of:
//    https://github.com/UBTEDU/uKitExplore-library
//-----------------------------------------------------------------------------
#include <Arduino.h>
#include "project.h"  // for CSerial
#include "UbTechServo.h"

#define swap8(x) ((x&0x0f) << 4 | (x&0xf0) >> 4)

/**@brief EN:JIMU Servo checksum/CN:JIMU舵机校验
 *
 * @param[in] len EN:Data length/CN：数据长度.
 * @param[in] len EN:Data buffer/CN:数据缓冲区.
 *
 * @returns sum EN: checksum value/CN:校验值.
 */
unsigned char SemiduplexSerial::CheckSum(unsigned char len, unsigned char *InBuf){
  unsigned char i, sum = 0;
  if(len > 254) return 0;
  for(i = 0; i < len; i++){
    sum += InBuf[i];
  }
  return  (uint8_t)(sum);
}

//-----------------------------------------------------------------------------
// SemiduplexSerial::TrxSetup - support function
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SemiduplexSerial::TrxSetup(unsigned char Head,unsigned char ServoNO,unsigned char CMD)
   {
      // clear buffers
      memset((void *)RxBuf,0,sizeof(RxBuf));
      memset((void *)TxBuf,0,sizeof(TxBuf));
      // setup tx message
      TxBuf[0] = Head;  //填充协议头
      TxBuf[1] = swap8(Head);
      TxBuf[2] = ServoNO; //舵机好
      TxBuf[3] = CMD;
   }

//-----------------------------------------------------------------------------
// SemiduplexSerial::ubtServoProtocol -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
unsigned short SemiduplexSerial::ubtServoProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data)
   {
      unsigned short tRet = 0;
      unsigned char tCnt = 0;
      unsigned char len = 9; //9+1
      unsigned char RxAckLen=0;

      TrxSetup(Head, ServoNO, CMD);

      memcpy((void *)&TxBuf[4],(void *)Data,4);
      TxBuf[len - 1] = CheckSum(len-3, &TxBuf[2]);
      TxBuf[len] = 0xED;

      if (((CMD == 0x01) && (Head != 0xFC)) || (CMD == 0x04) || (CMD == 0xCD)) {
         RxAckLen = 1;  //1,4命令只应答一个字节 - The command only responds with one byte
      } else if((CMD == 0x02) || (CMD==0x03)) {
         RxAckLen = 8;
      } else if ((CMD == 0x01) && (Head == 0xFC)) {
         RxAckLen = 4;
      }

Retry_Servo:
   tRet = UbtWrite(TxBuf, len+1,  RxBuf, RxAckLen+10);

   if (tRet == 0) { // No message received

      if (tCnt < 2) {
         tCnt ++;  // retry
         goto  Retry_Servo;
      }
   }  else {
      // Message received
      tRet = 0;
      switch(CMD){
         case 0x01:
            if(Head==0xFC){   // getServoId
               if (  RxBuf[len+1]==0xfc &&
                     RxBuf[len+2]==0xcf &&
                     RxBuf[len+4]==0xaa)   {
                  tRet=RxBuf[len+3];
                  CSerial.printf("c1 %d\n", tRet);
               }
               else if( (RxBuf[len+1]!=0xfc && RxBuf[len+1]!=0) ||
                        (RxBuf[len+2]!=0xcf && RxBuf[len+2]!=0) ||
                        (RxBuf[len+4]!=0xAA && RxBuf[len+4]!=0))   {
                  tRet=ServoNO+0xec;
                  CSerial.printf("c2 %d\n", tRet);
                  // received something but not fc cf ** aa
               }
               else{
                  tRet=0;
                  CSerial.printf("c3 %d\n", tRet);
               }
            }
            else{ // setServoAngle /setServoTurn
               tRet=RxBuf[len+1]-0xAA-ServoNO;
               CSerial.printf("c4 %d\n", tRet);
            }

            break;
         case 0x02:  // readServoAnglePD
            tRet=(RxBuf[len+7]<<8) |(RxBuf[len+8] & 0xff);
            //CSerial.printf("c5 %d (%d %d)\n", tRet, RxBuf[len+7], RxBuf[len+8]);
            break;
         case 0x03:  // readServoAngleNPD
            tRet=(RxBuf[len+7]<<8) |(RxBuf[len+8] & 0xff);

            // 5/6 seems to be setpoint
            // 7/8 seems to be actual, but only from -118 .. 60, while range is -118..118
            // 0 = no angle
            //CSerial.printf("c6 %d\n", tRet);
            break;
      }
  }
  return tRet;
}

//-----------------------------------------------------------------------------
// SemiduplexSerial::ubtServoIdProtocol -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
unsigned char SemiduplexSerial::ubtServoIdProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data)
   {
      unsigned char tRet = 0;
      unsigned char len = 9; //9+1
      unsigned char RxAckLen = 11;

      TrxSetup(Head, ServoNO, CMD);

      memcpy((void *)&TxBuf[4], (void *)Data, 4);
      TxBuf[len - 1] = CheckSum(len-3, &TxBuf[2]);
      TxBuf[len] = 0xED;

      tRet = UbtWrite(TxBuf, len+1, RxBuf, RxAckLen+len);

      //CSerial.printf("ubtServoIdProtocol %d\n", len);
      //Serial.println(CheckSum(len-3, &RxBuf[len+3]),HEX);
      //Serial.println(RxBuf[len+9],HEX);

      if (  RxBuf[len+1]==0xFC &&
            RxBuf[len+2]==0xCF &&
            RxBuf[len+4]==0xAA &&
            CheckSum(len-3, &RxBuf[len+3]) == RxBuf[len+9]) {
         // servo responded, return ID from answer
         tRet=RxBuf[len+3];
      } else if(  RxBuf[len+1]==0 &&
                  RxBuf[len+2]==0 &&
                  RxBuf[len+3]==0 &&
                  RxBuf[len+4]==0 &&
                  RxBuf[len+9]==0) {
         // no response
         tRet=0;
      } else {
         tRet=ServoNO+0xec;
         CSerial.printf("error?!? %d\n", tRet);
      }
      return tRet;
   }

//-----------------------------------------------------------------------------
// SemiduplexSerial::ubtServoActionProtocol -
//-----------------------------------------------------------------------------
// Send 4-byte data, receive 1 byte & return # of bytes received
//-----------------------------------------------------------------------------
unsigned char SemiduplexSerial::ubtServoActionProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data)
   {
      const unsigned char len = 9; //9+1
      const unsigned char RxAckLen=1;

      TrxSetup(Head, ServoNO, CMD);

      memcpy((void *)&TxBuf[4], (void *)Data, 4);
      TxBuf[len - 1] = CheckSum(len - 3, &TxBuf[2]);
      TxBuf[len] = 0xED;

      return UbtWrite(TxBuf, len+1,  RxBuf, RxAckLen+len+1);
   }

//-----------------------------------------------------------------------------
// uKitServo::setServoId - change Id of servo
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
unsigned char uKitServo::setServoId(char oldid,char newid)
   {
      unsigned char tRet = 0;
      unsigned char buf[2];
      buf[0] = 0x00;
      buf[1] = newid;
      tRet = ubtServoProtocol(0xFA, oldid, 0xCD, buf);
      delay(5);
      return tRet;
   }

//-----------------------------------------------------------------------------
// uKitServo::Scan - print ID's of available servo's
//-----------------------------------------------------------------------------
// Return: Id on success, 0 on failure
//-----------------------------------------------------------------------------
void uKitServo::Scan()
   {
      for (int testid=1; testid<=18; testid++) {

         if (getServoId(testid) == testid) {
            CSerial.printf("Servo #: %d\n", testid);
         }
      }
   }

//-----------------------------------------------------------------------------
// uKitServo::getServoId - check if servo with Id exists
//-----------------------------------------------------------------------------
// Return: Id on success, 0 on failure
//-----------------------------------------------------------------------------
unsigned char uKitServo::getServoId(char id)
   {
      unsigned char tRet = 0;
      unsigned char buf[4] = {0, 0, 0, 0};

      //tRet=TXD(0xFC,id,4,0x01,buf);
      tRet=ubtServoIdProtocol(0xFC,id,0x01,buf);
      delay(2);

      return tRet;
}

//-----------------------------------------------------------------------------
// uKitServo::setServoTurn - set continues rotation mode
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void uKitServo::setServoTurn(unsigned char id,int dir, int speed)
   {
      unsigned char buf[4];
      int speeds=0;

      if (speed > 0) speeds=map(speed,1,255,50,1000);

      if (dir==0) buf[0] = 0xFD;
      else        buf[0] = 0xFE;

      buf[1] = 0x00;
      buf[2] = (speeds & 0xFF00) >> 8;
      buf[3] =  speeds & 0x00FF;

      ubtServoActionProtocol(0xFA,id,0x01,buf);
   }

//-----------------------------------------------------------------------------
// uKitServo::setServoAngle -
//-----------------------------------------------------------------------------
// id represents the number of the servo,
// angle represents the angle (angle range -118°~118°),
// time represents the time in ms required for rotation (time range: 300~5000)
//-----------------------------------------------------------------------------
void uKitServo::setServoAngle(unsigned char id, int angle, int times)
   {
      unsigned char buf[4];
      int TimeMs = times / 20;
      buf[0]= angle + 120;
      buf[1]= TimeMs;
      buf[2]= 0;
      buf[3]= 0;

      ubtServoActionProtocol(0xFA,id,0x01,buf);
   }

//-----------------------------------------------------------------------------
// uKitServo::setServoStop - disable servo (Power-down, free-running)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void uKitServo::setServoStop(unsigned char id)
   {
      unsigned char buf[4]={0xFF,0,0,0};
      ubtServoProtocol(0xFA,id,0x01,buf);
   }

//- Stiffnes doesn't seem to work properly.
//- There seems to be a fix on 16-3-2023, but that uses TXD which has
//- been lost in my earlier refactoring.
//- One day, this might be integrated again...
//- //-----------------------------------------------------------------------------
//- // uKitServo::setServoStiffness - ?255 = off?  only 4 bytes (of 8) defined...
//- //-----------------------------------------------------------------------------
//- // stiffness inequal to 255 enables power and might turn servo to a
//- // new position...
//- //-----------------------------------------------------------------------------
//- void uKitServo::setServoStiffness(unsigned char id, unsigned char stiffness){
//-   unsigned char tData[4];
//-   tData[0]=stiffness;
//-   tData[1]=0;
//-   tData[2]=0;
//-   tData[3]=0;
//-
//-   //ubtServoActionProtocol(0xFA,id,0x01,tData);
//-   TXD(0xFA,id,8,0x01,tData ); // 16-3-2023 fix, same message but with 8 bytes of data ?!?
//- }

//-----------------------------------------------------------------------------
// uKitServo::readServoAnglePD - disable servo & read angle
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int uKitServo::readServoAnglePD(unsigned char id)
   {
      int tCmd=0,tRet=0;
      unsigned char aa[4]={0,0,0,0};

      tRet=ubtServoProtocol(0xFA,id,0x02,aa);

      delay(5);

      if (tRet==0) return 0;

      if (tRet==1) return -120;

      return tRet-120;
   }

//-----------------------------------------------------------------------------
// uKitServo::readServoAngleNPD  - read servo without power-down
//-----------------------------------------------------------------------------
// Note: no response from old servo, works with ubt-H04
//-----------------------------------------------------------------------------
int uKitServo::readServoAngleNPD(unsigned char id)
   {  int tCmd=0,tRet=0;
      unsigned char aa[4]={0,0,0,0};

      tRet=ubtServoProtocol(0xFA,id,0x03,aa);

      delay(5);

      if (tRet==0) return 0;

      if (tRet==1) return -120;

      return tRet-120;

   }
