

/*
*********************************************************************************************************
*
*	模块名称 : UART应用层模块
*	文件名称 : app_uart.h
*	版    本 : V1.0
*	说    明 : 
*		         头字节   命令字   数据长度   功能区      校验（CRC-8/MAXIM）
*		         header   data1    data2      data3       Verify
*		         4Bytes   1Byte    1Byte      ≤50Bytes   1Byte		
*				
*				
*
*
*********************************************************************************************************
*/

#ifndef __APP_UART_H__
#define __APP_UART_H__

#include "aconfig.h"
#include "bsp_uart.h"

//void UART_Tx_Send_Data(void);            //串口数据发送
void Uart_Rx_Handler(void);				//串口接收中断调用，处理接收数据
unsigned char Uart_Tx_Handler(void);		//串口发送中断调用，处理串口发送数据

void UART_Rx_a_Frame_IDLE_Time_Proc_Check(void);
void UART_Tx_End_Delay_Time_Proc(void);

void UART_TX_Multiple_Bytes_Data( uint8_t *Data, uint8_t ByteSize );
//int LPUART_printf( char *file );




#endif





