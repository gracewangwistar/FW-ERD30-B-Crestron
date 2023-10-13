/*******************************************************************************
** 文件名  :ymodem.h
** 作者:    杨卫波
** 创建日期:2019/8/2
** 版权:    杭州威仕达机电科技有限公司
** ----------------------------------------------------------------------------
** 修改者:  
** 修改日期:
** 修改内容:
*******************************************************************************/
#ifndef __YMODEM_H__
#define __YMODEM_H__

#include "aconfig.h"
 
#define YMODEM_CRC_ENABLE  		(1)

#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define RX_PACKET_SIZE					(PACKET_1K_SIZE + PACKET_OVERHEAD)

#define FILE_SIZE_LENGTH        (16)
#define FILE_NAME_LENGTH        (128 - FILE_SIZE_LENGTH -2)

#define SOH                     (0x01)  //128字节数据包开始
#define STX                     (0x02)  //1024字节的数据包开始
#define EOT                     (0x04)  //结束传输
#define ACK                     (0x06)  //回应
#define NAK                     (0x15)  //没回应
#define CA                      (0x18)  //这两个相继中止转移
#define CRC16                   (0x43)  //'C' == 0x43, 需要 16-bit CRC 
#define MODEM_C					(0x43)	//'C'

#define ABORT1                  (0x41)  //'A' == 0x41, 用户终止 
#define ABORT2                  (0x61)  //'a' == 0x61, 用户终止

#define NAK_TIMEOUT             (0x100000)
#define MAX_ERRORS              (5)

#define FLASH_IAP_START_FLAG    0x55AA33CC 
#define FLASH_IAP_OPREAT_FLAG   0x66778899 
#define FLASH_BOOT_SIZE         0x4000  // BOOT 16kb + 0.5KB    2k ram

#define	ACK_TIMEOUT				2000
#define UART_RX_IDLE_TIME_VALUE	30
#define ANALYSIS_WORK_END		0xff


#define FLASH_IMAGE_SIZE        (uint32_t) (FLASH_SIZE - FLASH_APP_ADDR)
#define DISABLE_IRQ()      		{ __disable_irq();  }
#define ENABLE_IRQ()       		{ __enable_irq();   }





#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))


 
/* 
 *********************************************************************************************************
 *	
 *    结构体类型定义
 *			  
 *********************************************************************************************************
 */



/* 
 *********************************************************************************************************
 *	
 *    枚举类型定义
 *			  
 *********************************************************************************************************
 */
 
typedef enum {
		YMODEM_SOH 		= 0X01,
		YMODEM_STX 		= 0X02,
		YMODEM_EOT 		= 0X04,	
		YMODEM_ACK 		= 0X06,	
		YMODEM_NAK 		= 0X15,	
		YMODEM_CAN 		= 0X18,
		YMODEM_C   		= 0X43,

}E_YMODEM_COMMAND_TYPDEF;	


typedef enum {
		YMODEM_DECODE_STATUS_C = 0,
		YMODEM_DECODE_STATUS_SOH,
		YMODEM_DECODE_STATUS_STX,		
		YMODEM_DECODE_STATUS_EOT,
		YMODEM_DECODE_STATUS_END_FRAME,	
	
}E_YMODEM_DECODE_STATUS_TYPDEF;	



/* 
 *********************************************************************************************************
 *	
 *    变量声明
 *			  
 *********************************************************************************************************
 */

extern uint8_t  Uart_Received_Packet_Head;
extern uint8_t  Uart_Receive_Packet[ RX_PACKET_SIZE ];
extern uint16_t Uart_Rx_Byte_Count;
extern uint16_t Uart_Rx_Byte_Count_Buf;
extern uint32_t Uart_Rx_Idle_Time;

extern uint8_t Ymodem_Analysis_Status;
extern uint16_t Ymodem_Decode_Status_Timeout; 


/* 
 *********************************************************************************************************
 *	
 *    函数声明
 *			  
 *********************************************************************************************************
 */

extern int32_t Uart_Ymodem_Analysis( void );

extern void Int2Str(uint8_t* str, int32_t intnum);
extern void Int2_To_Hex_Str(uint8_t* str, int32_t intnum);
extern uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum);
extern uint32_t FLASH_PagesMask(__IO uint32_t Size);
extern uint8_t Check_Flash(uint32_t addr);
extern void iap_load_app(uint32_t appxaddr);




#endif



