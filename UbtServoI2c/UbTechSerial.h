//-----------------------------------------------------------------------------
// UbTechSerial.h - setup Serial1 on Stm32f103 for UbTech comms.
//-----------------------------------------------------------------------------
// Hardcoded for StM32F103, Serial1 and pins PA9 & PA10
// Connect PA9 and PA10 both to the data-pin of the bus.
//-----------------------------------------------------------------------------

HardwareSerial Serial1  (PA10, PA9);

//-----------------------------------------------------------------------------
// SetModeTxPin - support function to toggle tx/rx
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SetModeTxPin(int Mode)
{
   GPIO_InitTypeDef GPIO_InitStructure;

   GPIO_InitStructure.Pin     = GPIO_PIN_9;
   GPIO_InitStructure.Mode    = Mode;
   GPIO_InitStructure.Speed   = GPIO_SPEED_FREQ_HIGH;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//-----------------------------------------------------------------------------
// UbtSetup - Setup serial1 for UBT protocol.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void UbtSetup()
{
   Serial1.begin(115200);
   SetModeTxPin(GPIO_MODE_INPUT);
   Serial1.setTimeout(10);
}

//-----------------------------------------------------------------------------
// UbtWrite - write & read, single pin / half duplex bus
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int UbtWrite(unsigned char *TxBuf, int TxLength, unsigned char *RxBuf, int RxLength)
{
   bool Debug = false;

   while (Serial1.available()) Serial1.read(); // flush RX data

   SetModeTxPin(GPIO_MODE_AF_PP);  // set pin to output (push-pull)

   for (int i=0; i<TxLength; i++) {
      USART1->DR = TxBuf[i];
      while ((USART1->SR & UART_FLAG_TC) == 0); // wait for char to be sent
   }

   SetModeTxPin(GPIO_MODE_INPUT); // set pin to input (high-impedance)

   if (Debug) {
      CSerial.print("***msg tx\n");
      HexDump(TxBuf, TxLength);
   }

   delayMicroseconds(90 * (RxLength - TxLength));    // wait for response

   delay(10);

   int Nr = Serial1.readBytes( RxBuf, RxLength);

   if (Debug) {
      CSerial.printf("msg rx %d vs %d\n", Nr, RxLength);
      HexDump(RxBuf, RxLength);
   }

   return Nr;  // bytes received (echo of message sent + response)
}