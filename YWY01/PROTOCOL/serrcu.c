/**
  ******************************************************************************
  * @file    serrcu.c
  * @author  乐易住
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   服务器对RCU的通讯协议实现
  ******************************************************************************
  * @attention
  *  
  *
  * 
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "serrcu.h"
#include "protocol.h"
#include "epfile.h"
#include "wifi.h"
#include "usermsg.h"
#include "relay.h"
#include "epfile.h"


/*外部引用----------------------------------------------------------------*/
extern PROTOCOLFRAMES Protocol_Frames;

/**
  * @brief  RCU对服务下发命令的应答
  * @param  None
  * @retval None
  */
void RCU_To_Ser_Response(void)
{
	Protocol_Buf[0] = Protocol_Frames.FrameID;
	
	memcpy(&(Protocol_Buf[1]), DEVICE_ID, 4);
	
	Protocol_Buf[5] = Protocol_Frames.CommandCode >> 8;
	Protocol_Buf[6] = Protocol_Frames.CommandCode & 0x00FF;

	Protocol_Buf[7] = Protocol_Frames.ResponseCode;
	Protocol_Buf[8] = Protocol_Frames.SendBuflen;

	if(Protocol_Frames.SendBuflen != 0)
	{
		memcpy(&(Protocol_Buf[9]), Protocol_Frames.SendBuf, Protocol_Frames.SendBuflen);
	}

	Protocol_Buf[9+Protocol_Frames.SendBuflen] = Check_NC(Protocol_Buf, 9+Protocol_Frames.SendBuflen);

	HAL_Wifi_SendData(Protocol_Buf, 10+Protocol_Frames.SendBuflen);
}

/**
  * @brief  RCU向服务器的主动上报
  * @param  commcode：命令码
  *			pData：数据
  *			len：数据长度
  * @retval None
  */
void RCU_To_Ser_Report(u16 commcode, u8 *pData, u8 len)
{
	Protocol_Buf[0] = 0xFF;
	
	memcpy(&(Protocol_Buf[1]), DEVICE_ID, 4);
	
	Protocol_Buf[5] = commcode >> 8;
	Protocol_Buf[6] = commcode & 0x00FF;
	Protocol_Buf[7] = 0xFE;
	
	Protocol_Buf[8] = len;

	if(len != 0)
	{
		memcpy(&(Protocol_Buf[9]), pData, len);
	}

	Protocol_Buf[9+len] = Check_NC(Protocol_Buf, 9+len);

	HAL_Wifi_SendData(Protocol_Buf, 10+len);
}

/**
  * @brief  RCU忙
  *			服务器在向RCU发送数据后，当数据处理未完成时，服务器继续发送请求，
  *			将会调用该函数，进行反馈。
  * @param  pData：协议数据帧
  * @retval None
  */
void RCU_To_Ser_Busy(u8 *pData)
{
	memcpy(&(Protocol_Buf[0]), pData, 7);
	
	Protocol_Buf[7] = RESPONSE_CODE_ERROR_DEVBUSY;
	Protocol_Buf[8] = 0;
	Protocol_Buf[9] = Check_NC(Protocol_Buf, 9);

	HAL_Wifi_SendData(Protocol_Buf, 10);	
}

/**
  * @brief  该帧已经被处理过
  *			服务器在向RCU发送数据后，服务器在未等到RCU的回应侯，会进行重发
  *			如果RCU已经处理过该协议，将会返回该命令
  * @param  pData：协议数据帧
  * @retval None
  */
void RCU_To_Ser_RepeatFrame(u8 *pData)
{
	memcpy(&(Protocol_Buf[0]), pData, 7);
	
	Protocol_Buf[7] = RESPONSE_CODE_ERROR_REPEATFRAME;
	Protocol_Buf[8] = 0;
	Protocol_Buf[9] = Check_NC(Protocol_Buf, 9);

	HAL_Wifi_SendData(Protocol_Buf, 10);	
}


/**
  * @brief  设置设备ID
  * @param  None
  * @retval None
  */
void SRP_Set_DeviceID(void)
{
	DEBUG_PRINTF("Set Device ID");
	
	Protocol_Frames.ResponseCode = RESPONSE_CODE_TRUE;
	RCU_To_Ser_Response();	

	memcpy(DEVICE_ID, Protocol_Frames.RecBuf, Protocol_Frames.RecBuflen);		
	
	EPFile_Save_DeviceID(DEVICE_ID);
	
	UserMsg_Add(UMSG_ID_CHANGEL, NULL, 0);
}

/**
  * @brief  设备重启
  * @param  None
  * @retval None
  */
void SRP_System_Restart(void)
{
	DEBUG_PRINTF("System Restart");

	Protocol_Frames.ResponseCode = RESPONSE_CODE_TRUE;
	RCU_To_Ser_Response();

	Set_Timer_Msg(TIMER_MSG_SYSTEM_RESET, 1000, System_Reset);
}

/**
  * @brief  读设备ID
  * @param  None
  * @retval None
  */
void SRP_Read_DeviceID(void)
{
	DEBUG_PRINTF("Read Device ID");

	Protocol_Frames.ResponseCode = RESPONSE_CODE_TRUE;
	Protocol_Frames.SendBuflen = 4;
	memcpy(Protocol_Frames.SendBuf, DEVICE_ID, 4);
	RCU_To_Ser_Response();
}

/**
  * @brief  设置zigbee信道
  * @param  None
  * @retval None
  */
void SRP_Set_Zigbee_Channel(void)
{
	DEBUG_PRINTF("Set Zigbee CH");


}

/**
  * @brief  读zigbee信道
  * @param  None
  * @retval None
  */
void SRP_Read_Zigbee_Channel(void)
{
	DEBUG_PRINTF("Read Zigbee CH");


}

/**
  * @brief  设置服务器IP地址，端口号
  * @param  None
  * @retval None
  */
void SRP_Set_Server_Info(void)
{
	u8 l1, l2;
	
	DEBUG_PRINTF("Set Server IP port");

	l1 = Protocol_Frames.RecBuf[0];

	memcpy(WIfI_SERVER_IP, &(Protocol_Frames.RecBuf[1]), l1);
	WIfI_SERVER_IP[l1] = 0;
	EPFile_Save_SERVER_IP(WIfI_SERVER_IP);

	l2 = Protocol_Frames.RecBuf[l1+1];
	memcpy(WIfI_SERVER_PORT, &(Protocol_Frames.RecBuf[l1+2]), l2);
	WIfI_SERVER_PORT[l2] = 0;
	EPFile_Save_SERVER_Port(WIfI_SERVER_PORT);

	Protocol_Frames.ResponseCode = RESPONSE_CODE_TRUE;
	RCU_To_Ser_Response();
	
	Set_Timer_Msg(TIMER_MSG_SYSTEM_RESET, 500, System_Reset);
}







