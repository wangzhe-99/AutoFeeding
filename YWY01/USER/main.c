/**
  ******************************************************************************
  * @file    main.c
  * @author  ����ס
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


/*ϵͳȫ�ֱ���----------------------------------------------------------------*/
u8 DEVICE_ID[4];						//�豸ID
u8 WIfI_AP_SSID[WIFIAPSSIDLEN];			//WIFI AP SSID
u8 WIfI_AP_PASSWORD[WIFIAPPASSWORDLEN];	//WIFI AP PassWord
u8 WIfI_SERVER_IP[WIFISERVERIPLEN];		//������IP��ַ
u8 WIfI_SERVER_PORT[WIFISERVERPORTLEN]; //�������˿ں�


/*ϵͳ��̬����----------------------------------------------------------------*/
static u8 Key_Down_Num = 0;
static u32 Key_Down_Long=0;


/**
  * @brief  ���������жϡ��������ӣ�ϵͳ����
  * @param  None
  * @retval None
  */
void Key_Down_End()
{
	//�����������
	if(Key_Down_Num >= 2)
	{
		DEBUG_PRINTF(" btn db down");

		System_Reset();
	}

	Key_Down_Num = 0;
}

/**
  * @brief  ���������жϡ���������wifiָ������ñ��ط�����
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
  * @brief  �����жϴ�����
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
  * @brief  GPIO��ʼ����ȫ����ʼ��Ϊģ������
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
	//ϵͳʱ�ӵȳ�ʼ��
	SystemInit();

	//����NVIC�жϷ���4:4λ��ռ���ȼ���2λ��Ӧ���ȼ�
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	

	//��ʱ��ʼ��
	delay_init(72);

	//GPIO��ʼ��
	Goip_All_Init();

	//��ʱ����ʼ��
	Timer_Init();

	//���ڳ�ʼ��
	uart_init();

	//������ʼ��
	Key_Init();

	//LED��ʼ��
	Led_Init();

	//EPROM��ʼ��
	AT24CXX_Init();	

	//��Ϣ���ݳ�ʼ��
	UserMsg_Init();

	delay_s(5);
	
	//ϵͳ��Ϣ��ʼ��
	EPFile_Init();


	delay_s(2);

    //WIFIģ���ʼ��
	HAL_Wifi_Init(WIfI_AP_SSID, WIfI_AP_PASSWORD, WIfI_SERVER_IP, 
					WIfI_SERVER_PORT, DEVICE_ID, USART5_Send_Data, Protocol_SerToRCU_Data);

	DEBUG_PRINTF("system start");	

	/* Infinite loop */
	while(1)
	{
		//�������ݴ���
		HAL_SerialPort_Process();

		//WIFI���ݴ���
		HAL_Wifi_Process();		

		//Э�����ݴ���
		Protocol_Process();
			
		//wifi���ݷ�������
		HAL_Wifi_SendProcess();

		//��Ϣ����
		UserMsg_Process();

		//��ʱ������
		Timer_Msg();
	}
}

/**
  * @brief  ϵͳ����
  * @param  None
  * @retval None
  */
void System_Reset(void)
{
	__set_FAULTMASK(1); 	 // �ر������ж�
	
	WIfi_Device_Reset();
	delay_s(1);
	
	NVIC_SystemReset();// ��λ
}


/************************END OF FILE**********************************************/
