/*
*********************************************************************************************************
*
*	模块名称 : 用户函数库模块
*	文件名称 : User_Lib.h
*	版    本 : V1.0
*	说    明 : 头文件
*	
*
*********************************************************************************************************
*/

#ifndef __USER_LIB_H__
#define __USER_LIB_H__

#include "n32g031.h"

 
#define ALL_IRQ_DISABLE() __set_PRIMASK(1)//关闭总中断 __disable_irq()
#define ALL_IRQ_ENABLE()  __set_PRIMASK(0)//开放总中断__enable_irq() 

#define	BIT_SET(VAR, BIT) (VAR |= (1 << BIT))
#define	BIT_CLR(VAR, BIT) (VAR &= (~(1 << BIT)))

#define GET_D32LOW(D32)													  ((uint8_t)((uint32_t)(D32)))       /* 获取32位数据低八位     */
#define GET_D32MIDDLE_LOW(D32)										((uint8_t)((uint32_t)(D32) >> 8))  /* 获取32位数据次低八位   */
#define GET_D32MIDDLE_HIGH(D32)										((uint8_t)((uint32_t)(D32) >> 16)) /* 获取32位数据次高八位   */
#define GET_D32HIGH(D32)													((uint8_t)((uint32_t)(D32) >> 24)) /* 获取32位数据高八位     */

#define GET_D16LOW(D16)													  ((uint8_t)((uint16_t)(D16)))
#define GET_D16HIGH(D16)													((uint8_t)((uint16_t)(D16) >> 8))

//从缓冲区取32位数据
#define ARRAY_PTR2D32(PTR)    (((uint32_t)*((uint8_t *)(PTR)+3) << 24) | ((uint32_t)*((uint8_t *)(PTR)+2) << 16) | ((uint32_t)*((uint8_t *)(PTR)+1) << 8) | (uint32_t)*(uint8_t *)(PTR)) /* p为 指针 指针指向的内容为uint8_t */
//从缓冲区取16位数据
#define ARRAY_PTR2D16(PTR)    (((uint16_t)*((uint8_t *)(PTR)+1) << 8) | (uint16_t)*(uint8_t *)(PTR))
	
 


#define RELOAD_VALUE		((uint32_t)0x0000BB7F) /*Systick 系统默认配置为1ms*/



typedef enum {
	FALSE = 0,
	TRUE = 1,

}E_TRUE_STATUS_TYPEDEF;	
	

 
typedef union{
    struct{
						uint8_t BIT0:1;
						uint8_t BIT1:1;
						uint8_t BIT2:1;
						uint8_t BIT3:1;
						uint8_t BIT4:1;
						uint8_t BIT5:1;
						uint8_t BIT6:1;
						uint8_t BIT7:1;
		}bit;
    uint8_t Byte;
				 
}Byte_BitField_typedef;


typedef union{
    struct{
						uint8_t BIT0:1;
						uint8_t BIT1:1;
						uint8_t BIT2:1;
						uint8_t BIT3:1;
						uint8_t BIT4:1;
						uint8_t BIT5:1;
						uint8_t BIT6:1;
						uint8_t BIT7:1;
			
						uint8_t BIT8:1;
						uint8_t BIT9:1;
						uint8_t BIT10:1;
						uint8_t BIT11:1;
						uint8_t BIT12:1;
						uint8_t BIT13:1;
						uint8_t BIT14:1;
						uint8_t BIT15:1;			
		}bit;
    uint16_t HalfWord;
				 
}HalfWord_BitField_typedef;


typedef union{
    struct{
						uint8_t BIT0:1;
						uint8_t BIT1:1;
						uint8_t BIT2:1;
						uint8_t BIT3:1;
						uint8_t BIT4:1;
						uint8_t BIT5:1;
						uint8_t BIT6:1;
						uint8_t BIT7:1;
			
						uint8_t BIT8:1;
						uint8_t BIT9:1;
						uint8_t BIT10:1;
						uint8_t BIT11:1;
						uint8_t BIT12:1;
						uint8_t BIT13:1;
						uint8_t BIT14:1;
						uint8_t BIT15:1;	

						uint8_t BIT16:1;
						uint8_t BIT17:1;
						uint8_t BIT18:1;
						uint8_t BIT19:1;
						uint8_t BIT20:1;
						uint8_t BIT21:1;
						uint8_t BIT22:1;
						uint8_t BIT23:1;
						
						uint8_t BIT24:1;
						uint8_t BIT25:1;
						uint8_t BIT26:1;
						uint8_t BIT27:1;
						uint8_t BIT28:1;
						uint8_t BIT29:1;
						uint8_t BIT30:1;
						uint8_t BIT31:1;				
		}bit;
    uint16_t Word;
				 
}Word_BitField_typedef;

typedef uint8_t boolean_t;


uint16_t CRC_Caculate(uint8_t *pBuffer, uint32_t Byte_Count) ;
void bubble_Sort(uint32_t a[], uint32_t n);
uint16_t u8Arr_u16data(uint8_t arr[]);
void D32_D8Array(uint32_t *pdata, uint8_t Arr[]);
void D16_D8Array(uint16_t *pdata, uint8_t Arr[]);
uint8_t crc8_chk_value(uint8_t *ptr, uint8_t len);
void delay_us( uint32_t nTime );
void Delay1ms( uint32_t nTime );


#endif

