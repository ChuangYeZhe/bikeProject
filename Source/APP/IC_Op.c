
/**********************************************************/

/*
	该文件主要实现与IC卡通讯相关的操作函数
*/

/**********************************************************/
#define IC

#include "USART.h"
#include "IC.h"

u8 Check_Sum(u8 *data, u8 len)
{
   u8 csum = 0x00;
   u8 i;

   for( i = 0; i < (len - 1); i++)
   		csum= csum^	data[i];	//计算校验XOR
	
	if(csum == data[len-1])
		return 0;
	else
		return 1;		

}

u8 Block_Read(u8 block)
{
 	u8 i,err;
	u8 csum;

	csum = 0x00;

	TX_Buf[0] = 0xFF;		//起始字0
	TX_Buf[1] = 0xFF;	    //起始字1
	TX_Buf[2] = 0x0A;		//数据长度 10字节
	TX_Buf[3] = 0x12;	  	//Block_Read命令类型

	TX_Buf[4] = 0x00;		//keyA认证读操作
	TX_Buf[5] = block;		//块号

	for(i = 6;i < 12; i++)
		TX_Buf[i] = KEY_A[i-6];	 //keyA值

	for(i = 2; i < 12; i++)	
		csum= csum^	TX_Buf[i];	//计算校验XOR

	TX_Buf[12] = csum;			//填充校验

 	USART4Write(TX_Buf,0x0C);  //发送13字节

	//等待读头应答

	OSSemPend(IC_Buf.RingBufRxSem,1000,&err);	//超时1s
	
	if(err==OS_TIMEOUT)							/*超时借车失败*/
	{
		return 0;
	}

	err = Check_Sum(IC_Buf.RingBufRx,IC_Buf.RingBufRxLen);

	if(err != 0 )	 //校验失败
	{
		return 0;
	}

	if(IC_Buf.RingBufRx[1] != 0x12)	//返回状态码必须正确
	{
		return 0;
	}

	return 1;
 
}

u8 Block_Write(u8 block,u8 *buf)
{
 	u8 i,err;
	u8 csum = 0x00;

	TX_Buf[0] = 0xFF;		//起始字0
	TX_Buf[1] = 0xFF;	    //起始字1
	TX_Buf[2] = 0x1A;		//数据长度 26字节
	TX_Buf[3] = 0x13;	  	//Block_Write命令类型

	TX_Buf[4] = 0x01;		//keyB认证写操作
	TX_Buf[5] = block;		//块号

	for(i = 6;i < 12; i++)
		TX_Buf[i] = KEY_B[i-6];	 //keyB值

	for(i = 12; i < 28; i++)
		TX_Buf[i] = buf[i-12]; 

	for(i = 2; i < 28; i++)	
		csum= csum^	TX_Buf[i];	//计算校验XOR

	TX_Buf[28] = csum;			//填充校验

 	USART4Write(TX_Buf,0x1D);  //发送29字节

		//等待读头应答

	OSSemPend(IC_Buf.RingBufRxSem,1000,&err);	//超时1s
	
	if(err==OS_TIMEOUT)							/*超时借车失败*/
	{
		return 0;
	}

	err = Check_Sum(IC_Buf.RingBufRx,IC_Buf.RingBufRxLen);

	if(err != 0 )	 //校验失败
	{
		return 0;
	}

	if(IC_Buf.RingBufRx[1] != 0x12)	//返回状态码必须正确
	{
		return 0;
	}

	return 1;
 
 
}

 u8 Read_Card_ID(void)
 {
   	u8 Buf[5];
	u8 err;

 	Buf[0] = 0xFF;		//起始字0
	Buf[1] = 0xFF;	    //起始字1
	Buf[2] = 0x02;		//数据长度 2字节
	Buf[3] = 0x11;	  	//命令类型
	Buf[4] = 0x13;	  	//校验

	USART2Write(Buf,0x05);  //发送5字节

	//等待读头应答

	OSSemPend(ID_Buf.RingBufRxSem,1000,&err);	//超时1s
	
	if(err==OS_TIMEOUT)							//读头无响应
	{
		return 0;
	}

	err = Check_Sum(ID_Buf.RingBufRx,ID_Buf.RingBufRxLen);

	if(err != 0 )	 //校验失败
	{
		return 0;
	}

	if(ID_Buf.RingBufRx[1] != 0x11)		//返回状态码必须正确
	{
		return 0;
	}
	/******************************
		ID号是否合法行检查 （师兄添加）
	******************************/

	return 1;
 }
