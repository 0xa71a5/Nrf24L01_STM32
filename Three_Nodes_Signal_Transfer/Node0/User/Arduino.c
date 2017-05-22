#include "Arduino.h"

const uint32_t OutBitBand[112]={0x42210180,0x42210184,0x42210188,0x4221018C,0x42210190,0x42210194,0x42210198,0x4221019C,0x422101A0,0x422101A4,0x422101A8,0x422101AC,0x422101B0,0x422101B4,0x422101B8,0x422101BC,0x42218180,0x42218184,0x42218188,0x4221818C,0x42218190,0x42218194,0x42218198,0x4221819C,0x422181A0,0x422181A4,0x422181A8,0x422181AC,0x422181B0,0x422181B4,0x422181B8,0x422181BC,0x42220180,0x42220184,0x42220188,0x4222018C,0x42220190,0x42220194,0x42220198,0x4222019C,0x422201A0,0x422201A4,0x422201A8,0x422201AC,0x422201B0,0x422201B4,0x422201B8,0x422201BC,0x42228180,0x42228184,0x42228188,0x4222818C,0x42228190,0x42228194,0x42228198,0x4222819C,0x422281A0,0x422281A4,0x422281A8,0x422281AC,0x422281B0,0x422281B4,0x422281B8,0x422281BC,0x42230180,0x42230184,0x42230188,0x4223018C,0x42230190,0x42230194,0x42230198,0x4223019C,0x422301A0,0x422301A4,0x422301A8,0x422301AC,0x422301B0,0x422301B4,0x422301B8,0x422301BC,0x42238180,0x42238184,0x42238188,0x4223818C,0x42238190,0x42238194,0x42238198,0x4223819C,0x422381A0,0x422381A4,0x422381A8,0x422381AC,0x422381B0,0x422381B4,0x422381B8,0x422381BC};
const uint32_t  InBitBand[112]={0x42210100,0x42210104,0x42210108,0x4221010C,0x42210110,0x42210114,0x42210118,0x4221011C,0x42210120,0x42210124,0x42210128,0x4221012C,0x42210130,0x42210134,0x42210138,0x4221013C,0x42218100,0x42218104,0x42218108,0x4221810C,0x42218110,0x42218114,0x42218118,0x4221811C,0x42218120,0x42218124,0x42218128,0x4221812C,0x42218130,0x42218134,0x42218138,0x4221813C,0x42220100,0x42220104,0x42220108,0x4222010C,0x42220110,0x42220114,0x42220118,0x4222011C,0x42220120,0x42220124,0x42220128,0x4222012C,0x42220130,0x42220134,0x42220138,0x4222013C,0x42228100,0x42228104,0x42228108,0x4222810C,0x42228110,0x42228114,0x42228118,0x4222811C,0x42228120,0x42228124,0x42228128,0x4222812C,0x42228130,0x42228134,0x42228138,0x4222813C,0x42230100,0x42230104,0x42230108,0x4223010C,0x42230110,0x42230114,0x42230118,0x4223011C,0x42230120,0x42230124,0x42230128,0x4223012C,0x42230130,0x42230134,0x42230138,0x4223013C,0x42238100,0x42238104,0x42238108,0x4223810C,0x42238110,0x42238114,0x42238118,0x4223811C,0x42238120,0x42238124,0x42238128,0x4223812C,0x42238130,0x42238134,0x42238138,0x4223813C};

uint64_t millis=0;
uint32_t Pwm_Period=1000;
void ArduinoInit()
{
	RCC_APB1PeriphClockCmd(0x7EFEC9FF,ENABLE);//0x81013600
	//RCC_APB2PeriphClockCmd(0x003FFFFD,ENABLE);//0xFFC00002
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO| RCC_APB2Periph_GPIOA| RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOE| RCC_APB2Periph_GPIOF| RCC_APB2Periph_GPIOG| RCC_APB2Periph_ADC1| RCC_APB2Periph_ADC2| RCC_APB2Periph_TIM1,ENABLE);
	// RCC_APB2Periph_SPI1 有bug
	SystickInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
}

void pinMode(uint32_t pin,uint32_t mode)
{
	int temp;
	GPIO_InitTypeDef gpio;
	GPIO_TypeDef *gpioW;
	switch(mode)
	{
		case OUTPUT:
			gpio.GPIO_Mode=GPIO_Mode_Out_PP;
			break;
		
		case INPUT:
			gpio.GPIO_Mode=GPIO_Mode_IN_FLOATING;
			break;
		
		case INPUT_PULLUP:
			gpio.GPIO_Mode=GPIO_Mode_IPU;
			break;	
		
		case INPUT_PULLDOWN:
			gpio.GPIO_Mode=GPIO_Mode_IPD;
			break;
		
		case PWM:
			Set_Mode_PWM(pin);
			gpio.GPIO_Mode=GPIO_Mode_AF_PP;
		break;
		
		
		default:
			gpio.GPIO_Mode=GPIO_Mode_Out_PP;
	}
	temp=pin%16;
	gpio.GPIO_Pin=((uint16_t)1)<<temp;
	gpio.GPIO_Speed=GPIO_Speed_50MHz;
	
	temp=pin/16;
	switch(temp)
	{
		case 0:
			gpioW=GPIOA;
			break;
		
		case 1:
			gpioW=GPIOB;
			break;
		
		case 2:
			gpioW=GPIOC;
			break;
		
		case 3:
			gpioW=GPIOD;
			break;
		
		case 4:
			gpioW=GPIOE;
			break;
		
		case 5:
			gpioW=GPIOF;
			break;
		
		case 6:
			gpioW=GPIOG;
			break;
		
		default:
			gpioW=GPIOA;
			break;
	}
	GPIO_Init(gpioW,&gpio);
}
void SystickInit(void)
{
		SysTick_Config(SystemCoreClock/1000);
}

void SysTick_Handler(void)
{
	if(millis>0)
		millis--;
}

void delay(uint32_t delayTime)
{
	SysTick_Config(SystemCoreClock/1000);
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	millis=delayTime;
	while(millis>0);
	SysTick->CTRL&=SysTick_CTRL_ENABLE_Msk;	
}

void delayMicroseconds(uint32_t delayTime)
{
	SysTick_Config(SystemCoreClock/1000000);
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	millis=delayTime;
	while(millis>0);
	SysTick->CTRL&=SysTick_CTRL_ENABLE_Msk;	
}

/*
stm32f103zet6核心板 可用PWM通道

TIM1
PA8 PA9 PA10 PA11
							√
TIM2
PA0 PA1 PA2  PA3
√		√			    √
TIM3
PA6 PA7 PB0  PB1
√		√		√			√
TIM4
PB6 PB7 PB8  PB9
				√			√
TIM8
PC6 PC7 PC8  PC9
√		√		√ 	  √


*/




void analogWrite(uint32_t pin,uint32_t percent)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TypeDef * TIMx;
	uint16_t pulse=Pwm_Period*percent/100;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = pulse;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平
	
	
	
	
	if(pin>=PA8&&pin<=PA11)
	{
		TIMx=TIM1;
	}
	else if(pin>=PA0&&pin<=PA3)
	{
		TIMx=TIM2;
	}
	else if(pin==PA6||pin==PA7||pin==PB0||pin==PB1)
	{
		TIMx=TIM3;
	}
	else if(pin>=PB6&&pin<=PB9)
	{
		TIMx=TIM4;
	}
	else if(pin>=PC6&&pin<=PC9)
	{
		TIMx=TIM8;
	}
	
	if(pin==PA8||pin==PA0||pin==PA6||pin==PB6||pin==PC6)
	{
		TIM_OC1Init(TIMx, &TIM_OCInitStructure);	
		TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if(pin==PA1||pin==PA7||pin==PA9||pin==PB7||pin==PC7)
	{
		TIM_OC2Init(TIMx, &TIM_OCInitStructure);	
		TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if(pin==PA2||pin==PA10||pin==PB0||pin==PB8||pin==PC8)
	{
		TIM_OC3Init(TIMx, &TIM_OCInitStructure);	
		TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if(pin==PA3||pin==PA11||pin==PB1||pin==PB9||pin==PC9)
	{
		TIM_OC4Init(TIMx, &TIM_OCInitStructure);	
		TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	
	
	
	TIM_ARRPreloadConfig(TIMx, ENABLE);		
}


void Set_Mode_PWM(uint32_t pin)
{
	TIM_TypeDef * TIMx;
	if(pin>=PA8&&pin<=PA11)
	{
		TIMx=TIM1;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	}
	else if(pin>=PA0&&pin<=PA3)
	{
		TIMx=TIM2;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	}
	else if(pin==PA6||pin==PA7||pin==PB0||pin==PB1)
	{
		TIMx=TIM3;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
	}
	else if(pin>=PB6&&pin<=PB9)
	{
		TIMx=TIM4;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	}
	else if(pin>=PC6&&pin<=PC9)
	{
		TIMx=TIM8;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE); 
	}
	
	Set_Pwm_Frequency(TIMx,1000000/Pwm_Period);
}


void Set_Pwm_Frequency(TIM_TypeDef * TIMx,uint32_t freq)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	Pwm_Period=1000000/freq;
	TIM_TimeBaseStructure.TIM_Period =(uint16_t)(Pwm_Period-1);       //1000x1us=1ms=1khz
  TIM_TimeBaseStructure.TIM_Prescaler = 71;	    //设置预分频：72M/72=1Mhz  1us
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频(这里用不到)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
  TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	if(TIMx==TIM8)
		TIM_CtrlPWMOutputs(TIM8, ENABLE); 
	TIM_Cmd(TIMx, ENABLE);                 
}

void pwmFrequency(uint32_t freq)
{
	Pwm_Period=((uint32_t)1000000)/freq;
}

