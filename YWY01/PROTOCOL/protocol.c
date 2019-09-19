/**
  ******************************************************************************
  * @file    protocol.c
  * @author  乐易住
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   协议框架
  ******************************************************************************
  * @attention
  *  protocol为协议框架。主要是与服务器数据通讯的协议封装。
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "protocol.h"
#include "serrcu.h"
#include "timer.h"


/*全局变量----------------------------------------------------------------*/
PROTOCOLFRAMES Protocol_Frames;
u8	Protocol_Buf[PROTOCOLBUFFLEN];

/*静态变量----------------------------------------------------------------*/
static u8	Protocol_Last_FrameID = 0;	//最后一次帧ID

static const PROTOCOLCLUSTER Protocol_Cluster[] = 
	{/*{0x0001, SRP_Set_DeviceID, NULL, 0},	 			//设置设备ID
   	 {0x0003, SRP_System_Restart, NULL, 0},				//重启设备
   	 {0x0004, SRP_Read_DeviceID, NULL, 0},				//读设备ID
   	 {0x0005, SRP_Set_Zigbee_Channel, NULL, 0},			//设置设备Zigbee信道
   	 {0x0006, SRP_Read_Zigbee_Channel, NULL, 0},		//读设备的zigbee信道
   	 {0x0007, SRP_Set_Server_Info, NULL, 0},			//预设置server ip port
   	 {0x0008, SRP_Lock_Install, SRP_Lock_Install_Results, 6000},			//安装门锁
   	 {0x0009, SRP_Lock_Set_Password, SRP_Lock_Set_Password_Results, 500},	//设置开锁密码
   	 {0x000A, SRP_Lock_Open, SRP_Lock_Open_Results, 500},					//远程开锁
   	 {0x000B, SRP_Lock_Del_One_Password, SRP_Lock_Del_One_Password_Results, 500},	//删除指定密码
   	 {0x000C, SRP_Lock_Del_All_Password, SRP_Lock_Del_All_Password_Results, 500},	//删除所有密码
   	 {0x000F, SRP_Lock_Read_Battery_level, SRP_Lock_Read_Battery_level_Results, 500}, //查询电池电量
   	 {0x0011, SRP_Lock_Set_NokeyOpen, NULL, 0},		//设置为无钥匙开锁状态
   	 {0x0012, SRP_Lock_Read_Info, NULL, 0},			//锁状态查询
   	 {0x0013, SRP_CS_Install, SRP_CS_Install_Results, 6000}, //安装复合传感器
   	 {0x0014, SRP_CurtainK_Install, NULL, 0},		//安装厚窗帘电机
   	 {0x0015, SRP_CurtainN_Install, NULL, 0},		//安装薄窗帘电机
   	 {0x0016, SRP_AirCtrl_Install, NULL, 0},		//安装空调温控器
   	 {0x0017, SRP_CheckOut, NULL, 0},				//退房
   	 {0x0018, SRP_Open_Relay, NULL, 0},				//开启房间内供电
   	 {0x0019, SRP_Close_Relay, NULL, 0},			//开启房间内供电
   	 {0x001A, SRP_CS_Open_IR, SRP_CS_Open_IR_Results, 500},		//打开红外遥控
   	 {0x001B, SRP_CS_Close_IR, SRP_CS_Close_IR_Results, 500},	//关闭红外遥控
   	 {0x001C, SRP_CS_Read_Temperature, SRP_CS_Read_Temperature_Results, 500},	//读房间温度
   	 {0x001D, SRP_CS_Read_Luminance, SRP_CS_Read_Luminance_Results, 500},		//读房间亮度
   	 {0x001E, SRP_CurtainK_Open, NULL, 0},			//开（厚）窗帘电机
   	 {0x001F, SRP_CurtainN_Open, NULL, 0},			//开（薄）窗帘电机
   	 {0x0020, SRP_Thermostat_Open, NULL, 0},		//开空调温控器
   	 {0x0021, SRP_LightCtrl_Install, NULL, 0},		//安装灯控
   	 {0x0022, SRP_LightCtrl_Open, NULL, 0},			//开灯
   	 {0x0023, SRP_BS_Install, SRP_BS_Install_Results, 6000},	//安装床感
   	 {0x0024, SRP_Lock_Reset_Password, SRP_Lock_Reset_Password_Results, 500},	//重设开锁密码
   	 {0x0025, SRP_Thermostat_Mode, NULL, 0},	//设置空调温控器的自动工作模式*/
   	 {0xFFFF, NULL}};

/**
  * @brief  协议帧超时处理流程
  * @param  None
  * @retval None
  */
void Protocol_OverTime_Process(void)
{
	if(Protocol_Frames.IsUse == 0)
	{
		return;
	}
	
	DEBUG_PRINTF_ERROR("Protocol 0x%x overtime", Protocol_Frames.CommandCode);

	Protocol_Frames.ResponseCode = RESPONSE_CODE_ERROR_TIMEOUT;
	RCU_To_Ser_Response();

	Protocol_Frames.IsUse = 0;

	Protocol_Last_FrameID = 0;
}

/**
  * @brief  协议帧处理流程
  * @param  None
  * @retval None
  */
void Protocol_Process(void)
{
	if(Protocol_Frames.IsUse == 0)
	{
		return;
	}

	if(Protocol_Frames.IsProcess == 1)
	{
		if(Protocol_Frames.pf_Process != NULL)
		{
			Protocol_Frames.pf_Process();

			if(Protocol_Frames.Overtime != 0)
			{
				Set_Timer_Msg(TIMER_MSG_PROTOCOL_OVERTIME, Protocol_Frames.Overtime, 
								Protocol_OverTime_Process);
			}
			else
			{
				Protocol_Frames.IsUse = 0;				
				
				return;
			}
		}
		
		Protocol_Frames.IsProcess = 0;	
	}

	if(Protocol_Frames.IsResults == 1)
	{
		if(Protocol_Frames.pf_Results != NULL)
		{
			Protocol_Frames.pf_Results();
		}
		
		Protocol_Frames.pf_Results = 0;
		Protocol_Frames.IsUse = 0;
	}
}

/**
  * @brief  协议帧数据分解流程
  *			从服务器接收到数据帧后，对数据帧进行拆解。
  * @param  pData：数据
  *			len：数据长度
  * @retval None
  */
void Protocol_SerToRCU_Data(u8 *pData, u8 len)
{
	u8	i;
	
	if(Protocol_Frames.IsUse != 0)
	{
		DEBUG_PRINTF_ERROR("RCU busies");
		RCU_To_Ser_Busy(pData);

		return;
	}

	//该协议帧已经被处理过
	if((Protocol_Last_FrameID != 0) && (Protocol_Last_FrameID == pData[0]))
	{
		DEBUG_PRINTF_ERROR("RCU Repeat Frame");
		RCU_To_Ser_RepeatFrame(pData);

		return;
	}

	//协议帧初始化
	Protocol_Frames.IsUse = 1;	
	Protocol_Frames.FrameID = pData[0];
	memcpy(Protocol_Frames.DeviceID, DEVICE_ID, 4);
	Protocol_Frames.CommandCode = ((u16)(pData[5]) << 8) + pData[6];
	Protocol_Frames.SendBuflen = 0;
	memset(Protocol_Frames.SendBuf, 0, PROTOCOLBUFFLEN);	
	Protocol_Frames.ResponseCode = 0;	
	Protocol_Frames.ResponseCode = 0x00;
	Protocol_Frames.IsProcess = 0;
	Protocol_Frames.IsResults = 0;
	Protocol_Frames.Overtime = 0;
	Protocol_Frames.pf_Process = NULL;
	Protocol_Frames.pf_Results = NULL;

	//帧长度检测
	if(len > 64)
	{
		//超出协议指定长度
		DEBUG_PRINTF_ERROR("Protocol len=%d overlen", len);

		Protocol_Frames.ResponseCode = RESPONSE_CODE_ERROR_FRAMELEN;
		RCU_To_Ser_Response();

		Protocol_Frames.IsUse = 0;
		
		return;
	}

	//ID检查
	if(memcmp(Protocol_Frames.DeviceID, &(pData[1]), 4) != 0)
	{
		//数据区长度超出范围	
		DEBUG_PRINTF_ERROR("Device ID error");

		Protocol_Frames.ResponseCode = RESPONSE_CODE_ERROR_DATALEN;
		RCU_To_Ser_Response();

		Protocol_Frames.IsUse = 0;
		
		return;
	}
	
	//数据区长度检测
	if(pData[7] > 50)
	{
		//数据区长度超出范围	
		DEBUG_PRINTF_ERROR("data buf len=%d over", pData[7]);

		Protocol_Frames.ResponseCode = RESPONSE_CODE_ERROR_DATALEN;
		RCU_To_Ser_Response();

		Protocol_Frames.IsUse = 0;
		
		return;
	}	

	//和校验
	if(pData[len-1] != Check_NC(pData, len-1))
	{
		//和校验错误
		DEBUG_PRINTF_ERROR("sum check error");
		
		Protocol_Frames.ResponseCode = RESPONSE_CODE_ERROR_CHECKNC;
		RCU_To_Ser_Response();
		
		Protocol_Frames.IsUse = 0;
		
		return;
	}

	Protocol_Frames.RecBuflen = pData[7];
	if(Protocol_Frames.RecBuflen != 0)
	{
		memcpy(Protocol_Frames.RecBuf, &(pData[8]), Protocol_Frames.RecBuflen);
	}

	i = 0;
	while(1)
	{
		if(Protocol_Cluster[i].CommandCode == 0xFFFF)
		{
			//未找到命令码
			DEBUG_PRINTF_ERROR("Commad code = 0x%x error", Protocol_Frames.CommandCode);
			
			Protocol_Frames.ResponseCode = RESPONSE_CODE_ERROR_COMMCODE;
			RCU_To_Ser_Response();
			
			Protocol_Frames.IsUse = 0;
			
			return;
		}
			
		if(Protocol_Cluster[i].CommandCode == Protocol_Frames.CommandCode)
		{
			Protocol_Frames.pf_Process = Protocol_Cluster[i].pf_Process;
			Protocol_Frames.pf_Results = Protocol_Cluster[i].pf_Results;

			Protocol_Frames.IsProcess = 1;
			Protocol_Frames.Overtime = Protocol_Cluster[i].Overtime;

			Protocol_Last_FrameID = Protocol_Frames.FrameID;
			
			break;
		}
		
		i++;
	}
}



