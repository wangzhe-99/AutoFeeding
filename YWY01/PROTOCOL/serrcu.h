/**
  ******************************************************************************
  * @file    serrcu.h
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
#ifndef __SERDEV_H
#define __SERDEV_H
#include "stm32f10x.h"

void SRP_Set_DeviceID(void);
void SRP_System_Restart(void);
void SRP_Read_DeviceID(void);
void SRP_Set_Zigbee_Channel(void);
void SRP_Read_Zigbee_Channel(void);
void SRP_Set_Server_Info(void);
void SRP_Lock_Install(void);
void SRP_Lock_Install_Results(void);
void SRP_Lock_Set_Password(void);
void SRP_Lock_Set_Password_Results(void);
void SRP_Lock_Reset_Password(void);
void SRP_Lock_Reset_Password_Results(void);
void SRP_Lock_Open(void);
void SRP_Lock_Open_Results(void);
void SRP_Lock_Del_One_Password(void);
void SRP_Lock_Del_One_Password_Results(void);
void SRP_Lock_Del_All_Password(void);
void SRP_Lock_Del_All_Password_Results(void);
void SRP_Lock_Read_Battery_level(void);
void SRP_Lock_Read_Battery_level_Results(void);
void SRP_Lock_Set_NokeyOpen(void);
void SRP_Lock_Read_Info(void);
void SRP_CS_Install(void);
void SRP_CS_Install_Results(void);
void SRP_CurtainK_Install(void);
void SRP_CurtainN_Install(void);
void SRP_AirCtrl_Install(void);
void SRP_CheckOut(void);
void SRP_Open_Relay(void);
void SRP_Close_Relay(void);
void SRP_CS_Open_IR(void);
void SRP_CS_Open_IR_Results(void);
void SRP_CS_Close_IR(void);
void SRP_CS_Close_IR_Results(void);
void SRP_CS_Read_Temperature(void);
void SRP_CS_Read_Temperature_Results(void);
void SRP_CS_Read_Luminance(void);
void SRP_CS_Read_Luminance_Results(void);
void SRP_CurtainK_Open(void);
void SRP_CurtainN_Open(void);
void SRP_Thermostat_Open(void);
void SRP_Thermostat_Mode(void);
void SRP_LightCtrl_Install(void);
void SRP_LightCtrl_Open(void);
void SRP_BS_Install(void);
void SRP_BS_Install_Results(void);

void RCU_To_Ser_Response(void);
void RCU_To_Ser_Busy(u8 *pData);
void RCU_To_Ser_RepeatFrame(u8 *pData);
void RCU_To_Ser_Report(u16 commcode, u8 *pData, u8 len);

#endif
