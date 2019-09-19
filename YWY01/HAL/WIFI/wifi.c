/**
  ******************************************************************************
  * @file    wifi.c
  * @author  ����ס
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   wifiģ���װ
  ******************************************************************************
  * @attention
  * 
  * 
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/  
#include "delay.h"
#include "wifi.h"
#include "main.h"

/*��̬����----------------------------------------------------------------*/
static u8 Wifi_Buf[WIFIBUFFLEN];
static WIFIWORKMODE	WIFI_WORK_MODE;
static WIFICMD 	Wifi_Cmd;
static WIFIBUFF	Wifi_Passthough_Data;
static WIFIBUFF	Wifi_Send_Data[3];
static WIFIBUFF	Wifi_Recive_Data;
static u8		Wifi_Error_Num;
static u8		*p_Wifi_AP_SSID;
static u8		*p_Wifi_AP_PASSWORD;
static u8		*p_Wifi_SERVER_IP;
static u8		*p_Wifi_SERVER_PORT;
static u8		*p_Wifi_Heartbeat_Data;
static void		(*pf_Hal_Wifi_Send_Data)(u8 *pbuf, u8 len);
static void		(*pf_Hal_Wifi_Passthough_Data_Procss)(u8 *pData, u8 len);

/**
  * @brief  ���뵽͸������״̬
  * @param  None
  * @retval None
  */
void Wifi_Cmd_PASSTHROUGH()
{
	if(Wifi_Passthough_Data.len != 0)
	{
		//������
		Wifi_Passthough_Data.data[Wifi_Passthough_Data.len] = 0x0D;
		Wifi_Passthough_Data.data[Wifi_Passthough_Data.len+1] = 0x0A;
		Wifi_Passthough_Data.data[Wifi_Passthough_Data.len+2] = 0x0D;
		Wifi_Passthough_Data.data[Wifi_Passthough_Data.len+3] = 0x0A;
	
		pf_Hal_Wifi_Send_Data(Wifi_Passthough_Data.data, Wifi_Passthough_Data.len+4);

		Wifi_Passthough_Data.len = 0;
		
		WIFI_WORK_MODE = WIFI_WORK_PASSTHROUGH;
	}
}

/**
  * @brief  �趨͸����������
  * @param  None
  * @retval None
  */
void Wifi_Cmd_CIPSEND(void)
{
	sprintf((char*)Wifi_Buf, "AT+CIPSEND");
	
	Wifi_SendCMD(Wifi_Buf, 2000, 5);

	WIFI_WORK_MODE = WIFI_CMD_CIPSEND;
}

/**
  * @brief  �趨͸����������
  * @param  None
  * @retval None
  */
void Wifi_Cmd_CIPMODE(void)
{	
	sprintf((char*)Wifi_Buf, "AT+CIPMODE=1");
	
	Wifi_SendCMD(Wifi_Buf, 2000, 5);

	WIFI_WORK_MODE = WIFI_CMD_CIPMODE;
}


/**
  * @brief  ���ӷ�����
  * @param  None
  * @retval None
  */
void Wifi_Cmd_CIPSTART(void)
{	
	sprintf((char*)Wifi_Buf, "AT+CIPSTART=\"TCP\",\"%s\",%s", 
				p_Wifi_SERVER_IP, p_Wifi_SERVER_PORT);
	
	Wifi_SendCMD(Wifi_Buf, 2000, 5);	

	WIFI_WORK_MODE = WIFI_CMD_CIPSTART;
}

/**
  * @brief  ����AP
  * @param  None
  * @retval None
  */
void Wifi_Cmd_CWJAP(void)
{
	sprintf((char*)Wifi_Buf, "AT+CWJAP=\"%s\",\"%s\"", 
				p_Wifi_AP_SSID, p_Wifi_AP_PASSWORD);    

	Wifi_SendCMD(Wifi_Buf, 1000, 5);

	WIFI_WORK_MODE = WIFI_CMD_CWJAP;
}

/**
  * @brief  �趨wifiģ��Ĺ���ģʽ
  * @param  None
  * @retval None
  */
void Wifi_Cmd_CWMODE(void)
{	
	Wifi_SendCMD("AT+CWMODE=1", 50, 1);

	WIFI_WORK_MODE = WIFI_CMD_CWMODE;	
}
/**
  * @brief  �ر�ģ��Ļ��Թ���
  * @param  None
  * @retval None
  */
void Wifi_Cmd_ATE0(void)
{
	Wifi_SendCMD("ATE0", 50, 1);
	
	WIFI_WORK_MODE = WIFI_CMD_ATE;
}

/**
  * @brief  ģ�����Ӳ���ָ��
  * @param  None
  * @retval None
  */
void Wifi_Cmd_AT(void)
{
	Wifi_SendCMD("AT", 50, 5);
	
	WIFI_WORK_MODE = WIFI_CMD_AT;
}

/**
  * @brief  ��λ
  * @param  None
  * @retval None
  */
void Wifi_Cmd_RST(void)
{
	sprintf((char*)Wifi_Buf, "AT+RST");

	pf_Hal_Wifi_Send_Data(Wifi_Buf, strlen((char*)Wifi_Buf));
		
	delay_s(2);
}


/**
  * @brief  ����������
  * @param  None
  * @retval None
  */
void Wifi_Heartbeat_Process(void)
{
	if((WIFI_WORK_MODE == WIFI_WORK_PASSTHROUGH) &&			
		(Wifi_Passthough_Data.len == 0))
	{
		Wifi_Passthough_Data.len = 10;
		Wifi_Passthough_Data.data[0] = 0xFF;
		Wifi_Passthough_Data.data[1] = *p_Wifi_Heartbeat_Data;
		Wifi_Passthough_Data.data[2] = *(p_Wifi_Heartbeat_Data+1);
		Wifi_Passthough_Data.data[3] = *(p_Wifi_Heartbeat_Data+2);
		Wifi_Passthough_Data.data[4] = *(p_Wifi_Heartbeat_Data+3);
		Wifi_Passthough_Data.data[5] = 0x00;
		Wifi_Passthough_Data.data[6] = 0x02;
		Wifi_Passthough_Data.data[7] = 0xFE;
		Wifi_Passthough_Data.data[8] = 0x00;		
		Wifi_Passthough_Data.data[9] = Check_NC(Wifi_Passthough_Data.data, 9);

		Wifi_Cmd_PASSTHROUGH();
	}

	Set_Timer_Msg(TIMER_MSG_HEARTBEAT_PACKET, 3000, Wifi_Heartbeat_Process);
}

/**
  * @brief  wifi ģ�� ATָ���ʼ����������
  * @param  None
  * @retval None
  */
void Wifi_Cmd_End(void)
{
	DEBUG_PRINTF("WIFI init end");

	Clear_Timer_Msg(TIMER_MSG_WIFIREC);
	
	Wifi_Passthough_Data.len = 0;
	WIFI_WORK_MODE = WIFI_WORK_PASSTHROUGH;

	if(p_Wifi_Heartbeat_Data != NULL)
	{
		Set_Timer_Msg(TIMER_MSG_HEARTBEAT_PACKET, 1000, Wifi_Heartbeat_Process);
	}
}

/**
  * @brief  ���ڽ��յ����ݺ�Ĵ�������
  * @param  None
  * @retval None
  */
void HAL_Wifi_Process(void)
{
	char *strx=0;
	u8 len;

	if(Wifi_Recive_Data.len == 0)
	{
		return;		
	}	

	DEBUG_PRINTF("rec = %s", Wifi_Recive_Data.data);

	//TCP �Ͽ�����
	strx = strstr((const char*)Wifi_Recive_Data.data, "CLOSED");		
	if(strx != NULL)
	{
		DEBUG_PRINTF("rec CLOSED");
		Clear_Timer_Msg(TIMER_MSG_WIFIREC);

		Wifi_Error_Num++;
		if(Wifi_Error_Num > 10)
		{
			goto _hal_wifi_process_fun_end;
		}

		delay_s(1);
		Wifi_Cmd_CIPSTART();
		
		goto _hal_wifi_process_fun_end;
	}

	//TCP ���Ӵ���
	strx = strstr((const char*)Wifi_Recive_Data.data, "link is not valid");		
	if(strx != NULL)
	{
		DEBUG_PRINTF("rec link error");
		Clear_Timer_Msg(TIMER_MSG_WIFIREC);

		Wifi_Error_Num++;
		if(Wifi_Error_Num > 10)
		{
			goto _hal_wifi_process_fun_end;
		}

		delay_s(1);
		Wifi_Cmd_CIPSTART();

		Wifi_Error_Num++;

		goto _hal_wifi_process_fun_end;
	}

	//���յ�����������������
	if(WIFI_WORK_MODE == WIFI_WORK_PASSTHROUGH)
	{
		len = 9 + Wifi_Recive_Data.data[7];		
		
		DEBUG_PRINTF("strlen = %d", len);

		pf_Hal_Wifi_Passthough_Data_Procss(Wifi_Recive_Data.data, len);

		goto _hal_wifi_process_fun_end;
	}
	
	//͸����������
	if(WIFI_WORK_MODE == WIFI_CMD_CIPSEND)
	{
		strx = strstr((const char*)Wifi_Recive_Data.data, ">");
		if(strx != NULL)
		{
			DEBUG_PRINTF("PASSTHROUGH");
			Clear_Timer_Msg(TIMER_MSG_WIFIREC);
			Wifi_Cmd_End();
			
			//Wifi_Cmd_PASSTHROUGH();						
		}			

		goto _hal_wifi_process_fun_end;
	}

	if(WIFI_WORK_MODE == WIFI_CMD_AT)
	{
		strx = strstr((const char*)Wifi_Recive_Data.data, "OK");
		if(strx != NULL)
		{
			DEBUG_PRINTF("WIFI_CMD_AT");			
			
			Clear_Timer_Msg(TIMER_MSG_WIFIREC);
			Wifi_Cmd_ATE0();
		}

		goto _hal_wifi_process_fun_end;
	}
		
	if(WIFI_WORK_MODE == WIFI_CMD_ATE)
	{
		strx = strstr((const char*)Wifi_Recive_Data.data, "OK");
		if(strx != NULL)
		{
			DEBUG_PRINTF("WIFI_CMD_ATE");
			
			Clear_Timer_Msg(TIMER_MSG_WIFIREC);
			Wifi_Cmd_CWMODE();
		}

		goto _hal_wifi_process_fun_end;
	}

	if(WIFI_WORK_MODE == WIFI_CMD_CWMODE)
	{
		strx = strstr((const char*)Wifi_Recive_Data.data, "OK");
		if(strx != NULL)
		{
			DEBUG_PRINTF("WIFI_CMD_CWMODE");
			
			Clear_Timer_Msg(TIMER_MSG_WIFIREC);
			Wifi_Cmd_CIPMODE();
		}

		goto _hal_wifi_process_fun_end;
	}

	if(WIFI_WORK_MODE == WIFI_CMD_CIPSTART)
	{
		strx = strstr((const char*)Wifi_Recive_Data.data, "OK");
		if(strx != NULL)
		{
			DEBUG_PRINTF("WIFI_CMD_CIPSTART");
			
			Clear_Timer_Msg(TIMER_MSG_WIFIREC);
			Wifi_Cmd_CIPSEND();
			
		}

		goto _hal_wifi_process_fun_end;
	}

	if(WIFI_WORK_MODE == WIFI_CMD_CIPMODE)
	{
		strx = strstr((const char*)Wifi_Recive_Data.data, "OK");
		if(strx != NULL)
		{
			DEBUG_PRINTF("WIFI_CMD_CIPMODE");
		
			Clear_Timer_Msg(TIMER_MSG_WIFIREC);
			Wifi_Cmd_CWJAP();
		}

		goto _hal_wifi_process_fun_end;
	}
	
	if(WIFI_WORK_MODE == WIFI_CMD_CWJAP)
	{
		strx = strstr((const char*)Wifi_Recive_Data.data, "OK");
		if(strx != NULL)
		{
			DEBUG_PRINTF("WIFI_CMD_CWJAP");
		
			Clear_Timer_Msg(TIMER_MSG_WIFIREC);
			Wifi_Cmd_CIPSTART();
		}

		goto _hal_wifi_process_fun_end;
	}

_hal_wifi_process_fun_end:	

	if(Wifi_Error_Num > 10)
	{
		DEBUG_PRINTF("Wifi_Error_Num: %d", Wifi_Error_Num);

		WIFI_WORK_MODE = WIFI_WORK_NULL;
		Wifi_Error_Num = 0;
        
		Clear_Timer_Msg(TIMER_MSG_WIFIREC);
		
		Set_Timer_Msg(TIMER_MSG_OVERTIME_RESET, 6000, HAL_Reset_Wifi);

	}

	Wifi_Recive_Data.len = 0;
	memset(Wifi_Recive_Data.data, 0, WIFIBUFFLEN);
}

/**
  * @brief  wifi������������
  * @param  None
  * @retval None
  */
void HAL_Wifi_SendProcess(void)
{
	u8 i;
	
	if((WIFI_WORK_MODE == WIFI_WORK_PASSTHROUGH) &&	(Wifi_Passthough_Data.len == 0))
	{
		for (i=0; i<3;i++)
		{
			if(Wifi_Send_Data[i].len != 0)
			{
				Wifi_Passthough_Data.len = Wifi_Send_Data[i].len;
				memcpy(Wifi_Passthough_Data.data, Wifi_Send_Data[i].data, Wifi_Send_Data[i].len);

				Wifi_Cmd_PASSTHROUGH();

				Wifi_Send_Data[i].len = 0;

				return;
			}
		}
	}
}


/**
  * @brief  wifi ����ATָ��󣬳�ʱ������
  *			�ú������ж�ATָ����ط������������ط�������������wifiģ��
  * @param  None
  * @retval None
  */
void Wifi_Wait_Rec_Timer_CallBack(void)
{
	DEBUG_PRINTF("%s overtime", Wifi_Cmd.send);

	//�����ط�����
	if(Wifi_Cmd.Num == 0x00)
	{
		DEBUG_PRINTF_ERROR("%s ERROR", Wifi_Cmd.send);

		Clear_Timer_Msg(TIMER_MSG_WIFI_LED_FLASHED);

		Set_Timer_Msg(TIMER_MSG_OVERTIME_RESET, 6000, HAL_Reset_Wifi);
		
		return;
	}

	//�ظ���ǰwifiָ��
	sprintf((char*)Wifi_Buf, "%s\r\n", Wifi_Cmd.send);
	pf_Hal_Wifi_Send_Data(Wifi_Buf, strlen((const char*)Wifi_Buf)); //��������

	Set_Timer_Msg(TIMER_MSG_WIFIREC, Wifi_Cmd.timer, Wifi_Wait_Rec_Timer_CallBack);

	//����ط�����Ϊ0xFF�������޴��ط�
	if(Wifi_Cmd.Num != 0xFF )
	{
		Wifi_Cmd.Num--;
	}
}

/**
  * @brief  wifiģ��Ӳ������
  * @param  None
  * @retval None
  */
void WIfi_Device_Reset(void)
{
	//��λWIFIģ��
	GPIO_ResetBits(GPIOE, GPIO_Pin_15);
	delay_ms(10);
	GPIO_SetBits(GPIOE, GPIO_Pin_15);
}

/**
  * @brief  ATָ���
  *			���ATָ��ͷ��ͺ��ڵȴ�ʱ���ڣ�δ�յ���Ӧ�������ط���ATָ��
  * @param  pcmd��ָ������
  *			waittime��ģ����Ӧ�ȴ�ʱ��
  *			num���ط�������
  * @retval None
  */
void Wifi_SendCMD(u8 *pcmd, u32 waittime, u8 num)
{	
	DEBUG_PRINTF("cmd=%s", pcmd);
	
	strcpy((char*)(Wifi_Cmd.send), (const char*)pcmd);
	Wifi_Cmd.timer = waittime;
	Wifi_Cmd.Num = num;

	sprintf((char*)Wifi_Buf, "%s\r\n", Wifi_Cmd.send);
	pf_Hal_Wifi_Send_Data(Wifi_Buf, strlen((const char*)Wifi_Buf)); //��������

	Set_Timer_Msg(TIMER_MSG_WIFIREC, Wifi_Cmd.timer, Wifi_Wait_Rec_Timer_CallBack);
} 

/**
  * @brief  wifiģ���ʼ��
  * @param  pSSID��wifi AP ssid
  *			pPassWord��wifi AP ����
  *			pServerIP��������IP��ַ
  *			pServerPort���������˿ں�
  *			pHeartData����������
  *			pf_SendData���������ݺ���
  *			pf_Passthough_data���������ݴ�����  
  * @retval None
  */
void HAL_Wifi_Init(u8 *pSSID, u8 *pPassWord, u8 *pServerIP, u8 *pServerPort, 
						u8* pHeartData, void(*pf_SendData)(u8 *pbuf, u8 len),
						void(*pf_Passthough_data)(u8 *pbuf, u8 len))
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	DEBUG_PRINTF("WIFI init start");	

	//RESET��λ��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE, GPIO_Pin_15);

	//WIFI �������ݳ�ʼ��	
	p_Wifi_AP_SSID 						= pSSID;
	p_Wifi_AP_PASSWORD					= pPassWord;
	p_Wifi_SERVER_IP					= pServerIP;
	p_Wifi_SERVER_PORT					= pServerPort;
	p_Wifi_Heartbeat_Data				= pHeartData;
	pf_Hal_Wifi_Send_Data				= pf_SendData;
	pf_Hal_Wifi_Passthough_Data_Procss 	= pf_Passthough_data;

	//WIFIģ���ʼ��
	Wifi_Cmd_RST();
	Wifi_Cmd_AT();
}

/**
  * @brief  wifiģ������
  * @param  None
  * @retval None
  */
void HAL_Reset_Wifi(void)
{	
	WIfi_Device_Reset();
	
	delay_s(10);

	Wifi_Cmd_AT();
}

/**
  * @brief  wifiģ���������
  * @param  len���������ݵĳ���
  *			pData������
  * @retval None
  */
void HAL_Wifi_SetRecData(u8 len, u8* pData)
{

	Wifi_Recive_Data.len = len;

	memset(Wifi_Recive_Data.data, 0, WIFIBUFFLEN);

	memcpy(Wifi_Recive_Data.data, pData, len);
}

/**
  * @brief  wifi ��������
  * @param  None
  * @retval None
  */
void HAL_Wifi_SendData(u8* pData, u8 len)
{
	u8 i;
	
	for (i=0; i<3;i++)
	{
		if(Wifi_Send_Data[i].len == 0)
		{
			Wifi_Send_Data[i].len = len;
			memcpy(Wifi_Send_Data[i].data, pData, len);

			return;
		}
	}

	DEBUG_PRINTF("WIFI busy");
}




