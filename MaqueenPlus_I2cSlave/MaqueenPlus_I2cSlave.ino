//-----------------------------------------------------------------------------
// MaqueenPlus_I2cSlave - alternative firmware for Maqueen Plus.
//-----------------------------------------------------------------------------
// Note: I2C interface is on same address as original firmware,
//       but this firmware has different register assignment
//       and features.
//       Hence *NOT* compatible with original firmware!
//-----------------------------------------------------------------------------

// Create the robot
#include "MaqueenPlus.h"
TMaqueenPlus Robot;

//-----------------------------------------------------------------------------
// I2c registers:
#define R_ID            0X00  // fixed number IDENTIFICATION (read only)
#define R_BUTTON        0X01  // status of calibration switch
#define R_ENC_L_H       0X02  // Encoders
#define R_ENC_L_L       0X03
#define R_ENC_R_H       0X04
#define R_ENC_R_L       0X05

#define R_PWM_L_H       0X06   // pwm values (output)
#define R_PWM_L_L       0X07
#define R_PWM_R_H       0X08
#define R_PWM_R_L       0X09

#define R_RGB_LEDS      0X0A  // 3 bits in each nibble for L, R
#define R_LINE_LEDS     0X0B  // lower 6 bits, one for each led near linesensor - 0x80 shows LINE_BITS
#define R_LINE_BITS     0X0C  // lower 6 bits, one for each linesensor

#define R_SENSOR_L3_H   0X10  // analog value of linesensor
#define R_SENSOR_L3_L   0X11
#define R_SENSOR_L2_H   0X12
#define R_SENSOR_L2_L   0X13
#define R_SENSOR_L1_H   0X14
#define R_SENSOR_L1_L   0X15
#define R_SENSOR_R1_H   0X16
#define R_SENSOR_R1_L   0X17
#define R_SENSOR_R2_H   0X18
#define R_SENSOR_R2_L   0X19
#define R_SENSOR_R3_H   0X1A
#define R_SENSOR_R3_L   0X1B

#define R_SETPOINT_L3_H 0X20  // threshold for LINE_BITS, a value for each linesensor
#define R_SETPOINT_L3_L 0X21
#define R_SETPOINT_L2_H 0X22
#define R_SETPOINT_L2_L 0X23
#define R_SETPOINT_L1_H 0X24
#define R_SETPOINT_L1_L 0X25
#define R_SETPOINT_R1_H 0X26
#define R_SETPOINT_R1_L 0X27
#define R_SETPOINT_R2_H 0X28
#define R_SETPOINT_R2_L 0X29
#define R_SETPOINT_R3_H 0X2A
#define R_SETPOINT_R3_L 0X2B

#define REG_MAP_SIZE   (0X30)

// Create 2nd wire port & tell I2cSlaveRegisters.h to use this one
#include <Wire.h>
TwoWire Wire2(PB11, PB10);
#define SLAVE_WIRE Wire2

#define SLAVE_ADDRESS   0x20/2   // slave address
#define IDENTIFICATION  0xFB     // read-only value of R_ID (register 0)

#include "I2cSlaveRegisters.h"

//-----------------------------------------------------------------------------
// MyRequestEvent - called when i2c master requests data
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MyRequestEvent()
{
   // Update encoder values in I2cRegisters.
   int RawLeft, RawRight;
   Robot.EncodersReadRaw(RawLeft, RawRight); // get raw encoder values
   RegisterSetWord(R_ENC_L_H, RawLeft);
   RegisterSetWord(R_ENC_R_H, RawRight);

   // call requestEvent of I2cSlaveRegister.h for further processing
   requestEvent();
}

//-----------------------------------------------------------------------------
// setup -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void setup()
{
   delay(100);  // allow IDE to activate terminal after programming...
   Serial.begin(115200);
   printf("Maqueen-plus ready.\n");

   I2cSlaveRegistersInit(SLAVE_ADDRESS);
   SLAVE_WIRE.onRequest(MyRequestEvent); // override I2cSlaveRegister default

   // Get initial register values from Robot class.
   RegisterSetWord(R_SETPOINT_L3_H, Robot.LineThreshold[0]);
   RegisterSetWord(R_SETPOINT_L2_H, Robot.LineThreshold[1]);
   RegisterSetWord(R_SETPOINT_L1_H, Robot.LineThreshold[2]);
   RegisterSetWord(R_SETPOINT_R1_H, Robot.LineThreshold[3]);
   RegisterSetWord(R_SETPOINT_R2_H, Robot.LineThreshold[4]);
   RegisterSetWord(R_SETPOINT_R3_H, Robot.LineThreshold[5]);

   RegisterSetByte(R_LINE_LEDS, 0x80); // Default : leds show linebit values
}

//-----------------------------------------------------------------------------
// loop -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void loop()
{  static int NextMainTakt;

   // Main takt interval
   int ms = millis();
   if ((ms - NextMainTakt) > 0) {
      NextMainTakt = ms + 50;  // set time for next interval

      // -----------------------------------
      // Note: encoder data is captured when
      //       requested, not in this loop.
      // -----------------------------------

      // get bits & analog values of linesensors
      RegisterSetByte(R_LINE_BITS, Robot.LineSensorBits());
      RegisterSetWord(R_SENSOR_L3_H, Robot.LineSensorRead(0));
      RegisterSetWord(R_SENSOR_L2_H, Robot.LineSensorRead(1));
      RegisterSetWord(R_SENSOR_L1_H, Robot.LineSensorRead(2));
      RegisterSetWord(R_SENSOR_R1_H, Robot.LineSensorRead(3));
      RegisterSetWord(R_SENSOR_R2_H, Robot.LineSensorRead(4));
      RegisterSetWord(R_SENSOR_R3_H, Robot.LineSensorRead(5));

      // copy button status
      RegisterSetByte(R_BUTTON, Robot.CalcKey());

      // Pass high nibble to Left RGB led and low nibble to Right one.
      int RgbLeds = RegisterGetByte(R_RGB_LEDS);
      Robot.RgbLeds(((RgbLeds >> 4) & 0xF), (RgbLeds & 0xF));

      int LineLeds = RegisterGetByte(R_LINE_LEDS);
      if (LineLeds < 0x80) {
         // if high bit of R_LINE_LEDS is low, lower 6 bits specify LED values
         Robot.SetLineSensorLeds(LineLeds);
      } else {
         // If high bit of R_LINE_LEDS is set, linesensor bit-values are displayed.
         Robot.SetLineSensorLeds(Robot.LineSensorBits());
      }
   }

   if (I2cNewDataAvailable()) {

      // update PWM values
      int PwmL = RegisterGetWord(R_PWM_L_H);
      int PwmR = RegisterGetWord(R_PWM_R_H);
      Robot.Motors(PwmL, PwmR);

      // update linsensor thresholds
      Robot.LineThreshold[0] = RegisterGetWord(R_SETPOINT_L3_H);
      Robot.LineThreshold[1] = RegisterGetWord(R_SETPOINT_L2_H);
      Robot.LineThreshold[2] = RegisterGetWord(R_SETPOINT_L1_H);
      Robot.LineThreshold[3] = RegisterGetWord(R_SETPOINT_R1_H);
      Robot.LineThreshold[4] = RegisterGetWord(R_SETPOINT_R2_H);
      Robot.LineThreshold[5] = RegisterGetWord(R_SETPOINT_R3_H);
   }
}
