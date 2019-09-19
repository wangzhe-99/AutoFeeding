/**
  ******************************************************************************
  * @file    relay.c
  * @author  乐易住
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   继电器
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */
#include "relay.h"
#include "delay.h"

/**
  * @brief  继电器 GPIO的初始化
  * @param  None
  * @retval None
  */
void Relay_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin_6 | GPIO_Pin_7);
}

/**
  * @brief  继电器 开
  * @param  None
  * @retval None
  */
void Relay_Open(void)
{
	GPIO_SetBits(GPIOD, GPIO_Pin_7);
	delay_ms(300);
	Relay_Restore();
}

/**
  * @brief  继电器 关
  * @param  None
  * @retval None
  */
void Relay_Close(void)
{
	GPIO_SetBits(GPIOD, GPIO_Pin_6);
	delay_ms(300);
	Relay_Restore();
}

/**
  * @brief  继电器 GPIO拉低
  * @param  None
  * @retval None
  */
void Relay_Restore(void)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_6 | GPIO_Pin_7);
}


