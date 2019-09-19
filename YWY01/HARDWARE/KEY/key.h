/**
  ******************************************************************************
  * @file    key.h  
  * @author  ÀÖÒ××¡
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   °´¼ü
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */
#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"

#define IS_KEY_DOWN (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6))

void Key_Init(void);		 				    
#endif
