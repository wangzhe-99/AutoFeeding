/**
  ******************************************************************************
  * @file    protocol.h
  * @author  ����ס
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   Э����
  ******************************************************************************
  * @attention
  *  
  * 
  ******************************************************************************
  */
#ifndef __PROTOCOL_H
#define __PROTOCOL_H
#include "stm32f10x.h"

#define PROTOCOLBUFFLEN 100

/* ���豸��ַ ------------------------------------------------------------------*/
#define DEV_LOCK_ADDR 0x02		//����ַ
#define DEV_CS_ADDR 0x03		//���ϴ�������ַ
#define DEV_WC_K_ADDR 0x04		//�ڹ������������ַ
#define DEV_WC_N_ADDR 0x05		//ɴ�����������ַ
#define DEV_THERMOSTAT_ADDR 0x06	//�յ��¿�����ַ
#define DEV_LIGHTCTRL1_ADDR 0x07	//�ƿ�����ַ
#define DEV_LIGHTCTRL2_ADDR 0x08
#define DEV_LIGHTCTRL3_ADDR 0x09
#define DEV_LIGHTCTRL4_ADDR 0x0A
#define DEV_BS_ADDR 0x0B			//���е�ַ

/* Ӧ����-----------------------------------------------------------------------*/
#define RESPONSE_CODE_TRUE 				0xFF		//��ȷ
#define RESPONSE_CODE_FALSE 			0x01		//����
#define RESPONSE_CODE_ERROR_CHECKNC 	0x02		//��У�����
#define RESPONSE_CODE_ERROR_DEVBUSY 	0x03		//�豸æ
#define RESPONSE_CODE_ERROR_FRAMELEN 	0x04		//֡���ȴ���
#define RESPONSE_CODE_ERROR_DATALEN 	0x05		//���ݳ��ȴ���
#define RESPONSE_CODE_ERROR_COMMCODE 	0x06		//���������
#define RESPONSE_CODE_ERROR_TIMEOUT 	0x07		//�����ʱ
#define RESPONSE_CODE_ERROR_DEVID	 	0x08		//�豸ID����
#define RESPONSE_CODE_ERROR_DEVREPORT 	0x09		//���豸����
#define RESPONSE_CODE_ERROR_REPEATFRAME 0x0A		//֡�Ѿ��������

typedef struct stProtocolCluster 
{
	u16 	CommandCode;		//������
	void	(*pf_Process)();	//���������
	void	(*pf_Results)();	//�����������
	u32		Overtime;			//Э�鳬ʱʱ��

}PROTOCOLCLUSTER;

typedef struct stProtocolFrames 
{
	u8  IsUse;						//�Ƿ�ʹ��
	u8 	FrameID;					//֡ID
	u8	DeviceID[4];				//�豸ID
	u16	CommandCode;				//������
	u8	SendBuflen;					//����������͵����ݳ���
	u8	SendBuf[PROTOCOLBUFFLEN];	//����������͵�����
	u8	RecBuflen;					//���շ����͵����ݳ���
	u8	RecBuf[PROTOCOLBUFFLEN];	//���շ����͵�����
	u8	ResponseCode;				//Ӧ����
	u8  IsProcess;					//�Ƿ��ڴ�������
	u8  IsResults;					//�Ƿ��д�����
	u32 Overtime;					//��ʱʱ��
	void	(*pf_Process)();		//Э��������ݺ�����
	void	(*pf_Results)();		//���ݴ�����ɺ�Ľ��������
	
}PROTOCOLFRAMES;

void Protocol_SerToRCU_Data(u8 *pData, u8 len);
void Protocol_DevToRCU_Data(u8 *pData, u8 len);
void Protocol_Process(void);

extern PROTOCOLFRAMES Protocol_Frames;
extern u8	Protocol_Buf[PROTOCOLBUFFLEN];

#endif
