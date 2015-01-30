
/**********************************************************/

/*
	���ļ���Ҫʵ����IC��ͨѶ��صĲ�������
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
   		csum= csum^	data[i];	//����У��XOR
	
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

	TX_Buf[0] = 0xFF;		//��ʼ��0
	TX_Buf[1] = 0xFF;	    //��ʼ��1
	TX_Buf[2] = 0x0A;		//���ݳ��� 10�ֽ�
	TX_Buf[3] = 0x12;	  	//Block_Read��������

	TX_Buf[4] = 0x00;		//keyA��֤������
	TX_Buf[5] = block;		//���

	for(i = 6;i < 12; i++)
		TX_Buf[i] = KEY_A[i-6];	 //keyAֵ

	for(i = 2; i < 12; i++)	
		csum= csum^	TX_Buf[i];	//����У��XOR

	TX_Buf[12] = csum;			//���У��

 	USART4Write(TX_Buf,0x0C);  //����13�ֽ�

	//�ȴ���ͷӦ��

	OSSemPend(IC_Buf.RingBufRxSem,1000,&err);	//��ʱ1s
	
	if(err==OS_TIMEOUT)							/*��ʱ�賵ʧ��*/
	{
		return 0;
	}

	err = Check_Sum(IC_Buf.RingBufRx,IC_Buf.RingBufRxLen);

	if(err != 0 )	 //У��ʧ��
	{
		return 0;
	}

	if(IC_Buf.RingBufRx[1] != 0x12)	//����״̬�������ȷ
	{
		return 0;
	}

	return 1;
 
}

u8 Block_Write(u8 block,u8 *buf)
{
 	u8 i,err;
	u8 csum = 0x00;

	TX_Buf[0] = 0xFF;		//��ʼ��0
	TX_Buf[1] = 0xFF;	    //��ʼ��1
	TX_Buf[2] = 0x1A;		//���ݳ��� 26�ֽ�
	TX_Buf[3] = 0x13;	  	//Block_Write��������

	TX_Buf[4] = 0x01;		//keyB��֤д����
	TX_Buf[5] = block;		//���

	for(i = 6;i < 12; i++)
		TX_Buf[i] = KEY_B[i-6];	 //keyBֵ

	for(i = 12; i < 28; i++)
		TX_Buf[i] = buf[i-12]; 

	for(i = 2; i < 28; i++)	
		csum= csum^	TX_Buf[i];	//����У��XOR

	TX_Buf[28] = csum;			//���У��

 	USART4Write(TX_Buf,0x1D);  //����29�ֽ�

		//�ȴ���ͷӦ��

	OSSemPend(IC_Buf.RingBufRxSem,1000,&err);	//��ʱ1s
	
	if(err==OS_TIMEOUT)							/*��ʱ�賵ʧ��*/
	{
		return 0;
	}

	err = Check_Sum(IC_Buf.RingBufRx,IC_Buf.RingBufRxLen);

	if(err != 0 )	 //У��ʧ��
	{
		return 0;
	}

	if(IC_Buf.RingBufRx[1] != 0x12)	//����״̬�������ȷ
	{
		return 0;
	}

	return 1;
 
 
}

 u8 Read_Card_ID(void)
 {
   	u8 Buf[5];
	u8 err;

 	Buf[0] = 0xFF;		//��ʼ��0
	Buf[1] = 0xFF;	    //��ʼ��1
	Buf[2] = 0x02;		//���ݳ��� 2�ֽ�
	Buf[3] = 0x11;	  	//��������
	Buf[4] = 0x13;	  	//У��

	USART2Write(Buf,0x05);  //����5�ֽ�

	//�ȴ���ͷӦ��

	OSSemPend(ID_Buf.RingBufRxSem,1000,&err);	//��ʱ1s
	
	if(err==OS_TIMEOUT)							//��ͷ����Ӧ
	{
		return 0;
	}

	err = Check_Sum(ID_Buf.RingBufRx,ID_Buf.RingBufRxLen);

	if(err != 0 )	 //У��ʧ��
	{
		return 0;
	}

	if(ID_Buf.RingBufRx[1] != 0x11)		//����״̬�������ȷ
	{
		return 0;
	}
	/******************************
		ID���Ƿ�Ϸ��м�� ��ʦ����ӣ�
	******************************/

	return 1;
 }
