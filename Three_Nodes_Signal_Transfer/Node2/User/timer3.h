#ifndef TIMER3_H
#define TIMER3_H
#include "stm32f10x.h"
#define Enable_Timer3_Int()	TIM3->DIER |= TIM_IT_Update;
#define Disable_Timer3_Int()	TIM3->DIER &= (uint16_t)~TIM_IT_Update;
void Tim3_Init(u32 period);
#endif
