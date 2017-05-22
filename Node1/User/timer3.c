#include "timer3.h"
#include "stm32f10x.h"

void Tim3_Init(u32 period_us)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;		
	NVIC_InitTypeDef NVIC_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//PB0被用在了PWM上
	
	
																			
	
	/* 设置TIM3CLK 时钟为72MHZ */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 					//使能TIM3时钟

  /* 基本定时器配置 */		  //period_us表明这么多us发生一次中断
  TIM_TimeBaseStructure.TIM_Period = period_us;       							//当定时器从0计数到255，即为266次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = 143;	    							  //设置预分频：
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//设置时钟分频系数：不分频(这里用不到)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM模式配置 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;	//禁止输出
  TIM_OCInitStructure.TIM_Pulse = 0;										  			//设置初始PWM脉冲宽度为0	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //当定时器计数值小于CCR1_Val时为低电平

  TIM_OC3Init(TIM3, &TIM_OCInitStructure);	 									//使能通道3
	

  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);						//使能预装载	

  TIM_ARRPreloadConfig(TIM3, ENABLE);			 										//使能TIM3重载寄存器ARR

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);                   										//使能定时器3	
	
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);										//使能update中断

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置TIM3_IRQ中断为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
