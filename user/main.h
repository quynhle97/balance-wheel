#include "stm8s.h"
#include "stm8s_i2c.h"
#include "stm8s_tim1.h"
#include "math.h"
#include "stm8s_gpio.h"

#define KP 30
#define KI 1
#define KD 0.3

//#define KP 39
//#define KI 6
//#define KD 0.5

void readAngle(volatile int16_t* data);
void calculateAngle(volatile float* value, volatile int16_t* data);

void newTIM2_TimeBaseInit(uint8_t TIM2_Prescaler, uint16_t TIM2_Period);
void newTIM2_OC1Init(uint16_t TIM2_Pulse);
void newTIM2_OC3Init(uint16_t TIM2_Pulse);
void newTIM2_OC1PreloadConfig(FunctionalState NewState);
void newTIM2_OC3PreloadConfig(FunctionalState NewState);
void newTIM2_ARRPreloadConfig(FunctionalState NewState);
void newTIM2_Cmd(FunctionalState NewState);
void newTIM2_SetCompare1(uint16_t Compare1);
void newTIM2_SetCompare3(uint16_t Compare3);

void newTIM1_TimeBaseInit(uint16_t TIM1_Prescaler, uint16_t TIM1_Period, uint8_t TIM1_RepetitionCounter);
void newTIM1_ITConfig(FunctionalState NewState);
void newTIM1_Cmd(FunctionalState NewState);
void newTIM1_ClearITPendingBit();
