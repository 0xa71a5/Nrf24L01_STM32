#include "other.h"
#include "stm32f10x.h"
u32 millis=0;
void Systick_Config()
{
	SysTick_Config(SystemCoreClock / 1000000);
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;//¹Ø±Õ
}
void delay_us(u32 time)
{
	u32 last=millis;
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
	while(millis-last<time);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void delay_ms(u32 time)
{
	int i=0;
	for(i=0;i<time;i++)
		delay_us(1000);
}
