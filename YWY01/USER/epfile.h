/**
  ******************************************************************************
  * @file    	epfile.h
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
#include "sys.h"


#ifndef __EPFILE_H
#define __EPFILE_H	  

#define RCUDEVICEIDADDR 	5		//设备ID在EPRONG的起始地址
#define ZIGBEECHANNELADDR 	10		//Zigbee信道在EPRONG的起始地址
#define WIFIAPSSIDADDR		20		//wifi ssid在EPRONG的起始地址
#define WIFIAPPASSWORDADDR	70		//wifi 密码在EPRONG的起始地址
#define WIFISERVERIPADDR	120		//服务器IP地址在EPRONG的起始地址
#define WIFISERVERPORTADDR	170		//服务器端口号在EPRONG的起始地址
#define THERMOSTATMODEADDR	200		//服务器端口号在EPRONG的起始地址

void EPFile_Init(void);
void EPFile_Save_DeviceID(u8 *pId);
void EPFile_Save_ZigbeeChannel(u8 *pCH);
void EPFile_Save_SERVER_IP(u8 *pID);
void EPFile_Save_SERVER_Port(u8 *pW);
void EPFile_Save_Thermostat_Mode(u8 mode);
u8 EPFile_Read_Thermostat_Mode(void);

#endif
