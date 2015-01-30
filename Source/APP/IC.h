#ifndef __IC_H
#define __IC_H

/***************************************/
/***************************************
此处是我自己初步定义的与IC卡交互的协议
初步思路是：
	2个字节的起始字：  0xAA   0xBB
	1个字节的命令字：  0xXX         //XX为命令类型字
	n字节的参数：	   0xXX 0xXX ......

***************************************/

#define IC_CMD_START0 0xFF	//IC卡命令起始字0
#define IC_CMD_START1 0x55 	//IC卡命令起始字1 

#ifdef IC

	u8 KEY_A[6]={0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5} ;
	u8 KEY_B[6]={0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5} ;
	u8 CSN[4]={0x00, 0x01, 0x02, 0x03} ;
	u8 TX_Buf[64];
	
	u8 Block_Read(u8 block);
	u8 Block_Write(u8 block,u8 *buf);

#else
	extern u8 KEY_A[6];
	extern u8 TX_Buf[64];
	extern u8 CSN[4];
	extern u8 TX_Buf[64];
	extern u8 Block_Read(u8 block);
	extern u8 Block_Write(u8 block,u8 *buf);

#endif

#endif

