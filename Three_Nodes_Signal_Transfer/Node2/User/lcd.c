#include "lcd.h"
#include "stm32f10x.h"
#include "other.h"

uint cursor=0; 
ErrorStatus HSEStartUpStatus;
GPIO_InitTypeDef GPIO_InitStructure;
uchar word1[] ="01234567891234567890";
uchar word2[] ="abcdefg";

void LCD_setCursor(uint row,uint col)
{
	uint cur=0x0080;
	if(row==1)
	{
		cur=0x0080;
	}
	if(row==2)
	{
		cur=0x00C0;
	}
	cur=cur+col;
	LCD1602_writeCom(cur);
}
//////////1602清屏/////////
void clean(void)
{
	LCD1602_writeCom(0x0001);
	LCD1602_writeCom(0x0080);
	cursor=0;
}
/////////各种配置初始化begin/////
void begin(void)
{
	GPIO_Configuration( );
	RCC_Configuration( );
	LCD1602_init( );
}
//////////1602写指令函数////////
void LCD1602_writeCom(uchar com)
{
      LCD_RS_LOW;
      LCD_RW_LOW;
      GPIO_Write(GPIOE,com);
      delay_ms(10);
      LCD_EN_HIGH;
      delay_ms(10);
      LCD_EN_LOW;
      delay_ms(10);
      LCD_RS_HIGH;
      LCD_RW_HIGH;
}

//////////1602写数据函数////////

void LCD1602_writeData(uchar wdata)
{
      LCD_RS_HIGH;
      LCD_RW_LOW;
	  GPIO_Write(GPIOE,wdata);
      delay_ms(10);
      LCD_EN_HIGH;
      delay_ms(10);
      LCD_EN_LOW;
      delay_ms(10);
      LCD_RS_LOW;
      LCD_RW_HIGH;
}

//////////初始化1602写指令函数////////

void LCD1602_init(void)
{
      LCD1602_writeCom(0x0038); //16*2显示 执行时间40um    
      LCD1602_writeCom(0x0008);//关显示，关光标
      LCD1602_writeCom(0x0001);//清除显示内容
      LCD1602_writeCom(0x0006);    //写完数据自动右移光标
      LCD1602_writeCom(0x000c);//开显示，关光标

}

//////////初始化1602写数据函数////////

void LCD_print (uchar DAT[],uint n)
{
      uchar i;
			uint surplus=15-cursor;
      //LCD1602_writeCom(ONE);
      for(i=0;i<n;i++)
      {
				LCD1602_writeData(DAT[i]);
				cursor+=1;
				if(i>surplus)
				{
					LCD1602_writeCom(TWO);
					for(;i<n;i++)
					{
						LCD1602_writeData(DAT[i]);
						cursor+=1;
					}
				}
			}
			
			LCD1602_writeCom(0x0014); 
			cursor+=1;
}



void GPIO_Configuration(void)
{

   /*配置LED对应位IO口*/
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;		//选择IO端口的第6位
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE, ENABLE);    // 使能端口时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);    // 使能端口时钟
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			  //时钟速度为50M
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			  //端口模式为推挽输出方式
  GPIO_Init(GPIOE, &GPIO_InitStructure);					  //用以上几个参数初始化PA口
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
  GPIO_Init(GPIOC, &GPIO_InitStructure);					  //用以上几个参数初始化PA口 
}

void RCC_Configuration(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();										//时钟控制寄存器全部恢复默认值

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);						//外部高速时钟源开启（8M晶振）

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();		//等待外部时钟就绪

  if(HSEStartUpStatus == SUCCESS)					//如果时钟启动成功
  {
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);				//定义AHB设备时钟为系统时钟1分频

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);					//定义AHB2设备时钟为HCLK时钟1分频

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);					//定义AHB1设备时钟为HCLK时钟2分频

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);				//设定内部FLASH的的延时周期为2周期
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	 //使能FLASH预存取缓冲区

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);	 //配置PLL时钟为外部高速时钟的9倍频，8MHz * 9 = 72 MHz

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);										 //使能PLL时钟

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)		 //等待PLL时钟设置完成准备就绪
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				 //使用PLL时钟作为系统时钟源

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)					 //返回系统所用时钟源确认为外部高速晶振，8M晶振。
    {
    }
  }
  /*Enble GPIOA、GPIOB、GPIOC*/
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	//使能由APB2时钟控制的外设中的PA端口
}

#ifndef  USE_FULL_ASSERT
void LCD_begin(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	LCD1602_init();
}
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
