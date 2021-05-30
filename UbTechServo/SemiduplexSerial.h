

#ifndef SEMIDUPLEXSERIAL_h
#define SEMIDUPLEXSERIAL_h

#include <Arduino.h>

class SemiduplexSerial
{
public:

    unsigned short   ubtServoProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data);
    unsigned char    ubtServoIdProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data);
   unsigned char  ubtServoActionProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data);

   unsigned char  CheckSum(unsigned char len, unsigned char *buf);
    unsigned long TXD(unsigned char Head,unsigned char ServoNO,unsigned char len,unsigned char CMD,unsigned char * Data);

    signed long TXD(unsigned char len,unsigned char choice,unsigned char * Data);

    unsigned char Get_Servo_list(unsigned char Servo_NO);  //Check whether the servo exists? Is it normal?
    void Set_Servo_list(unsigned char Servo_NO);
    void Clr_Servo_list(unsigned char Servo_NO);
    void check_servo();

    unsigned char redvalue,greenvalue,bluevalue=0;

private:
   #ifndef swap8
   #define swap8(x) ((x&0x0f) << 4 | (x&0xf0) >> 4)
   #endif

   #define RXD_OFFSET  10  // Receive message offset
   
   void TrxSetup(unsigned char Head,unsigned char ServoNO,unsigned char CMD);

   unsigned char RxBuf[120];
   unsigned char TxBuf[10];
   unsigned char RxBuf_count=0;
};
#endif
