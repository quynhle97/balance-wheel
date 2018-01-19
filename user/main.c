#include "main.h"

/* Private variables ---------------------------------------------------------*/
#define degconvert 57.29
#define MPU_ADDRESS 0xD0
#define FIRST_ADDRESS 0x3B

volatile int16_t MPU_data[4] = {0};
volatile float compAngleY = 0, pitch = 0;
volatile float pwmPID = 0, lastError = 0, errorI = 0;
// setPoint = 0;
/* Private functions ---------------------------------------------------------*/
void configMPU();
void delay(__IO uint32_t nCount);
void sendChar(char c);

// PB5 : SDA
// PB4 : SCL

void main()
{  
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

  UART1_DeInit();
  UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
              UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
//  UART1_HalfDuplexCmd(ENABLE);
  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST); // BIN1 
  GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_FAST); // BIN2 
  GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST); // AIN1
  GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST); // AIN2
  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_WriteHigh(GPIOD, GPIO_PIN_3);
  GPIO_WriteHigh(GPIOD, GPIO_PIN_2);
  
  I2C_DeInit();
  I2C_Init(100000, 0x50, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);

  newTIM1_TimeBaseInit(15999, 25, 0);
  
  newTIM2_TimeBaseInit(TIM2_PRESCALER_1, 999); // 16Mhz / 1 >> 1 Khz / 16 Mhz = 1/16Khz = 0.0000625 s
  
  newTIM2_OC1Init(0);
  newTIM2_OC1PreloadConfig(ENABLE);
        
  newTIM2_OC3Init(0);
  newTIM2_OC3PreloadConfig(ENABLE);

  newTIM2_ARRPreloadConfig(ENABLE);

  newTIM2_Cmd(ENABLE);
  
  configMPU();
  delay(0xff);
  readAngle(MPU_data);
  
  pitch = atan2(-MPU_data[0], MPU_data[1])*degconvert;
  compAngleY = pitch;
  
  sendChar('A');
  
  newTIM1_ITConfig(ENABLE);
  newTIM1_Cmd(ENABLE);
  enableInterrupts();
  
  while(1){


    char tmp1[10];
    int i = 0;
    
    if (compAngleY < 0) sendChar('-');    
    float tmpVal = (compAngleY < 0) ? -compAngleY : compAngleY;
    int tmpInt1 = (int) tmpVal;      
    while (tmpInt1 > 0) {
      tmp1[i] = tmpInt1%10;
      tmpInt1 = tmpInt1/10;
      i++;
    }
    
    for (--i; i>=0; --i) {
      sendChar(tmp1[i] + '0'); 
    }
    
    sendChar(' ');
    i=0;
        
    if (pwmPID < 0) sendChar('-');    
    tmpVal = (pwmPID < 0) ? -pwmPID : pwmPID;
    tmpInt1 = (int) tmpVal;      
    while (tmpInt1 > 0) {
      tmp1[i] = tmpInt1%10;
      tmpInt1 = tmpInt1/10;
      i++;
    }
    
    for (--i; i>=0; --i) {
      sendChar(tmp1[i] + '0'); 
    }
    
    sendChar(' ');
    i=0;
        
    if (errorI < 0) sendChar('-');    
    tmpVal = (errorI < 0) ? -errorI : errorI;
    tmpInt1 = (int) tmpVal;      
    while (tmpInt1 > 0) {
      tmp1[i] = tmpInt1%10;
      tmpInt1 = tmpInt1/10;
      i++;
    }
    
    for (--i; i>=0; --i) {
      sendChar(tmp1[i] + '0'); 
    }
    
    sendChar('\r');
    sendChar('\n');
    
    GPIO_WriteReverse(GPIOD, GPIO_PIN_2);
    
    delay(0xffff);
  }
}

void configMPU() {
  I2C_GenerateSTART(ENABLE);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  // send slave address
  I2C_Send7bitAddress(MPU_ADDRESS, I2C_DIRECTION_TX);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  // 1/ send address of first byte to be read & wait event detection
  I2C_SendData(0x6B);
  I2C_SendData(0);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_GenerateSTOP(ENABLE); 
  
  I2C_GenerateSTART(ENABLE);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  I2C_Send7bitAddress(MPU_ADDRESS, I2C_DIRECTION_TX);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); // EV6
  I2C_SendData(0x06);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING )); // EV8
  I2C_SendData(0xff);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING )); // EV8
  I2C_SendData(0xf9);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING )); // EV8
  I2C_SendData(0xff);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING )); // EV8
  I2C_SendData(0xfe);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING )); // EV8
  I2C_SendData(0x3f);
  I2C_SendData(0xfe);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)); // EV8_2
  I2C_GenerateSTOP(ENABLE);
    
  I2C_GenerateSTART(ENABLE);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  I2C_Send7bitAddress(MPU_ADDRESS, I2C_DIRECTION_TX);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); // EV6
  I2C_SendData(0x15);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING )); // EV8
  I2C_SendData(0x00);
  I2C_SendData(0x00);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)); // EV8_2
  I2C_GenerateSTOP(ENABLE);
}

void readAngle(volatile int16_t* data) {
  uint8_t rBuf[12];
  while (I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));
  I2C_AcknowledgeConfig(I2C_ACK_CURR);
  I2C_GenerateSTART(ENABLE);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); 
  // send slave address
  I2C_Send7bitAddress(MPU_ADDRESS, I2C_DIRECTION_TX);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  // send address of first byte to be read & wait event detection
  I2C_SendData(FIRST_ADDRESS);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_GenerateSTOP(ENABLE);
  
  I2C_GenerateSTART(ENABLE);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)); // EV5
  I2C_Send7bitAddress(MPU_ADDRESS, I2C_DIRECTION_RX); 
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)); // EV6
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
  rBuf[0] = I2C_ReceiveData();
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
  rBuf[1] = I2C_ReceiveData();
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
  rBuf[2] = I2C_ReceiveData();
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
  rBuf[3] = I2C_ReceiveData();
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
  rBuf[4] = I2C_ReceiveData();
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
  rBuf[5] = I2C_ReceiveData();
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
  rBuf[6] = I2C_ReceiveData();
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
  rBuf[7] = I2C_ReceiveData();
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
  rBuf[8] = I2C_ReceiveData();
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
  while ((I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) == RESET)); /* Poll on BTF */
  I2C_AcknowledgeConfig(I2C_ACK_NONE);
  disableInterrupts();
  rBuf[9] = I2C_ReceiveData();
  // while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED));
  I2C_GenerateSTOP(ENABLE);    
  rBuf[10] = I2C_ReceiveData();
  enableInterrupts();
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED));
  rBuf[11] = I2C_ReceiveData();
  // Co so can tinh
  // AcX = (rBuf[0] << 8)|(rBuf[1]);
  // AcY = (rBuf[2] << 8)|(rBuf[3]);
  // AcZ = (rBuf[4] << 8)|(rBuf[5]);
  // GyY = (rBuf[10] << 8)|(rBuf[11]);
  // Tinh goc 
  // double pitch = atan2(-AcX, AcZ)*degconvert;
  // double compAngleY = pitch;
  data[0] = (rBuf[0] << 8)|(rBuf[1]);
  data[1] = (rBuf[2] << 8)|(rBuf[3]);
  data[2] = (rBuf[4] << 8)|(rBuf[5]);
  data[3] = (rBuf[10] << 8)| (rBuf[11]);   
}

void calculateAngle(volatile float* value, volatile int16_t* data) {
  pitch = atan2((float) -data[0], (float) data[2])*degconvert;
  *value = 0.9677*((*value) + data[3]/5240.0) + 0.0323*pitch; //131 dt = 40
}

void sendChar(char c){
  /* Write a character to the UART1 */
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);

}

void delay(__IO uint32_t nCount)
{
  for (; nCount != 0; nCount--);
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/* print float value in printf
  char tmp[100];    
  char *tmpSign = (compAngleY < 0) ? "-" : "";
  float tmpVal = (compAngleY < 0) ? -compAngleY : compAngleY;
  
  int tmpInt1 = (int) tmpVal;                  // Get the integer (678).
  float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
  int tmpInt2 = (int) trunc(tmpFrac * 10000);  // Turn into integer (123).
  
  // Print as parts, note that you need 0-padding for fractional bit.
  
  sprintf (tmp, "Angle = %s%d.%04d\n", tmpSign, tmpInt1, tmpInt2);
  printf("%s", tmp);
*/