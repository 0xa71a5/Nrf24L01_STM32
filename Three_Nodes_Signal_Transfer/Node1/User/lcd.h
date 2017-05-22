#ifndef LCD_H
#define LCD_H

#define uchar unsigned char
#define uint  unsigned int

#define  LCD_RS_HIGH   GPIO_SetBits(GPIOC, GPIO_Pin_0)
#define  LCD_RS_LOW    GPIO_ResetBits(GPIOC, GPIO_Pin_0)
#define  LCD_RW_HIGH   GPIO_SetBits(GPIOC, GPIO_Pin_1)
#define  LCD_RW_LOW    GPIO_ResetBits(GPIOC, GPIO_Pin_1)
#define  LCD_EN_HIGH   GPIO_SetBits(GPIOC, GPIO_Pin_2)
#define  LCD_EN_LOW    GPIO_ResetBits(GPIOC, GPIO_Pin_2)
#define  LCD_DATA      GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7
					   
#define ONE 0x0080       //1602LCD第1行
#define TWO 0x00C0       //第二行
#define CLEAR 0x0001    //清屏   
void GPIO_Configuration(void);
void RCC_Configuration(void);
void LCD1602_writeCom(uchar com);
void LCD1602_writeData(uchar wdata);
void LCD1602_init(void);
//void LCD1602_initWriteData(void);
void LCD_print(uchar DAT[],uint n);
void LCD_setCursor(uint row,uint col);

void LCD_begin(void);
void clean(void);

#endif
