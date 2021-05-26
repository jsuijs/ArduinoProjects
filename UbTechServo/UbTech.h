// test for UbTech servo comms

HardwareSerial Serial1  (PB7, PB6);

void SetModePB6(int Mode)
{
   GPIO_InitTypeDef GPIO_InitStructure;

   GPIO_InitStructure.Pin      = GPIO_PIN_6;
   GPIO_InitStructure.Mode     = Mode;
   GPIO_InitStructure.Speed    = GPIO_SPEED_FREQ_HIGH;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void UbtSetup()
{
   Serial1.begin(115200);
   SetModePB6(GPIO_MODE_INPUT);
}

void UbtTest()
{
   SetModePB6(GPIO_MODE_AF_PP);  // set pin to output (push-pull)

   USART1->DR = 'U';
   while ((USART1->SR & UART_FLAG_TC) == 0); // wait for char to be sent

   SetModePB6(GPIO_MODE_INPUT); // set pin to input (high-impedance)



   UbtServo.setServoAngle(1, 45, 200); // servo 1, to 45 graden in 200 ms(?)

}
