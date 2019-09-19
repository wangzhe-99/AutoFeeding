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

#define USERMSGNUM 3			//��Ϣ���ĳ��ȡ��ò���������ϵͳ��������Ϣ������
#define USERMSGBUFLEN 100		//��Ϣ���buffer�Ĳ���

typedef struct stUserMsgInfo 
{
	u8 isUse;				//�Ƿ�ʹ��
	u8 ID;					//��ϢID
	u8 len;					//���ݳ���
	u8 data[USERMSGBUFLEN];	//����buffer
}USERMSGINFO;

typedef enum euUserMsgID
{
	UMSG_NULL = 0,	
	UMSG_LOCK_OPEN,			//������Ϣ
	UMSG_USER_CHECKOUT,		//�˷���Ϣ
	UMSG_KEY_LONGDOWN,		//��������
	UMSG_ID_CHANGEL,		//��������
	UMSG_END,
}USERMSGID;

void UserMsg_Init(void);
void UserMsg_Process(void);
void UserMsg_Add(u16 UMID, u8 *pData, u8 len);

#endif
