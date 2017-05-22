#include "stm32f10x.h"
#include "timer3.h"
#include "bsp_uart.h"
#include "other.h"
#include "adc.h"
#include "bsp_spi_nrf.h"
#include "dac.h"
#include "lcd.h"

#define Set_Dac_Val(x)  *(__IO uint32_t *)dac_address = x
extern u32 millis;
extern u32 counter_value;
extern __IO uint32_t dac_address ;

u8 ram[36000];
u32 ram_max_address=36000;
u32 ram_pointer=0;

u8 status;	//用于判断接收/发送状态
u8 txbuf[32]={0,1,2,3};	 //发送缓冲
u8 rxbuf[32];			 //接收缓冲
int i=0;
u8 my_addr[]= {0x12,0x34,0x56,0x78,0x00};
u8 his_addr[]={0x12,0x34,0x56,0x78,0x01};

u8 left_addr[]={0x12,0x34,0x56,0x78,0x01};
u8 middle_addr[]={0x12,0x34,0x56,0x78,0x02};
u8 right_addr[]={0x12,0x34,0x56,0x78,0x00};

u8 my_node_number=0x00;
u16 node_number_mask;
u8 my_node_mac='B';

extern int TX_PLOAD_WIDTH ;
extern int RX_PLOAD_WIDTH ;


u8 A_node_mac='A';
u8 B_node_mac='B';
u8 C_node_mac='C';

u8 A_node_number=1;
u8 B_node_number=0;
u8 C_node_number=2;

u16 node_number_mask;

u8 my_frequency=17;

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
	
	printf("\r\n 这是一个 NRF24L01 无线传输实验 \r\n");
  printf("\r\n 这是无线传输 从机端 的反馈信息\r\n");
  printf("\r\n   正在检测NRF与MCU是否正常连接。。。\r\n");

  /*检测NRF模块与MCU的连接*/
  status = NRF_Check();   		
  if(status == SUCCESS)	   
    printf("\r\n      NRF与MCU连接成功\r\n");  
  else	  
    printf("\r\n   正在检测NRF与MCU是否正常连接。。。\r\n");
	
		
    printf("\r\n 从机端 进入 死循环接受状态"); 
    NRF_RX_Mode(right_addr);
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
	
	
	printf("\r\n   正在检测NRF与MCU是否正常连接。。。\r\n");
  status = NRF_Check(); 
  if(status == SUCCESS)	   
    printf("\r\n      NRF与MCU连接成功！\r\n");  
  else	  
    printf("\r\n  NRF与MCU连接失败，请重新检查接线。\r\n");
	
	NRF_TX_Mode(his_addr);
	
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
	u8 rx_buffer[32];
	u16 temp;
	u32 duration;
	u32 packet_count;
	u8 node_number=0xff;
	float volts;
	USART1_Config(256000);
	Systick_Config();
	Adc_Init();
	Button_Init();
	SPI_NRF_Init();
	//Tim3_Init(1000);
	Dac1_Init();
	//NRF_RX_Mode(my_addr);
	
	printf("Ready to receive!\r\n");
  Set_Dac_Val(0);
	//LCD1602_Init();
	packet_count=0;
	
	LCD_begin();
	clean();
	delay_ms(100);
	LCD_setCursor(0,0);
	sprintf(display,"%c %d",A_node_mac,A_node_number);
	LCD_print(display,3);
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
				//LCD_print(display,14);
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
			my_node_number=0;
			NRF_RX_Mode_With_Frequency(right_addr,my_frequency);
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
		LED_ON();
			printf("Change to p2p mode\r\n");
			delay_ms(1000);
			TX_PLOAD_WIDTH=2;
			RX_PLOAD_WIDTH=2;
			NRF_RX_Mode_No_Ack(right_addr);
			printf("P2P receive!\r\n");
			
			while(1)
			{
				u8 status=NRF_Rx_Dat(rx_buffer,100000);
				if(status==RX_DR)//receive true data
				{
						temp=(*(u16 *)rx_buffer)&0x0fff;
						node_number=(*(u16 *)rx_buffer)>>12;
						Set_Dac_Val(temp);
				}
				if(Read_Button())
						break;
			}
			delay_ms(1000);
			while(Read_Button());
			
			
LED_OFF();
			printf("Change to transfer mode!\r\n");
			TX_PLOAD_WIDTH=32;
			RX_PLOAD_WIDTH=32;
			NRF_RX_Mode(right_addr);
			while(1)
			{
				while(1) 
				{
					if(Read_Button())
								goto OUT;
					status = NRF_Rx_Dat(rxbuf,100000);
					if(status == RX_DR)//收到有效数据包
					{
						if(rxbuf[31]==0xf1)//start packet
						{
							ram_pointer=0;
						}
						for(i=0;i<30;i++)
						{
							if(ram_pointer>=ram_max_address)goto outwhile;
							ram[ram_pointer]=rxbuf[i];
							ram_pointer++;
						}
						if(rxbuf[31]==0xf2)//end packet
						{
							break;
						}
						if(Read_Button())
								goto OUT;
					}
				} 
				outwhile:;
				printf("Output dac\r\n");
				for(i=0;i<ram_max_address;i++)
				{
					u16 tmp=((u16)ram[i])<<4;
					Set_Dac_Val(tmp);
					delay_us(333);
					if(Read_Button())
								goto OUT;
				}
				printf("Finish dac\r\n");
			}
			OUT:;
			
	}
	
}
