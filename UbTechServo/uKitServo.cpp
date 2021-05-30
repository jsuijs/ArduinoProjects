
#include <Arduino.h>
extern HardwareSerial Serial2;
#define Serial Serial2

#include"uKitServo.h"

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
// uKitServo::getServoId - Return Id of lowest servo
//-----------------------------------------------------------------------------
// Return: Id on success, 0 on failure
//-----------------------------------------------------------------------------
unsigned char uKitServo::getServoId()
   {
      for (int testid=1; testid<=18; testid++) {

         if (getServoId(testid) == testid) return testid;
      }
      return 0;
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

// Servo geeft antwoord maar geen actie??
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

//void uKitServo::setServoTurns(unsigned char *id,int *dir, int *speed){
//  for(int i=0;i<sizeof(id)/sizeof(id[0]);i++){
//    setServoTurn(id[i],dir[i],speed[i]);
//  }
//}

//-----------------------------------------------------------------------------
// uKitServo::setServoAngle -
//-----------------------------------------------------------------------------
// id represents the number of the servo,
// angle represents the angle (angle range -118°~118°),
// time represents the time required for rotation (time range: 300~5000)
//-----------------------------------------------------------------------------
void uKitServo::setServoAngle(unsigned char id,int angle,int times){
  unsigned char buf[4];
  buf[0]=angle+120;
  buf[1]=(times/20);
  buf[2]=((times/20) & 0xFF00) >> 8;
  buf[3]=(times/20) & 0x00FF;

  ubtServoActionProtocol(0xFA,id,0x01,buf);
}

//void uKitServo::setServoAngles(unsigned char *id,int *angle,int times){
//    for(unsigned int i=0;i<sizeof(id)/sizeof(id[0]);i++){
//      setServoAngle(id[i],angle[i],times);
//    }
//    delay(times);
//}

void uKitServo::setServoStop(unsigned char id){
  unsigned char buf[4]={0xFF,0,0,0};
  ubtServoProtocol(0xFA,id,0x01,buf);
}

//-----------------------------------------------------------------------------
// uKitServo::setServoStiffness - ?? 255 = off?  only 4 bytes (of 8) defined...
//-----------------------------------------------------------------------------
// stiffness inequal to 255 enables power and might turn servo to a
// new position...
//-----------------------------------------------------------------------------
void uKitServo::setServoStiffness(unsigned char id,unsigned char stiffness){
  unsigned char tData[8];  // JS: was 4!!
  tData[0]=stiffness;
  tData[1]=0;
  tData[2]=0;
  tData[3]=0;

  TXD(0xFA,id,8,0x01,tData ); // param3 must be 8 -> send 8 bytes.
}

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

      tCmd=tRet-120;
      if (tCmd>=-120 && tCmd<=120) return tCmd;

      return 0;
   }

//void uKitServo::readServoAnglePD_M(unsigned char *read_id,char num)//舵机回读
//{
//    int setServoAngle=0;
//    Serial.println("");
//    Serial.print("----------");
//    Serial.print(read_num++);
//    Serial.println("----------");
//    Serial.print("{");
//    for(int i=0;i<num;i++)
//    {
//        setServoAngle=readServoAnglePD(read_id[i]);
//        delay(20);
//        if(setServoAngle>=-118 && setServoAngle<=118)
//        {
//          Serial.print(setServoAngle);
//          if(i<num-1)
//            Serial.print(",");
//        }
//
//        else
//        {
//          Serial.print("id-");
//          Serial.print(setServoAngle);
//          Serial.print(":Out of range,");
//        }
//    }
//      Serial.print("}");
//}

// uKitServo::readServoAngleNPD - no response from servo...
int uKitServo::readServoAngleNPD(unsigned char id){ // Single servo read back (read back without power down)
  int tCmd=0,tRet=0;
  unsigned char aa[4]={0,0,0,0};
  tRet=ubtServoProtocol(0xFA,id,0x03,aa);
  if(tRet==0){
    tCmd=0;
  }
  else if(tRet==1){
    tCmd=-120;
  }
  else{
    tCmd=tRet-120;
  }

  delay(5);

  if(tCmd>=-120 && tCmd<=120)
    return tCmd;
  else
    return 0;

}

//void uKitServo::readServoAngleNPD_M(unsigned char *read_id,char num)//舵机回读
//{
//    int setServoAngle=0;
//    Serial.println("");
//    Serial.print("----------");
//    Serial.print(read_num++);
//    Serial.println("----------");
//    Serial.print("{");
//    for(int i=0;i<num;i++)
//    {
//        setServoAngle=readServoAngleNPD(read_id[i]);
//        delay(20);
//        if(setServoAngle>=-118 && setServoAngle<=118)
//        {
//          Serial.print(setServoAngle);
//          if(i<num-1)
//            Serial.print(",");
//        }
//
//        else
//        {
//          Serial.print("id-");
//          Serial.print(setServoAngle);
//          Serial.print(":Out of range,");
//        }
//    }
//      Serial.print("}");
//}

void uKitServo::ServoRead(){
  unsigned char t=0;
  static unsigned char ServoId[18]={0},ServoIdRead[18]={0};
  static int start=0;
  if(start==0){
  Serial.print("Servo currently read ID：{");
  for(int i=1;i<=18;i++){
    ServoId[i]=getServoId(i);
    if(ServoId[i]!=0){
      ServoIdRead[t]=ServoId[i];
      Serial.print(ServoIdRead[t]);
      Serial.print(",");
      t++;
    }
  }
  Serial.println("}");
  start=1;
  }
  Serial.println("dit nog herstellen?!?"); //  readServoAnglePD_M(ServoIdRead,t);
}

//void uKitServo::playMotion(unsigned char *id,signed char **action,int *times){
//  for(int i=0;i<sizeof(action)/sizeof(action[0]);i++){
//    for(int t=0;t<sizeof(id)/sizeof(id[0]);t++){
//      setServoAngle(id[t],action[i][t],500);
//    }
//      delay(times[i]);
//    }
//}




















