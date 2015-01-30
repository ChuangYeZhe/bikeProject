
/*
*********************************************************************************************************
*                                             ���� CODE
*							���ļ���Ҫʵ�ֻ��������Ĵ�����������ز���	 							
*                         	��Ҫʵ���ǵ�������������⵽�仯������������жϼ�������������
*********************************************************************************************************
*/

#include "SysInit.h"
#include "USART.h"
#include "CAN.h"
#include "IC.h"
#include "RTC.h"

#define Exti_HUOER0  0x00000040
#define Exti_HUOER1  0x00000080

/*�����ж�ʹ��PC6��PC7,��Ӧ�ж�6��7*/
void EXTI9_5_IRQHandler(void)
{
	u8 err;

	OSIntEnter();

 	if(EXTI_GetITStatus(EXTI_Line6)!=RESET)
    {

        HUO_ER_DISABLE;	   	//�ȹر��ⲿ�����жϣ���ֹ�Ŷ���������
		OSFlagPost(SYS_EVENT_FLAG,HUOER_FLAG,OS_FLAG_SET,&err);	 	//post���������¼�

	  	EXTI_ClearITPendingBit(EXTI_Line6);
    }
	else if(EXTI_GetITStatus(EXTI_Line7)!=RESET)
	{

		HUO_ER_DISABLE;	   	//�ȹر��ⲿ�����жϣ���ֹ�Ŷ���������
		OSFlagPost(SYS_EVENT_FLAG,HUOER_FLAG,OS_FLAG_SET,&err);	 	//post���������¼�
	  	EXTI_ClearITPendingBit(EXTI_Line7);
	
	}

	OSIntExit();
}

/*******************************����������*************************************/
/*
		��ϵͳ״̬��Ϊ���ɻ���״̬��ʱ��Task_Transaction_Process����øú���
*/

/*********************************************************************************/

void Task_Bike_Return(void)
{
	u8 err;

	OSFlagPend(SYS_EVENT_FLAG,HUOER_FLAG,OS_FLAG_WAIT_SET_ALL+OS_FLAG_CONSUME,0,&err);//�ȴ��������������¼�
		
	//Ϊ��ֹ�ź��Ŷ���ǰ���Ѿ��ر����жϣ��˴�ͨ����IO��ƽ����һ��ȷ���Ƿ�����г�����

	OSTimeDly(500);	//��ʱ500ms

	if( (HUOER0 == 0) | (HUOER1 == 0))	  //�͵�ƽ��ʾ�г����ߵ�ƽ��ʾ�޳�
	{
		//����������Ѿ���⵽�����룬���ȡ����ID�ж��Ƿ�Ϸ�
	
	}
	/*ʦ����ӻ�������*/

   	SYS_STATE = RENT_AVAILABLE;  //�����ɹ���ϵͳ״̬��Ϊ�ɽ賵״̬
	HUO_ER_DISABLE;				//�ػ����ж�

}

