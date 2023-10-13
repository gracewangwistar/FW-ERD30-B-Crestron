/*
*********************************************************************************************************
*
*	模块名称 : MCU FLASH驱动模块
*	文件名称 : bsp_flash.h
*	版    本 : V1.0
*	说    明 : 1,实时限位和遥控器是掉电存储
*				
*			
*				
*			
*         
*********************************************************************************************************
*/


#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "aconfig.h"

#define FLASH_PAGE_SIZE    0X200 /* 512Bytes */
 

typedef enum {
	FMC_STATUS_BUSY_POS = 0, /* 繁忙指示标志位
															0：FLASH 操作不繁忙；
															1：正在处理 FLASH 操作，在 FLASH 操作开始时设置该位（同时 EOP 位清零），操作完成后或者硬件出错时复位； */
	FMC_STATUS_ERR_POS = 2, /* 编程错误标志位
															0：读 0 表示无错误，写 0 将该位清零；
															1：下列情形是由硬件进行设置
															－开始擦除/编程，查找超过页边界的地址；
															－开始擦除/编程，查找不合规则的地址（>ROM size）；
															－填充数据到超过页边界的地址； */
	FMC_STATUS_ICACHE_ERR_POS = 3, /* 当加密使能时，FLASH 编程地址为 ICACHE CFM 映射地址范围内，则产生错误标志
																		0：读 0 表示无错误，写 0 将该位清零；
																		1：有错误； */
	
}E_FMC_STATUS_POS_TYPEDEF;	


typedef enum {
	FMC_CTRL_PG_POS = 0, /* FLASH 编程操作选择位。该位只能由软件置 1，BUSY 位复位时，该位也复位。
													1：启动；
													0：停止；*/
	FMC_CTRL_PER_POS = 1, /* 页擦除操作选择位 该位只能由软件置 1，BUSY 位复位时，该位也复位。
													 1：启动；
													 0：停止；*/
	FMC_CTRL_START_POS = 6, /* 开始页擦除或者页编程，该位只能由软件置 1，BUSY 位复位时，该位也复位。
														 1：启动；
														 0：停止； */
	
	FMC_CTRL_CHK_POS = 7, /* CRC16 CHKSUM 计算选择。自动计算 FAR 指定页的 1024字节的 FLASH 校验和。不能与 PER 或者 PG 同时置为 1。
													 该位只能由软件置 1，BUSY 位复位时，该位也复位。
													 1：启动；
													 0：停止； */	
	
}E_FMC_CTRL_POS_TYPEDEF;	

extern volatile uint32_t u32Flash_TimeOut;


void Flash_SectorErase(uint32_t addr);
void Flash_WriteWord(uint32_t addr, uint32_t data);
uint32_t Flash_ReadWord(uint32_t addr);
uint16_t Flash_ReadHalfWord(uint32_t addr);
uint8_t Flash_ReadByte(uint32_t addr);
void bsp_Flash_Read(uint32_t u32Flash_Addr, uint8_t *pu8Data_Pointer, uint32_t ReadByteCount); //void bsp_Flash_Read(uint32_t, uint8_t *, uint32_t);


#endif


