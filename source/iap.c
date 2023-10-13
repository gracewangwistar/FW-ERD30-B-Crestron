
#include "main.h"

/* --------  MCU函数库包含 --------- */
#include "n32g031.h"
#include "core_cm0.h"

/* --------  用户函数库包含 -------- */
#include "bsp_uart.h"
#include "app_uart.h"
#include "bsp_flash.h"
#include "bsp_timer.h"
#include "ymodem.h"
#include "iap.h"


//
#define SYS_PRST_ALL    	(AD_SYS1->PRST = 0x1FCFFF)



#define MMU_VTOR               ((__IO unsigned*)(0x40024C30))					

#define _VTOREN()              (*MMU_VTOR = (*MMU_VTOR) | 0x80000000);	

#define _VTORVALUE()           (*MMU_VTOR = (*MMU_VTOR) | FLASH_APP_ADDR);	

#define _VTORVALUE_CLEAR()     (*MMU_VTOR = (*MMU_VTOR) & 0x00000000);

#define _VTORVALUE_READ()      ( (*MMU_VTOR) ); 

typedef void (*IAPfun)(void);

IAPfun jump2app; 
uint8_t bootpin_flag;

char DebugArray[ DEBUG_ARRAY_SIZE ] = {0}; 
uint32_t DebugArrayDataSize = 0;	
unsigned char jump = 0;


/**
  * @brief  Configures the memory mapping at address 0x00000000.
  * @param  SYSCFG_MemoryRemap: selects the memory remapping.
  *          This parameter can be one of the following values:
  *            @arg SYSCFG_MemoryRemap_Flash: Main Flash memory mapped at 0x00000000  
  *            @arg SYSCFG_MemoryRemap_SystemMemory: System Flash memory mapped at 0x00000000
  *            @arg SYSCFG_MemoryRemap_SRAM: Embedded SRAM mapped at 0x00000000
  * @retval None
  */
void SYSCFG_MemoryRemapConfig(uint32_t SYSCFG_MemoryRemap)
{
#if 0	
  uint32_t tmpctrl = 0;
	FLASH_STS Return_Status;

  /* Check the parameter */
  assert_param(IS_SYSCFG_MEMORY_REMAP(SYSCFG_MemoryRemap));
	
	tmpctrl = FLASH_GetUserOB();	
	Return_Status = FLASH_EraseOB();
  if ( FLASH_COMPL != Return_Status )
	{
	
	}
	FLASH_ProgramOBData(uint32_t Address, uint32_t Data);	
	
  /* Get CFGR1 register value */
  tmpctrl = SYSCFG->CFGR1;

  /* Clear MEM_MODE bits */
  tmpctrl &= (uint32_t) (~SYSCFG_CFGR1_MEM_MODE);

  /* Set the new MEM_MODE bits value */
  tmpctrl |= (uint32_t) SYSCFG_MemoryRemap;

  /* Set CFGR1 register with the new memory remap configuration */
  SYSCFG->CFGR1 = tmpctrl;
	
#endif	
}


/*******************************************************************************
** 函数名: IsJumpToUserApp
** 功能  : 检测是否跳转userapp
** 输入参数:无
** 输出参数:无
** 作者:    胡天翔
** 创建日期:2022/02/28
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
void IsJumpToUserApp(void)
{
	uint32_t write_app_flag;
	uint8_t app_exsit_flag[4] = {0};

	bootpin_flag = 0;
	bsp_Flash_Read( FLASH_APP_EXIST_FLAG_ADDR, app_exsit_flag, 4);		
	write_app_flag = ARRAY_PTR2D32( app_exsit_flag );

	if ( write_app_flag != 0x55aa55aa ) 
	{
		return;
	}	

	jumpToApplication (APP, FLASH_APP_ADDR); 
}


unsigned char jumpToApplication (E_BOOT_JUMP_TYPEDEF app_jumpType, unsigned int appAddress)
{	
	uint32_t vtor_value;

	if(app_jumpType == APP)  
	{  			
		/*RAM起始地址：0x20000000 ，N32G031RAM大小为8KB*/
		if(((*(vu32*)appAddress)&0x0FFFFFFF) < 1024*8) 
		{ 
			/* UART接收中断失能  */
			NVIC_ClearPendingIRQ(LPUARTy_IRQn);
			NVIC_DisableIRQ(LPUARTy_IRQn);
			/* 时基定时器 失能计数功能 */
			TIM_Enable(TIMER_TIME_BASE, DISABLE);	
			/* 时基定时器 失能 update IRQ */
			TIM_ConfigInt(TIMER_TIME_BASE, TIM_INT_UPDATE, DISABLE);		

			/* 清除UART中断标志  */
			LPUART_ClrIntPendingBit(LPUART_INT_FIFO_NE | LPUART_INT_TXC | LPUART_INT_WUF 
									| LPUART_INT_FIFO_OV | LPUART_INT_FIFO_HF | LPUART_INT_PE | LPUART_INT_FIFO_FU);
			LPUART_ClrIntPendingBit(LPUART_INT_FIFO_NE);			

			/* Disable LPUARTy Receive and Transmit interrupts */
			LPUART_ConfigInt(LPUART_INT_FIFO_NE, DISABLE);
			LPUART_ConfigInt(LPUART_INT_TXC, DISABLE);	
								 
			/* 内核级UART中断失能  */
			NVIC_ClearPendingIRQ(LPUARTy_IRQn);
			NVIC_DisableIRQ(LPUARTy_IRQn);	

			uint32_t loopTimes = 0;
			do
			{
				_VTORVALUE_CLEAR();
				_VTORVALUE(); 
				_VTOREN();    
				vtor_value = _VTORVALUE_READ();
				loopTimes++;
			}while ( vtor_value != ( FLASH_APP_ADDR | 0x80000000 ) );			

			jump2app = (IAPfun)*(vu32*)( appAddress + 4 );				
			__set_MSP( *(vu32*)appAddress );
			jump2app();	
		}
	}
	
	return 1; 
}






