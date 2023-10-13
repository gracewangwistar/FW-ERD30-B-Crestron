/*
*********************************************************************************************************
*
*	模块名称 : 在綫升級功能模塊
*	文件名称 : IAP_1.h
*	版    本 : V1.0
*	说    明 : 
*		         
*		        
*		        
*				
*				
*
*
*********************************************************************************************************
*/
#ifndef __IAP_1_H__
#define __IAP_1_H__

#include "aconfig.h"
#include "bsp_flash.h"

/* 
 *********************************************************************************************************
 *	
 *    宏定义
 *			  
 *********************************************************************************************************
 */
 
#define LED_DEBUG_ENABLE			0
#define UART_PRINTF_DEBUG_ENABLE    1
 
#define APPOffset					(0x0002000) //(8*1024) 
#define FLASH_APP_EXIST_FLAG_ADDR	(0x08001E00) /* FLASH存儲地址： 用戶程序是否存在標記   */
#define FLASH_APP_ADDR				(uint32_t)(0x08000000 | APPOffset) //APP1的入口地址
#define PAGE_SIZE               	FLASH_PAGE_SIZE /* 512字节 */
#define FLASH_SIZE              	(0x10000) /* 64KBytes */
#define FLASH_APP_SIZE				(0x08010000 - FLASH_APP_ADDR)
#define FLASH_APP_PAGE_SIZE     	(FLASH_APP_SIZE / PAGE_SIZE) 
 
#define DEBUG_ARRAY_SIZE 			200
#define BOOT_PINs_ENABLE 			0


/** @defgroup SYSCFG_Memory_Remap_Config 
  * @{
  */ 
#define SYSCFG_MemoryRemap_Flash                ((uint8_t)0x00)
#define SYSCFG_MemoryRemap_SystemMemory         ((uint8_t)0x01)
#define SYSCFG_MemoryRemap_SRAM                 ((uint8_t)0x03)


#define IS_SYSCFG_MEMORY_REMAP(REMAP) (((REMAP) == SYSCFG_MemoryRemap_Flash) || \
                                       ((REMAP) == SYSCFG_MemoryRemap_SystemMemory) || \
                                       ((REMAP) == SYSCFG_MemoryRemap_SRAM))

 
/* 
 *********************************************************************************************************
 *	
 *    定义枚举类型
 *			  
 *********************************************************************************************************
 */

typedef enum
{
		BOOT_LOADER,  //向bootloader跳转
		APP,          //向应用程序跳转
	
}E_BOOT_JUMP_TYPEDEF;



/* 
 *********************************************************************************************************
 *	
 *    定义结构体类型
 *			  
 *********************************************************************************************************
 */



/* 
 *********************************************************************************************************
 *	
 *    变量声明
 *			  
 *********************************************************************************************************
 */

extern unsigned char jump;
extern uint8_t bootpin_flag;
extern char DebugArray[ DEBUG_ARRAY_SIZE ]; 
extern uint32_t DebugArrayDataSize;	

/* 
*********************************************************************************************************
*	
*    函数声明
*			  
*********************************************************************************************************
*/

void FlashLoader(void);
void IsJumpToUserApp(void);
unsigned char jumpToApplication (E_BOOT_JUMP_TYPEDEF app_jumpType, unsigned int appAddress); //此函数用于跳转前



#endif





