/**
  ******************************************************************************
  * @file    usermsg.c
  * @author  乐易住
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *用户自定义消息
  * 
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "usermsg.h"
#include "relay.h"
#include "timer.h"
#include "wifi.h"
#include "usart.h"
#include "protocol.h"
#include "led.h"
#include "epfile.h"

/*系统静态变量----------------------------------------------------------------*/
static USERMSGINFO User_Msg_Info[USERMSGNUM];

/**
  * @brief  添加新的消息
  * @param  UMID：消息ID
  * @retval None
  */
void UserMsg_LockOpen()
{

}


/**
  * @brief  添加新的消息
  * @param  UMID：消息ID
  *			pData：消息中的数据buffer
  *			len：数据buffer中的有效长度
  * @retval None
  */
void UserMsg_Add(u16 UMID, u8 *pData, u8 len)
{
	u8 i;

	for (i=0; i<USERMSGNUM; i++)
	{
		if(User_Msg_Info[i].isUse == 0)
		{
			User_Msg_Info[i].isUse = 1;
			User_Msg_Info[i].ID = UMID;
			User_Msg_Info[i].len = len;
			
			if(len != 0)
			{
				memcpy(User_Msg_Info[i].data, pData, len);
			}

			break;
		}
	}	
}

/**
  * @brief  消息处理函数
  * @param  None
  * @retval None
  */
void UserMsg_Process(void)
{
	u8 i;

	for (i=0; i<USERMSGNUM;  i++)
	{
		if(User_Msg_Info[i].isUse != 0)
		{
			if((User_Msg_Info[i].ID == UMSG_NULL) || (User_Msg_Info[i].ID == UMSG_END))
			{
				User_Msg_Info[i].isUse = 0;
				return;
			}

			switch(User_Msg_Info[i].ID)
			{
				//按键长按
				case UMSG_KEY_LONGDOWN:
					//Wifi_Led_Flashed_1S();
				
					WIfi_Device_Reset();
				
					delay_s(5);

					//重新设定wifi指向的服务器地址
					strcpy((char*)WIfI_AP_SSID, "LYZ-CS");
					strcpy((char*)WIfI_AP_PASSWORD, "147258369");
					strcpy((char*)WIfI_SERVER_IP, "192.168.1.116");
					strcpy((char*)WIfI_SERVER_PORT, "20001");
					
					HAL_Wifi_Init(WIfI_AP_SSID, WIfI_AP_PASSWORD, WIfI_SERVER_IP, 
									WIfI_SERVER_PORT, DEVICE_ID, USART5_Send_Data, Protocol_SerToRCU_Data);
					break;
			}

			User_Msg_Info[i].isUse = 0;
		}		
	}
}

/**
  * @brief  消息数据初始化
  * @param  None
  * @retval None
  */
void UserMsg_Init(void)
{
	u8 i;

	for (i=0; i<USERMSGNUM; i++)
	{
		User_Msg_Info[i].isUse = 0;
	}
}
