/**
  ******************************************************************************
  * @file    protocol.h
  * @author  乐易住
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   协议框架
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

/* 子设备地址 ------------------------------------------------------------------*/
#define DEV_LOCK_ADDR 0x02		//锁地址
#define DEV_CS_ADDR 0x03		//复合传感器地址
#define DEV_WC_K_ADDR 0x04		//遮光帘窗帘电机地址
#define DEV_WC_N_ADDR 0x05		//纱帘窗帘电机地址
#define DEV_THERMOSTAT_ADDR 0x06	//空调温控器地址
#define DEV_LIGHTCTRL1_ADDR 0x07	//灯控面板地址
#define DEV_LIGHTCTRL2_ADDR 0x08
#define DEV_LIGHTCTRL3_ADDR 0x09
#define DEV_LIGHTCTRL4_ADDR 0x0A
#define DEV_BS_ADDR 0x0B			//床感地址

/* 应答码-----------------------------------------------------------------------*/
#define RESPONSE_CODE_TRUE 				0xFF		//正确
#define RESPONSE_CODE_FALSE 			0x01		//错误
#define RESPONSE_CODE_ERROR_CHECKNC 	0x02		//和校验错误
#define RESPONSE_CODE_ERROR_DEVBUSY 	0x03		//设备忙
#define RESPONSE_CODE_ERROR_FRAMELEN 	0x04		//帧长度错误
#define RESPONSE_CODE_ERROR_DATALEN 	0x05		//数据长度错误
#define RESPONSE_CODE_ERROR_COMMCODE 	0x06		//命令码错误
#define RESPONSE_CODE_ERROR_TIMEOUT 	0x07		//命令处理超时
#define RESPONSE_CODE_ERROR_DEVID	 	0x08		//设备ID错误
#define RESPONSE_CODE_ERROR_DEVREPORT 	0x09		//子设备错误
#define RESPONSE_CODE_ERROR_REPEATFRAME 0x0A		//帧已经被处理过

typedef struct stProtocolCluster 
{
	u16 	CommandCode;		//命令码
	void	(*pf_Process)();	//命令处理流程
	void	(*pf_Results)();	//结果处理流程
	u32		Overtime;			//协议超时时间

}PROTOCOLCLUSTER;

typedef struct stProtocolFrames 
{
	u8  IsUse;						//是否被使用
	u8 	FrameID;					//帧ID
	u8	DeviceID[4];				//设备ID
	u16	CommandCode;				//命令码
	u8	SendBuflen;					//向服务器发送的数据长度
	u8	SendBuf[PROTOCOLBUFFLEN];	//向服务器发送的数据
	u8	RecBuflen;					//接收服务发送的数据长度
	u8	RecBuf[PROTOCOLBUFFLEN];	//接收服务发送的数据
	u8	ResponseCode;				//应答码
	u8  IsProcess;					//是否在处理流程
	u8  IsResults;					//是否有处理结果
	u32 Overtime;					//超时时间
	void	(*pf_Process)();		//协议接收数据后处理函数
	void	(*pf_Results)();		//数据处理完成后的结果处理函数
	
}PROTOCOLFRAMES;

void Protocol_SerToRCU_Data(u8 *pData, u8 len);
void Protocol_DevToRCU_Data(u8 *pData, u8 len);
void Protocol_Process(void);

extern PROTOCOLFRAMES Protocol_Frames;
extern u8	Protocol_Buf[PROTOCOLBUFFLEN];

#endif
