//-----------------------------------------------------------------------------
// MaqueenPlus.h - (c) 2020 Joep Suijs
//-----------------------------------------------------------------------------
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MaqueenPlus hardware definitions
//-----------------------------------------------------------------------------
#define MAQUEENPLUS_PIN_SENSOR_L3      PB1
#define MAQUEENPLUS_PIN_SENSOR_L2      PB0
#define MAQUEENPLUS_PIN_SENSOR_L1      PA5
#define MAQUEENPLUS_PIN_SENSOR_R1      PA4
#define MAQUEENPLUS_PIN_SENSOR_R2      PA3
#define MAQUEENPLUS_PIN_SENSOR_R3      PA2

#define MAQUEENPLUS_PIN_LED_L3         PB15
#define MAQUEENPLUS_PIN_LED_L2         PB2
#define MAQUEENPLUS_PIN_LED_L1         PC14
#define MAQUEENPLUS_PIN_LED_R1         PC13
#define MAQUEENPLUS_PIN_LED_R2         PA15
#define MAQUEENPLUS_PIN_LED_R3         PB3

#define MAQUEENPLUS_PIN_LED_L_R        PB12
#define MAQUEENPLUS_PIN_LED_L_G        PB13
#define MAQUEENPLUS_PIN_LED_L_B        PB14
#define MAQUEENPLUS_PIN_LED_R_R        PB4
#define MAQUEENPLUS_PIN_LED_R_G        PB5
#define MAQUEENPLUS_PIN_LED_R_B        PB9

#define MAQUEENPLUS_PIN_MOTOR_L_INA    PA9
#define MAQUEENPLUS_PIN_MOTOR_L_INB    PA8
#define MAQUEENPLUS_PIN_MOTOR_R_INA    PA10
#define MAQUEENPLUS_PIN_MOTOR_R_INB    PA11

#define MAQUEENPLUS_PIN_ENCODER_L_A    PA6
#define MAQUEENPLUS_PIN_ENCODER_L_B    PA7
#define MAQUEENPLUS_TIMER_ENCL         TIM3  // 16 bits timer/counter

#define MAQUEENPLUS_PIN_ENCODER_R_A    PA0
#define MAQUEENPLUS_PIN_ENCODER_R_B    PA1
#define MAQUEENPLUS_TIMER_ENCR         TIM2  // 16 bits timer/counter

#define MAQUEENPLUS_PIN_CALC_KEY       PA12

#define MAQUEENPLUS_PIN_I2C_SCL        PB10  // Usart3 tx
#define MAQUEENPLUS_PIN_I2C_SDA        PB11  // Usart3 rx

#define MAQUEENPLUS_PIN_SERVO_S1       PB6   // Usart1 tx (remap)
#define MAQUEENPLUS_PIN_SERVO_S2       PB7   // Usart1 rx (remap)
#define MAQUEENPLUS_PIN_SERVO_S3       PB8

//-----------------------------------------------------------------------------
#include <HardwareTimer.h>

class TMaqueenPlus
{
public:
	TMaqueenPlus();

   void EncodersReadRaw(int &Left, int &Right);
   void EncodersReadDelta(int &Left, int &Right);
   void EncodersRead(int &Left, int &Right);
   void EncodersReset() { EncoderLeft = 0; EncoderRight = 0; }

   int  LineSensorRead(int Nr);
   int  LineSensorBits();
   void SetLineSensorLeds(int Bits);

   bool CalcKey() { return (digitalRead(MAQUEENPLUS_PIN_CALC_KEY) == false); }

   void Motors(int PwmL, int PwmR);
   void Led(int Nr, bool On);

   void RgbLeft(int Color);
   void RgbRight(int Color);
   void RgbLeds(int ColorL, int ColorR);
   void RgbLeds(int Color);

private:
   HardwareTimer TimerEncL, TimerEncR;

   int RawEncoderLeft, RawEncoderRight;   // for EncodersReadDelta
   int EncoderLeft, EncoderRight;         // for EncodersRead
};
//-----------------------------------------------------------------------------

// https://github.com/stm32duino/wiki/wiki/API

//-----------------------------------------------------------------------------
// SystemClock_Config - HSI oscillator, 36 MHz
//-----------------------------------------------------------------------------
// From Core\Src\main.c, generated with CubeMX - override weak function.
// (Don't forget 'extern "C"' prefix)
//-----------------------------------------------------------------------------
extern "C" void SystemClock_Config(void)
{
   #pragma GCC diagnostic ignored "-Wmissing-field-initializers"  // JS: Suppress warning of CubeMX generated code.
   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
   #pragma GCC diagnostic warning "-Wmissing-field-initializers"

   /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
   RCC_OscInitStruct.HSIState = RCC_HSI_ON;
   RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
   RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
      Error_Handler();
   }
   /** Initializes the CPU, AHB and APB buses clocks
   */
   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                               |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
   {
      Error_Handler();
   }
}

//-----------------------------------------------------------------------------
#ifdef MAQUEEN_USE_USART3
   HardwareSerial Serial(USART3);   // Usart3 on default pins (PB11/PB10) - I2C pins

   // Override _write to send printf output to 'Serial' (the one we have chosen) over the default USART1
   extern "C" int _write(int __attribute__ ((unused)) file, char *ptr, int len) { return Serial.write(ptr, len); }
#else
   // Usart1 on remapped pins - Maqueen servo pins S1 & S2
   HardwareSerial Serial(PB7, PB6);
#endif

//-----------------------------------------------------------------------------
// TMaqueenPlus::TMaqueenPlus - Constructor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TMaqueenPlus::TMaqueenPlus() : TimerEncL(MAQUEENPLUS_TIMER_ENCL), TimerEncR(MAQUEENPLUS_TIMER_ENCR)
   {
      //-----------------------------------------------------------------------
      // Init generic IO
      //-----------------------------------------------------------------------
      pinMode(MAQUEENPLUS_PIN_LED_L3 , OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_L3 , true);
      pinMode(MAQUEENPLUS_PIN_LED_L2 , OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_L2 , true);
      pinMode(MAQUEENPLUS_PIN_LED_L1 , OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_L1 , true);
      pinMode(MAQUEENPLUS_PIN_LED_R1 , OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_R1 , true);
      pinMode(MAQUEENPLUS_PIN_LED_R2 , OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_R2 , true);
      pinMode(MAQUEENPLUS_PIN_LED_R3 , OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_R3 , true);

      pinMode(MAQUEENPLUS_PIN_LED_L_R, OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_L_R, true);
      pinMode(MAQUEENPLUS_PIN_LED_L_G, OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_L_G, true);
      pinMode(MAQUEENPLUS_PIN_LED_L_B, OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_L_B, true);
      pinMode(MAQUEENPLUS_PIN_LED_R_R, OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_R_R, true);
      pinMode(MAQUEENPLUS_PIN_LED_R_G, OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_R_G, true);
      pinMode(MAQUEENPLUS_PIN_LED_R_B, OUTPUT);     digitalWrite(MAQUEENPLUS_PIN_LED_R_B, true);

      pinMode(MAQUEENPLUS_PIN_CALC_KEY, INPUT_PULLUP);

      //-----------------------------------------------------------------------
      // Init hardware quadrature encoders & corresponding IO
      //-----------------------------------------------------------------------
      pinMode(MAQUEENPLUS_PIN_ENCODER_L_A, INPUT_PULLUP);
      pinMode(MAQUEENPLUS_PIN_ENCODER_L_B, INPUT_PULLUP);
      pinMode(MAQUEENPLUS_PIN_ENCODER_R_A, INPUT_PULLUP);
      pinMode(MAQUEENPLUS_PIN_ENCODER_R_B, INPUT_PULLUP);

      // setup hardware quadrature encoders (requires STM32Cube functions)
      TIM_HandleTypeDef    Encoder_Handle;
      TIM_Encoder_InitTypeDef sEncoderConfig;

      /* Initialize TIM* peripheral as follow:
          + Period = 65535
          + Prescaler = 0
          + ClockDivision = 0
          + Counter direction = Up

         https://community.st.com/s/question/0D50X00009XkfbN/quadrature-encoder
         STM32Cube_FW_F4_V1.16.0\Projects\STM32469I_EVAL\Examples\TIM\TIM_Encoder\Src\main.c
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

      Encoder_Handle.Instance = MAQUEENPLUS_TIMER_ENCL;
      if(HAL_TIM_Encoder_Init(&Encoder_Handle, &sEncoderConfig) != HAL_OK) Serial.println("TIM2 init error");
      HAL_TIM_Encoder_Start(&Encoder_Handle, TIM_CHANNEL_ALL);

      Encoder_Handle.Instance = MAQUEENPLUS_TIMER_ENCR;
      if(HAL_TIM_Encoder_Init(&Encoder_Handle, &sEncoderConfig) != HAL_OK) Serial.println("TIM3 init error");
      HAL_TIM_Encoder_Start(&Encoder_Handle, TIM_CHANNEL_ALL);

      Motors(0,0);   // setup motor IO
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::Motors -
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TMaqueenPlus::Motors(int PwmL, int PwmR)
   {
      // set PWM & direction pin for LEFT motor
      int Pwm = constrain(PwmL, -255, 255);  // flip direction here if required.
      if (Pwm >=0) {
         analogWrite(MAQUEENPLUS_PIN_MOTOR_L_INA, 255);
         analogWrite(MAQUEENPLUS_PIN_MOTOR_L_INB, 256-Pwm);
      } else {
         analogWrite(MAQUEENPLUS_PIN_MOTOR_L_INA, 256+Pwm);
         analogWrite(MAQUEENPLUS_PIN_MOTOR_L_INB, 255);
      }

      delayMicroseconds(10);

      // set PWM & direction pin for RIGHT motor
      Pwm = constrain(PwmR, -255, 255);;  // flip direction here if required.
      if (Pwm >=0) {
         analogWrite(MAQUEENPLUS_PIN_MOTOR_R_INA, 255);
         analogWrite(MAQUEENPLUS_PIN_MOTOR_R_INB, 256-Pwm);
      } else {
         analogWrite(MAQUEENPLUS_PIN_MOTOR_R_INA, 256+Pwm);
         analogWrite(MAQUEENPLUS_PIN_MOTOR_R_INB, 255);
      }
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::EncodersReadRaw - read encoder & store result in RawEncoder*.
//-----------------------------------------------------------------------------
// There are three EncoderRead functions:
//    EncodersReadRaw()
//    EncodersReadDelta()
//    EncodersRead()
// *** Do not mix *** - choose one and stick to it.
//
// Features:
// * updates RawEncoder* class vars
//
// About 0.15mm / tick
//-----------------------------------------------------------------------------
void TMaqueenPlus::EncodersReadRaw(int &Left, int &Right)
   {
      Left  = -TimerEncL.getCount(); // flip sign here if required
      Right = -TimerEncR.getCount(); // flip sign here if required
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::EncodersReadDelta - read encoder and put *DELTA* in vars.
//-----------------------------------------------------------------------------
// There are three EncoderRead functions:
//    EncodersReadRaw()
//    EncodersReadDelta()
//    EncodersRead()
// *** Do not mix *** - choose one and stick to it.
//
// Features:
// * returns encoder counters deta from previous call
//
// About 0.15mm / tick
//-----------------------------------------------------------------------------
void TMaqueenPlus::EncodersReadDelta(int &Left, int &Right)
   {
      // save prev values
      int PrevEncoderLeft  = RawEncoderLeft;
      int PrevEncoderRight = RawEncoderRight;

      EncodersReadRaw(RawEncoderLeft, RawEncoderRight);   // Store encoder values in RawEncoder*

      // return difference (cast to short int required to properly handle wrap around of 16-bit counters)
      Left  = (short int)(RawEncoderLeft  - PrevEncoderLeft);
      Right = (short int)(RawEncoderRight - PrevEncoderRight);
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::EncodersRead - read encoder and put result in vars.
//-----------------------------------------------------------------------------
// There are three EncoderRead functions:
//    EncodersReadRaw()
//    EncodersReadDelta()
//    EncodersRead()
// *** Do not mix *** - choose one and stick to it.
//
// Features:
// * returns incremental encoder counters
// * emulates 32-bit counters.
// * supports EncodersReset()
//
// About 0.15mm / tick
//-----------------------------------------------------------------------------
void TMaqueenPlus::EncodersRead(int &Left, int &Right)
   {
      int LeftDelta, RightDelta;
      EncodersReadDelta(LeftDelta, RightDelta);

      EncoderLeft  += LeftDelta;
      EncoderRight += RightDelta;

      // assign return-values
      Left  = EncoderLeft;
      Right = EncoderRight;
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::Led - turn Nr On or Off
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TMaqueenPlus::Led(int Nr, bool On)
   {
      switch(Nr) {
         // LineSensor leds, from left to right
         case  0 : digitalWrite(MAQUEENPLUS_PIN_LED_L3 , !On); break;
         case  1 : digitalWrite(MAQUEENPLUS_PIN_LED_L2 , !On); break;
         case  2 : digitalWrite(MAQUEENPLUS_PIN_LED_L1 , !On); break;
         case  3 : digitalWrite(MAQUEENPLUS_PIN_LED_R1 , !On); break;
         case  4 : digitalWrite(MAQUEENPLUS_PIN_LED_R2 , !On); break;
         case  5 : digitalWrite(MAQUEENPLUS_PIN_LED_R3 , !On); break;

         // left RGB led
         case  6 : digitalWrite(MAQUEENPLUS_PIN_LED_L_R, !On); break;
         case  7 : digitalWrite(MAQUEENPLUS_PIN_LED_L_G, !On); break;
         case  8 : digitalWrite(MAQUEENPLUS_PIN_LED_L_B, !On); break;

         // right RGB led
         case  9 : digitalWrite(MAQUEENPLUS_PIN_LED_R_R, !On); break;
         case 10 : digitalWrite(MAQUEENPLUS_PIN_LED_R_G, !On); break;
         case 11 : digitalWrite(MAQUEENPLUS_PIN_LED_R_B, !On); break;

         default : printf("Unknown LED#: %d\n", Nr);           break;
      }
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::RgbLeft - Set color of left RGB led
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TMaqueenPlus::RgbLeft(int Color)
   {
      digitalWrite(MAQUEENPLUS_PIN_LED_L_R, (Color & 1) == 0);
      digitalWrite(MAQUEENPLUS_PIN_LED_L_G, (Color & 2) == 0);
      digitalWrite(MAQUEENPLUS_PIN_LED_L_B, (Color & 4) == 0);
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::RgbRight - Set color of right RGB led
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TMaqueenPlus::RgbRight(int Color)
   {
      digitalWrite(MAQUEENPLUS_PIN_LED_R_R, (Color & 1) == 0);
      digitalWrite(MAQUEENPLUS_PIN_LED_R_G, (Color & 2) == 0);
      digitalWrite(MAQUEENPLUS_PIN_LED_R_B, (Color & 4) == 0);
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::RgbLeds - Set color of right RGB leds (different color)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TMaqueenPlus::RgbLeds(int ColorL, int ColorR)
   {
      RgbLeft(ColorL);
      RgbRight(ColorR);
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::RgbLeds - Set color of right RGB leds (same color)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TMaqueenPlus::RgbLeds(int Color)
   {
      RgbLeft(Color);
      RgbRight(Color);
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::LineSensorRead - returns analog value, lower value => darker
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int TMaqueenPlus::LineSensorRead(int Nr)
   {
      switch(Nr) {
         case 0 : return analogRead(MAQUEENPLUS_PIN_SENSOR_L3);
         case 1 : return analogRead(MAQUEENPLUS_PIN_SENSOR_L2);
         case 2 : return analogRead(MAQUEENPLUS_PIN_SENSOR_L1);
         case 3 : return analogRead(MAQUEENPLUS_PIN_SENSOR_R1);
         case 4 : return analogRead(MAQUEENPLUS_PIN_SENSOR_R2);
         case 5 : return analogRead(MAQUEENPLUS_PIN_SENSOR_R3);
      }

      printf("Unknown Linesensor#: %d\n", Nr);
      return -1;
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::LineSensorBits - One bit per linesensor. One => black
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int TMaqueenPlus::LineSensorBits()
   {  int Bits = 0;
      int Value[6];

      Value[0] = analogRead(MAQUEENPLUS_PIN_SENSOR_L3);
      Value[1] = analogRead(MAQUEENPLUS_PIN_SENSOR_L2);
      Value[2] = analogRead(MAQUEENPLUS_PIN_SENSOR_L1);
      Value[3] = analogRead(MAQUEENPLUS_PIN_SENSOR_R1);
      Value[4] = analogRead(MAQUEENPLUS_PIN_SENSOR_R2);
      Value[5] = analogRead(MAQUEENPLUS_PIN_SENSOR_R3);

      if (Value[0] < 500) Bits |= 0x01;
      if (Value[1] < 500) Bits |= 0x02;
      if (Value[2] < 500) Bits |= 0x04;
      if (Value[3] < 500) Bits |= 0x08;
      if (Value[4] < 500) Bits |= 0x10;
      if (Value[5] < 500) Bits |= 0x20;

//      printf("LS %3d %3d %3d %3d %3d %3d\n", Value[0], Value[1], Value[2], Value[3], Value[4], Value[5]);

      return Bits;
   }

//-----------------------------------------------------------------------------
// TMaqueenPlus::SetLineSensorLeds - Works on LineSensorBits output
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TMaqueenPlus::SetLineSensorLeds(int Bits)
   {
      digitalWrite(MAQUEENPLUS_PIN_LED_L3, (Bits & 0x01) == 0);
      digitalWrite(MAQUEENPLUS_PIN_LED_L2, (Bits & 0x02) == 0);
      digitalWrite(MAQUEENPLUS_PIN_LED_L1, (Bits & 0x04) == 0);
      digitalWrite(MAQUEENPLUS_PIN_LED_R1, (Bits & 0x08) == 0);
      digitalWrite(MAQUEENPLUS_PIN_LED_R2, (Bits & 0x10) == 0);
      digitalWrite(MAQUEENPLUS_PIN_LED_R3, (Bits & 0x20) == 0);
   }