#ifndef __USART_H
#define __USART_H

#include "includes.h"


#define COMM_RX_BUF_SIZE  64
/* 
 * DATA TYPES 
 */  
typedef struct {  
    unsigned short     RingBufRxLen;                /* Number of characters in the Rx ring buffer              */  
    OS_EVENT  		  *RingBufRxSem;                /* Pointer to Rx semaphore                                 */  
    unsigned char      RingBufRx[COMM_RX_BUF_SIZE]; /* Ring buffer character storage (Rx)                      */  
} COMM_RING_BUF;

#ifdef USART

	COMM_RING_BUF IC_Buf;
	COMM_RING_BUF ID_Buf;

	void  COM_Init(void);
	void USART1_SendByte(char dat);
	void USART1_Configuration(void);
	void USART1Write(char* data,u16 len);		

#else

	extern COMM_RING_BUF IC_Buf;
	extern COMM_RING_BUF ID_Buf;

	extern void  COM_Init(void);
	extern void USART1_SendByte(char dat);
	extern void USART1_Configuration(void);
	extern void USART1Write(char* data,u16 len);

#endif
  

#endif
