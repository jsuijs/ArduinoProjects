#include "project.h"
#include <HardwareTimer.h>

HardwareTimer Timer2(TIM2);
HardwareTimer Timer4(TIM4);

int EncoderSpeedL, EncoderSpeedR;           // in ticks/10sec, shared, sanitized result, updated by EncoderRead
static volatile int EncTimeL, EncTimeR;   // raw data
#define ENCODER_LEFT  PA0
#define ENCODER_RIGHT PB6

static void IsrEdgeLeft()
{  static int LastTime;
   int Now = micros();
   EncTimeL = Now - LastTime;
   LastTime = Now;
}

static void IsrEdgeRight()
{  static int LastTime;
   int Now = micros();
   EncTimeR = Now - LastTime;
   LastTime = Now;
}

//https://community.st.com/s/question/0D50X00009XkfbN/quadrature-encoder
//STM32Cube_FW_F4_V1.16.0\Projects\STM32469I_EVAL\Examples\TIM\TIM_Encoder\Src\main.c

//-----------------------------------------------------------------------------
// EncodersInit - init hardware quadrature encoders & corresponding IO
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void EncodersInit()
{
   pinMode(PA0, INPUT_PULLUP);
   pinMode(PA1, INPUT_PULLUP);
   pinMode(PB6, INPUT_PULLUP);
   pinMode(PB7, INPUT_PULLUP);

   // setup hardware quadrature encoders (requires STM32Cube functions)
   TIM_HandleTypeDef    Encoder_Handle;
   TIM_Encoder_InitTypeDef sEncoderConfig;

   /* -1- Initialize TIM1 to handle the encoder sensor */
   /* Initialize TIM1 peripheral as follow:
       + Period = 65535
       + Prescaler = 0
       + ClockDivision = 0
       + Counter direction = Up
   */

   Encoder_Handle.Init.Period             = 65535;
   Encoder_Handle.Init.Prescaler          = 0;
   Encoder_Handle.Init.ClockDivision      = 0;
   Encoder_Handle.Init.CounterMode        = TIM_COUNTERMODE_UP;
   Encoder_Handle.Init.RepetitionCounter  = 0;
   Encoder_Handle.Init.AutoReloadPreload  = TIM_AUTORELOAD_PRELOAD_DISABLE;

   sEncoderConfig.EncoderMode             = TIM_ENCODERMODE_TI12;

   sEncoderConfig.IC1Polarity             = TIM_ICPOLARITY_RISING;
   sEncoderConfig.IC1Selection            = TIM_ICSELECTION_DIRECTTI;
   sEncoderConfig.IC1Prescaler            = TIM_ICPSC_DIV1;
   sEncoderConfig.IC1Filter               = 0;

   sEncoderConfig.IC2Polarity             = TIM_ICPOLARITY_RISING;
   sEncoderConfig.IC2Selection            = TIM_ICSELECTION_DIRECTTI;
   sEncoderConfig.IC2Prescaler            = TIM_ICPSC_DIV1;
   sEncoderConfig.IC2Filter               = 0;

   Encoder_Handle.Instance = TIM2;
   if(HAL_TIM_Encoder_Init(&Encoder_Handle, &sEncoderConfig) != HAL_OK) Serial.println("TIM2 init error");
   HAL_TIM_Encoder_Start(&Encoder_Handle, TIM_CHANNEL_ALL);

   Encoder_Handle.Instance = TIM4;
   if(HAL_TIM_Encoder_Init(&Encoder_Handle, &sEncoderConfig) != HAL_OK) Serial.println("TIM4 init error");
   HAL_TIM_Encoder_Start(&Encoder_Handle, TIM_CHANNEL_ALL);

   // Measure time between edges to get high resolution speed info.
   // Note: single edge, since encoder duty cycle is not exactly 50%
   attachInterrupt(ENCODER_LEFT,  IsrEdgeLeft, RISING);
   attachInterrupt(ENCODER_RIGHT, IsrEdgeRight, RISING);
}

//-----------------------------------------------------------------------------
// EncodersRead - read encoder and put *DELTA* in vars.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void EncodersRead (int &Left, int &Right)
{  static short int LeftEncoder, RightEncoder;

   // save prev values
   short int PrevLeftEncoder  = LeftEncoder;
   short int PrevRightEncoder = RightEncoder;

   // read encoders
   LeftEncoder    = Timer2.getCount(); // flip sign here if required
   RightEncoder   = Timer4.getCount(); // flip sign here if required

   // return difference (temp value in short int to handle wrap around properly)
   short int L = LeftEncoder  - PrevLeftEncoder;
   short int R = RightEncoder - PrevRightEncoder;
   Left  = L;
   Right = R;

   // get & sanitize time measurements
   int TimeL, TimeR;
   noInterrupts();
   TimeL = EncTimeL;
   TimeR = EncTimeR;
   EncTimeL = EncTimeR = 0;
   interrupts();

   if (TimeL > 0) {
      EncoderSpeedL = (4 * 10 * 1000 * 1000) / TimeL;    // 4 ticks, 10 secs, sec->usec
      int TickSpeedL = Left * 10000 / CfgLoopTime;       // 10.000 msecs
      if (Left < 0) TickSpeedL = -TickSpeedL;            // ABS speed for now

      // Sanitize:
      // * cope with 'edge lag', especially when speed is reduced.
      // * cope with spurious encoder pin edges
      if (EncoderSpeedL > (TickSpeedL * 1.5)) EncoderSpeedL = TickSpeedL;

      if (Left < 0) EncoderSpeedL = -EncoderSpeedL;            // set speed sign
   } else {
      EncoderSpeedL = 0;
   }

   if (TimeR > 0) {
      EncoderSpeedR = (4 * 10 * 1000 * 1000) / TimeR;    // 4 ticks, 10 secs, sec->usec
      int TickSpeedR = Right * 10000 / CfgLoopTime;      // 10.000 msecs
      if (Right < 0) TickSpeedR = -TickSpeedR;           // ABS speed for now

      // Sanitize:
      // * cope with 'edge lag', especially when speed is reduced.
      // * cope with spurious encoder pin edges
      if (EncoderSpeedR > (TickSpeedR * 1.5)) EncoderSpeedR = TickSpeedR;

      if (Right < 0) EncoderSpeedR = -EncoderSpeedR;           // Set speed sign
   } else {
      EncoderSpeedR = 0;
   }
}
