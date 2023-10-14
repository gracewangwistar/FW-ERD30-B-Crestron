/*******************************************************************************
** 文件名  :ymodem.c
** 包含文件:
** 实现功能:和Ymodem.c的相关的协议文件
            负责从超级终端接收数据(使用Ymodem协议)，并将数据加载到内部RAM中。
            如果接收数据正常，则将数据编程到Flash中；如果发生错误，则提示出错。
** 作者:    杨卫波
** 创建日期:2019/8/2
** 版权:    杭州威仕达机电科技有限公司
** ----------------------------------------------------------------------------
** 修改者:  
** 修改日期:
** 修改内容:
*******************************************************************************/

#include "stdbool.h"
#include "User_Lib.h"
#include "bsp_flash.h"
#include "bsp_uart.h"
#include "app_uart.h"
#include "ymodem.h"
#include "iap.h"


uint8_t IAP_Status;	//IAP处理状态
uint16_t PageSize = PAGE_SIZE;
uint32_t EraseCounter = 0;
uint32_t NbrOfPage = 0;
uint32_t FlashDestination = FLASH_APP_ADDR;

uint8_t  Uart_Received_Packet_Head; /* IAP使用的串口 接收到的数据包头 */
uint16_t Uart_Rx_Byte_Count_Buf = 0;
uint16_t Uart_Rx_Byte_Count = 0; 
uint32_t Uart_Rx_Idle_Time;

uint8_t  Ymodem_Analysis_Status = YMODEM_DECODE_STATUS_C;
uint16_t Ymodem_Decode_Status_Timeout;

uint8_t Uart_Receive_Packet[ RX_PACKET_SIZE ];
uint8_t file_name[FILE_NAME_LENGTH];
uint8_t file_size[FILE_SIZE_LENGTH];

void MSR_MSP(unsigned  int addr);	//设置堆栈地址

/****************************Info********************************************** 
 * Name:    CRC-16/XMODEM       x16+x12+x5+1 
 * Width:    16 
 * Poly:    0x1021 
 * Init:    0x0000 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x0000 
 * Alias:   CRC-16/ZMODEM,CRC-16/ACORN 
 *****************************************************************************/ 
unsigned short CRC16_XMODEM(unsigned char *data, unsigned int datalen)
{
    unsigned short wCRCin = 0x0000;
    unsigned short wCPoly = 0x1021;
    
    while (datalen--)     
    {
        wCRCin ^= (*(data++) << 8);
        for(int i = 0;i < 8;i++)
        {
            if(wCRCin & 0x8000)
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else
                wCRCin = wCRCin << 1;
        }
    }
    return (wCRCin);
}



/*******************************************************************************
** 函数名:  Str2Int
** 功能  :  字符串转到数据
** 输入参数:inputstr:需转换的字符串
** 输出参数:intnum：转好的数据    1：正确  0：错误
** 作者:    杨卫波
** 创建日期:2019/8/2
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }

        if (i >= 11)
        {
            res = 0;
        }
    }
    else//最多10为2输入
    {
        for (i = 0; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
            {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }
        //超过10位无效，返回0
        if (i >= 11)
        {
            res = 0;
        }
    }

    return res;
}


/*******************************************************************************
** 函数名:  Send_Byte
** 功能  :  发送一个字符
** 输入参数:c: 发送的字符
** 输出参数:0：成功发送
** 作者:    杨卫波
** 创建日期:2019/8/2
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
static uint32_t Send_Byte (uint8_t c)
{
	Uart_Tx_Start( c );
    return 0;
}


/*******************************************************************************
** 函数名:  Ymodem_File_Name_Analysis
** 功能  :  解析第一帧数据
** 输入参数:无
** 输出参数:文件长度
** 作者:    杨卫波
** 创建日期:2019/8/30
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
int32_t Ymodem_File_Name_Analysis( void )
{
	uint8_t *file_ptr;
	int32_t i,size;
	file_ptr = Uart_Receive_Packet + PACKET_HEADER;
	//
	for (i = 0, file_ptr = Uart_Receive_Packet + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
	{
		file_name[i++] = *file_ptr++;
	}
	file_name[i++] = '\0';
	for (i = 0, file_ptr ++; (*file_ptr != ' ') && (*file_ptr != '\0') && (i < FILE_SIZE_LENGTH);)
	{
		file_size[i++] = *file_ptr++;
	}
	file_size[i++] = '\0';
	Str2Int(file_size, &size);
	
	return size;
}

/**
* 发送停止传输命令到发送方
* uint8_t cnt 发送 CA 的数量
*/
void ymd_ca_send(void)
{
	DebugArray[ 0 ] = CA;
	DebugArray[ 1 ] = CA;
	UART_TX_Multiple_Bytes_Data( (uint8_t*)&DebugArray[ 0 ], 2);
}

/*******************************************************************************
** 函数名:  Uart_Ymodem_Analysis
** 功能  :  解析上位机通过串口5进行YMODEM协议传输的数据
** 输入参数:无
** 输出参数:文件长度
** 作者:    
** 创建日期:2019/8/2
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
int32_t Uart_Ymodem_Analysis( void )
{
	static int32_t File_Size = 1;
	static uint32_t Packets_Count_Received = 0; 
	uint16_t A_Packets_Size_Received; 
	uint16_t i; 
	uint16_t Calculated_CRC16 = 0; 
	uint16_t Packet_CRC16; 
	uint32_t u32Write_Data = 0;

	switch( Ymodem_Analysis_Status )
	{
		case YMODEM_DECODE_STATUS_C:
		{
			Send_Byte(MODEM_C);	
			Ymodem_Decode_Status_Timeout = ACK_TIMEOUT;
			Ymodem_Analysis_Status = YMODEM_DECODE_STATUS_SOH;
			Packets_Count_Received = 0;
			break;
		}
		//////////////////////接收文件名称及大小///////////////////////////////////////////////
		case YMODEM_DECODE_STATUS_SOH:
		{
			if( 0 == Ymodem_Decode_Status_Timeout) 
			{
				Ymodem_Analysis_Status = YMODEM_DECODE_STATUS_C; 
			}
			else
			{
				if( ( SOH == Uart_Received_Packet_Head ) || ( STX == Uart_Received_Packet_Head ) )
				{
					Uart_Received_Packet_Head = 0;
					Ymodem_Analysis_Status = YMODEM_DECODE_STATUS_C;

					if( Uart_Receive_Packet[PACKET_SEQNO_INDEX] != ( ( Uart_Receive_Packet[PACKET_SEQNO_COMP_INDEX] ^ 0xff ) & 0xff ) )
					{
						ymd_ca_send(); // 有错误直接结束传输
		
						return -1;						
					}

					File_Size = Ymodem_File_Name_Analysis();	
					if ( File_Size > ( FLASH_APP_SIZE ) )
					{
						ymd_ca_send(); // 有错误直接结束传输

						return -1;
					}

				#if YMODEM_CRC_ENABLE
					Packet_CRC16 = 0;
					Packet_CRC16 = Uart_Receive_Packet[PACKET_HEADER + PACKET_SIZE];
					Packet_CRC16 <<= 8;
					Packet_CRC16 |= Uart_Receive_Packet[PACKET_HEADER + PACKET_SIZE + 1] ;
					Calculated_CRC16 = 0;
					Calculated_CRC16 = CRC16_XMODEM(&Uart_Receive_Packet[PACKET_HEADER], PACKET_SIZE);
					if ( Calculated_CRC16 != Packet_CRC16 )
					{								
						ymd_ca_send(); // 有错误直接结束传输
						
						return -1;																		
					}							
				#endif

					// 按页擦除需要用到的Flash
					uint8_t PageCount = 0;
					for (PageCount = 0; PageCount < FLASH_APP_PAGE_SIZE ; PageCount++)
					{
						Flash_SectorErase( FLASH_APP_ADDR + (PAGE_SIZE * PageCount) );				
					}

					DebugArray[ 0 ] = ACK;
					DebugArray[ 1 ] = MODEM_C;
					UART_TX_Multiple_Bytes_Data( (uint8_t*)&DebugArray[ 0 ], 2 );

					FlashDestination = FLASH_APP_ADDR;
					Ymodem_Decode_Status_Timeout = ACK_TIMEOUT;
					Ymodem_Analysis_Status = YMODEM_DECODE_STATUS_STX;
				}
			}
			break;
		}
		//////////////////////接收数据主体///////////////////////////////////////////////
		case YMODEM_DECODE_STATUS_STX:
		{
			if( 0 == Ymodem_Decode_Status_Timeout) 
			{
				File_Size = -2;
				Ymodem_Analysis_Status = ANALYSIS_WORK_END; /* 退出 */
				ymd_ca_send(); // 有错误直接结束传输
			}
			else
			{
				if( ( STX == Uart_Received_Packet_Head ) || ( SOH == Uart_Received_Packet_Head ) )
				{
					if( STX == Uart_Received_Packet_Head )
					{
						A_Packets_Size_Received = PACKET_1K_SIZE;
					}
					else
					{
						A_Packets_Size_Received = PACKET_SIZE;
					}
					Uart_Received_Packet_Head = 0;
					Packets_Count_Received++;

					// 帧序号 和 帧序号取反 对比判断
					if( Uart_Receive_Packet[PACKET_SEQNO_INDEX] != ( ( Uart_Receive_Packet[PACKET_SEQNO_COMP_INDEX] ^ 0xff ) & 0xff ) )
					{
						ymd_ca_send(); // 有错误直接结束传输
					
						return -1;						
					}
					
				#if YMODEM_CRC_ENABLE			
					Packet_CRC16 = 0;
					Packet_CRC16 = Uart_Receive_Packet[PACKET_HEADER + A_Packets_Size_Received];
					Packet_CRC16 <<= 8;
					Packet_CRC16 |= Uart_Receive_Packet[PACKET_HEADER + A_Packets_Size_Received + 1] ;
					Calculated_CRC16 = 0;
					Calculated_CRC16 = CRC16_XMODEM(&Uart_Receive_Packet[PACKET_HEADER], A_Packets_Size_Received);
					if (Calculated_CRC16 != Packet_CRC16)
					{
						ymd_ca_send(); // 有错误直接结束传输

						return -1;																		
					}
				#endif
   
				#if 1      					
					for ( i = 0; ( i < A_Packets_Size_Received ) && ( FlashDestination < FLASH_APP_ADDR + File_Size ); )
					{
						// 把接收到的数据编写到Flash中
						u32Write_Data = ARRAY_PTR2D32( &Uart_Receive_Packet[PACKET_HEADER + i] ); 
						Flash_WriteWord( FlashDestination, u32Write_Data );
						FlashDestination += 4;							
						i += 4;									
					}
				#endif					
					Send_Byte(ACK);	/* 发送ACK */
					Ymodem_Decode_Status_Timeout = ACK_TIMEOUT;
				}else if( EOT == Uart_Received_Packet_Head ) {
					Ymodem_Analysis_Status = YMODEM_DECODE_STATUS_EOT;
					Uart_Received_Packet_Head = 0;
					Send_Byte(NAK);	/* 发送NAK */
					Ymodem_Decode_Status_Timeout = ACK_TIMEOUT;
				}
			}
			break;
		}
		////////////////////////////EOT切换//////////////////////////////////////////
		case YMODEM_DECODE_STATUS_EOT:
		{
			if( 0 == Ymodem_Decode_Status_Timeout) 
			{
				File_Size = -2;
				Ymodem_Analysis_Status = ANALYSIS_WORK_END; /* 退出 */
				ymd_ca_send(); // 有错误直接结束传输
			}
			else
			{
				if( EOT == Uart_Received_Packet_Head )
				{
					Uart_Received_Packet_Head = 0;
					Ymodem_Decode_Status_Timeout = ACK_TIMEOUT;
					DebugArray[ 0 ] = ACK;
					DebugArray[ 1 ] = MODEM_C;
					UART_TX_Multiple_Bytes_Data( (uint8_t*)&DebugArray[ 0 ], 2 );							

					Ymodem_Analysis_Status = YMODEM_DECODE_STATUS_END_FRAME;
				}
			}
			break;
		}
		////////////////////////////接收最后一帧数据//////////////////////////////////////////
		case YMODEM_DECODE_STATUS_END_FRAME:
		{
			if( 0 == Ymodem_Decode_Status_Timeout) 
			{
				File_Size = -2;
				Ymodem_Analysis_Status = ANALYSIS_WORK_END;	/* 退出 */
				ymd_ca_send(); // 有错误直接结束传输
			}
			else
			{
				if( SOH == Uart_Received_Packet_Head )
				{
					Uart_Received_Packet_Head = 0;
					Send_Byte(ACK);	/* 发送ACK */
					Ymodem_Analysis_Status = ANALYSIS_WORK_END;	/* 结束 */
				}
			}
		}break;

		default:
		{
			File_Size = -2;
			Ymodem_Analysis_Status = ANALYSIS_WORK_END;	/* 退出 */
			ymd_ca_send(); // 有错误直接结束传输
		}break;
	}

	return File_Size;
}

#if 0
/*******************************************************************************
** 函数名:  iap_load_app
** 功能  :  跳转到应用程序段
** 输入参数:appxaddr:用户代码起始地址
** 输出参数:无
** 作者:    杨卫波
** 创建日期:2019/7/27
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
void iap_load_app(uint32_t appxaddr)
{
	
//	if(((*(__IO uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)//检查栈顶地址是否合法.
//	{ 
//		/*
//		！！！此句非常重要
//		！！！不可用__disable_irq()代替
//		！！！__disable_irq()为关闭所有中断，当用__enable_irq()开启时
//		会造成一些七七八八的乱七八糟的问题
//		但是用NVIC_ICER[0] = 0XFFFFFFFF 时，为失能中断，再跳转后的程序中
//		对需要的中断再次进行失能即可，也不用再在跳转后的程序中加入__enable_irq()
//		*/
//		//NVIC->ICER[0] = 0XFFFFFFFF; //使能M0芯片中所有的外设中断 		
//		jump2app=(iapfun)*(__IO uint32_t*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
//		SysTick->CTRL=0; //关闭系统定时器
//		
//		uint32_t i;
//		for(i=0;i<48;i++) //映射中断向量到 RAM区的0x20000000为始的48个字中
//		{
//			*((uint32_t*)(0x20000000 + (i << 2)))=*(volatile unsigned int*)(appxaddr + (i<<2));
//		}
//		
//	//	i = AD_SYS1->CONFIG;
//		/* Clear MEM_MODE bits */
//		i &= (uint32_t) (~( 1 << 0 ) );

//		/* Set the new MEM_MODE bits value */
//		i |= (uint32_t) ( 1 << 0 );

//		/* Set CFGR1 register with the new memory remap configuration */
//	//	AD_SYS1->CONFIG = i;				
//		//AD_SYS1->CONFIG |= 1; /* Remap SRAM at 0x00000000 */ 		; //将入口地址映射成为SRAM
//		//RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, ENABLE);	
//		//MSR_MSP(*(__IO uint32_t*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
//		/* Initialize user application's Stack Pointer */
//		__set_MSP(*(__IO uint32_t*) appxaddr);
//		//ALL_IRQ_DISABLE();
//		jump2app();	//跳转到APP.   
//	}	
	
}

/*******************************************************************************
** 函数名:  Check_Flash
** 功能  :  内部FALSH读检查空
** 输入参数:flash地址
** 输出参数:无
** 作者:    杨卫波
** 创建日期:2019/7/27
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
uint8_t Check_Flash(uint32_t addr)
{ 
    uint32_t temp; 
    temp =  *(volatile uint32_t *)(addr + 504);
    if(temp != FLASH_IAP_START_FLAG)
    {
        return 0;
    }
    temp =  *(volatile uint32_t *)(addr + 508);
    if(temp != FLASH_IAP_OPREAT_FLAG)
    {
        return 0;
    }
	 return 1;
}


/*******************************************************************************
** 函数名:  FLASH_PagesMask
** 功能  :  计算Falsh页
** 输入参数:Size:文件长度
** 输出参数:页的数量
** 作者:    杨卫波
** 创建日期:2019/8/2
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
    uint32_t pagenumber = 0x0;
    uint32_t size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;

}
#endif

