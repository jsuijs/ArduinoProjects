

#ifndef UKITSERVO_h
#define UKITSERVO_h
#include"SemiduplexSerial.h"
//#include"uKitId.h"
#include <Arduino.h>
class uKitServo : public SemiduplexSerial
{
public:


   // from uKitId
    unsigned char setServoId(char oldid,char newid);
    unsigned char getServoId();
    unsigned char getServoId(char id);

    unsigned char setServoId1M(char oldid,char newid);
    unsigned char getServoId1M();
    unsigned char getServoId1M(char id);
   // uKitId end


  void setServoTurn(unsigned char id,int dir, int speed);//舵机轮模式控制，id是舵机号，dir是方向（0顺时针，1逆时针），speed是速度（0-255）
  void setServoTurns(unsigned char *id,int *dir, int *speed);//舵机轮模式控制，id是舵机号，dir是方向（0顺时针，1逆时针），speed是速度（0-255）
  void setServoStiffness(unsigned char id,unsigned char stiffness);
  void setServoAngle(unsigned char id,int angle, int times);//舵机舵机模式，id是舵机号，angle是角度（-118°~118°），times是运行时间（300-5000）
  void setServoAngles(unsigned char *id, int *angle,int times);

  void setServoStop(unsigned char id);                         // Single servo stop function
  int  readServoAnglePD(unsigned char id);                     // Single servo read back, return the servo angle value (return after power off)
  void readServoAnglePD_M(unsigned char *read_id,char num);    // Read back multiple servos, return the angle value of the servos (read back after power failure)
  int  readServoAngleNPD(unsigned char id);                    // Single servo read back, return the servo angle value (read back without power down)
  void readServoAngleNPD_M(unsigned char *read_id,char num);   // Read back multiple servos, return the angle value of the servos (read back without power failure)
  void ServoRead();

  void playMotion(unsigned char *id,signed char **action,int *times);


  void setServoTurn1M(unsigned char id,int dir, int speed);//舵机轮模式控制，id是舵机号，dir是方向（1顺时针，0逆时针），speed是速度（0-5）
  void setServoStiffness1M(unsigned char id,unsigned char stiffness);
  void setServoAngle1M(unsigned char id,int angle, int times);//舵机舵机模式，id是舵机号，angle是角度（-118°~118°），times是运行时间（300-5000）

  void setServoStop1M(unsigned char id);//单个舵机停止函数
  int  readServoAnglePD1M(unsigned char id);//单个舵机回读，返回舵机角度值(掉电回读）
  int  readServoAngleNPD1M(unsigned char id);//单个舵机回读，返回舵机角度值(不掉电回读）
  void playMotion1M(unsigned char *id,signed char **action,int *times);
  int read_num=1;
};

#endif
