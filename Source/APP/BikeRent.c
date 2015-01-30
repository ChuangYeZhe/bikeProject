/*
*********************************************************************************************************
*                                             �賵 CODE
*							���ļ���Ҫʵ�ֽ賵������ز���	 							
*                         	��ϵͳ״̬��Ϊ���ɽ賵״̬��ʱ��Task_Transaction_Process����øú���
*********************************************************************************************************
*/
#include "SysInit.h"
#include "USART.h"
#include "CAN.h"
#include "IC.h"
#include "RTC.h"

/*�û�ˢIC���ж�*/
void EXTI2_IRQHandler(void)
{
	u8 err;

	OSIntEnter();

	if(EXTI_GetITStatus(EXTI_Line2)!=RESET)
    {
		if(SYS_STATE == RENT_AVAILABLE)	 //�ɽ賵״̬��ˢ���ж�
			OSFlagPost(SYS_EVENT_FLAG,RENT_IC_FLAG,OS_FLAG_SET,&err);	 	//post�賵ˢ���¼�

		if(SYS_STATE == RETURN_AVAILABLE) //�ɻ���״̬��ˢ���ж�
			OSFlagPost(SYS_EVENT_FLAG,RETURN_IC_FLAG,OS_FLAG_SET,&err);	 	//post����ˢ����־

	  	EXTI_ClearITPendingBit(EXTI_Line2);
    }

	OSIntExit();

}

/*************�賵������**********************/

void Task_Bike_Rent(void)
{
	u8 	i,err,isValid,tranState;
	u8 buf[16] = {0};
	OSFlagPend(SYS_EVENT_FLAG,RENT_IC_FLAG,OS_FLAG_WAIT_SET_ALL+OS_FLAG_CONSUME,0,&err);//pend�賵�¼�,���¼����ⲿˢ���жϴ���
	
	if(Block_Read(0) == 0)			//���Ͷ�Block0ָ��,��ȡ4�ֽڵ�Ψһ����CSN
		goto RentFail;

	for(i = 0; i < 4; i++)	   		//�ӽ��ջ�������ȡ4�ֽڵ�Ψһ����CSN
		CSN[i] = IC_Buf.RingBufRx[2+i];

	if(Block_Read(4) == 0)			//���Ͷ�Block4ָ��,��ȡ���д��롢������ˮ������֤��
		goto RentFail;

	/*������ʦ����Ӵ���*/

	if(Block_Read(8) == 0)
		goto RentFail;
	isValid = IC_Buf.RingBufRx[3+0]; //������
	if(isValid == 1)
	{
	// �ÿ�Ϊ�ڿ�
		goto RentFail;
	}
	tranState = IC_Buf.RingBufRx[3+3];
	if(tranState == TRAN_STATE_RETRUN)
	{
	//����״̬��˵�������ٴν賵
	   memcpy(buf,&IC_Buf.RingBufRx[3],4);	 //��ԭ����Ϣд��
	   //���վ���
	   buf[4+0] = 0;
	   buf[4+1] = 0;
	   buf[4+2] = 0;
	   //����ж˱��
	   buf[4+3] = 0;
	   buf[4+4] = 0;
	   buf[4+5] = 0;
	   //����ʱ��
	   get_clock((TIMER*)&buf[9]);
	   //��������Ϣд�뿨��
	   if(Block_Write(8,buf) == 0)
		goto RentFail;	//д��ʧ��

	}
	//����CAN�����ѯ���Ƿ�Ϸ�
	//CAN_Transmit();
	//hart_module=(CanRxBUF *)OSQPend(CAN_QUEUE,1000,&err);
	


	

	
	SYS_STATE = RETURN_AVAILABLE;  //�賵�ɹ���ϵͳ״̬��Ϊ�ɻ���״̬
	HUO_ER_ENABLE;					//�������ж�

RentFail:

	SYS_STATE = RENT_AVAILABLE;	  //�賵ʧ�ܣ�ϵͳ״̬��Ϊ�ɽ賵״̬
}
