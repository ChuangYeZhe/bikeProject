#ifndef __CAN_H
#define	__CAN_H

/****************************************
*	功能：CAN初始化
****************************************/
void CAN_Config(u32 ID);

/****************************************
*	功能：设置报文内容
****************************************/   
void CAN_SendMsg(u8 *buf,u8 len);

#define   CAN_RX_BUF_SIZE 64
/* 
 * DATA TYPES 
 */  
typedef struct {  
    unsigned short     BufRxLen;                /* Number of characters in the Rx ring buffer              */
	unsigned short     R_Index;
	unsigned short     W_Index;
	OS_EVENT  		   *RingBufRxSem;           /* Pointer to Rx semaphore                                 */  
    unsigned char      BufRx[CAN_RX_BUF_SIZE]; /* Ring buffer character storage (Rx)                      */  
} CanRxBUF; 

#ifdef CAN_RX

	CanRxBUF CAN_RX_BUF;
	OS_EVENT 	*CAN_TX_Mutex; 	

#else 

	extern CanRxBUF CAN_RX_BUF;
	extern OS_EVENT 	*CAN_TX_Mutex; 	
#endif
 
#endif
