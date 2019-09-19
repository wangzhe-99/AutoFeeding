/**
  ******************************************************************************
  * @file    protocol.c
  * @author  ����ס
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   Э����
  ******************************************************************************
  * @attention
  *  protocolΪЭ���ܡ���Ҫ�������������ͨѶ��Э���װ��
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "protocol.h"
#include "serrcu.h"
#include "timer.h"


/*ȫ�ֱ���----------------------------------------------------------------*/
PROTOCOLFRAMES Protocol_Frames;
u8	Protocol_Buf[PROTOCOLBUFFLEN];

/*��̬����----------------------------------------------------------------*/
static u8	Protocol_Last_FrameID = 0;	//���һ��֡ID

static const PROTOCOLCLUSTER Protocol_Cluster[] = 
	{/*{0x0001, SRP_Set_DeviceID, NULL, 0},	 			//�����豸ID
   	 {0x0003, SRP_System_Restart, NULL, 0},				//�����豸
   	 {0x0004, SRP_Read_DeviceID, NULL, 0},				//���豸ID
   	 {0x0005, SRP_Set_Zigbee_Channel, NULL, 0},			//�����豸Zigbee�ŵ�
   	 {0x0006, SRP_Read_Zigbee_Channel, NULL, 0},		//���豸��zigbee�ŵ�
   	 {0x0007, SRP_Set_Server_Info, NULL, 0},			//Ԥ����server ip port
   	 {0x0008, SRP_Lock_Install, SRP_Lock_Install_Results, 6000},			//��װ����
   	 {0x0009, SRP_Lock_Set_Password, SRP_Lock_Set_Password_Results, 500},	//���ÿ�������
   	 {0x000A, SRP_Lock_Open, SRP_Lock_Open_Results, 500},					//Զ�̿���
   	 {0x000B, SRP_Lock_Del_One_Password, SRP_Lock_Del_One_Password_Results, 500},	//ɾ��ָ������
   	 {0x000C, SRP_Lock_Del_All_Password, SRP_Lock_Del_All_Password_Results, 500},	//ɾ����������
   	 {0x000F, SRP_Lock_Read_Battery_level, SRP_Lock_Read_Battery_level_Results, 500}, //��ѯ��ص���
   	 {0x0011, SRP_Lock_Set_NokeyOpen, NULL, 0},		//����Ϊ��Կ�׿���״̬
   	 {0x0012, SRP_Lock_Read_Info, NULL, 0},			//��״̬��ѯ
   	 {0x0013, SRP_CS_Install, SRP_CS_Install_Results, 6000}, //��װ���ϴ�����
   	 {0x0014, SRP_CurtainK_Install, NULL, 0},		//��װ�������
   	 {0x0015, SRP_CurtainN_Install, NULL, 0},		//��װ���������
   	 {0x0016, SRP_AirCtrl_Install, NULL, 0},		//��װ�յ��¿���
   	 {0x0017, SRP_CheckOut, NULL, 0},				//�˷�
   	 {0x0018, SRP_Open_Relay, NULL, 0},				//���������ڹ���
   	 {0x0019, SRP_Close_Relay, NULL, 0},			//���������ڹ���
   	 {0x001A, SRP_CS_Open_IR, SRP_CS_Open_IR_Results, 500},		//�򿪺���ң��
   	 {0x001B, SRP_CS_Close_IR, SRP_CS_Close_IR_Results, 500},	//�رպ���ң��
   	 {0x001C, SRP_CS_Read_Temperature, SRP_CS_Read_Temperature_Results, 500},	//�������¶�
   	 {0x001D, SRP_CS_Read_Luminance, SRP_CS_Read_Luminance_Results, 500},		//����������
   	 {0x001E, SRP_CurtainK_Open, NULL, 0},			//�����񣩴������
   	 {0x001F, SRP_CurtainN_Open, NULL, 0},			//���������������
   	 {0x0020, SRP_Thermostat_Open, NULL, 0},		//���յ��¿���
   	 {0x0021, SRP_LightCtrl_Install, NULL, 0},		//��װ�ƿ�
   	 {0x0022, SRP_LightCtrl_Open, NULL, 0},			//����
   	 {0x0023, SRP_BS_Install, SRP_BS_Install_Results, 6000},	//��װ����
   	 {0x0024, SRP_Lock_Reset_Password, SRP_Lock_Reset_Password_Results, 500},	//���迪������
   	 {0x0025, SRP_Thermostat_Mode, NULL, 0},	//���ÿյ��¿������Զ�����ģʽ*/
   	 {0xFFFF, NULL}};

/**
  * @brief  Э��֡��ʱ��������
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
  * @brief  Э��֡��������
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
  * @brief  Э��֡���ݷֽ�����
  *			�ӷ��������յ�����֡�󣬶�����֡���в�⡣
  * @param  pData������
  *			len�����ݳ���
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

	//��Э��֡�Ѿ��������
	if((Protocol_Last_FrameID != 0) && (Protocol_Last_FrameID == pData[0]))
	{
		DEBUG_PRINTF_ERROR("RCU Repeat Frame");
		RCU_To_Ser_RepeatFrame(pData);

		return;
	}

	//Э��֡��ʼ��
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

	//֡���ȼ��
	if(len > 64)
	{
		//����Э��ָ������
		DEBUG_PRINTF_ERROR("Protocol len=%d overlen", len);

		Protocol_Frames.ResponseCode = RESPONSE_CODE_ERROR_FRAMELEN;
		RCU_To_Ser_Response();

		Protocol_Frames.IsUse = 0;
		
		return;
	}

	//ID���
	if(memcmp(Protocol_Frames.DeviceID, &(pData[1]), 4) != 0)
	{
		//���������ȳ�����Χ	
		DEBUG_PRINTF_ERROR("Device ID error");

		Protocol_Frames.ResponseCode = RESPONSE_CODE_ERROR_DATALEN;
		RCU_To_Ser_Response();

		Protocol_Frames.IsUse = 0;
		
		return;
	}
	
	//���������ȼ��
	if(pData[7] > 50)
	{
		//���������ȳ�����Χ	
		DEBUG_PRINTF_ERROR("data buf len=%d over", pData[7]);

		Protocol_Frames.ResponseCode = RESPONSE_CODE_ERROR_DATALEN;
		RCU_To_Ser_Response();

		Protocol_Frames.IsUse = 0;
		
		return;
	}	

	//��У��
	if(pData[len-1] != Check_NC(pData, len-1))
	{
		//��У�����
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
			//δ�ҵ�������
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



