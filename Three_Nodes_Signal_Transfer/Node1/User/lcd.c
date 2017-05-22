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
//////////1602����/////////
void clean(void)
{
	LCD1602_writeCom(0x0001);
	LCD1602_writeCom(0x0080);
	cursor=0;
}
/////////�������ó�ʼ��begin/////
void begin(void)
{
	GPIO_Configuration( );
	RCC_Configuration( );
	LCD1602_init( );
}
//////////1602дָ���////////
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

//////////1602д���ݺ���////////

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

//////////��ʼ��1602дָ���////////

void LCD1602_init(void)
{
      LCD1602_writeCom(0x0038); //16*2��ʾ ִ��ʱ��40um    
      LCD1602_writeCom(0x0008);//����ʾ���ع��
      LCD1602_writeCom(0x0001);//�����ʾ����
      LCD1602_writeCom(0x0006);    //д�������Զ����ƹ��
      LCD1602_writeCom(0x000c);//����ʾ���ع��

}

//////////��ʼ��1602д���ݺ���////////

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

   /*����LED��ӦλIO��*/
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;		//ѡ��IO�˿ڵĵ�6λ
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE, ENABLE);    // ʹ�ܶ˿�ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);    // ʹ�ܶ˿�ʱ��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			  //ʱ���ٶ�Ϊ50M
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			  //�˿�ģʽΪ���������ʽ
  GPIO_Init(GPIOE, &GPIO_InitStructure);					  //�����ϼ���������ʼ��PA��
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
  GPIO_Init(GPIOC, &GPIO_InitStructure);					  //�����ϼ���������ʼ��PA�� 
}

void RCC_Configuration(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();										//ʱ�ӿ��ƼĴ���ȫ���ָ�Ĭ��ֵ

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);						//�ⲿ����ʱ��Դ������8M����

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();		//�ȴ��ⲿʱ�Ӿ���

  if(HSEStartUpStatus == SUCCESS)					//���ʱ�������ɹ�
  {
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);				//����AHB�豸ʱ��Ϊϵͳʱ��1��Ƶ

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);					//����AHB2�豸ʱ��ΪHCLKʱ��1��Ƶ

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);					//����AHB1�豸ʱ��ΪHCLKʱ��2��Ƶ

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);				//�趨�ڲ�FLASH�ĵ���ʱ����Ϊ2����
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	 //ʹ��FLASHԤ��ȡ������

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);	 //����PLLʱ��Ϊ�ⲿ����ʱ�ӵ�9��Ƶ��8MHz * 9 = 72 MHz

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);										 //ʹ��PLLʱ��

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)		 //�ȴ�PLLʱ���������׼������
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				 //ʹ��PLLʱ����Ϊϵͳʱ��Դ

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)					 //����ϵͳ����ʱ��Դȷ��Ϊ�ⲿ���پ���8M����
    {
    }
  }
  /*Enble GPIOA��GPIOB��GPIOC*/
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	//ʹ����APB2ʱ�ӿ��Ƶ������е�PA�˿�
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
