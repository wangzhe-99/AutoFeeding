#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART1_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
#define USART_REC_LEN  			64  	//定义最大接收字节数 200
#define EN_USART1_RX 			0		//使能（1）/禁止（0）串口1接收
#define EN_USART2_RX 			0		//使能（1）/禁止（0）串口2接收
#define EN_USART3_RX 			0		//使能（1）/禁止（0）串口3接收
#define EN_USART4_RX 			1		//使能（1）/禁止（0）串口3接收
#define EN_USART5_RX 			1		//使能（1）/禁止（0）串口3接收



#if EN_USART1_RX	  	
extern u8  USART1_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART1_RX_STA;         		//接收状态标记	
void uart1_init(u32 bound);


#endif

#if EN_USART2_RX
extern u8 USART2_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
extern u16 USART2_RX_STA;       //接收状态标记	  
void uart2_init(u32 bound);
void USART2_Send_Data(u8 *buf,u8 len);

#endif

#if EN_USART3_RX
extern u8 USART3_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
extern u16 USART3_RX_STA;       //接收状态标记	  
void uart3_init(u32 bound);
void USART3_Send_Data(u8 *buf,u8 len);

#endif

#if EN_USART4_RX
extern u8 USART4_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
extern u16 USART4_RX_STA;       //接收状态标记	  
void uart4_init(u32 bound);
void USART4_Send_Data(u8 *buf,u8 len);
void USART4_Set_BaudRate(u32 baud);
#endif

#if EN_USART5_RX
extern u8 USART5_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
extern u16 USART5_RX_STA;       //接收状态标记	  
void uart5_init(u32 bound);
void USART5_Send_Data(u8 *buf,u8 len);
void USART5_Set_BaudRate(u32 baud);
#endif

void uart_init(void);
#endif


