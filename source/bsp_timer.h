/*
*********************************************************************************************************
*
*	模块名称 : 定时器驱动模块
*	文件名称 : bsp_timer.h
*	版    本 : V1.0
*	说    明 : 头文件
*	
*
*********************************************************************************************************
*/

#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include "aconfig.h"

#define TIMER_TIME_BASE  		             	TIM6
#define TIMER_TIME_BASE_IRQn             	LPTIM_TIM6_IRQn
#define TIMER_TIME_BASE_RCC              	RCC_APB1_PERIPH_TIM6
#define TIMER_TIME_BASE_IRQ_PRIORITY     	3 /* 1Ms定时中断 */ 


extern uint16_t Background_Timebase_Count;



void TIMER_Timebase_Init(void);



#endif

