#ifndef __IC_H
#define __IC_H

/***************************************/
/***************************************
�˴������Լ������������IC��������Э��
����˼·�ǣ�
	2���ֽڵ���ʼ�֣�  0xAA   0xBB
	1���ֽڵ������֣�  0xXX         //XXΪ����������
	n�ֽڵĲ�����	   0xXX 0xXX ......

***************************************/

#define IC_CMD_START0 0xFF	//IC��������ʼ��0
#define IC_CMD_START1 0x55 	//IC��������ʼ��1 

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

