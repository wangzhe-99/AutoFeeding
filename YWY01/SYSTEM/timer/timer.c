/**
  ******************************************************************************
  * @file    timer.h
  * @author  ����ס
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   ��ʱ������
  ******************************************************************************
  * @attention
  *  ����һ��10ms��ϵͳ��ʱ����ϵͳ�У�ʹ�ö�ʱ����ʱ�򣬶���10msΪ��׼ʹ��
  *
  * 
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "sys.h"
#include "main.h"
#include <stdio.h>

/*��̬����----------------------------------------------------------------*/
static u32 SYSTEM_REFERENCE_TIMER;
static SYSTIMERMSG	Sys_Timer_Msg[TIMERMSGNUM];

/**
  * @brief  ��ʱ����ʼ��
  *			����10ms�Ļ�׼��ʱ��
  * @param  None
  * @retval None
  */
void Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//TIM4 10ms������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 100; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	TIM_Cmd(TIM4, ENABLE);//������ʱ��4

	SYSTEM_REFERENCE_TIMER = 0;
}


/**
  * @brief  10ms��ʱ���жϴ�����
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //����жϱ�־
		
		SYSTEM_REFERENCE_TIMER++;		
	}	    
}

/**
  * @brief  ���һ����ʱ��
  * @param  msg����ʱ��ID
  *			timer��ʱ�䳤�ȡ�10ms*timer=ʵ��ʱ��(ms  )
  *			pCB������ʱ���ģ��ص�������
  * @retval None
  */
void Set_Timer_Msg(TIMERMSGSTYLE msg,  u32 timer, void (*pCB)())
{
	Sys_Timer_Msg[msg].flag = timer + SYSTEM_REFERENCE_TIMER;
	Sys_Timer_Msg[msg].pCb = pCB;
}

/**
  * @brief  �ر�һ����ʱ��
  * @param  msg����ʱ��ID
  * @retval None
  */
void Clear_Timer_Msg(TIMERMSGSTYLE msg)
{
	Sys_Timer_Msg[msg].flag = 0;
}

/**
  * @brief  ��ʱ��������
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

