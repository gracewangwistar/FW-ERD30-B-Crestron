

/*
*********************************************************************************************************
*
*	模块名称 : 用户函数库模块
*	文件名称 : User_Lib.c
*	版    本 : V1.0
*	说    明 : 源文件
*	
*
*********************************************************************************************************
*/

/* --------  通用函数库包含 -------- */

#include "User_Lib.h"



#define CRC16_MODBUS 0xFF
//#define CRC16_XMODEM 0xFF

#if CRC16_XMODEM
//CRC-16/XMODEM  X16 + X12 + X5 + 1 = 
static const unsigned int crc16tab[256]= 
{
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

#elif CRC16_MODBUS

/* CRC-16/Modbus  X16 + X15 + X2 + 1 = */
static const uint16_t crc_tab16[256] = 
{
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
};
#endif



#define        CRC_POLY_16            0xA001
#define        CRC_START_MODBUS       0xFFFF


/********************************************************************************************
函数: CRC_Caculate

描述: crc16计算

调用: 

被调用: 

输入: *pBuffer, Byte_Count

输出: 无 

返回: crc16校验码

其它: 
*********************************************************************************************/
uint16_t CRC_Caculate(uint8_t *pBuffer, uint32_t Byte_Count) 
{
    uint16_t Cyclic_Redundancy_Check; 
    uint16_t tmp;
    uint16_t short_c;
    uint16_t i;
	
	  if(0 == pBuffer) return 0xFFFF;
	  if(0 == Byte_Count) return 0xFFFF;	
	
    Cyclic_Redundancy_Check = CRC_START_MODBUS;
    pBuffer = pBuffer;
			
		for(i = 0; i < Byte_Count; i++) 
		{
				short_c = 0x00ff & (uint16_t)*pBuffer;
				tmp = Cyclic_Redundancy_Check ^ short_c;
				Cyclic_Redundancy_Check = (Cyclic_Redundancy_Check >> 8) ^ crc_tab16[tmp & 0xff];
				pBuffer++;
		}	  
    return Cyclic_Redundancy_Check;
}




void bubble_Sort(uint32_t a[], uint32_t n)//下面是函数bubble_Sort的程序 
{
    int i,j,temp;    //定义三个整型变量
  
    for (j = 0; j < n-1; j++)    //用一个嵌套循环来遍历一遍每一对相邻元素 （所以冒泡函数慢嘛，时间复杂度高）  
    {                           
        for (i = 0; i < n-1-j; i++)
        {
            if(a[i] > a[i+1])  //从大到小排就把左边的">"改为"<" ！！！
            {
                temp = a[i];      //a[i]与a[i+1](即a[i]后面那个) 交换
                a[i] = a[i+1];    //基本的交换原理"c=a;a=b;b=c" 
                a[i+1] = temp;
            }
        }
    }    
}





#if 0
#define keyType int
  
typedef struct {
    keyType key;//查找表中每个数据元素的值
    //如果需要，还可以添加其他属性
}ElemType;

typedef struct{
    ElemType *elem;//存放查找表中数据元素的数组
    int length;//记录查找表中数据的总数量
}SSTable;

//创建查找表
void Create(SSTable **st, int length){
    (*st) = (SSTable*)malloc(sizeof(SSTable));
    (*st)->length = length;
    (*st)->elem = (ElemType*)malloc((length + 1) * sizeof(ElemType));
    printf("输入表中的数据元素：\n");
    //根据查找表中数据元素的总长度，在存储时，从数组下标为 1 的空间开始存储数据
	  int i = 1;
    for ( i = 1; i <= length; i++) {
        scanf("%d", &((*st)->elem[i]. key));
    }
}




//折半查找算法 (使用前需要使用冒泡排序 将数据从小到大排序)
int Search_Bin(SSTable *ST, keyType key){
    int low = 1;//初始状态 low 指针指向第一个关键字
    int high = ST->length;//high 指向最后一个关键字
    int mid;
    while (low <= high) {
        mid = (low + high) / 2;//int 本身为整形，所以，mid 每次为取整的整数
        if (ST->elem[mid].key == key)//如果 mid 指向的同要查找的相等，返回 mid 所指向的位置
        {
            return mid;
        }else if(ST->elem[mid].key > key)//如果mid指向的关键字较大，则更新 high 指针的位置
        {
            high = mid - 1;
        }
        //反之，则更新 low 指针的位置
        else{
            low = mid + 1;
        }
    }
    return 0;
}

#endif


///**
//  * 函数功能:延时n 微秒(us)
//  * 输入参数: 无 
//  * 输入变量: 无
//  * 输出变量: 无
//  *           
//  * 返 回 值: 无
//  * 说    明：无
//  */
//void delay_nus(volatile	uint32_t timer_count)
//{
//		volatile uint32_t	temp = 0;
//	
//		temp = SysTick->VAL;
//		if(temp < (timer_count * 72))
//		{
//				while(SysTick->VAL < temp);				
//				while((temp + (RELOAD_VALUE - SysTick->VAL)) < (timer_count * 72));									
//		}
//		else
//		{
//				while((temp - SysTick->VAL) < (timer_count * 72));				
//		}
//}





///**
//  * 函数功能: 延时n毫秒
//  * 输入参数: 无 
//  * 输入变量: 无
//  * 输出变量: 无
//  *           
//  * 返 回 值: 无
//  * 说    明：无
//  */
//void delay_nms(volatile	uint32_t timer_count)
//{
//		volatile uint32_t	i = timer_count << 1;
//		while(i --)
//		{
//				delay_nus(500 - 1);
//		}
//}


/*******************************************************************************
** 函数名:  u8Arr_u16data
** 功能  :  将一维数组2byte转换成1个16位整形数据
** 输入参数:arr[]数组
** 输出参数:16位的整形数据
** 作者:    韩浩
** 创建日期:2019/10/30
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
uint16_t u8Arr_u16data(uint8_t arr[])
{
    return     (((uint32_t)arr[1]) & 0xff) << 8
              | (((uint32_t)arr[0]) & 0xff) << 0;
}


/*******************************************************************************
** 函数名:  D32_D8Array
** 功能  :  将1个32位整形数据转换成一维数组4byte
** 输入参数:arr[]数组，32位的数据地址
** 输出参数:
** 作者:    杨卫波
** 创建日期:2019/7/10
** 修改者  :胡天翔
** 修改日期:2021/12/31
** 修改内容:
*******************************************************************************/
void D32_D8Array(uint32_t *pdata, uint8_t arr[])
{
    uint8_t i;
    for(i = 0; i < 4; i ++ )
    {
        arr[i] = *((uint8_t *)pdata + i);
    }
}


/*******************************************************************************
** 函数名:  D16_D8Array
** 功能  :  将1个16位整形数据转换成一维数组2byte
** 输入参数:arr[]数组，16位的数据地址
** 输出参数:
** 作者:    杨卫波
** 创建日期:2019/7/10
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
void D16_D8Array(uint16_t *pdata, uint8_t Arr[])
{
    uint8_t i;
    for(i = 0; i < 2; i ++ )
    {
        Arr[i] = *((uint8_t *)pdata + i);
    }
}


/*******************************************************************************
** 函数名:  crc8_chk_value
** 功能  :  生成crc8校验值
** 输入参数:*ptr：数组指针，len:数据长度
** 输出参数:en_result_t，Error失败  Ok成功
** 作者:    杨卫波
** 创建日期:2019/7/10
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
uint8_t crc8_chk_value(uint8_t *ptr, uint8_t len)
{
    uint8_t crc = 0;
    uint8_t i;
    while(len--)
    {
        crc ^= *ptr++;
        for(i = 0;i < 8;i++)
        {
            if(crc & 0x01)
            {
                crc = crc >> 1;
                crc = crc ^ 0x8c;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc; 

}

#define SYS_TICK_fac 48
/*******************************************************************
函数名：delay_us
输入：uint32_t nTime
输出：无
作用：延时us
*******************************************************************/
void delay_us( uint32_t Us )
{
		SysTick->LOAD = SYS_TICK_fac*Us;
		SysTick->CTRL = 0x00000005;
		while ( !( SysTick->CTRL&0x00010000 ) );
		SysTick->CTRL &= (~0x00000001); /* systick停止计数 */
}


/*******************************************************************************
** 函数名:  Delay1ms
** 功能  :  SYS TICK 1ms倍数延时
** 输入参数:time: 1ms时间倍数值
** 输出参数:无
** 作者:    杨卫波
** 创建日期:2019/7/10
** 修改者  :
** 修改日期:
** 修改内容:
*******************************************************************************/
void Delay1ms(uint32_t Ms)
{
		for(; Ms>0; Ms--)
		delay_us(1000);
}




