/*
*********************************************************************************************************
*
*	模块名称 : 按键应用模块
*	文件名称 : app_key.c
*	版    本 : V1.0
*	说    明 : 
*				(1) 
*				(2) 
*				(3) 
*				(4) 
*
*
*********************************************************************************************************
*/

#include "User_Lib.h"
#include "bsp_uart.h"
#include "app_uart.h"
#include "bsp_flash.h"
#include "bsp_timer.h"
#include "ymodem.h"
#include "iap.h"


TIM_TimeBaseInitType Timebase_TimeBaseStructure;
OCInitType Timebase_OCInitStructure;
uint16_t Timebase_PrescalerValue = 0;



/**
 * @brief  Configure the nested vectored interrupt controller.
 */
static void TIMER_Timebase_NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;

    /* Enable the TIME BASE global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = TIMER_TIME_BASE_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority           = TIMER_TIME_BASE_IRQ_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;

    NVIC_Init(&NVIC_InitStructure);  
}


/**
 * @brief  Configures tim6 clocks. 1Ms定时中断
 */
static void TIMER_Timebase_Configuration(void)
{
    /* Compute the prescaler value */
    Timebase_PrescalerValue = 0; //(uint16_t) (SystemCoreClock / 12000000) - 1;

    /* Time base configuration */
    Timebase_TimeBaseStructure.Period    = 25000;
    Timebase_TimeBaseStructure.Prescaler = 0;
    Timebase_TimeBaseStructure.ClkDiv    = 0;
    Timebase_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIMER_TIME_BASE, &Timebase_TimeBaseStructure);

    /* Prescaler configuration */
    TIM_ConfigPrescaler(TIMER_TIME_BASE, Timebase_PrescalerValue, TIM_PSC_RELOAD_MODE_IMMEDIATE);

    /* TIMER_TIME_BASE enable update irq */
    //TIM_ConfigInt(TIMER_TIME_BASE, TIM_INT_UPDATE, ENABLE);

    /* TIMER_TIME_BASE enable counter */
    //TIM_Enable(TIMER_TIME_BASE, ENABLE);
}
/**
 * @brief  Configures the different system clocks.
 */
static void TIMER_Timebase_RCC_Configuration(void)
{
    /* PCLK1 = HCLK */
    RCC_ConfigPclk1(RCC_HCLK_DIV1);

    /* TIMER_TIME_BASE clock enable */
    RCC_EnableAPB1PeriphClk(TIMER_TIME_BASE_RCC, ENABLE);

}


/*******************************************************************************
* Description    : 时基定时器初始化
* Input          : 无
* Output         : None
* Return         : None
* Attention	     :                   
*******************************************************************************/
void TIMER_Timebase_Init(void)	
{
    /* System Clocks Configuration */
    TIMER_Timebase_RCC_Configuration();

    /* NVIC Configuration */
    TIMER_Timebase_NVIC_Configuration();
	
	/* Configures time base clocks. */
	TIMER_Timebase_Configuration();

	/* time base 中断使能 */
	NVIC_ClearPendingIRQ(TIMER_TIME_BASE_IRQn);
	NVIC_EnableIRQ(TIMER_TIME_BASE_IRQn);
}


/*******************************************************************************
* Description    : timer base(定时1ms) / LPTIM 中断(EROO mcu的TIMER2 作用未知)   
* Input          : None
* Output         : None
* Return         : None
* Attention	     : None
*******************************************************************************/
void LPTIM_TIM6_IRQHandler(void)
{	
	if ( TIM_GetIntStatus(TIMER_TIME_BASE, TIM_INT_UPDATE) != RESET )
	{
		TIM_ClrIntPendingBit(TIMER_TIME_BASE, TIM_INT_UPDATE);

		UART_Rx_a_Frame_IDLE_Time_Proc_Check();
		UART_Tx_End_Delay_Time_Proc();	

		if ( u32Flash_TimeOut )
		{
			u32Flash_TimeOut--;
		}

		if( Ymodem_Decode_Status_Timeout )
		{
			Ymodem_Decode_Status_Timeout --;
		}
		
	#if LED_DEBUG_ENABLE
		//Led_Appication_Proc();
	#endif				
	}		
}


