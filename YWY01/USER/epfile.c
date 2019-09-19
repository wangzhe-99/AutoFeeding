/**
  ******************************************************************************
  * @file    	epfile.c
  * @author  	LYZ
  * @version 	V1.0.0
  * @date    	2018/1/5
  * @brief   	
  ******************************************************************************
  * @attention
  *	EPROM������װ��
  *
  * 
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "epfile.h"
#include "main.h"
#include "24cxx.h"

/**
  * @brief  �����豸ID
  * @param  pID���豸ID
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
  * @brief  ����wifi SSID
  * @param  pID��wifi SSID
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
  * @brief  ����wifi ����
  * @param  pID��wifi ����
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
  * @brief  ���������IP��ַ
  * @param  pIP��������IP��ַ
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
  * @brief  ����������˿ں�
  * @param  pIP���������˿ں�
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
  * @brief  �豸��Ϣ��ʼ��
  * @param  None
  * @retval None
  */
void EPFile_Init(void)
{
	u8 v, i;

	v = AT24CXX_ReadOneByte(0);
	
	DEBUG_PRINTF("EPFile_Init = %x", v);

	//ֻ�ó�ʼ��һ��
	if(v != 0xAA)
	//if(v == 0xAA)
	{
		//δʹ��ʱ����ʼIDΪ0100		
        DEVICE_ID[0] = 0x00;
        DEVICE_ID[1] = 0x01;
        DEVICE_ID[2] = 0x00;
        DEVICE_ID[3] = 0x00;
        
		for(i=0; i<4; i++)
		{
			AT24CXX_WriteOneByte(RCUDEVICEIDADDR+i, DEVICE_ID[i]);			
		}
		

		//д��AP
		memset(WIfI_AP_SSID, 0, WIFIAPSSIDLEN);
		strcpy((char*)WIfI_AP_SSID, "LYZ_DEVICE");		
		EPFile_Save_AP_SSID(WIfI_AP_SSID);

		//д��AP������	
		memset(WIfI_AP_PASSWORD, 0, WIFIAPPASSWORDLEN);		
		strcpy((char*)WIfI_AP_PASSWORD,"lyzimportantequipment");		
		EPFile_Save_AP_PassWord(WIfI_AP_PASSWORD);
        
        //д��Server IP	
		memset(WIfI_SERVER_IP, 0, WIFISERVERIPLEN);		
		strcpy((char*)WIfI_SERVER_IP,"47.106.9.53");			
		EPFile_Save_SERVER_IP(WIfI_SERVER_IP);

		//д��Server Port
		memset(WIfI_SERVER_PORT, 0, WIFISERVERPORTLEN);
		strcpy((char*)WIfI_SERVER_PORT,"20001");
		EPFile_Save_SERVER_Port(WIfI_SERVER_PORT);
     
		//EPROM��־λ
		AT24CXX_WriteOneByte(0, 0xAA);
	}
	else
	{
		//��EPROM������豸ID
		for(i=0; i<4; i++)
		{
			DEVICE_ID[i] = AT24CXX_ReadOneByte(RCUDEVICEIDADDR+i);
		}

		//��AP SSID
		memset(WIfI_AP_SSID, 0, WIFIAPSSIDLEN);
		for(i=0; i<WIFIAPSSIDLEN; i++)
		{
			WIfI_AP_SSID[i] = AT24CXX_ReadOneByte(WIFIAPSSIDADDR+i);

			if(WIfI_AP_SSID[i] == 0)
			{
				break;
			}
		}

		//��AP Password
		memset(WIfI_AP_PASSWORD, 0, WIFIAPPASSWORDLEN);
		for(i=0; i<WIFIAPPASSWORDLEN; i++)
		{
			WIfI_AP_PASSWORD[i] = AT24CXX_ReadOneByte(WIFIAPPASSWORDADDR+i);

			if(WIfI_AP_PASSWORD[i] == 0)
			{
				break;
			}
		}

		//��Server IP
		memset(WIfI_SERVER_IP, 0, WIFISERVERIPLEN);
		for(i=0; i<WIFISERVERIPLEN; i++)
		{
			WIfI_SERVER_IP[i] = AT24CXX_ReadOneByte(WIFISERVERIPADDR+i);

			if(WIfI_SERVER_IP[i] == 0)
			{
				break;
			}
		}

		//��Server Port
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


