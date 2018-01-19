#include "main.h"

void newTIM1_TimeBaseInit(uint16_t TIM1_Prescaler,
                       uint16_t TIM1_Period,
                       uint8_t TIM1_RepetitionCounter)
{  
  /* Set the Autoreload value */
  TIM1->ARRH = (uint8_t)(TIM1_Period >> 8);
  TIM1->ARRL = (uint8_t)(TIM1_Period);
  
  /* Set the Prescaler value */
  TIM1->PSCRH = (uint8_t)(TIM1_Prescaler >> 8);
  TIM1->PSCRL = (uint8_t)(TIM1_Prescaler);
  
  /* Select the Counter Mode */
  TIM1->CR1 = (uint8_t)((uint8_t)(TIM1->CR1 & (uint8_t)(~(TIM1_CR1_CMS | TIM1_CR1_DIR))));
  
  /* Set the Repetition Counter value */
  TIM1->RCR = TIM1_RepetitionCounter;
}

void newTIM1_ITConfig(FunctionalState NewState)
{  
  if (NewState != DISABLE)
  {
    /* Enable the Interrupt sources */
    TIM1->IER |= (uint8_t)0x01;
  }
  else
  {
    /* Disable the Interrupt sources */
    TIM1->IER &= (uint8_t)(~(uint8_t)0x01);
  }
}

void newTIM1_Cmd(FunctionalState NewState)
{  
  /* set or Reset the CEN Bit */
  if (NewState != DISABLE)
  {
    TIM1->CR1 |= TIM1_CR1_CEN;
  }
  else
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_CEN);
  }
}

void newTIM1_ClearITPendingBit()
{
  /* Clear the IT pending Bit */
  TIM1->SR1 = (uint8_t)(~(uint8_t)0x01);
}




void newTIM2_TimeBaseInit(uint8_t TIM2_Prescaler,
                        uint16_t TIM2_Period)
{
  /* Set the Prescaler value */
  TIM2->PSCR = (uint8_t)(TIM2_Prescaler);
  /* Set the Autoreload value */
  TIM2->ARRH = (uint8_t)(TIM2_Period >> 8);
  TIM2->ARRL = (uint8_t)(TIM2_Period);
}

void newTIM2_OC1Init(uint16_t TIM2_Pulse)
{
  /* Disable the Channel 1: Reset the CCE Bit, Set the Output State , the Output Polarity */
  TIM2->CCER1 &= (uint8_t)(~( TIM2_CCER1_CC1E | TIM2_CCER1_CC1P));
  /* Set the Output State &  Set the Output Polarity  */
  TIM2->CCER1 |= (uint8_t)((uint8_t)(0x11 & TIM2_CCER1_CC1E ) | 
                           (uint8_t)(0x00 & TIM2_CCER1_CC1P));
  
  /* Reset the Output Compare Bits  & Set the Ouput Compare Mode */
  TIM2->CCMR1 = (uint8_t)((uint8_t)(TIM2->CCMR1 & (uint8_t)(~TIM2_CCMR_OCM)) |
                          (uint8_t)0x60);
  
  /* Set the Pulse value */
  TIM2->CCR1H = (uint8_t)(TIM2_Pulse >> 8);
  TIM2->CCR1L = (uint8_t)(TIM2_Pulse);
}

void newTIM2_OC3Init(uint16_t TIM2_Pulse)
{
 /* Disable the Channel 1: Reset the CCE Bit, Set the Output State, the Output Polarity */
  TIM2->CCER2 &= (uint8_t)(~( TIM2_CCER2_CC3E  | TIM2_CCER2_CC3P));
  /* Set the Output State & Set the Output Polarity */
  TIM2->CCER2 |= (uint8_t)((uint8_t)(0x11 & TIM2_CCER2_CC3E) |  
                           (uint8_t)(0x00 & TIM2_CCER2_CC3P));
  
  /* Reset the Output Compare Bits & Set the Output Compare Mode */
  TIM2->CCMR3 = (uint8_t)((uint8_t)(TIM2->CCMR3 & (uint8_t)(~TIM2_CCMR_OCM)) |
                          (uint8_t)0x60);
  
  /* Set the Pulse value */
  TIM2->CCR3H = (uint8_t)(TIM2_Pulse >> 8);
  TIM2->CCR3L = (uint8_t)(TIM2_Pulse);
}

void newTIM2_OC1PreloadConfig(FunctionalState NewState)
{  
  /* Set or Reset the OC1PE Bit */
  if (NewState != DISABLE)
  {
    TIM2->CCMR1 |= (uint8_t)TIM2_CCMR_OCxPE;
  }
  else
  {
    TIM2->CCMR1 &= (uint8_t)(~TIM2_CCMR_OCxPE);
  }
}

void newTIM2_OC3PreloadConfig(FunctionalState NewState)
{
  /* Set or Reset the OC3PE Bit */
  if (NewState != DISABLE)
  {
    TIM2->CCMR3 |= (uint8_t)TIM2_CCMR_OCxPE;
  }
  else
  {
    TIM2->CCMR3 &= (uint8_t)(~TIM2_CCMR_OCxPE);
  }
}

void newTIM2_ARRPreloadConfig(FunctionalState NewState)
{
  /* Set or Reset the ARPE Bit */
  if (NewState != DISABLE)
  {
    TIM2->CR1 |= (uint8_t)TIM2_CR1_ARPE;
  }
  else
  {
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_ARPE);
  }
}

void newTIM2_Cmd(FunctionalState NewState)
{
  /* set or Reset the CEN Bit */
  if (NewState != DISABLE)
  {
    TIM2->CR1 |= (uint8_t)TIM2_CR1_CEN;
  }
  else
  {
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_CEN);
  }
}

void newTIM2_SetCompare1(uint16_t Compare1)
{
  /* Set the Capture Compare1 Register value */
  TIM2->CCR1H = (uint8_t)(Compare1 >> 8);
  TIM2->CCR1L = (uint8_t)(Compare1);
}

void newTIM2_SetCompare3(uint16_t Compare3)
{
  /* Set the Capture Compare3 Register value */
  TIM2->CCR3H = (uint8_t)(Compare3 >> 8);
  TIM2->CCR3L = (uint8_t)(Compare3);
}