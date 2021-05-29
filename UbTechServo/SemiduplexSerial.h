

#ifndef SEMIDUPLEXSERIAL_h
#define SEMIDUPLEXSERIAL_h

#include <Arduino.h>
#define VISION_SERIAL_BUFSIZE     256
#define VISION_SOFTSERIAL_RXPIN   64
#define VISION_SOFTSERIAL_TXPIN   63

class SemiduplexSerial
{
public:

    unsigned short   ubtServoProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data);
    unsigned char    ubtServoIdProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data);
    void             ubtServoActionProtocol(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data);
    unsigned short ubtServoProtocol1M(unsigned char Head,unsigned char ServoNO,unsigned char CMD,unsigned char * Data);

    unsigned char Cheak_Sum(unsigned char len, unsigned char *buf);
    unsigned long TXD(unsigned char Head,unsigned char ServoNO,unsigned char len,unsigned char CMD,unsigned char * Data);

    signed long TXD(unsigned char len,unsigned char choice,unsigned char * Data);

    unsigned char Get_Servo_list(unsigned char Servo_NO);  //Check whether the servo exists? Is it normal?
    void Set_Servo_list(unsigned char Servo_NO);
    void Clr_Servo_list(unsigned char Servo_NO);
    void check_servo();

    unsigned char redvalue,greenvalue,bluevalue=0;

private:
  #define tems(val) (val*87*110/100/400)
  #define POLY    (0x1070U << 3)
  #define SERVO_NUMER_MAX    16  // Most servos supported
  // Set a bit of the variable
  #define  Clr_bit(val, bit)  (val &= ~(1 << bit))
  // Clear a bit of a variable
  #define  Set_bit(val, bit)  (val |= (1 << bit))
  // Read a bit of a variable
  #define RXD_OFFSET  10  //Receive message offset
  //#define  Read_bit(val, bit)  (val & (1 << bit))  // The result is a problem with the calculation
  #define  Read_bit(val, bit)  ((val & (1 << bit)) ? 1 : 0)
  #ifndef NULL
  #define  NULL         ((void *) 0)
  #endif

  #ifndef swab16
  #define swab16(x) ((x&0x00ff) << 8 | (x&0xff00) >> 8)
  #endif

  #ifndef swab8
  #define swab8(x) ((x&0x0f) << 4 | (x&0xf0) >> 4)
  #endif

  #ifndef swab32
  #define swab32(x) (((x << 8)&0x00ff0000)|((x >> 8)&0x0000ff00)|((x << 24)&0xff000000) | ((x >> 24)&0x000000ff))
  #endif

  typedef  struct _ANGLE_PACK_V002_Struct_ // The structure sequence is not adjustable, you can add V0.02 at the back
  {
    unsigned  char Angle[SERVO_NUMER_MAX];  //1~20 Steering gear angle
    unsigned  short Run_time16; // Run time, resolution ms
  }ANGLE_PACK_V002;

  unsigned char gServos = SERVO_NUMER_MAX;  // Number of servos
  unsigned char gServo_list[SERVO_NUMER_MAX / 8]; // Servo list, bit representation
  unsigned char Usart3_Rx_Buf[120];
  unsigned char Usart3_Rx_Buf_count=0;
  volatile ANGLE_PACK_V002 gsSave_Angle;    // Save the angle, the same angle will not be issued

};
#endif
