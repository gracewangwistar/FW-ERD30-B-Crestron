/*
*********************************************************************************************************
*
*	模块名称 : UART驱动模块
*	文件名称 : bsp_uart.c
*	版    本 : V1.0
*	说    明 : 
*				
*				
*				
*
*
*********************************************************************************************************
*/

#include "bsp_uart.h"
#include "app_uart.h"
#include "bsp_timer.h"


typedef enum {
	UART_RX_SEARCH_FRAME_HEAD0_STATUS = 0,
	UART_RX_SEARCH_FRAME_HEAD1_STATUS,
	UART_RX_SEARCH_FRAME_HEAD2_STATUS,	
	UART_RX_SEARCH_FRAME_HEAD3_STATUS,

	UART_RX_SEARCH_FRAME_COM_STATUS,
	UART_RX_SEARCH_FRAME_LEN_STATUS,	
	UART_RX_SEARCH_FRAME_FUNCTION_STATUS,	
	UART_RX_SEARCH_FRAME_CRC_STATUS,	

}E_UART_RX_SEARCH_FRAME_STATUS_TYPEDEF;	

typedef enum {
	UART_RX_BYTE_HEAD0_POS = 0,
	UART_RX_BYTE_HEAD1_POS = 1,	
	UART_RX_BYTE_HEAD2_POS = 2,	
	UART_RX_BYTE_HEAD3_POS = 3,
	UART_RX_BYTE_COM_POS   = 4,	
	UART_RX_BYTE_LENTH_POS = 5,
	UART_RX_BYTE_FUNCTION_POS = 6,
	UART_RX_BYTE_CRC_POS = 7,	
	
}E_UART_RX_FRAME_CONTENT_POS_TYPEDEF;

static LPUART_InitType LPUART_InitStructure;

static void LPUART_RCC_Configuration(uint32_t LPUART_CLK_SRC);
static void LPUART_GPIO_Configuration(void);
static void LPUART_NVIC_Configuration(void);



/*******************************************************************************
** 函数名:  bsp_Uart_Init
** 功能  :  UART 初始化
** 输入参数:无
** 返回值:  无
** 作者:    胡天翔
** 创建日期:2021/5/17
** 修改者  :
** 修改日期:
** 修改内容:    		
*******************************************************************************/
void bsp_Uart_Init(void)
{
    //UART_RX_Queue_Reset(); /*UART接收队列参数初始化*/
	UART_Init(UART_BAUD); /*UART硬件参数初始化*/
}


/**
 * @brief  Configures the nested vectored interrupt controller.
 */
void LPUART_NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;

    /* Enable the LPUARTx Wakeup Interrupt through EXTI line 22 */
    NVIC_InitStructure.NVIC_IRQChannel                   = LPUART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority           = LPUART_IRQn_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
 * @brief  Configure EXTI_LINE23 as EXTI_Line and set the Priority.
 */
void LPUART_EXTI_Configuration(FunctionalState Cmd)
{    
    EXTI_InitType EXTI_InitStructure;
    
    EXTI_InitStructure.EXTI_Line    = EXTI_LINE22;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = Cmd;
    EXTI_InitPeripheral(&EXTI_InitStructure);
}


/**
 * @brief Configures the System clock frequency.
 * 
 */
void RCC_SetSysClock(void)
{
    /* Configures the PLL clock source and multiplication factor(SYSCLK_USE_HSI_PLL), SYSCLK_FREQ = 48MHz */
    RCC_ConfigPll(RCC_PLL_SRC_HSI, RCC_PLL_MUL_6, RCC_PLL_PRE_1, RCC_PLLOUT_DIV_1);

    /* Configures the system clock */
    RCC_ConfigSysclk(RCC_SYSCLK_SRC_PLLCLK);    
}


/*******************************************************************************
** 函数名:  UART_Init
** 功能  :  串口初始化
** 输入参数:baud: 波特率            
** 输出参数:无
** 作者:    胡天翔
** 创建日期:2022/5/17
** 修改者  :
** 修改日期:
** 修改内容:
** 说明: 1.使用管脚PB3(TX),PB4(RX)复用串口引脚
				 2.低功耗模式下使用LSE LSI才可以唤醒,实际测试LSI 唤醒最高波特率为4800bps(LUART STOP状态下唤醒 是通过EXTI LINE22)
				 3.串口配置：8数据位，1起始位，1停止位，无校验
				 4.使能发送、接收，启用接收中断
*******************************************************************************/
void UART_Init(uint32_t baud)  
{	
    /* Configure the GPIO ports */
    LPUART_GPIO_Configuration();			
	
    /* System Clocks Configuration */
	  LPUART_RCC_Configuration(RCC_LPUARTCLK_SRC_HSI);
	  //LPUART_RCC_Configuration(RCC_LPUARTCLK_SRC_SYSCLK);
    //LPUART_RCC_Configuration(RCC_LPUARTCLK_SRC_LSI); 
			
    /* Configure NVIC */
    LPUART_NVIC_Configuration();			
	
    /* Configure EXTI Line 22 */
    //LPUART_EXTI_Configuration(DISABLE);	
	
    /* LPUART configuration ------------------------------------------------------*/	
    LPUART_DeInit();    
    LPUART_InitStructure.BaudRate            = UART_BAUD;
    LPUART_InitStructure.Parity              = LPUART_PE_NO;
    LPUART_InitStructure.RtsThreshold        = LPUART_RTSTH_FIFOFU;
    LPUART_InitStructure.HardwareFlowControl = LPUART_HFCTRL_NONE;
    LPUART_InitStructure.Mode                = LPUART_MODE_RX | LPUART_MODE_TX;
    /* Configure LPUARTy */
    LPUART_Init(&LPUART_InitStructure);

    /* Enable LPUARTy Receive and Transmit interrupts */
    LPUART_ConfigInt(LPUART_INT_FIFO_NE, ENABLE);
    LPUART_ConfigInt(LPUART_INT_TXC, ENABLE);	
		
		/* 清除中断标志  */
		LPUART_ClrIntPendingBit(LPUART_INT_FIFO_NE | LPUART_INT_TXC | LPUART_INT_WUF 
		                        | LPUART_INT_FIFO_OV | LPUART_INT_FIFO_HF | LPUART_INT_PE | LPUART_INT_FIFO_FU);
		LPUART_ClrIntPendingBit(LPUART_INT_FIFO_NE);												
 
    /* Clear EXTI Line22 Pending Bit */
    EXTI_ClrITPendBit(EXTI_LINE22);
    LPUART_ClrIntPendingBit(LPUART_INT_WUF); 
 
    /* 接收中断  */
    NVIC_ClearPendingIRQ(LPUARTy_IRQn);
    NVIC_EnableIRQ(LPUARTy_IRQn);	
		
}


/**
 * @brief  Configures the different system clocks.
 * @param  LPUART_CLK_SRC: specifies the LPUARTy clock source.
 */
static void LPUART_RCC_Configuration(uint32_t LPUART_CLK_SRC)
{
    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(LPUARTy_GPIO_CLK, ENABLE);

    switch(LPUART_CLK_SRC)
    {
        case RCC_LPUARTCLK_SRC_HSI:
        {
            /* Configures the High Speed Internal RC clock (HSI) */
            RCC_EnableHsi(ENABLE); /* .s文件已经启动HSI 则该处不需要再配置 */
            while (RCC_WaitHsiStable() != SUCCESS)
            {
            }
            /* Specifies the LPUARTy clock source, HSI selected as LPUART clock */
            RCC_ConfigLPUARTClk(RCC_LPUARTCLK_SRC_HSI);
        }
        break;
        case RCC_LPUARTCLK_SRC_HSE:
        {
            /* Configures the External High Speed oscillator (HSE) */
            RCC_ConfigHse(RCC_HSE_ENABLE);
            while (RCC_WaitHseStable() != SUCCESS)
            {
            }
            /* Specifies the LPUARTy clock source, HSE selected as LPUARTy clock */
            RCC_ConfigLPUARTClk(RCC_LPUARTCLK_SRC_HSE);
        }
        break;
        case RCC_LPUARTCLK_SRC_LSI:
        {
            /* Enables the Internal Low Speed oscillator (LSI) */
            RCC_EnableLsi(ENABLE);
            while (RCC_GetFlagStatus(RCC_LSCTRL_FLAG_LSIRD) == RESET)
            {
            }
            /* Specifies the LPUARTy clock source, LSI selected as LPUARTy clock */
            RCC_ConfigLPUARTClk(RCC_LPUARTCLK_SRC_LSI);
        }
        break;
        case RCC_LPUARTCLK_SRC_LSE:
        {  
            /* Configures the External Low Speed oscillator (LSE) */
            RCC_ConfigLse(RCC_LSE_ENABLE);
            while (RCC_GetFlagStatus(RCC_LSCTRL_FLAG_LSERD) == RESET)
            {
            }
            /* Specifies the LPUARTy clock source, LSE selected as LPUARTy clock */
            RCC_ConfigLPUARTClk(RCC_LPUARTCLK_SRC_LSE);
        }
        break;        
        case RCC_LPUARTCLK_SRC_SYSCLK:
        {
            /* Specifies the LPUARTy clock source, SYSCLK selected as LPUARTy clock */
            RCC_ConfigLPUARTClk(RCC_LPUARTCLK_SRC_SYSCLK);
        }
        break;
        default:
        {
            /* Specifies the LPUARTy clock source, APB1 selected as LPUARTy clock */
            RCC_ConfigLPUARTClk(RCC_LPUARTCLK_SRC_APB1);
        }
        break;
    }    
    
    /* Enable LPUARTy Clock */
    RCC_EnableAPB1PeriphClk(LPUARTy_RCC_CLK, ENABLE);
}


/**
 * @brief  Configures the different GPIO ports.
 */
static void LPUART_GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(LPUARTy_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);       

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure LPUARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = LPUARTy_TxPin;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = LPUARTy_Tx_GPIO_AF;
    GPIO_InitPeripheral(LPUARTy_GPIO, &GPIO_InitStructure);

    /* Configure LPAURTx Rx as alternate function push-pull and pull-up */
    GPIO_InitStructure.Pin            = LPUARTy_RxPin;  
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Alternate = LPUARTy_Rx_GPIO_AF;    
    GPIO_InitPeripheral(LPUARTy_GPIO, &GPIO_InitStructure);

	
    /* Configure LPUARTx MODULE_POWER_EN as OUTPUT push-pull */
    GPIO_InitStructure.Pin            = LPUARTy_MODULE_POWER_EN_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.GPIO_Alternate = LPUARTy_Tx_GPIO_AF;
    GPIO_InitPeripheral(LPUARTy_MODULE_POWER_EN_PORT, &GPIO_InitStructure);	
		
		LPUARTy_MODULE_POWER_EN_OFF;
}


/**
 * @brief  Configures the different GPIO ports.
 */
void LPUART_GPIO_Configuration_io(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(LPUARTy_GPIO_CLK | RCC_APB2_PERIPH_AFIO, ENABLE);       

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure LPUARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = LPUARTy_TxPin;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;	
    GPIO_InitStructure.GPIO_Alternate = GPIO_NO_AF;
    GPIO_InitPeripheral(LPUARTy_GPIO, &GPIO_InitStructure);

    /* Configure LPAURTx Rx as alternate function push-pull and pull-up */
    GPIO_InitStructure.Pin            = LPUARTy_RxPin; 
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_INPUT;	
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_NO_AF;    
    GPIO_InitPeripheral(LPUARTy_GPIO, &GPIO_InitStructure);

	
    /* Configure LPUARTx MODULE_POWER_EN as OUTPUT push-pull */
    GPIO_InitStructure.Pin            = LPUARTy_MODULE_POWER_EN_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.GPIO_Alternate = LPUARTy_Tx_GPIO_AF;
    GPIO_InitPeripheral(LPUARTy_MODULE_POWER_EN_PORT, &GPIO_InitStructure);	
		
		LPUARTy_MODULE_POWER_EN_OFF;
}


/*******************************************************************************
* Description    : 串口发送1字节数据
* Input          : my_ch: 发送的数据
* Output         : None
* Return         : 返回已发送的数据
* Attention	     : 查询发送
*******************************************************************************/
unsigned char UartPutc(unsigned char my_ch)
{ 
    while( LPUART_GetFlagStatus( LPUART_FLAG_TXC ) );
    LPUART_SendData(my_ch);
    return (my_ch);
}


/*******************************************************************************
* Description    : 串口发送N字节数据
* Input          : *pu8_Send_data: 发送的数据指针 ,  uint32_t ByteCount
* Output         : None
* Return         : 返回已发送的数据
* Attention	     : 查询发送
*******************************************************************************/
uint32_t UartPutcs( uint8_t *pu8_Send_data, uint32_t ByteCount )
{
	  uint32_t i;
	  for ( i = 0; i < ByteCount; i++ )
		{
				UartPutc( pu8_Send_data[i] );		
		
		}
    return ByteCount;		
}


void Uart_Tx_Start(unsigned char my_ch)
{
    //while( LPUART_GetFlagStatus( LPUART_FLAG_TXC ) );
    LPUART_SendData(my_ch);
		LPUART_ConfigInt(LPUART_INT_TXC, ENABLE); /* 开启发送完成中断 */
}

/*******************************************************************************
* Description    : 串口中断
* Input          : None
* Output         : None
* Return         : None
* Attention	     : 返回已接收到数据
*******************************************************************************/
void LPUARTy_IRQHandler(void)
{
    /* Pin output high */
    //GPIO_WriteBit(REAL_TIME_GPIO_PORT, REAL_TIME_GPIO_PIN, Bit_SET);

    /* Clear EXTI Line22 Pending Bit */
    EXTI_ClrITPendBit(EXTI_LINE22);	
	  //if ( ( LPUART->INTEN & ( (uint16_t)1 << 6 ) ) && ( LPUART->STS & ( (uint16_t)1 << 7 ) ) )
		if ( ( LPUART->INTEN & LPUART_INT_WUF ) && ( LPUART_GetIntStatus(LPUART_INT_WUF) != RESET ) )	
		{
				LPUART_ClrIntPendingBit(LPUART_INT_WUF); /* 清LPUART唤醒中断标志位 */
			  //LPUART->STS = (uint16_t)1 << 7;			
			  LPUART_ConfigInt(LPUART_INT_WUF, DISABLE); /* 关闭LPUART唤醒中断使能位 */
				//LUART_IRQ_Proc();
		}	
	
	
    /* 接收中断标志置位 */
    if ( ( LPUART_GetIntStatus(LPUART_INT_FIFO_NE) != RESET ) && ( LPUART->INTEN & LPUART_INT_FIFO_NE ) )
		{
        /* Clear NE Pending Bit */
        LPUART_ClrIntPendingBit(LPUART_INT_FIFO_NE);		
				Uart_Rx_Handler();
		}
			
    /* 发送完成中断标志置位 */		
    if (LPUART_GetIntStatus(LPUART_INT_TXC) != RESET)
    {               
        /* Clear TXC Pending Bit */
        LPUART_ClrIntPendingBit(LPUART_INT_TXC);   
				/* Disable the LPUART Transmit interrupt */
				LPUART_ConfigInt(LPUART_INT_TXC, DISABLE);
				Uart_Tx_Handler();
		}
    /* Pin output low */
    //GPIO_WriteBit(REAL_TIME_GPIO_PORT, REAL_TIME_GPIO_PIN, Bit_RESET);		
}


void uart_send(uint8_t *buf, uint32_t len)
{
    for(int i=0;i<len;i++)
    {
        UartPutc(*(buf+i));
    }
	
}

