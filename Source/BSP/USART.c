
#define USART
#include "includes.h"
#include "SysInit.h"
#include "USART.h"
#include "IC.h"

void  COM_Init(void)  
{  
    COMM_RING_BUF *pbuf;  
  
    pbuf                  = &IC_Buf;                     //Initialize the  buffer for USART1 for IC�� 
    pbuf->RingBufRxLen    = 0; 							//buf�����ݳ��ȳ�ʼ��Ϊ0 
    pbuf->RingBufRxSem    = OSSemCreate(0);  			//�����ź���
   
  
    pbuf                  = &ID_Buf;                    //Initialize the  buffer for  USART2 for ���г�ID  
    pbuf->RingBufRxLen    = 0;   
    pbuf->RingBufRxSem    = OSSemCreate(0);  
    
}
     
/********************����4(IC��)������غ���********************************/
//����4��ʼ��	
void USART4_Configuration(void)
{
    //���ڳ�ʼ�����ݽṹ����
	USART_InitTypeDef USART_InitStructure; 
		
	//��ʼ������Ϊ38400,n,8,1
	USART_InitStructure.USART_BaudRate            = 9600  ;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	//��ʼ��
	USART_Init(UART4, &USART_InitStructure);
	//�������ж�
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	//��������
	USART_Cmd(UART4, ENABLE);
}

//���͵��ֽ�����
void USART4_SendByte(char Data)
{ 

   while (!(UART4->SR & USART_FLAG_TXE));
   UART4->DR = (Data & 0x00FF);
   while (!(UART4->SR & USART_FLAG_TC));   
}

//���Ͷ���ֽ�����
void USART4Write(char* data,u16 len)
{
	u16 i;

	for (i=0; i<len; i++)
	{
		  USART4_SendByte(data[i]);	
		  if((i >= 2)&&(data[i] == IC_CMD_START0))	   //�ӵ�3���ֽڿ�ʼ,��Э�鳤�ȿ�ʼ����0xff�����ӷ�0x00
		  		  USART4_SendByte(0x00);
	}		
}

/*************************************************/
/*
	US4�����жϷ������,���������ݴ洢������buf��
*/
/*************************************************/
void UART4_IRQHandler(void)
{
	static u8 c_pre = 0;
	u8 c;
	
	OSIntEnter();

	//�ж��Ƿ��������Ľ��������ж�
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{ 	
	   
	    c=UART4->DR;	   									//�������յ�������

		if( (c== IC_CMD_START1) && (c_pre == IC_CMD_START0) ) //������Э��ͷ0xFF��0xFF
		{
		   IC_Buf.RingBufRxLen = 0;
		}
		else
		{
			if( IC_Buf.RingBufRxLen == 0)			//Э��ͷ��ĵ�һ���ֽڣ�ӦΪ���ݳ���
			{
				IC_Buf.RingBufRx[0]= c;  		//���ݴ������buf��1���ֽ�
				IC_Buf.RingBufRxLen = 1;		//����ǽ��յ������ݳ���Ϊ1

			}
			else
			{
			  	
				if(c_pre != IC_CMD_START0)		//���ǰһ����������ʼ����ͬ�������϶�Ϊ0�������ֽڲ����Ҳ��Ƴ���
				{
					IC_Buf.RingBufRx[IC_Buf.RingBufRxLen] = c;	 //�����ݴ��buf��
					IC_Buf.RingBufRxLen++; 						// ���յ������ݳ���++

					if( IC_Buf.RingBufRxLen == IC_Buf.RingBufRx[0])	  //������յ�Э����ָ���ĳ�����post�ź���
						 OSSemPost(IC_Buf.RingBufRxSem);
				
				}	
				
			}
				
		
		}
		c_pre = c;
	
	}

	USART_ClearITPendingBit(UART4, USART_IT_RXNE);//������ж�״̬λ

	OSIntExit();

}


/*********************����2(���г�ID)������غ���********************************/

 //����2��ʼ��	
void USART2_Configuration(void)
{
    //���ڳ�ʼ�����ݽṹ����
	USART_InitTypeDef USART_InitStructure; 
		
	//��ʼ������Ϊ38400,n,8,1
	USART_InitStructure.USART_BaudRate            = 9600  ;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	//��ʼ��
	USART_Init(USART2, &USART_InitStructure);
	//�������ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//��������
	USART_Cmd(USART2, ENABLE);
}

//���͵��ֽ�����
void USART2_SendByte(char Data)
{ 

   while (!(USART2->SR & USART_FLAG_TXE));
   USART2->DR = (Data & 0x00FF);
   while (!(USART2->SR & USART_FLAG_TC));   
}

//���Ͷ���ֽ�����
void USART2Write(char* data,u16 len)
{
	u16 i;

	for (i=0; i<len; i++)
	{
		  USART2_SendByte(data[i]);	
		  if((i >= 2)&&(data[i] == IC_CMD_START0))	   //�ӵ�3���ֽڿ�ʼ,��Э�鳤�ȿ�ʼ����0xff�����ӷ�0x00
		  		  USART2_SendByte(0x00);
	}		
}
//�жϴ���
void USART2_IRQHandler(void)
{

	static u8 c_pre = 0;
	u8 c;

	OSIntEnter();

	//�ж��Ƿ��������Ľ��������ж�
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{ 	
	   
	    c=USART2->DR;	   									//�������յ�������

		if( (c== IC_CMD_START1) && (c_pre == IC_CMD_START0) ) //������Э��ͷ0xFF��0xFF
		{
		   ID_Buf.RingBufRxLen = 0;
		}
		else
		{
			if( ID_Buf.RingBufRxLen == 0)			//Э��ͷ��ĵ�һ���ֽڣ�ӦΪ���ݳ���
			{
				ID_Buf.RingBufRx[0]= c;  		//���ݴ������buf��1���ֽ�
				ID_Buf.RingBufRxLen = 1;		//����ǽ��յ������ݳ���Ϊ1

			}
			else
			{
			  	
				if(c_pre != IC_CMD_START0)		//���ǰһ����������ʼ����ͬ�������϶�Ϊ0�������ֽڲ����Ҳ��Ƴ���
												//�����Ϊ0�Ļ���˵����֡�϶��д��������жϴ��
				{
					ID_Buf.RingBufRx[ID_Buf.RingBufRxLen] = c;	 //�����ݴ��buf��
					ID_Buf.RingBufRxLen++; 						// ���յ������ݳ���++

					if( ID_Buf.RingBufRxLen == ID_Buf.RingBufRx[0])	  //������յ�Э����ָ���ĳ�����post�ź���	
						 OSSemPost(ID_Buf.RingBufRxSem);
				
				}	
				
			}
				
		
		}
		c_pre = c;
	
	}

	USART_ClearITPendingBit(USART2, USART_IT_RXNE);//������ж�״̬λ

	OSIntExit();

}
