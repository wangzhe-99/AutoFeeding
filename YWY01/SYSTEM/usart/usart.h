#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART1_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
#define USART_REC_LEN  			64  	//�����������ֽ��� 200
#define EN_USART1_RX 			0		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART2_RX 			0		//ʹ�ܣ�1��/��ֹ��0������2����
#define EN_USART3_RX 			0		//ʹ�ܣ�1��/��ֹ��0������3����
#define EN_USART4_RX 			1		//ʹ�ܣ�1��/��ֹ��0������3����
#define EN_USART5_RX 			1		//ʹ�ܣ�1��/��ֹ��0������3����



#if EN_USART1_RX	  	
extern u8  USART1_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART1_RX_STA;         		//����״̬���	
void uart1_init(u32 bound);


#endif

#if EN_USART2_RX
extern u8 USART2_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u16 USART2_RX_STA;       //����״̬���	  
void uart2_init(u32 bound);
void USART2_Send_Data(u8 *buf,u8 len);

#endif

#if EN_USART3_RX
extern u8 USART3_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u16 USART3_RX_STA;       //����״̬���	  
void uart3_init(u32 bound);
void USART3_Send_Data(u8 *buf,u8 len);

#endif

#if EN_USART4_RX
extern u8 USART4_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u16 USART4_RX_STA;       //����״̬���	  
void uart4_init(u32 bound);
void USART4_Send_Data(u8 *buf,u8 len);
void USART4_Set_BaudRate(u32 baud);
#endif

#if EN_USART5_RX
extern u8 USART5_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u16 USART5_RX_STA;       //����״̬���	  
void uart5_init(u32 bound);
void USART5_Send_Data(u8 *buf,u8 len);
void USART5_Set_BaudRate(u32 baud);
#endif

void uart_init(void);
#endif


