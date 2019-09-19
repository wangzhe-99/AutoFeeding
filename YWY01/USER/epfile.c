/**
  ******************************************************************************
  * @file    	epfile.c
  * @author  	LYZ
  * @version 	V1.0.0
  * @date    	2018/1/5
  * @brief   	
  ******************************************************************************
  * @attention
  *	EPROM操作封装。
  *
  * 
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "epfile.h"
#include "main.h"
#include "24cxx.h"

/**
  * @brief  保存设备ID
  * @param  pID：设备ID
  * @retval None
  */
void EPFile_Save_DeviceID(u8 *pId)
{
	u8 i;
	
	for(i=0; i<4; i++)
	{
		AT24CXX_WriteOneByte(RCUDEVICEIDADDR+i, *(pId+i));
	}
}

/**
  * @brief  保存wifi SSID
  * @param  pID：wifi SSID
  * @retval None
  */
void EPFile_Save_AP_SSID(u8 *pID)
{
	u8 i, l;
	
	l = strlen((const char*)pID);
	for(i=0; i<l; i++)
	{
		AT24CXX_WriteOneByte(WIFIAPSSIDADDR+i, *(pID+i));		
	}

	AT24CXX_WriteOneByte(WIFIAPSSIDADDR+i, 0);
}

/**
  * @brief  保存wifi 密码
  * @param  pID：wifi 密码
  * @retval None
  */
void EPFile_Save_AP_PassWord(u8 *pW)
{
	u8 i, l;
	
	l = strlen((const char*)pW);
	for(i=0; i<l; i++)
	{
		AT24CXX_WriteOneByte(WIFIAPPASSWORDADDR+i, *(pW+i));
	}

	AT24CXX_WriteOneByte(WIFIAPPASSWORDADDR+i, 0);
}

/**
  * @brief  保存服务器IP地址
  * @param  pIP：服务器IP地址
  * @retval None
  */
void EPFile_Save_SERVER_IP(u8 *pIP)
{
	u8 i, l;
	
	l = strlen((const char*)pIP);
	for(i=0; i<l; i++)
	{
		AT24CXX_WriteOneByte(WIFISERVERIPADDR+i, *(pIP+i));		
	}

	AT24CXX_WriteOneByte(WIFISERVERIPADDR+i, 0);
}

/**
  * @brief  保存服务器端口号
  * @param  pIP：服务器端口号
  * @retval None
  */
void EPFile_Save_SERVER_Port(u8 *pT)
{
	u8 i, l;
	
	l = strlen((const char*)pT);
	for(i=0; i<l; i++)
	{
		AT24CXX_WriteOneByte(WIFISERVERPORTADDR+i, *(pT+i));		
	}

	AT24CXX_WriteOneByte(WIFISERVERPORTADDR+i, 0);
}


/**
  * @brief  设备信息初始化
  * @param  None
  * @retval None
  */
void EPFile_Init(void)
{
	u8 v, i;

	v = AT24CXX_ReadOneByte(0);
	
	DEBUG_PRINTF("EPFile_Init = %x", v);

	//只用初始化一次
	if(v != 0xAA)
	//if(v == 0xAA)
	{
		//未使用时，初始ID为0100		
        DEVICE_ID[0] = 0x00;
        DEVICE_ID[1] = 0x01;
        DEVICE_ID[2] = 0x00;
        DEVICE_ID[3] = 0x00;
        
		for(i=0; i<4; i++)
		{
			AT24CXX_WriteOneByte(RCUDEVICEIDADDR+i, DEVICE_ID[i]);			
		}
		

		//写入AP
		memset(WIfI_AP_SSID, 0, WIFIAPSSIDLEN);
		strcpy((char*)WIfI_AP_SSID, "LYZ_DEVICE");		
		EPFile_Save_AP_SSID(WIfI_AP_SSID);

		//写入AP的密码	
		memset(WIfI_AP_PASSWORD, 0, WIFIAPPASSWORDLEN);		
		strcpy((char*)WIfI_AP_PASSWORD,"lyzimportantequipment");		
		EPFile_Save_AP_PassWord(WIfI_AP_PASSWORD);
        
        //写入Server IP	
		memset(WIfI_SERVER_IP, 0, WIFISERVERIPLEN);		
		strcpy((char*)WIfI_SERVER_IP,"47.106.9.53");			
		EPFile_Save_SERVER_IP(WIfI_SERVER_IP);

		//写入Server Port
		memset(WIfI_SERVER_PORT, 0, WIFISERVERPORTLEN);
		strcpy((char*)WIfI_SERVER_PORT,"20001");
		EPFile_Save_SERVER_Port(WIfI_SERVER_PORT);
     
		//EPROM标志位
		AT24CXX_WriteOneByte(0, 0xAA);
	}
	else
	{
		//从EPROM里读出设备ID
		for(i=0; i<4; i++)
		{
			DEVICE_ID[i] = AT24CXX_ReadOneByte(RCUDEVICEIDADDR+i);
		}

		//读AP SSID
		memset(WIfI_AP_SSID, 0, WIFIAPSSIDLEN);
		for(i=0; i<WIFIAPSSIDLEN; i++)
		{
			WIfI_AP_SSID[i] = AT24CXX_ReadOneByte(WIFIAPSSIDADDR+i);

			if(WIfI_AP_SSID[i] == 0)
			{
				break;
			}
		}

		//读AP Password
		memset(WIfI_AP_PASSWORD, 0, WIFIAPPASSWORDLEN);
		for(i=0; i<WIFIAPPASSWORDLEN; i++)
		{
			WIfI_AP_PASSWORD[i] = AT24CXX_ReadOneByte(WIFIAPPASSWORDADDR+i);

			if(WIfI_AP_PASSWORD[i] == 0)
			{
				break;
			}
		}

		//读Server IP
		memset(WIfI_SERVER_IP, 0, WIFISERVERIPLEN);
		for(i=0; i<WIFISERVERIPLEN; i++)
		{
			WIfI_SERVER_IP[i] = AT24CXX_ReadOneByte(WIFISERVERIPADDR+i);

			if(WIfI_SERVER_IP[i] == 0)
			{
				break;
			}
		}

		//读Server Port
		memset(WIfI_SERVER_PORT, 0, WIFISERVERPORTLEN);
		for(i=0; i<WIFISERVERPORTLEN; i++)
		{
			WIfI_SERVER_PORT[i] = AT24CXX_ReadOneByte(WIFISERVERPORTADDR+i);

			if(WIfI_SERVER_PORT[i] == 0)
			{
				break;
			}
		}
	}
}


