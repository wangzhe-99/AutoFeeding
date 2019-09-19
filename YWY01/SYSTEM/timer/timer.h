#ifndef _TIMER_H
#define _TIMER_H

#include "stm32f10x.h"
#include "sys.h"

#define TIMERMSGNUM  20
typedef enum euTimerMsgStyle
{
	TIMER_MSG_NULL					= 0,	
	TIMER_MSG_UART1 				= 1,
	TIMER_MSG_UART2 				= 2,
	TIMER_MSG_UART3 				= 3,
	TIMER_MSG_UART4					= 4,
	TIMER_MSG_WIFIREC				= 5,
	TIMER_MSG_HEARTBEAT_PACKET		= 6,
	TIMER_MSG_ZIGBEE_LED_FLASHED	= 7,
	TIMER_MSG_WIFI_LED_FLASHED		= 8,
	TIMER_MSG_ZIGBEE_RADIO			= 9,
	TIMER_MSG_ZIGBEE_NETTING_END	= 10,	
	TIMER_MSG_RESETKEY				= 11,
	TIMER_MSG_RESETKEY_LONGPRESS	= 12,
	TIMER_MSG_SETAP_END				= 13,
	TIMER_MSG_OVERTIME_RESET		= 14,
	TIMER_MSG_SYSTEM_RESET			= 15,
	TIMER_MSG_PROTOCOL_OVERTIME		= 16,
	TIMER_MSG_PROTOCOL_CHECKOUT		= 17,
	TIMER_MSG_LOCK_HEARTBEAT		= 18,
	TIMER_MSG_LOCKOPEN				= 19,
	TIMER_MSG_UART5					= 20,
}TIMERMSGSTYLE;

typedef struct stSysTimerMsg
{
	u32 flag;
	void (*pCb)();
}SYSTIMERMSG;


void Timer_Init(void);
void Set_Timer_Msg(TIMERMSGSTYLE msg,  u32 timer, void (*pCB)());
void Clear_Timer_Msg(TIMERMSGSTYLE msg);
void Timer_Msg(void);






#endif
