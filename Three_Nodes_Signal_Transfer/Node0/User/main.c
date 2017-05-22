#include "stm32f10x.h"
#include "timer3.h"
#include "bsp_uart.h"
#include "other.h"
#include "adc.h"
#include "bsp_spi_nrf.h"
#include "lcd.h"
extern u32 millis;
extern u32 counter_value;

u8 status;	//??????/????
u8 txbuf[32]={0,1,2,3};	 //????
u8 rxbuf[32];			 //????
int i=0;
u8 my_addr[]= {0x12,0x34,0x56,0x78,0x01};
u8 his_addr[]={0x12,0x34,0x56,0x78,0x00};

u8 ram[36000];
u32 ram_max_address=36000;
u32 ram_pointer=0;

u8 left_addr[]={0x12,0x34,0x56,0x78,0x01};
u8 middle_addr[]={0x12,0x34,0x56,0x78,0x02};
u8 right_addr[]={0x12,0x34,0x56,0x78,0x00};

extern int TX_PLOAD_WIDTH ;
extern int RX_PLOAD_WIDTH ;

u8 my_node_number=0x01;
u8 my_node_mac='A';
u16 node_number_mask;

u8 my_frequency=17;

u8 A_node_mac='A';
u8 B_node_mac='B';
u8 C_node_mac='C';

u8 A_node_number=1;
u8 B_node_number=0;
u8 C_node_number=2;
extern uchar word2[];
void Set_Record_Begin()
{
	millis=0;
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
}

u32 Get_Record_Value()
{
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
	return millis;
}
void Nrf_Rx_Test()
{
	u16 packet_count=0;
	u32 temp;
	u32 total_packets=100;
	u32 error_packets=0;
	

  printf("\r\n  检测是否正常连接 \r\n");

  /*??NRF???MCU???*/
  status = NRF_Check();   		
  if(status == SUCCESS)	   
    printf("\r\n  Nrf连接成功  \r\n");  
  else	  
    printf("\r\n  Nrf连接失败 \r\n");
	
		
    printf("\r\n 从机端进入死循环接受模式 \r\n"); 
    NRF_RX_Mode(my_addr);
	  //NRF_RX_Mode_No_Ack(my_addr);
	  packet_count=0;
		while(1)
		{
			status = NRF_Rx_Dat(rxbuf,100000);
			if(packet_count==0)
				Set_Record_Begin();
			if(status == RX_DR)
			{
				packet_count++;
				//printf("%d\r\n",packet_count);
			}
			if(packet_count==total_packets)
			{
				temp=Get_Record_Value();
				printf("Duration:%dus\r\nTotal Packets:%d\r\nError Packets:%d\r\nAverage tx time:%dus\r\n",temp,total_packets,total_packets-packet_count,temp/total_packets);
				break;
			}	
		} 
}
void Nrf_Tx_Test()
{

	u16 error_packets=0;
	u32 temp;
	u16 total_packets=100;
	
	
	printf("\r\n  检测是否正常连接 \r\n");
  status = NRF_Check(); 
  if(status == SUCCESS)	   
    printf("\r\n  Nrf连接成功  \r\n");  
  else	  
    printf("\r\n  Nrf连接失败 \r\n");
	printf("\r\n 主机端开始发送测试 \r\n"); 
	NRF_TX_Mode(middle_addr);
	
	Set_Record_Begin();
	  for(i=0;i<total_packets;i++)
	{
    status = NRF_Tx_Dat(txbuf);	  //MAX_RT TX_DS ERROR =>3 status
    if(status!=TX_DS)
			error_packets++;
	}
	temp=Get_Record_Value();
	printf("Duration:%dus\r\nTotal Packets:%d\r\nError Packets:%d\r\nAverage tx time:%dus\r\n",temp,total_packets,error_packets,temp/total_packets);
}
void Button_Init()
{
	#define Read_Button()	((GPIOB->IDR & GPIO_Pin_7)!= (uint32_t)Bit_RESET)
	#define Read_Button_Mode()	((GPIOB->IDR & GPIO_Pin_8)!= (uint32_t)Bit_RESET)
	#define LED_ON() GPIOA->BSRR = GPIO_Pin_2
	#define LED_OFF() GPIOA->BRR = GPIO_Pin_2
	#define G7() (GPIOG->IDR & GPIO_Pin_7)!= (uint32_t)Bit_RESET
	#define G8() (GPIOG->IDR & GPIO_Pin_8)!= (uint32_t)Bit_RESET
	#define G6() (GPIOG->IDR & GPIO_Pin_6)!= (uint32_t)Bit_RESET
	//PB7 Input
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOG,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ;  //
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	LED_ON();
	//GPIOB->IDR & GPIO_Pin
}

int main(void)
{
	u8 display[16];
	u32 i,j,k;
	u16 packets=0;
	u8 status;
	u16 adc_value;
	u32 temp;
	u32 count=0;
	u32 durtion=0;
	
	u8 send_buffer[32];
	u16 sampleTime;
	USART1_Config(256000);
	Systick_Config();
	Adc_Init();
	Button_Init();
	SPI_NRF_Init();
	LCD_begin();
	clean();
	delay_ms(100);
	LCD_setCursor(0,0);
	sprintf(display,"%c %d",A_node_mac,A_node_number);
	LCD_print(display,3);
	//Tim3_Init(1000);

	//below is low transfer part
	//set freq,set node_number
	//any two node transfer ,each two show node number currently in communication
	//transfer and show three node number
	while(1)
	{
		if(G6()==0)break;
		if(G7()==0&&G8()==0)
		{
			printf("Tx to macA\r\n");
			NRF_TX_Mode(left_addr);
			txbuf[0]=my_node_number;//From
			txbuf[1]=my_node_mac;
			txbuf[2]=A_node_number;//测试使用中间向左边发送
			txbuf[3]=A_node_mac;
			status = NRF_Tx_Dat(txbuf);
			if(status==TX_DS)
			{
				printf("Success send to %c,his id: %d\r\n",txbuf[3],txbuf[2]);
				sprintf(display,"Sent to %d success ",A_node_number);
				LCD_setCursor(1,0);
				LCD_print(display,14);
			}
			else
			{
				printf("Send fail\r\n");
				sprintf(display,"Sent to %d failed    ",A_node_number);
				LCD_setCursor(1,0);
				//LCD_print(display,14);
			}
			delay_ms(50);
		}
		else if(G7()==0&&G8()==1)
		{
			printf("Tx to macB\r\n");
			NRF_TX_Mode(right_addr);
			txbuf[0]=my_node_number;//From
			txbuf[1]=my_node_mac;
			txbuf[2]=B_node_number;//测试使用中间向左边发送
			txbuf[3]=B_node_mac;
			status = NRF_Tx_Dat(txbuf);
			if(status==TX_DS)
			{
				printf("Success send to %c,his id: %d\r\n",txbuf[3],txbuf[2]);
				sprintf(display,"Sent to %d success ",B_node_number);
				LCD_setCursor(1,0);
				LCD_print(display,14);
			}
			else
			{
				printf("Send fail\r\n");
				sprintf(display,"Sent to %d failed    ",B_node_number);
				LCD_setCursor(1,0);
///LCD_print(display,14);
			}
			delay_ms(50);
		}
		else if(G7()==1&&G8()==0)
		{
			printf("Tx to macC\r\n");
			NRF_TX_Mode(middle_addr);
			txbuf[0]=my_node_number;//From
			txbuf[1]=my_node_mac;
			txbuf[2]=C_node_number;//测试使用中间向左边发送
			txbuf[3]=C_node_mac;
			status = NRF_Tx_Dat(txbuf);
			if(status==TX_DS)
			{
				printf("Success send to %c,his id: %d\r\n",txbuf[3],txbuf[2]);
				sprintf(display,"Sent to %d success ",C_node_number);
				LCD_setCursor(1,0);
				LCD_print(display,14);
			}
			else
			{
				printf("Send fail\r\n");
				sprintf(display,"Sent to %d failed    ",C_node_number);
				LCD_setCursor(1,0);
				//LCD_print(display,14);
			}
			delay_ms(50);
		}
		else if(G7()==1&&G8()==1)
		{
			printf("Rx\r\n");
			my_frequency=17;
			my_node_number=1;
			NRF_RX_Mode_With_Frequency(left_addr,my_frequency);
			status = NRF_Rx_Dat(rxbuf,100000);
			
			LCD_setCursor(1,0);
			LCD_print("Rx mode   ",14);
			if(status==RX_DR)
			{
				printf("Recv from %c ,his id: %d\r\n",rxbuf[1],rxbuf[0]);
				sprintf(display,"Recv from %d      ",rxbuf[0]);
				LCD_setCursor(1,0);
				LCD_print(display,14);
				delay_ms(200);
			}
		}
	}
	
	//below is fast transfer part
	while(1)
	{
		node_number_mask=(((u16)my_node_number)<<12)|0x0fff;
		LED_ON();
		TX_PLOAD_WIDTH=2;
		RX_PLOAD_WIDTH=2;
		NRF_TX_Mode_No_Ack(right_addr);
		while(Read_Button());
		printf("P2P transfer\r\n");
		delay_ms(1000);
		while(1)
		{
			u16 adc_value=Get_Adc();
			*(u16 *)send_buffer=adc_value&node_number_mask;
			status=NRF_Tx_Dat(send_buffer);	
			if(Read_Button())
					break;
		}
		printf("End!\r\n");
		delay_ms(1000);

		
		LED_OFF();
		printf("Transfer mode\r\n");
		TX_PLOAD_WIDTH=32;
		RX_PLOAD_WIDTH=32;
		NRF_TX_Mode(middle_addr);
		while(1)
		{
			printf("Record to ram!\r\n");
			//below is going to record 12s sample data
			
			sampleTime=333;//us
			for(i=0;i<ram_max_address;i++)
			{
				adc_value=Get_Adc();
				ram[i]=adc_value>>4;
				delay_us(sampleTime);
				if(Read_Button())
							goto OUT;
			}
			
			delay_ms(1000);
			printf("Finish recording,begin transfer!\r\n");
			k=ram_max_address/30;
			k=1200;
			for(i=0;i<k;i++)
			{
				for(j=0;j<30;j++)
				{
					txbuf[j]=ram[i*30+j];
				}
				if(i==0)
				{
					txbuf[30]=0x00;
					txbuf[31]=0xf1;
				}
				else if(i==k-1)
				{
					txbuf[30]=0x00;
					txbuf[31]=0xf2;
				}
				else
				{
					txbuf[30]=0x00;
					txbuf[31]=0xff;
				}
				if(Read_Button())
							goto OUT;
				status = NRF_Tx_Dat(txbuf);	
				printf("[%d]\r\n",status==TX_DS);
				delay_us(2000);
			}
			printf("Finish transfering!\r\n");
			delay_ms(1000);//延迟一秒钟留给接收方
		}
		OUT:;
	}
	
	printf("Return 0\r\n");
	return 0;
}

