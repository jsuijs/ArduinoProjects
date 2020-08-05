#include "project.h"
#include <HardwareTimer.h>

HardwareTimer Timer2(TIM2);
HardwareTimer Timer4(TIM4);

//https://community.st.com/s/question/0D50X00009XkfbN/quadrature-encoder
//STM32Cube_FW_F4_V1.16.0\Projects\STM32469I_EVAL\Examples\TIM\TIM_Encoder\Src\main.c

//-----------------------------------------------------------------------------
// EncodersInit -
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

   // return difference (temp value in short int to handle wrap arount properly)
   short int L = LeftEncoder  - PrevLeftEncoder;
   short int R = RightEncoder - PrevRightEncoder;
   Left  = L;
   Right = R;
}
