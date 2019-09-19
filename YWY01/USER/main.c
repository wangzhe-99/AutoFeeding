/**
  ******************************************************************************
  * @file    main.c
  * @author  乐易住
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "timer.h"
#include "relay.h"
#include "key.h"
#include "led.h"
#include "24cxx.h"
#include "epfile.h"
#include "wifi.h"
#include "serialport.h"
#include "protocol.h"
#include "serrcu.h"
#include "usermsg.h"


/*系统全局变量----------------------------------------------------------------*/
u8 DEVICE_ID[4];						//设备ID
u8 WIfI_AP_SSID[WIFIAPSSIDLEN];			//WIFI AP SSID
u8 WIfI_AP_PASSWORD[WIFIAPPASSWORDLEN];	//WIFI AP PassWord
u8 WIfI_SERVER_IP[WIFISERVERIPLEN];		//服务器IP地址
u8 WIfI_SERVER_PORT[WIFISERVERPORTLEN]; //服务器端口号


/*系统静态变量----------------------------------------------------------------*/
static u8 Key_Down_Num = 0;
static u32 Key_Down_Long=0;


/**
  * @brief  按键连击判断。按键连接，系统重启
  * @param  None
  * @retval None
  */
void Key_Down_End()
{
	//如果按键连击
	if(Key_Down_Num >= 2)
	{
		DEBUG_PRINTF(" btn db down");

		System_Reset();
	}

	Key_Down_Num = 0;
}

/**
  * @brief  按键长按判断。按键长按wifi指向调试用本地服务器
  * @param  None
  * @retval None
  */

void Key_Long_Press()
{
	if(IS_KEY_DOWN == 0)
	{
		Key_Down_Long++;

		if(Key_Down_Long > 50)
		{
			DEBUG_PRINTF(" btn long press");

			UserMsg_Add(UMSG_KEY_LONGDOWN, NULL, 0);

			Key_Down_Long = 0;

			return;
		}

		Set_Timer_Msg(TIMER_MSG_RESETKEY_LONGPRESS, 5, Key_Long_Press);
	}
	else
	{
		Key_Down_Long = 0;
	}
}

/**
  * @brief  按键中断处理函数
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
	delay_ms(20);
	
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)
	{	
		if(IS_KEY_DOWN == 0)
		{
			DEBUG_PRINTF("btn down");
						
			if(Key_Down_Num == 0)
			{
				Set_Timer_Msg(TIMER_MSG_RESETKEY, 100, Key_Down_End);
				Set_Timer_Msg(TIMER_MSG_RESETKEY_LONGPRESS, 5, Key_Long_Press);
			}
			
			Key_Down_Num++;
			
		}
		
		EXTI_ClearITPendingBit(EXTI_Line6);		
	}
}

/**
  * @brief  GPIO初始化。全部初始化为模拟输入
  * @param  None
  * @retval None
  */
void Goip_All_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;		
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_All ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{	
	//系统时钟等初始化
	SystemInit();

	//设置NVIC中断分组4:4位抢占优先级，2位响应优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	

	//延时初始化
	delay_init(72);

	//GPIO初始化
	Goip_All_Init();

	//定时器初始化
	Timer_Init();

	//串口初始化
	uart_init();

	//按键初始化
	Key_Init();

	//LED初始化
	Led_Init();

	//EPROM初始化
	AT24CXX_Init();	

	//消息数据初始化
	UserMsg_Init();

	delay_s(5);
	
	//系统信息初始化
	EPFile_Init();


	delay_s(2);

    //WIFI模块初始化
	HAL_Wifi_Init(WIfI_AP_SSID, WIfI_AP_PASSWORD, WIfI_SERVER_IP, 
					WIfI_SERVER_PORT, DEVICE_ID, USART5_Send_Data, Protocol_SerToRCU_Data);

	DEBUG_PRINTF("system start");	

	/* Infinite loop */
	while(1)
	{
		//串口数据处理
		HAL_SerialPort_Process();

		//WIFI数据处理
		HAL_Wifi_Process();		

		//协议数据处理
		Protocol_Process();
			
		//wifi数据发送流程
		HAL_Wifi_SendProcess();

		//消息处理
		UserMsg_Process();

		//定时器处理
		Timer_Msg();
	}
}

/**
  * @brief  系统重启
  * @param  None
  * @retval None
  */
void System_Reset(void)
{
	__set_FAULTMASK(1); 	 // 关闭所有中端
	
	WIfi_Device_Reset();
	delay_s(1);
	
	NVIC_SystemReset();// 复位
}


/************************END OF FILE**********************************************/
