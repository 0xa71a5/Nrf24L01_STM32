#ifndef OTHER_H
#define OTHER_H
#include "stm32f10x.h"
#define OPEN_INT	_set_PRIMASK(1)
#define CLOSE_INT	_set_PRIMASK(0)
void Systick_Config();
void delay_ms(u32 time);
void delay_us(u32 time);
#endif
