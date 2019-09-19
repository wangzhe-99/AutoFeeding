/**
  ******************************************************************************
  * @file    main.h
  * @author  ����ס
  * @version V0.0.1
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */  
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h> 
#include "delay.h"
#include "sys.h"
#include "usermsg.h"
/* Includes ------------------------------------------------------------------*/

//DEBUG���Կ���
#define __DEBUG
#ifdef __DEBUG
#define DEBUG_PRINTF(format,...) printf("[%s]"format"\r\n", __func__, ##__VA_ARGS__)
#define DEBUG_PRINTF_ERROR(format,...) printf("FILE: "__FILE__", LINE: %d:"format"\r\n", __LINE__, ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(format,...)
#define DEBUG_PRINTF_ERROR(format,...)
#endif

#define WIFIAPSSIDLEN		30		//wifi AP SSID����
#define WIFIAPPASSWORDLEN	30		//wifi AP ���볤��
#define WIFISERVERIPLEN		20		//������IP��ַ����
#define WIFISERVERPORTLEN	10		//�������˿ںų���

extern u8 DEVICE_ID[4];
extern u8 WIfI_AP_SSID[WIFIAPSSIDLEN];
extern u8 WIfI_AP_PASSWORD[WIFIAPPASSWORDLEN];
extern u8 WIfI_SERVER_IP[WIFISERVERIPLEN];
extern u8 WIfI_SERVER_PORT[WIFISERVERPORTLEN];
extern u16 ZIGBEE_PANID;
extern u8 ZIGBEE_CHANEL;

void System_Reset(void);

#endif
