/**
  ******************************************************************************
  * @file    wifi.h
  * @author  乐易住
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   wifi模块封装
  ******************************************************************************
  * @attention
  * 
  * 
  ******************************************************************************
  */
#ifndef _wifi_H
#define _wifi_H

#include "stm32f10x.h"
#include "sys.h"
#include "timer.h"

#define WIFIAPSSIDLEN		30		//wifi ssid长度
#define WIFIAPPASSWORDLEN	30		//wifi 密码长度
#define WIFISERVERIPLEN		20		//服务器IP长度
#define WIFISERVERPORTLEN	10		//服务器端口号长度

#define WIFIBUFFLEN			100		//wifi 数据缓存区长度

//wifi模块的工作状态
typedef enum euWifiWorkMode 
{	
	WIFI_WORK_NULL,

	WIFI_CMD_AT,
	WIFI_CMD_ATE,
	WIFI_CMD_CWMODE,
	WIFI_CMD_CWJAP,
	WIFI_CMD_CIPSTART,
	WIFI_CMD_CIPSEND,
	WIFI_CMD_CIPMODE,
	WIFI_WORK_PASSTHROUGH,		
}WIFIWORKMODE;

typedef struct stWifiCmd 
{
	u8 send[WIFIBUFFLEN];

	u32 timer;	//执行时间
	u8	Num;	//0,不执行次数，0xFF一直执行

}WIFICMD;

typedef struct stWifiBuffer 
{
	u8 len;					//数据长度
	u8 data[WIFIBUFFLEN];	//数据buffer
}WIFIBUFF;

void HAL_Wifi_SendCMD(u8 *pcmd, u32 waittime, u8 num);
void HAL_Reset_Wifi(void);
void HAL_Wifi_Init(u8 *pSSID, u8 *pPassWord, u8 *pServerIP, u8 *pServerPort, 
						u8* pHeartData, void(*pf_SendData)(u8 *pbuf, u8 len),
						void(*pf_Passthough_data)(u8 *pbuf, u8 len));
void HAL_Wifi_Process(void);
void HAL_Wifi_SetRecData(u8 len, u8* pData);
void HAL_Wifi_SendData(u8* pData, u8 len);
void Wifi_SendCMD(u8 *pcmd, u32 waittime, u8 num);
void WIfi_Device_Reset(void);
void HAL_Wifi_SendProcess(void);

#endif
