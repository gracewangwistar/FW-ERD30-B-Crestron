
/*
*********************************************************************************************************
*
*	模块名称 : MCU FLASH驱动模块
*	文件名称 : bsp_flash.c
*	版    本 : V1.0
*	说    明 : 提供以下底层函数：
*				(1) 单字节写入
*				(2) 4字节写入
*				(3) 单字节读
*				(4) 2字节读
*       (5) 4字节读
*       (6) page擦除
*
*********************************************************************************************************
*/

#include "bsp_flash.h"


volatile uint32_t u32Flash_TimeOut = 0; 


/**
 *****************************************************************************
 * @brief     Flash页擦除
 * 
 * @param     uint32_t addr：Flash地址
 * @param     无
 * @retval    无
 * 
 * @see       Flash_SectorErase
 * @note      Something to note.
 * @warning   Warning.
 *****************************************************************************
 */
void Flash_SectorErase(uint32_t addr)
{
    /* Unlocks the FLASH Program Erase Controller */
    FLASH_Unlock();	
    // if(0x800fc00 <  addr) return;
		if (FLASH_COMPL != FLASH_EraseOnePage(addr))
		{
		
		}

    /* Locks the FLASH Program Erase Controller */
    FLASH_Lock();	
}


/**
 *****************************************************************************
 * @brief     Flash指定地址写4字节数据
 * 
 * @param     uint32_t addr：Flash地址
 * @param     uint32_t data：数据
 * @retval    无
 * 
 * @see       Flash_WriteByte
 * @note      Something to note.
 * @warning   Warning.
 *****************************************************************************
 */
void Flash_WriteWord(uint32_t addr, uint32_t data)
{
    /* Unlocks the FLASH Program Erase Controller */
    FLASH_Unlock();		
	
		if (FLASH_COMPL != FLASH_ProgramWord(addr, data)) 
    {
		
		}
    /* Locks the FLASH Program Erase Controller */
    FLASH_Lock();			
}


///**
// *****************************************************************************
// * @brief     Flash指定地址读4字节数据
// * 
// * @param     uint32_t addr：Flash地址
// * @param     uint32_t data：数据
// * @retval    无
// * 
// * @see       Flash_WriteWord
// * @note      Something to note.
// * @warning   Warning.
// *****************************************************************************
// */
//void Flash_WriteWord(uint32_t addr, uint32_t data)
//{
//    AD_FMC->CTRL = 0x01;   // ENABLE THE PAGE PROGRAM
//    AD_FMC->ADDR = addr;
//    AD_FMC->DATA = data;
//    AD_FMC->CTRL = 0x41;   // START THE FLASH PROGRAM
//    while((AD_FMC->STATUS & 0x01) == 0x01);    
//}


/**
 *****************************************************************************
 * @brief     Flash指定地址读4字节数据
 * 
 * @param     uint32_t addr：Flash地址
 * @param     无
 * @retval    uint32_t :4字节数据
 * 
 * @see       Flash_ReadWord
 * @note      Something to note.
 * @warning   Warning.
 *****************************************************************************
 */
uint32_t Flash_ReadWord(uint32_t addr)
{
		//FLASH_STS FLASH_WaitForLastOpt(ProgramTimeout);	
    return *(__IO uint32_t*)addr;
}


/**
 *****************************************************************************
 * @brief     Flash指定地址读2字节数据
 * 
 * @param     uint32_t addr：Flash地址
 * @param     无
 * @retval    uint16_t :2字节数据
 * 
 * @see       Flash_ReadHalfWord
 * @note      Something to note.
 * @warning   Warning.
 *****************************************************************************
 */
uint16_t Flash_ReadHalfWord(uint32_t addr)
{
    return *(__IO uint16_t*)addr;
}


/**
 *****************************************************************************
 * @brief     Flash指定地址读1字节数据
 * 
 * @param     uint32_t addr：Flash地址
 * @param     无
 * @retval    uint16_t :1字节数据
 * 
 * @see       Flash_ReadByte
 * @note      Something to note.
 * @warning   Warning.
 *****************************************************************************
 */
uint8_t Flash_ReadByte(uint32_t addr)
{
    return *(__IO uint8_t*)addr;
}


/*******************************************************************************
** 函数名:  bsp_Flash_Read
** 功能  :  FLASH  读服务函数
** 输入参数:u32Flash_Addr读入首地址， *pu8Data_Pointer，数据指针，ReadByteCount，读入个数
** 输出参数:无
** 作者:    胡天翔
** 创建日期:2021/12/31
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
void bsp_Flash_Read(uint32_t u32Flash_Addr, uint8_t *pu8Data_Pointer, uint32_t ReadByteCount)
{
	  uint32_t Data_Buf;

    while(ReadByteCount > 0)
    {
			  if (ReadByteCount >= sizeof(uint32_t))
				{
				    Data_Buf = Flash_ReadWord(u32Flash_Addr);
				    D32_D8Array(&Data_Buf, pu8Data_Pointer);
					  pu8Data_Pointer += 4;
					  u32Flash_Addr += 4;
					  ReadByteCount -= 4;
				}
				else 
				if (ReadByteCount >= sizeof(uint16_t))
				{
				    Data_Buf = Flash_ReadHalfWord(u32Flash_Addr);
				    D16_D8Array((uint16_t *)&Data_Buf, pu8Data_Pointer);
					  pu8Data_Pointer += 2;
					  u32Flash_Addr += 2;
					  ReadByteCount -= 2;					
				}
				else 
				if (ReadByteCount >= sizeof(uint8_t))
				{
				    Data_Buf = Flash_ReadHalfWord(u32Flash_Addr);
				    *pu8Data_Pointer = (uint8_t)Data_Buf;
					  pu8Data_Pointer += 1;
					  u32Flash_Addr += 1;
					  ReadByteCount -= 1;					
				}	
				else 
				{
				
				}
    }
}





