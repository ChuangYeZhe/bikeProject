
#define USART
#include "includes.h"
#include "SysInit.h"
#include "USART.h"
#include "IC.h"

void  COM_Init(void)  
{  
    COMM_RING_BUF *pbuf;  
  
    pbuf                  = &IC_Buf;                     //Initialize the  buffer for USART1 for IC卡 
    pbuf->RingBufRxLen    = 0; 							//buf中数据长度初始化为0 
    pbuf->RingBufRxSem    = OSSemCreate(0);  			//创建信号量
   
  
    pbuf                  = &ID_Buf;                    //Initialize the  buffer for  USART2 for 自行车ID  
    pbuf->RingBufRxLen    = 0;   
    pbuf->RingBufRxSem    = OSSemCreate(0);  
    
}
     
/********************串口4(IC卡)操作相关函数********************************/
//串口4初始化	
void USART4_Configuration(void)
{
    //串口初始化数据结构定义
	USART_InitTypeDef USART_InitStructure; 
		
	//初始化串口为38400,n,8,1
	USART_InitStructure.USART_BaudRate            = 9600  ;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	//初始化
	USART_Init(UART4, &USART_InitStructure);
	//开接收中断
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	//启动串口
	USART_Cmd(UART4, ENABLE);
}

//发送单字节数据
void USART4_SendByte(char Data)
{ 

   while (!(UART4->SR & USART_FLAG_TXE));
   UART4->DR = (Data & 0x00FF);
   while (!(UART4->SR & USART_FLAG_TC));   
}

//发送多个字节数据
void USART4Write(char* data,u16 len)
{
	u16 i;

	for (i=0; i<len; i++)
	{
		  USART4_SendByte(data[i]);	
		  if((i >= 2)&&(data[i] == IC_CMD_START0))	   //从第3个字节开始,即协议长度开始，有0xff则后面加发0x00
		  		  USART4_SendByte(0x00);
	}		
}

/*************************************************/
/*
	US4串口中断服务程序,将串口数据存储到环形buf中
*/
/*************************************************/
void UART4_IRQHandler(void)
{
	static u8 c_pre = 0;
	u8 c;
	
	OSIntEnter();

	//判断是否是正常的接收数据中断
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{ 	
	   
	    c=UART4->DR;	   									//读出接收到的数据

		if( (c== IC_CMD_START1) && (c_pre == IC_CMD_START0) ) //解析到协议头0xFF、0xFF
		{
		   IC_Buf.RingBufRxLen = 0;
		}
		else
		{
			if( IC_Buf.RingBufRxLen == 0)			//协议头后的第一个字节，应为数据长度
			{
				IC_Buf.RingBufRx[0]= c;  		//数据存进接收buf第1个字节
				IC_Buf.RingBufRxLen = 1;		//并标记接收到的数据长度为1

			}
			else
			{
			  	
				if(c_pre != IC_CMD_START0)		//如果前一个数据与起始字相同（该数肯定为0），该字节不存且不计长度
				{
					IC_Buf.RingBufRx[IC_Buf.RingBufRxLen] = c;	 //将数据存进buf中
					IC_Buf.RingBufRxLen++; 						// 接收到的数据长度++

					if( IC_Buf.RingBufRxLen == IC_Buf.RingBufRx[0])	  //如果接收到协议内指定的长度则post信号量
						 OSSemPost(IC_Buf.RingBufRxSem);
				
				}	
				
			}
				
		
		}
		c_pre = c;
	
	}

	USART_ClearITPendingBit(UART4, USART_IT_RXNE);//清接收中断状态位

	OSIntExit();

}


/*********************串口2(自行车ID)操作相关函数********************************/

 //串口2初始化	
void USART2_Configuration(void)
{
    //串口初始化数据结构定义
	USART_InitTypeDef USART_InitStructure; 
		
	//初始化串口为38400,n,8,1
	USART_InitStructure.USART_BaudRate            = 9600  ;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	//初始化
	USART_Init(USART2, &USART_InitStructure);
	//开接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//启动串口
	USART_Cmd(USART2, ENABLE);
}

//发送单字节数据
void USART2_SendByte(char Data)
{ 

   while (!(USART2->SR & USART_FLAG_TXE));
   USART2->DR = (Data & 0x00FF);
   while (!(USART2->SR & USART_FLAG_TC));   
}

//发送多个字节数据
void USART2Write(char* data,u16 len)
{
	u16 i;

	for (i=0; i<len; i++)
	{
		  USART2_SendByte(data[i]);	
		  if((i >= 2)&&(data[i] == IC_CMD_START0))	   //从第3个字节开始,即协议长度开始，有0xff则后面加发0x00
		  		  USART2_SendByte(0x00);
	}		
}
//中断处理
void USART2_IRQHandler(void)
{

	static u8 c_pre = 0;
	u8 c;

	OSIntEnter();

	//判断是否是正常的接收数据中断
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{ 	
	   
	    c=USART2->DR;	   									//读出接收到的数据

		if( (c== IC_CMD_START1) && (c_pre == IC_CMD_START0) ) //解析到协议头0xFF、0xFF
		{
		   ID_Buf.RingBufRxLen = 0;
		}
		else
		{
			if( ID_Buf.RingBufRxLen == 0)			//协议头后的第一个字节，应为数据长度
			{
				ID_Buf.RingBufRx[0]= c;  		//数据存进接收buf第1个字节
				ID_Buf.RingBufRxLen = 1;		//并标记接收到的数据长度为1

			}
			else
			{
			  	
				if(c_pre != IC_CMD_START0)		//如果前一个数据与起始字相同（该数肯定为0），该字节不存且不计长度
												//如果不为0的话，说明此帧肯定有错，早晚都被判断错的
				{
					ID_Buf.RingBufRx[ID_Buf.RingBufRxLen] = c;	 //将数据存进buf中
					ID_Buf.RingBufRxLen++; 						// 接收到的数据长度++

					if( ID_Buf.RingBufRxLen == ID_Buf.RingBufRx[0])	  //如果接收到协议内指定的长度则post信号量	
						 OSSemPost(ID_Buf.RingBufRxSem);
				
				}	
				
			}
				
		
		}
		c_pre = c;
	
	}

	USART_ClearITPendingBit(USART2, USART_IT_RXNE);//清接收中断状态位

	OSIntExit();

}
