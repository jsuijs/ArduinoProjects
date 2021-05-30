
#include <Arduino.h>
extern HardwareSerial Serial2;
#define Serial Serial2
#define CSerial Serial2

int UbtWrite(unsigned char *TxBuf, int TxLength, unsigned char *RxBuf, int RxLength);


#include"SemiduplexSerial.h"

#define u8 unsigned char


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
      TxBuf[1] = ((Head & 0x0f) << 4) | ((Head & 0xf0) >> 4); // swap8(Head);
      TxBuf[2] = ServoNO; //舵机好
      TxBuf[3] = CMD;
   }
unsigned short SemiduplexSerial::ubtServoProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data)
   {
      unsigned short tRet = 0;
      unsigned char tCnt = 0;
      unsigned char len = 9; //9+1
      unsigned char Usart3_Rx_Ack_Len=0;

      TrxSetup(Head, ServoNO, CMD);

      memcpy((void *)&TxBuf[4],(void *)Data,4);
      TxBuf[len - 1] = CheckSum( (len - 3),(u8*)&TxBuf[2]);
      TxBuf[len] = 0xED;

      if (((CMD == 0x01) && (Head != 0xFC)) || (CMD == 0x04) || (CMD == 0xCD)) {
         Usart3_Rx_Ack_Len = 1;  //1,4命令只应答一个字节
      } else if((CMD == 0x02) || (CMD==0x03)) {
         Usart3_Rx_Ack_Len = 8;
      } else if ((CMD == 0x01) && (Head == 0xFC)) {
         Usart3_Rx_Ack_Len = 4;
      }

Retry_Servo:

   //3  Serial2.write(buf,len + 1);  //发送消息
   //3  tRet = Serial1.readBytes( RxBuf, Usart3_Rx_Ack_Len+10); //接收应答
   tRet = UbtWrite(TxBuf, len+1,  RxBuf, Usart3_Rx_Ack_Len+10);

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
      unsigned char Usart3_Rx_Ack_Len = 11;

      TrxSetup(Head, ServoNO, CMD);

      memcpy((void *)&TxBuf[4], (void *)Data, 4);
      TxBuf[len - 1] = CheckSum( (len - 3),(u8*)&TxBuf[2]);
      TxBuf[len] = 0xED;

      tRet = UbtWrite(TxBuf, len+1, RxBuf, Usart3_Rx_Ack_Len+len);

      Serial.printf("ubtServoIdProtocol %d\n", len);
      Serial.println(CheckSum( (len - 3),(u8*)&RxBuf[len+3]),HEX);
      Serial.println(RxBuf[len+9],HEX);

      if (  RxBuf[len+1]==0xFC &&
            RxBuf[len+2]==0xCF &&
            RxBuf[len+4]==0xAA &&
            CheckSum( (len - 3),(u8*)&RxBuf[len+3]) == RxBuf[len+9]) {
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
      const unsigned char Usart3_Rx_Ack_Len=1;

      TrxSetup(Head, ServoNO, CMD);

      memcpy((void *)&TxBuf[4], (void *)Data, 4);
      TxBuf[len - 1] = CheckSum(len - 3, &TxBuf[2]);
      TxBuf[len] = 0xED;

      return UbtWrite(TxBuf, len+1,  RxBuf, Usart3_Rx_Ack_Len+len+1);
   }

/**@brief EN:Communication protocol sending and receiving functions/CN:通讯协议发送和接受函数.
 *
 * @param[in] Head EN:Protocol head/CN:协议头.
 * @param[in] ServoNO EN:Servo id/CN:舵机号.
 * @param[in] len EN:data length/CN:数据长度.
 * @param[in] CMD EN:CMD code/CN:CMD码.
 * @param[in] Data EN:data buffer/CN:数据缓冲区.
 *
 * @returns tRet EN:Accept return value/CN:接受返回值.
 */
unsigned long SemiduplexSerial::TXD(unsigned char Head,unsigned char ServoNO,unsigned char len,unsigned char CMD,unsigned char * Data){
  unsigned long tRet = 0;
  unsigned char tCnt = 0;
  unsigned char buf[60];
  unsigned char length = 9; //9+1

  unsigned char Usart3_Rx_Ack_Len=0;

  memset((void *)Usart3_Rx_Buf,0,sizeof(Usart3_Rx_Buf));
  memset((void *)buf,0,sizeof(buf));
  Usart3_Rx_Ack_Len = 17; // Reply message length

  buf[0] = Head;  // Fill the protocol header
  buf[1] = swab8(Head);
  if(Head >= 0xFA)
  {
    buf[2] = ServoNO; //Good steering gear
  }
  else if(Head > 0x00)  // Variable Length Agreement
  {
    length = len + 5; // length
    buf[2] = length;
  }
  buf[3] = CMD;
  memcpy((void *)&buf[4],(void *)Data,len);
  buf[length - 1] = CheckSum( (length - 3),(u8*)&buf[2]);
  buf[length ] = 0xED;


  if(Head == 0xFA)
  {
    if(CMD == 1 )
    {
      if(ServoNO == 0)
      {
        // There is a problem with this sentence -memset- does not support double-byte padding
        //memset((void *)&gsSave_Angle,0xFF,sizeof(gsSave_Angle));  // Save all angles
        //memset((void *)&gsSave_Angle,Data,sizeof(gsSave_Angle));  // Save all angles
        memset((void *)&gsSave_Angle,Data[0],sizeof(gsSave_Angle)); // Save all angles
      }
      else
      {
        if(Data[0] == gsSave_Angle.Angle[ServoNO - 1])  return  0;  // Do not send, return directly
        else
        {
          gsSave_Angle.Angle[ServoNO - 1] = Data[0];  // Save a single angle, it may be double-byte angle in the future
        }
      }
      Usart3_Rx_Ack_Len = 1;  //1,4 The command only responds with one byte
    }
    else if( CMD == 4)  Usart3_Rx_Ack_Len = 1;  //1,4 The command only responds with one byte
  }

Retry_Servo:

  tRet = UbtWrite(buf, length+1,  Usart3_Rx_Buf, Usart3_Rx_Ack_Len+10);

  if(tRet == 0) // No message received
  {
    if( tCnt < 2)
    {
      tCnt ++;  // Retry
      goto  Retry_Servo;
    }
  }
  else  // Message received
  {
  Usart3_Rx_Buf_count = tRet;
    tRet = 0;
    if(Head == 0xFA)
    {
      if( (CMD == 1) || (CMD == 4) )
      {
        if( (Usart3_Rx_Buf[RXD_OFFSET + 1] == 0xAA + ServoNO) )
        {
          tRet = ServoNO;
        }
      }
      else if(CMD == 2 | CMD==3 |CMD==8)
      {
        if( (Usart3_Rx_Buf[RXD_OFFSET + 3] == 0xAA) && (Usart3_Rx_Buf[RXD_OFFSET + 2] == ServoNO) )
        {
          tRet = Usart3_Rx_Buf[10+6] << 8;
          tRet |= Usart3_Rx_Buf[10+7] << 0;
        }
      }


      else if(CMD == 0xCD )
      {
        if( (Usart3_Rx_Buf[RXD_OFFSET + 5] == ServoNO) )
        {
          tRet = Usart3_Rx_Buf[RXD_OFFSET + 5];
        }
      }
      else if(CMD == 0xD2 )
      {
        if( (Usart3_Rx_Buf[RXD_OFFSET + 3] == 0xAA) && (Usart3_Rx_Buf[RXD_OFFSET + 2] == ServoNO) )
        {
          tRet = 1;
        }
      }
      else if(CMD == 0xD4 )
      {
        if( (Usart3_Rx_Buf[RXD_OFFSET + 3] == 0xAA) && (Usart3_Rx_Buf[RXD_OFFSET + 2] == ServoNO) )
        {
          tRet = Usart3_Rx_Buf[RXD_OFFSET + 6] << 8;
          tRet |= Usart3_Rx_Buf[RXD_OFFSET + 7] << 0;
        }
      }
    }
    else if(Head == 0xFC)
    {
      if(CMD == 1 )
      {
        if((Usart3_Rx_Buf[RXD_OFFSET + 3] == 0xAA)  ) // Change the version number to read ID
        {
          tRet=Usart3_Rx_Buf[RXD_OFFSET + 2];

        }
      }
    }
    else if( (Head < 0xFA) && (Head > 0x00) && (Usart3_Rx_Buf[length + 4] == CMD) ) // sensor
    {
      if(CMD == 0x02 )
      {
        tRet = Usart3_Rx_Buf[length + 5] - 0xAA;
      }
      else if (CMD == 0x03)
      {
        tRet = Usart3_Rx_Buf[length + 5] - 0xAA;
      }
      else if (CMD == 0x04)
      {
        //Serial.println(Usart3_Rx_Buf[length + 3]);
        if(Usart3_Rx_Buf[length + 3] == 7)
          tRet = ((Usart3_Rx_Buf[length + 5] - 0xAA) << 8) + Usart3_Rx_Buf[length + 6];
        else   if(Usart3_Rx_Buf[length + 3] == 8)
          tRet = (Usart3_Rx_Buf[length + 6] << 8) + Usart3_Rx_Buf[length + 7];
        else   if(Usart3_Rx_Buf[length + 3] == 9)
          tRet = (Usart3_Rx_Buf[length + 7] << 8) + Usart3_Rx_Buf[length + 8];
        else   if(Usart3_Rx_Buf[length + 3] == 17)
        {

             tRet = ((Usart3_Rx_Buf[length + 5] - 0xAA) << 8) + Usart3_Rx_Buf[length + 6];
             redvalue = Usart3_Rx_Buf[length + 6];
             greenvalue = Usart3_Rx_Buf[length + 7];
             bluevalue = Usart3_Rx_Buf[length + 8];
        }

      }
      else if((CMD==0x06) | (CMD==0x07))
      {
        if(Usart3_Rx_Buf[length + 4]==7 && Usart3_Rx_Buf[length + 5]-0xAA!=Data[0]){
           tRet =254;
        }
        else if(Usart3_Rx_Buf[length + 4]==7&& Usart3_Rx_Buf[length + 5]-0xAA==Data[0]){
          tRet = Usart3_Rx_Buf[length + 5] - 0xAA;
        }
        else{
          tRet = 0;
        }
      }
    }
  }
  return tRet;
}






