// test for UbTech servo comms

HardwareSerial Serial1  (PB7, PB6);

void SetModePB6(int Mode)
{
   GPIO_InitTypeDef GPIO_InitStructure;

   GPIO_InitStructure.Pin     = GPIO_PIN_6;
   GPIO_InitStructure.Mode    = Mode;
   GPIO_InitStructure.Speed   = GPIO_SPEED_FREQ_HIGH;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void UbtSetup()
{
   Serial1.begin(115200);
   SetModePB6(GPIO_MODE_INPUT);
}

//-----------------------------------------------------------------------------
// UbtWrite - write & read, single pin / half duplex bus
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int UbtWrite(unsigned char *TxBuf, int TxLength, unsigned char *RxBuf, int RxLength)
{
   while (Serial1.available()) Serial1.read(); // flush RX data

   SetModePB6(GPIO_MODE_AF_PP);  // set pin to output (push-pull)

   for (int i=0; i<TxLength; i++) {
      USART1->DR = TxBuf[i];
      while ((USART1->SR & UART_FLAG_TC) == 0); // wait for char to be sent
   }

   SetModePB6(GPIO_MODE_INPUT); // set pin to input (high-impedance)

   CSerial.print("***msg tx\n");
   HexDump(TxBuf, TxLength);

   delayMicroseconds(90 * (RxLength - TxLength));    // wait for response

   delay(10);

   int Nr = Serial1.readBytes( RxBuf, RxLength);

   CSerial.printf("msg rx %d vs %d\n", Nr, RxLength);
   HexDump(RxBuf, RxLength);

   return Nr;  // bytes received (echo of message sent + response)
}