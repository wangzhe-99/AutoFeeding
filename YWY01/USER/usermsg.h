/**
  ******************************************************************************
  * @file    	usermsg.h
  * @author  	LYZ
  * @version 	V1.0.0
  * @date    	2018/1/5
  * @brief   	
  ******************************************************************************
  * @attention
  *	
  *
  * 
  ******************************************************************************
  */
#ifndef __USERMSG_H
#define __USERMSG_H
#include "stm32f10x.h"

#define USERMSGNUM 3			//消息链的长度。该参数决定了系统容许缓存消息的数量
#define USERMSGBUFLEN 100		//消息相关buffer的参数

typedef struct stUserMsgInfo 
{
	u8 isUse;				//是否被使用
	u8 ID;					//消息ID
	u8 len;					//数据长度
	u8 data[USERMSGBUFLEN];	//数据buffer
}USERMSGINFO;

typedef enum euUserMsgID
{
	UMSG_NULL = 0,	
	UMSG_LOCK_OPEN,			//开锁消息
	UMSG_USER_CHECKOUT,		//退房消息
	UMSG_KEY_LONGDOWN,		//按键长按
	UMSG_ID_CHANGEL,		//按键长按
	UMSG_END,
}USERMSGID;

void UserMsg_Init(void);
void UserMsg_Process(void);
void UserMsg_Add(u16 UMID, u8 *pData, u8 len);

#endif
