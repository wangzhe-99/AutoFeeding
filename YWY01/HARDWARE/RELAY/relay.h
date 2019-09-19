/**
  ******************************************************************************
  * @file    relay.h
  * @author  ÀÖÒ××¡
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   ¼ÌµçÆ÷
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */
#ifndef __RELAY_H
#define __RELAY_H 			   
#include "stm32f10x.h"
#include "sys.h"
	 
void Relay_Init(void);
void Relay_Open(void);
void Relay_Close(void);
void Relay_Restore(void);

#endif

