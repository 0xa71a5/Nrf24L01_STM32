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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // �����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//PB0��������PWM��
	
	
																			
	
	/* ����TIM3CLK ʱ��Ϊ72MHZ */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 					//ʹ��TIM3ʱ��

  /* ������ʱ������ */		  //period_us������ô��us����һ���ж�
  TIM_TimeBaseStructure.TIM_Period = period_us;       							//����ʱ����0������255����Ϊ266�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Prescaler = 143;	    							  //����Ԥ��Ƶ��
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//���ϼ���ģʽ
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWMģʽ���� */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;	//��ֹ���
  TIM_OCInitStructure.TIM_Pulse = 0;										  			//���ó�ʼPWM������Ϊ0	
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //����ʱ������ֵС��CCR1_ValʱΪ�͵�ƽ

  TIM_OC3Init(TIM3, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
	

  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	

  TIM_ARRPreloadConfig(TIM3, ENABLE);			 										//ʹ��TIM3���ؼĴ���ARR

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);                   										//ʹ�ܶ�ʱ��3	
	
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);										//ʹ��update�ж�

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* ����TIM3_IRQ�ж�Ϊ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
