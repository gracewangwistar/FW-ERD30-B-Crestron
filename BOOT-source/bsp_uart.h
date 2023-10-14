/*
*********************************************************************************************************
*
*	模块名称 : UART驱动模块
*	文件名称 : bsp_uart.h
*	版    本 : V1.0
*	说    明 : 
*				
*				
*				
*
*
*********************************************************************************************************
*/

#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "aconfig.h"


#define LPUARTy             LPUART
#define LPUARTy_CLK         RCC_LPUARTCLK
#define LPUARTy_RCC_CLK     RCC_APB1_PERIPH_LPUART
#define LPUARTy_GPIO        GPIOB 
#define LPUARTy_GPIO_CLK    RCC_APB2_PERIPH_GPIOB
#define LPUARTy_RxPin       GPIO_PIN_4 
#define LPUARTy_TxPin       GPIO_PIN_3 
#define LPUARTy_Rx_GPIO_AF  GPIO_AF5_LPUART
#define LPUARTy_Tx_GPIO_AF  GPIO_AF5_LPUART
#define LPUARTy_IRQn        LPUART_IRQn
#define LPUARTy_IRQHandler  LPUART_IRQHandler 
#define LPUART_IRQn_PRIORITY 0 
 
#define UART_RX_RECEIVE_LENGTH  64
#define UART_BAUD 115200//4800//9600//300//600//1200//115200//1200//2400 //9800//9900//4800//9600 //115200

/* LPUART 3.3V对外供电 控制端 */
#define LPUARTy_MODULE_POWER_EN_PORT			GPIOA	
#define LPUARTy_MODULE_POWER_EN_PIN				GPIO_PIN_15

#define LPUARTy_MODULE_POWER_EN_ON   GPIO_ResetBits(LPUARTy_MODULE_POWER_EN_PORT, LPUARTy_MODULE_POWER_EN_PIN)   
#define LPUARTy_MODULE_POWER_EN_OFF  GPIO_SetBits(LPUARTy_MODULE_POWER_EN_PORT, LPUARTy_MODULE_POWER_EN_PIN)   


 
typedef enum {
	UART_RX_QUEUE_NULL = 0,
	UART_RX_QUEUE_POINT_ERR,	
	UART_RX_QUEUE_FULL,	 
	UART_RX_QUEUE_EMPTY,	
	UART_RX_QUEUE_PUSH_SUCCESS,
	UART_RX_QUEUE_POP_SUCCESS,	
	
} E_UART_RX_QUEUE_STATE_TYPEDEF; 



void bsp_Uart_Init(void);
E_UART_RX_QUEUE_STATE_TYPEDEF UART_RX_Queue_Push(uint8_t Push_data);
uint8_t UART_RX_Search_a_Frame( uint8_t *pGet_data, uint8_t * pdata_size );
void uart_send(uint8_t *buf, uint32_t len);
void UART_Init(uint32_t baud);
unsigned char UartPutc(unsigned char my_ch);
uint32_t UartPutcs( uint8_t *pu8_Send_data, uint32_t ByteCount );
void Uart_Tx_Start(unsigned char my_ch);
void Uart_Rx_Handler(void);
void bsp_UART_enterStop_Init(void);
void bsp_UART_exitStop_Init(void);


#endif


