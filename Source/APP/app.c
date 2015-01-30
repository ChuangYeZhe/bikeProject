/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#define SYS_INIT

#include "includes.h"
#include "SysInit.h"
#include "USART.h"
#include "CAN.h"
#include "IC.h"

/* ----------------- APPLICATION GLOBALS ------------------ */

#define   APP_TASK_START_STK_SIZE            256	  //Main�������ջ
#define   APP_TASK_TRANSACTION_STK_SIZE		 512	 //�賵�ͻ������������ջ
#define   APP_TASK_CAN_RX_STK_SIZE		 	 512 	 //CAN���մ��������ջ
#define   APP_TASK_CAN_CFG_STK_SIZE		 	 512 	 //CAN���ô��������ջ

/*ϵͳ�����ջ����*/

static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK Task_Transaction[APP_TASK_TRANSACTION_STK_SIZE];  
static OS_STK Task_Can_Rx_Stk[APP_TASK_CAN_RX_STK_SIZE];
static OS_STK Task_Can_Cfg_Stk[APP_TASK_CAN_CFG_STK_SIZE];


/*�������ȼ�------------------------------------------------*/

#define APP_TASK_START_PRIO        	7		 /*��ʼ������*/
#define TASK_CAN_RX_PRIO			4		/*CAN�������ݴ�������*/
#define TASK_CAN_CFG_PRIO			6		/*CAN���ô�������*/
#define TASK_TRANSACTION_PRIO		5		/*�賵�ͻ�����������*/

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void AppTaskStart(void *p_arg);
extern void Task_Transaction_Process (void *p_arg);
extern void Task_CAN_RX (void *p_arg);
extern void Task_CAN_CFG (void *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{

    BSP_IntDisAll();                   /* Disable all interrupts until we are ready to accept them */

	BSP_Init();  				       /* ϵͳ�弶Ӳ����ʼ��	*/

    OSInit();                         /* Initialize "uC/OS-II, The Real-Time Kernel"              */

   /* Create the start task */

	OSTaskCreateExt(AppTaskStart,(void *)0,(OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE-1],\
	APP_TASK_START_PRIO,APP_TASK_START_PRIO,(OS_STK *)&AppTaskStartStk[0],APP_TASK_START_STK_SIZE,\
	(void *)0,OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

    OSStart();                        /* Start multitasking (i.e. give control to uC/OS-II)       */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
/*-------------------------------------����ϵͳ����--------------------------------------*/
/*
					1.Task_CAN_RX 		CAN���մ�������
					2.Task_CAN_IDLE		CAN �����ȼ�������
					3.Task_Transaction_Process	 �賵�ͻ������̴�������
					4.Task_Main_500ms			 ������
*/
/*--------------------------------------------------------------------------------------*/

void Create_Sys_Task(void)
{
	u8 	err;

	/*�����賵�ͻ������̴�������*/

	OSTaskCreateExt(Task_Transaction_Process,(void *)0,(OS_STK *)&Task_Transaction[APP_TASK_TRANSACTION_STK_SIZE-1],\
	TASK_TRANSACTION_PRIO,TASK_TRANSACTION_PRIO,(OS_STK *)&Task_Transaction[0],APP_TASK_TRANSACTION_STK_SIZE, \
	(void *)0,OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);	  
		
	
	/*����CAN�������ݴ�������*/
	OSTaskCreateExt(Task_CAN_RX,(void *)0,(OS_STK *)&Task_Can_Rx_Stk[APP_TASK_CAN_RX_STK_SIZE-1],\
	TASK_CAN_RX_PRIO,TASK_CAN_RX_PRIO,(OS_STK *)&Task_Can_Rx_Stk[0],APP_TASK_CAN_RX_STK_SIZE, \
	(void *)0,OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);	   	/*US1�������ݴ�������*/
		
	
 	/*����CAN���ô�������*/
	OSTaskCreateExt(Task_CAN_CFG,(void *)0,(OS_STK *)&Task_Can_Cfg_Stk[APP_TASK_CAN_CFG_STK_SIZE-1],\
	TASK_CAN_CFG_PRIO,TASK_CAN_CFG_PRIO,(OS_STK *)&Task_Can_Cfg_Stk[0],APP_TASK_CAN_CFG_STK_SIZE, \
	(void *)0,OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);	   	/*US1�������ݴ�������*/


	/*����ϵͳ�ж��¼���־*/
 	SYS_EVENT_FLAG=OSFlagCreate(0,&err);

	CAN_QUEUE=OSQCreate(&can_msg[0],CAN_QUEUE_SIZE);		/*����CAN ������Ϣ����*/
	CFG_QUEUE=OSQCreate(&cfg_msg[0],CFG_QUEUE_SIZE);		/*����CAN ������Ϣ����*/

	CAN_TX_Mutex = OSMutexCreate(9, &err);

	
}

/*---------------Task_Main_500ms ������-------------------------*/

static  void  AppTaskStart (void *p_arg)
{
   	u8 count;
	
  	NVIC_Configuration();    	//�ж�����

  	GPIO_Config();			 	//GPIO����

	AFIO_Config(); 				//�ж�ӳ��			

	EXTI_Config(); 				//�ⲿ�ж�ʹ��

  	USART4_Configuration();	 	//IC��������
	USART2_Configuration();	 	//ID��������

	Can_Addr = (u32)(~CAN_ID);	//��ȡCAN ID����õ�CAN ��ַ��1- 255��
  	CAN_Config(Can_Addr); 		//CANӲ�����ã����ù���

	/*------------�ϵ�����г��޳���ȷ����ʼ��״̬-----------*/

	if(Read_Card_ID()) 			//��ȡ�Ƿ��кϷ��ĵ��ӱ�ǩID
		SYS_STATE = EXIST;
	else
		SYS_STATE = NON_EXIST;

	Volume_Ctrl_Flag = 0;
	Rent_Return_Limit = 0;
	State_Toggle = 0;
	count = 0;

	Create_Sys_Task();			/*����ϵͳ����*/

	while(1) 					/*Main 500ms ����*/
	{ 
		count++;
/*------------------ָʾ�ƿ���-----------------------------------*/
		 
	  	if(count%2)				//��ɫLED��Ϊ����ָʾ��500ms��˸
		 	LED_GREEN_ON;
		else
			LED_GREEN_OFF;

		if(SYS_STATE == SYS_ERR)   //ϵͳ���󣬺�ɫLED����
			LED_RED_ON;
		else
			LED_RED_OFF;

/*-------------------���ӱ�ǩɨ��----------------------------------*/

#if 0
/*----------------------�������Ƴ���---------------------------------*/

		if(Volume_Ctrl_Flag == 1)	//���綨ʱʱ�䵽
		{
		 	 Voice_Volume();		//�����������
			 Volume_Ctrl_Flag = 0;
			 //CAN_Transmit();		//����Уʱ����24h����һ��Уʱ

		}
		else if(Volume_Ctrl_Flag == 2)	//ҹ��ʱʱ�䵽
		{
		 	 Voice_Volume();			//����������С
			 Volume_Ctrl_Flag = 0;
		}

/*--------------------�軹��ʱ������----------------------------------*/

		if(Rent_Return_Limit == 1)		//���綨ʱʱ�䵽
		{
		 	 Open_Card_In();			//��ˢ���жϣ��������û�ˢ��
			 Rent_Return_Limit = 0;
		}
		else if(Volume_Ctrl_Flag == 2)	//ҹ���ֹ�賵ʱ�䵽
		{
		 	 Close_Card_In();;			//�ر�ˢ���жϣ�������û�ˢ����ֻ�е����˻�����ʱ��ſ���
			 Rent_Return_Limit = 0;
		}
	
/*------------------CAN ͵������------------------------------------*/

		if(SYS_STATE == RENT_AVAILABLE)	 //����ǿɽ賵״̬
		{
			if(Bike_State == NON_EXIST)	 //����������
				Bike_Steal_Alarm();		//͵������
		}

/*------------------CAN ��ʱ��δ�賵����-----------------------------*/
		
		if(State_Toggle >= 10)			//10�����˽賵��δȡ�߳�����˵�������������б���,��������0
		{
		   	Bike_Spare_Alarm();			//��ʱ��δ�賵����
			State_Toggle = 0;
		}
#endif					
		OSTimeDly(500);
	
	}
}
