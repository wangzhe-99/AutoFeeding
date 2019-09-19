/**
  ******************************************************************************
  * @file    serialport.c
  * @author  乐易住
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   串口硬件封装
  ******************************************************************************
  * @attention
  * 串口硬件中间层
  * 
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/  
#include "serialport.h"
#include "main.h"
#include "usart.h"
#include "wifi.h"

/**
  * @brief  串口接收数据处理过程
  *			硬件的串口接收到数据后，转发到相应的处理函数中。
  * @param  None
  * @retval None
  */
void HAL_SerialPort_Process(void)
{

	if(USART5_RX_STA&0x8000)
	{
        USART5_RX_BUF[USART5_RX_STA&0x7FFF] = 0;        
		HAL_Wifi_SetRecData(USART5_RX_STA&0x7FFF, USART5_RX_BUF);

		USART5_RX_STA = 0;
		memset(USART5_RX_BUF, 0, USART_REC_LEN);
	}

	/*if(USART4_RX_STA&0x8000)
	{
        USART4_RX_BUF[USART4_RX_STA&0x7FFF] = 0;        
		//HAL_RS485_SetRecData(USART4_RX_STA&0x7FFF, USART4_RX_BUF);

		USART4_RX_STA = 0;
		memset(USART4_RX_BUF, 0, USART_REC_LEN);
	}*/
}

