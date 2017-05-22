#ifndef _PWM_H
#define _PWM_H
#include "stm32f10x.h"
void PWM5_Init(void);
void pwmWrite(uint16_t pulse);
void PWM3_Init(void);
#endif