/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file main.c
 * @author Nations 
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#include "main.h"
#include "bsp_uart.h"
#include "app_uart.h"
#include "bsp_flash.h"
#include "bsp_timer.h"
#include "ymodem.h"
#include "iap.h"

/* 
 *********************************************************************************************************
 *	
 *    宏定义
 *			  
 *********************************************************************************************************
 */

#define IAP_OK_PRINF_ENABLE 0


/* 
 *********************************************************************************************************
 *	
 *    枚举类型定义
 *			  
 *********************************************************************************************************
 */


/* 
 *********************************************************************************************************
 *	
 *    变量定义
 *			  
 *********************************************************************************************************
 */ 

static __IO uint8_t Flash_SectorErase_Enable = 0;  
//static uint8_t u8Read_Data[PAGE_SIZE] = {0};  
 
 
/* 
 *********************************************************************************************************
 *	
 *    函数声明
 *			  
 *********************************************************************************************************
 */
 
static void NVIC_Interrupt_Priority_Init(void); 
//static void Iap_Ok_Flash_App_Data_Printf(void);
void Is_boot_Mode_Info_print(void);
void RCC_Configuration(void);
void DebugArrayClear(void);



/* 
 *********************************************************************************************************
 *	
 *    Main函数
 *			  
 *********************************************************************************************************
 */ 

int main(void)
{	
	int32_t Size = 0;	
	ALL_IRQ_DISABLE(); /* 关总中断 */	
	bsp_Uart_Init(); /* UART初始化  */
	RCC_Configuration(); /* 系统时钟配置 */	
	NVIC_Interrupt_Priority_Init(); /* 中断优先级配置  */
	TIMER_Timebase_Init(); /* 时基定时器初始化  */	
	
	Delay1ms(500); 
	IsJumpToUserApp();	
	
	Ymodem_Analysis_Status = 0;
	TIM_ConfigInt(TIMER_TIME_BASE, TIM_INT_UPDATE, ENABLE); /* 时基定时器 Update IRQ 使能 */
	TIM_Enable(TIMER_TIME_BASE, ENABLE); /*时基定时器 计数功能 使能  */
	ALL_IRQ_ENABLE(); /* 开总中断 */

	while(1)
	{ 
		Size = Uart_Ymodem_Analysis();
		if( ANALYSIS_WORK_END == Ymodem_Analysis_Status )
		{
			Ymodem_Analysis_Status = 0;
			if( Size > 500 ) 
			{						
				Flash_SectorErase( FLASH_APP_EXIST_FLAG_ADDR );
				Flash_WriteWord( FLASH_APP_EXIST_FLAG_ADDR, 0x55aa55aa );

				// 向应用程序跳转
				jumpToApplication(APP, FLASH_APP_ADDR);
			}
		} 									
	}
}


/**
 * @brief  Configures the different system clocks.
 */
void RCC_Configuration(void)
{
    /* TIM1, GPIOA, GPIOB, GPIOE and AFIO clocks enable */
//    RCC_EnableAPB2PeriphClk(//RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB
//                          //| RCC_APB2_PERIPH_TIM1  
//													//| RCC_APB1_PERIPH_TIM3  
//													//| RCC_APB1_PERIPH_TIM6  
//	                        //| RCC_APB2_PERIPH_TIM8 
//													| RCC_APB2_PERIPH_AFIO,
//                            ENABLE);
    //When debug ,TIM1 and TIM8 stop
    DBG_ConfigPeriph(DBG_TIM1_STOP | DBG_TIM3_STOP | DBG_TIM6_STOP | DBG_TIM8_STOP , ENABLE);
#if 0	
    /* Keep Debugger Connection during STOP Mode */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    DBG_ConfigPeriph(DBG_STOP, ENABLE);	
	
    /* Keep Debugger Connection during PD Mode */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    DBG_ConfigPeriph(DBG_PD, ENABLE);	
	
#endif	
}



#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param file pointer to the source file name
 * @param line assert_param error line source number
 */
void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    while (1)
    {
    }
}

#endif

/*******************************************************************************
** 函数名:  NVIC_Interrupt_Priority_Init
** 功能  :  外设中断优先级配置(Cortex-M0内核，具体见内核使用手册P448)
** 输入参数:无
** 输出参数:无
** 作者:    胡天翔
** 创建日期:2022/01/15
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
void NVIC_Interrupt_Priority_Init(void)
{   
		NVIC_SetPriority( LPUARTy_IRQn, 0 );	
    NVIC_SetPriority( TIMER_TIME_BASE_IRQn, 2 );
	
}	


#if 0
/**
 * @brief  Selects PLL clock as System clock source and configure HCLK, PCLK2
 *         and PCLK1 prescalers.
 */
void SetSysClockToPLL(uint32_t freq, uint8_t src)
{
    ErrorStatus HSEStartUpStatus;
    ErrorStatus HSIStartUpStatus;
    uint32_t pllmul;
    uint32_t plldiv = RCC_PLLOUT_DIV_1;
    uint32_t latency;
    uint32_t pclk1div, pclk2div;

    if (HSE_VALUE != 8000000)
    {
        /* HSE_VALUE == 8000000 is needed in this project! */
        while (1);
    }

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration
     * -----------------------------*/
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    if (src == SYSCLK_USE_HSI) 
    {
        /* Enable HSI */
        RCC_ConfigHsi(RCC_HSI_ENABLE);

        /* Wait till HSI is ready */
        HSIStartUpStatus = RCC_WaitHsiStable();

        if (HSIStartUpStatus != SUCCESS)
        {
            /* If HSI fails to start-up, the application will have wrong clock
               configuration. User can add here some code to deal with this
               error */

            /* Go to infinite loop */
            while (1);
        }

    }
    else if (src == SYSCLK_USE_HSE) 
    {
        /* Enable HSE */
        RCC_ConfigHse(RCC_HSE_ENABLE);

        /* Wait till HSE is ready */
        HSEStartUpStatus = RCC_WaitHseStable();

        if (HSEStartUpStatus != SUCCESS)
        {
            /* If HSE fails to start-up, the application will have wrong clock
               configuration. User can add here some code to deal with this
               error */

            /* Go to infinite loop */
            while (1);
        }

    }

    

    switch (freq)
    {
        case 32000000:
            latency  = FLASH_LATENCY_1;
            if(src == SYSCLK_USE_HSI)
            {
                pllmul = RCC_PLL_MUL_8;
            }
            else if(src == SYSCLK_USE_HSE)
            {
                pllmul = RCC_PLL_MUL_8;
            }
            pclk1div = RCC_HCLK_DIV2;
            pclk2div = RCC_HCLK_DIV1;
            break;
        case 48000000:
            latency  = FLASH_LATENCY_2;
            if(src == SYSCLK_USE_HSI)
            {
              pllmul = RCC_PLL_MUL_12;
            }
            else if(src == SYSCLK_USE_HSE)
            {
               pllmul = RCC_PLL_MUL_12;
            }
            pclk1div = RCC_HCLK_DIV2;
            pclk2div = RCC_HCLK_DIV1;
            break;
        default:
            while (1);
    }

    FLASH_SetLatency(latency);

    /* HCLK = SYSCLK */
    RCC_ConfigHclk(RCC_SYSCLK_DIV1);

    /* PCLK2 = HCLK */
    RCC_ConfigPclk2(pclk2div);

    /* PCLK1 = HCLK */
    RCC_ConfigPclk1(pclk1div);
    if(src == SYSCLK_USE_HSE)
    {
        RCC_ConfigPll(RCC_PLL_SRC_HSE,pllmul, RCC_PLL_PRE_2, plldiv);
        
    }
    else
    {
        RCC_ConfigPll(RCC_PLL_SRC_HSI,pllmul, RCC_PLL_PRE_2, plldiv);
    }

    /* Enable PLL */
    RCC_EnablePll(ENABLE);

    /* Wait till PLL is ready */
   // while (RCC_GetFlagStatus(RCC_CTRL_FLAG_PLLRDF) == RESET);
     /* Wait till PLL is ready */
    while ((RCC->CTRL & RCC_CTRL_PLLRDF) == 0)
    {
    }
    /* Select PLL as system clock source */
    RCC_ConfigSysclk(RCC_SYSCLK_SRC_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_PLL);
}



/*******************************************************************************
** 函数名: Iap_Ok_Flash_App_Data_Printf
** 功能  : 串口打印app的程序（字符串形式16进制格式输出）
** 输入参数:无
** 输出参数:无
** 作者:    胡天翔
** 创建日期:2022/02/28
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
/*static void Iap_Ok_Flash_App_Data_Printf(void)
{
#if IAP_OK_PRINF_ENABLE 
		//读取Flash 串口打印
		uint8_t PageCount = 0;
		for (PageCount = 0; PageCount < 5 ; PageCount++)
		{
				bsp_Flash_Read( FLASH_APP_ADDR + (PAGE_SIZE * PageCount), &u8Read_Data[0], PAGE_SIZE );
				uint32_t i;
				uint32_t faddr = FLASH_APP_ADDR + (PAGE_SIZE * PageCount);
				for ( i = 0; i < PAGE_SIZE; i++ )
				{
						memset ( &DebugArray[ 0 ], 0, DEBUG_ARRAY_SIZE);
						sprintf( &DebugArray[ 0 ], " %s%X%s%X ", "\r\n Flash Address = 0X", faddr+i, "\r\n Flash data = 0X", u8Read_Data[i] );
						DebugArrayDataSize = strlen( (char *)DebugArray );
						UartPutcs( &DebugArray[0], DebugArrayDataSize );																	
										
				}				
		}							
#endif	

}
*/

/*******************************************************************************
** 函数名: Is_boot_Mode_Info_print
** 功能  : Mcu启动时进入boot功能串口打印提示信息（字符串形式输出）
** 输入参数:无
** 输出参数:无
** 作者:    胡天翔
** 创建日期:2022/02/28
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
void Is_boot_Mode_Info_print(void)
{
		DebugArrayClear();
		sprintf( &DebugArray[ 0 ], " %s ", "\r\n->>> enter boot Mode! \r\n" );
		LPUART_printf( DebugArray )	;			
}


/*******************************************************************************
** 函数名: DebugArrayClear
** 功能  : 清0打印调试信息用的数组
** 输入参数:无
** 输出参数:无
** 作者:    胡天翔
** 创建日期:2022/02/28
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
void DebugArrayClear(void)
{
    memset( DebugArray, 0, DEBUG_ARRAY_SIZE );
}

#endif


