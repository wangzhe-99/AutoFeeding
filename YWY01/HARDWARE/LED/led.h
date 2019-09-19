/**
  ******************************************************************************
  * @file    led.h
  * @author  ����ס
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   LED��
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */
#ifndef __LED_H
#define __LED_H	 
#include "stm32f10x.h"


void Led_Init(void);
void Led_On(void);
void Led_Off(void);
void Wifi_Led_Flashed_1S(void);

#endif
