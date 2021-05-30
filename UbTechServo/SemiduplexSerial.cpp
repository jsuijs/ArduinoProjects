
#include <Arduino.h>
extern HardwareSerial Serial2;
#define Serial Serial2
#define CSerial Serial2

int UbtWrite(unsigned char *TxBuf, int TxLength, unsigned char *RxBuf, int RxLength);

#include"SemiduplexSerial.h"

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
         RxAckLen = 1;  //1,4命令只应答一个字节
      } else if((CMD == 0x02) || (CMD==0x03)) {
         RxAckLen = 8;
      } else if ((CMD == 0x01) && (Head == 0xFC)) {
         RxAckLen = 4;
      }

Retry_Servo:

   //3  Serial2.write(buf,len + 1);  //发送消息
   //3  tRet = Serial1.readBytes( RxBuf, RxAckLen+10); //接收应答
   tRet = UbtWrite(TxBuf, len+1,  RxBuf, RxAckLen+10);

   if (tRet == 0) { // No message received

      if (tCnt < 2) {
         tCnt ++;  // retry
         goto  Retry_Servo;
      }
   }  else {
      // Message received

      RxBuf_count = tRet;
      tRet = 0;
      switch(CMD){
         case 0x01:
            if(Head==0xFC){   // getServoId
               if (  RxBuf[len+1]==0xfc &&
                     RxBuf[len+2]==0xcf &&
                     RxBuf[len+4]==0xaa)   {
                  tRet=RxBuf[len+3];
                  Serial.printf("c1 %d\n", tRet);
               }
               else if( (RxBuf[len+1]!=0xfc && RxBuf[len+1]!=0) ||
                        (RxBuf[len+2]!=0xcf && RxBuf[len+2]!=0) ||
                        (RxBuf[len+4]!=0xAA && RxBuf[len+4]!=0))   {
                  tRet=ServoNO+0xec;
                  Serial.printf("c2 %d\n", tRet);
                  // received something but not fc cf ** aa
               }
               else{
                  tRet=0;
                  Serial.printf("c3 %d\n", tRet);
               }
            }
            else{ // setServoAngle /setServoTurn
               tRet=RxBuf[len+1]-0xAA-ServoNO;
               Serial.printf("c4 %d\n", tRet);
            }

            break;
         case 0x02:  // readServoAnglePD
            tRet=(RxBuf[len+7]<<8) |(RxBuf[len+8] & 0xff);
            Serial.printf("c5 %d (%d %d)\n", tRet, RxBuf[len+7], RxBuf[len+8]);
            break;
         case 0x03:  // readServoAngleNPD
            tRet=(RxBuf[len+7]<<8) |(RxBuf[len+8] & 0xff);

            // 5/6 seems to be setpoint
            // 7/8 seems to be actual, but only from -118 .. 60, while range is -118..118
            // 0 = no angle
            Serial.printf("c6 %d\n", tRet);
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
//      unsigned char buf[10];
      unsigned char len = 9; //9+1
      unsigned char RxAckLen = 11;

      TrxSetup(Head, ServoNO, CMD);

      memcpy((void *)&TxBuf[4], (void *)Data, 4);
      TxBuf[len - 1] = CheckSum(len-3, &TxBuf[2]);
      TxBuf[len] = 0xED;

      tRet = UbtWrite(TxBuf, len+1, RxBuf, RxAckLen+len);

      Serial.printf("ubtServoIdProtocol %d\n", len);
      Serial.println(CheckSum(len-3, &RxBuf[len+3]),HEX);
      Serial.println(RxBuf[len+9],HEX);

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

// Send 4-byte data, receive 1 byte & return # of bytes received

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
