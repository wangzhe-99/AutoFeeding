/**
  ******************************************************************************
  * @file    led.c
  * @author  乐易住
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   LED灯
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/  
#include "stm32f10x.h"
#include "led.h"
#include "sys.h"
#include "timer.h"
#include "main.h"

/**
  * @brief  LED GPIO的初始化
  * @param  None
  * @retval None
  */
void Led_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_5);
}

/**
  * @brief  LED2 开
  * @param  None
  * @retval None
  */
void Led_On(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
}

/**
  * @brief  LED2 关
  * @param  None
  * @retval None
  */
void Led_Off(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_5);
}

/**
  * @brief  wifi指示灯1秒闪烁
  * @param  None
  * @retval None
  */
/*void Wifi_Led_Flashed_1S(void)
{

	if(IS_WIFI_LED == Bit_RESET)
	{
		Led1_Off();
	}
	else
	{
		Led1_On();
	}
	
	Set_Timer_Msg(TIMER_MSG_WIFI_LED_FLASHED, 100, Wifi_Led_Flashed_1S);
} */


