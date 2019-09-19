/**
  ******************************************************************************
  * @file    	epfile.h
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
#include "sys.h"


#ifndef __EPFILE_H
#define __EPFILE_H	  

#define RCUDEVICEIDADDR 	5		//�豸ID��EPRONG����ʼ��ַ
#define ZIGBEECHANNELADDR 	10		//Zigbee�ŵ���EPRONG����ʼ��ַ
#define WIFIAPSSIDADDR		20		//wifi ssid��EPRONG����ʼ��ַ
#define WIFIAPPASSWORDADDR	70		//wifi ������EPRONG����ʼ��ַ
#define WIFISERVERIPADDR	120		//������IP��ַ��EPRONG����ʼ��ַ
#define WIFISERVERPORTADDR	170		//�������˿ں���EPRONG����ʼ��ַ
#define THERMOSTATMODEADDR	200		//�������˿ں���EPRONG����ʼ��ַ

void EPFile_Init(void);
void EPFile_Save_DeviceID(u8 *pId);
void EPFile_Save_ZigbeeChannel(u8 *pCH);
void EPFile_Save_SERVER_IP(u8 *pID);
void EPFile_Save_SERVER_Port(u8 *pW);
void EPFile_Save_Thermostat_Mode(u8 mode);
u8 EPFile_Read_Thermostat_Mode(void);

#endif
