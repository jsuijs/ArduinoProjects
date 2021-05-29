
#include <Arduino.h>
extern HardwareSerial Serial2;
#define Serial Serial2
#define CSerial Serial2
//void UbtWrite(unsigned char *buf, int length);
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
unsigned char SemiduplexSerial::Cheak_Sum(unsigned char len, unsigned char *buf){
  unsigned char i, sum = 0;
  if(len > 254) return 0;
  for(i = 0; i < len; i++){
    sum += buf[i];
  }
  return  (uint8_t)(sum);
}

unsigned short SemiduplexSerial::ubtServoProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data){
  unsigned short tRet = 0;
  unsigned char tCnt = 0;
  unsigned long temp = 2; //2ms 发完
  unsigned char buf[10];
  unsigned char len = 9; //9+1
  unsigned char Usart3_Rx_Ack_Len=0;
  memset((void *)Usart3_Rx_Buf,0,sizeof(Usart3_Rx_Buf));
  memset((void *)buf,0,sizeof(buf));
  buf[0] = Head;  //填充协议头
  buf[1] = swab8(Head);
  buf[2] = ServoNO; //舵机好
  buf[3] = CMD;
  memcpy((void *)&buf[4],(void *)Data,4);
  buf[len - 1] = Cheak_Sum( (len - 3),(u8*)&buf[2]);
  buf[len] = 0xED;

  if((CMD == 0x01&& Head!=0xFC) || CMD==0x04 ||CMD==0xCD ){
    Usart3_Rx_Ack_Len = 1;  //1,4命令只应答一个字节
  }
  else if(CMD == 0x02 || CMD==0x03){
    Usart3_Rx_Ack_Len = 8;
  }
  else if((CMD == 0x01&& Head==0xFC)){
    Usart3_Rx_Ack_Len = 4;
  }

Retry_Servo:

  temp = (Usart3_Rx_Ack_Len+ 5) ;  //接收消息长度,用于计算接收时间,1个字节 0.087ms,预留5个空闲,10%误差
//3  Serial3.begin(115200);  //uart3

//5  Serial1.setTimeout(temp*87*110/100 / 400);  //设置超时ms

//3  Serial2.begin(115200);  //设置波特率
//3  Serial2.write(buf,len + 1);  //发送消息
//3  Serial2.end();  //关闭串口2,否则会影响接收消息

//  UbtWrite(buf, len+1);
  tRet = UbtWrite(buf, len+1,  Usart3_Rx_Buf, Usart3_Rx_Ack_Len+len);


//  tRet = Serial1.readBytes( Usart3_Rx_Buf, Usart3_Rx_Ack_Len+10); //接收应答

//3  Serial3.end();  //关闭串口3,否则会影响接收消息

  if(tRet == 0) // No message received
  {
    if( tCnt < 2)
    {
      tCnt ++;  // retry
      goto  Retry_Servo;
    }
  }
  else  // Message received
  {

    Usart3_Rx_Buf_count = tRet;
    tRet = 0;
    switch(CMD){
      case 0x01:
        if(Head==0xFC){
          if(Usart3_Rx_Buf[len+1]==0xfc && Usart3_Rx_Buf[len+2]==0xcf && Usart3_Rx_Buf[len+4]==0xaa){
            tRet=Usart3_Rx_Buf[len+3];
          }
          else if((Usart3_Rx_Buf[len+1]!=0xfc && Usart3_Rx_Buf[len+1]!=0) || (Usart3_Rx_Buf[len+2]!=0xcf && Usart3_Rx_Buf[len+2]!=0) || (Usart3_Rx_Buf[len+4]!=0xAA && Usart3_Rx_Buf[len+4]!=0)){
            tRet=ServoNO+0xec;
          }
          else{
            tRet=0;
          }
        }
        else{
          tRet=Usart3_Rx_Buf[len+1]-0xAA-ServoNO;
        }

        break;
      case 0x02:
        tRet=(Usart3_Rx_Buf[len+7]<<8) |(Usart3_Rx_Buf[len+8] & 0xff);
        break;
      case 0x03:
        tRet=(Usart3_Rx_Buf[len+7]<<8) |(Usart3_Rx_Buf[len+8] & 0xff);
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
      unsigned char buf[10];
      unsigned char len = 9; //9+1
      unsigned char Usart3_Rx_Ack_Len = 11;

      // clear buffers
      memset((void *)Usart3_Rx_Buf,0,sizeof(Usart3_Rx_Buf));
      memset((void *)buf,0,sizeof(buf));

      // setup tx message
      buf[0] = Head;  //填充协议头
      buf[1] = swab8(Head);
      buf[2] = ServoNO; //舵机好
      buf[3] = CMD;
      memcpy((void *)&buf[4],(void *)Data,4);
      buf[len - 1] = Cheak_Sum( (len - 3),(u8*)&buf[2]);
      buf[len] = 0xED;

      tRet = UbtWrite(buf, len+1, Usart3_Rx_Buf, Usart3_Rx_Ack_Len+len);

      Serial.printf("ubtServoIdProtocol %d\n", len);
      Serial.println(Cheak_Sum( (len - 3),(u8*)&Usart3_Rx_Buf[len+3]),HEX);
      Serial.println(Usart3_Rx_Buf[len+9],HEX);

      if (  Usart3_Rx_Buf[len+1]==0xFC &&
            Usart3_Rx_Buf[len+2]==0xCF &&
            Usart3_Rx_Buf[len+4]==0xAA &&
            Cheak_Sum( (len - 3),(u8*)&Usart3_Rx_Buf[len+3]) == Usart3_Rx_Buf[len+9]) {
         // servo responded, return ID from answer
         tRet=Usart3_Rx_Buf[len+3];
      } else if(  Usart3_Rx_Buf[len+1]==0 &&
                  Usart3_Rx_Buf[len+2]==0 &&
                  Usart3_Rx_Buf[len+3]==0 &&
                  Usart3_Rx_Buf[len+4]==0 &&
                  Usart3_Rx_Buf[len+9]==0) {
         // no response
         tRet=0;
      } else {
         tRet=ServoNO+0xec;
         CSerial.printf("error?!? %d\n", tRet);
      }
      return tRet;
   }

void SemiduplexSerial::ubtServoActionProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data){
  unsigned char tRet = 0;
  unsigned char buf[10]={0};
  const unsigned char len = 9; //9+1
  const unsigned char Usart3_Rx_Ack_Len=1;
  unsigned char Rx_Buf[11]={0};
  buf[0] = Head;  //填充协议头
  buf[1] = swab8(Head);
  buf[2] = ServoNO; //舵机好
  buf[3] = CMD;
  memcpy((void *)&buf[4],(void *)Data,4);
  buf[len - 1] = Cheak_Sum( (len - 3),(u8*)&buf[2]);
  buf[len] = 0xED;

  UbtWrite(buf, len+1,  Usart3_Rx_Buf, 0);
}

unsigned short SemiduplexSerial::ubtServoProtocol1M(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data){
  unsigned short tRet = 0;
  unsigned char tCnt = 0;
  unsigned long temp = 2; //2ms 发完
  unsigned char buf[10];
  unsigned char len = 9; //9+1
  unsigned char Usart3_Rx_Ack_Len=0;
  memset((void *)Usart3_Rx_Buf,0,sizeof(Usart3_Rx_Buf));
  memset((void *)buf,0,sizeof(buf));
  buf[0] = Head;  //填充协议头
  buf[1] = swab8(Head);
  buf[2] = ServoNO; //舵机好
  buf[3] = CMD;
  memcpy((void *)&buf[4],(void *)Data,4);
  buf[len - 1] = Cheak_Sum( (len - 3),(u8*)&buf[2]);
  buf[len] = 0xED;

  if((CMD == 0x01&& Head!=0xFC) || CMD==0x04 ||CMD==0xCD ){
    Usart3_Rx_Ack_Len = 1;  //1,4命令只应答一个字节
  }
  else if(CMD == 0x02 || CMD==0x03){
    Usart3_Rx_Ack_Len = 8;
  }
  else if((CMD == 0x01&& Head==0xFC)){
    Usart3_Rx_Ack_Len = 4;
  }

Retry_Servo:

  temp = (Usart3_Rx_Ack_Len+ 5) ;  //接收消息长度,用于计算接收时间,1个字节 0.087ms,预留5个空闲,10%误差
//3  Serial3.begin(1000000);  //uart3
//5  Serial3.setTimeout(temp*87*110/100 / 400);  //设置超时ms
//3  Serial2.begin(1000000);  //设置波特率
//3  Serial2.write(buf,len + 1);  //发送消息
//3  Serial2.end();  //关闭串口2,否则会影响接收消息
//3  tRet = Serial3.readBytes( Usart3_Rx_Buf, Usart3_Rx_Ack_Len+10); //接收应答
//3  Serial3.end();  //关闭串口3,否则会影响接收消息

  if(tRet == 0) //没有接收到消息
  {
    if( tCnt < 2)
    {
      tCnt ++;  //重试
      goto  Retry_Servo;
    }
  }
  else  //接收到消息
  {

    Usart3_Rx_Buf_count = tRet;
    tRet = 0;
    switch(CMD){
      case 0x01:
        if(Head==0xFC){
          tRet=Usart3_Rx_Buf[len+3];
        }
        else{
          tRet=Usart3_Rx_Buf[len+1]-0xAA-ServoNO;
        }

        break;
      case 0x02:
        tRet=(Usart3_Rx_Buf[len+7]<<8) |(Usart3_Rx_Buf[len+8] & 0xff);
        break;
      case 0x03:
        tRet=(Usart3_Rx_Buf[len+7]<<8) |(Usart3_Rx_Buf[len+8] & 0xff);
        break;

    }




  }
  return tRet;
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
  unsigned long temp = 2; //2ms 发完
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
  buf[length - 1] = Cheak_Sum( (length - 3),(u8*)&buf[2]);
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

  temp = (Usart3_Rx_Ack_Len + 5) ;  // The length of the received message, used to calculate the receiving time, 1 byte 0.087ms, reserved 5 free, 10% error
//3  Serial3.begin(115200);  //uart3
//5  Serial1.setTimeout(temp*87*110/100 / 400);  // Set timeout ms


//  Serial2.begin(115200);  // Set the baud rate
//  Serial2.write(buf,length + 1);  // Send a message
//  Serial2.end();  // Close the serial port 2, otherwise it will affect the reception of messages

//   UbtWrite(buf, length+1);
//  tRet = Serial1.readBytes( Usart3_Rx_Buf, Usart3_Rx_Ack_Len+10); //Receive reply

  tRet = UbtWrite(buf, length+1,  Usart3_Rx_Buf, Usart3_Rx_Ack_Len+10);

//3  Serial3.end();  // Close serial port 3, otherwise it will affect receiving messages

//  if(tRet > RXD_OFFSET) // Print return message
//  {
//      Serial.write(Usart3_Rx_Buf + RXD_OFFSET,tRet - RXD_OFFSET);
//      Serial.print("\r\n");
//  }

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
      else if(CMD == 0x03 )
      {
        tRet = Usart3_Rx_Buf[length + 5] - 0xAA;
      }
      else if(CMD == 0x04 )
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
      else if(CMD==0x06|CMD==0x07)
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

/**@brief EN:Communication protocol sending and receiving functions/CN:检查舵机是否存在。
 *
 * @param[in] Head EN:servo id/CN:舵机号.
 *
 * @returns tRet EN:Accept return value/CN:接受返回值.
 */
unsigned char SemiduplexSerial::Get_Servo_list(unsigned char Servo_NO){
  return(Read_bit(gServo_list[Servo_NO / 8],Servo_NO % 8 ));
}

void SemiduplexSerial::Set_Servo_list(unsigned char Servo_NO){
  Set_bit(gServo_list[Servo_NO / 8],Servo_NO % 8 );
}

void SemiduplexSerial::Clr_Servo_list(unsigned char Servo_NO){
  Clr_bit(gServo_list[Servo_NO / 8],Servo_NO % 8 );
}

/**@brief EN:Check the number of the servo/CN:检查舵机个数
 */
void SemiduplexSerial::check_servo(){// Take two seconds
  unsigned char tID = 1;
  unsigned char tData[4] = {0,0,0,0};

  memset((void*)gServo_list,0,sizeof(gServo_list));
  gServos = 0;
  TXD(0xFC,tID,4,0x01,tData ); //预先测试一下
  for(tID = 1; tID < SERVO_NUMER_MAX + 1; tID++)
  {
    if( TXD(0xFC,tID,4,0x01,tData ) != 0 )
    {
      Set_Servo_list(tID - 1);  // Set the list of servos, and use multiple servos later
      gServos++;
    }
    else
    {
      Clr_Servo_list(tID - 1);
    }
  }
  Serial.print("\tTotal Servo:");
  Serial.print(gServos, DEC);
  Serial.print("\r\n");
}


