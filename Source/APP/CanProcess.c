
/*
*********************************************************************************************************
*                                             CANЭ�鴦�� CODE
*							���ļ���Ҫʵ��CAN���ݴ�����ز���	 							
*                       
*********************************************************************************************************
*/
#include "SysInit.h"
#include "CAN.h"

/*�軹�����״�������*/

void Task_Transaction_Process (void *p_arg)
{

	COM_Init();

	while(1)
	{
		if(SYS_STATE == RENT_AVAILABLE)
			Task_Bike_Rent();

		if(SYS_STATE == RETURN_AVAILABLE)
			Task_Bike_Return();
	
	}


}

/*CAN ���մ�������*/
void  Task_CAN_RX (void *p_arg)
{
	u8 err;
	while(1)
	{
	   	OSSemPend(CAN_RX_BUF.RingBufRxSem,0,&err);	//�ȴ�CAN�����ж��ź���
		/*�յ����ݽ��н���(��ʦ�����)*/
	
	}


}

/*CAN ���ý��մ�������*/
void  Task_CAN_CFG (void *p_arg)
{
   while(1)
   {


   }


}
