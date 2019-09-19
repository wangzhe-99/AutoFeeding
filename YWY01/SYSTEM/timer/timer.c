/**
  ******************************************************************************
  * @file    timer.h
  * @author  乐易住
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   定时器处理
  ******************************************************************************
  * @attention
  *  定义一个10ms的系统定时器。系统中，使用定时器的时候，都以10ms为基准使用
  *
  * 
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "sys.h"
#include "main.h"
#include <stdio.h>

/*静态变量----------------------------------------------------------------*/
static u32 SYSTEM_REFERENCE_TIMER;
static SYSTIMERMSG	Sys_Timer_Msg[TIMERMSGNUM];

/**
  * @brief  定时器初始化
  *			定义10ms的基准定时器
  * @param  None
  * @retval None
  */
void Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//TIM4 10ms计数器
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period = 100; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	TIM_Cmd(TIM4, ENABLE);//开启定时器4

	SYSTEM_REFERENCE_TIMER = 0;
}


/**
  * @brief  10ms定时器中断处理函数
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//是更新中断
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //清除中断标志
		
		SYSTEM_REFERENCE_TIMER++;		
	}	    
}

/**
  * @brief  添加一个定时器
  * @param  msg：定时器ID
  *			timer：时间长度。10ms*timer=实际时间(ms  )
  *			pCB：到达时间后的，回调处理函数
  * @retval None
  */
void Set_Timer_Msg(TIMERMSGSTYLE msg,  u32 timer, void (*pCB)())
{
	Sys_Timer_Msg[msg].flag = timer + SYSTEM_REFERENCE_TIMER;
	Sys_Timer_Msg[msg].pCb = pCB;
}

/**
  * @brief  关闭一个定时器
  * @param  msg：定时器ID
  * @retval None
  */
void Clear_Timer_Msg(TIMERMSGSTYLE msg)
{
	Sys_Timer_Msg[msg].flag = 0;
}

/**
  * @brief  定时器处理函数
  * @param  None
  * @retval None
  */
void Timer_Msg(void)
{
	u8 i;
	
	for(i=0; i<TIMERMSGNUM; i++)
	{

		if(Sys_Timer_Msg[i].flag != 0)
		{
			if(SYSTEM_REFERENCE_TIMER > Sys_Timer_Msg[i].flag)
			{
				Sys_Timer_Msg[i].flag = 0;
				
				if(Sys_Timer_Msg[i].pCb != NULL)				
				{
					Sys_Timer_Msg[i].pCb();
				}
			}
		}
	}
}

