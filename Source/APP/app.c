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

#define   APP_TASK_START_STK_SIZE            256	  //Main主任务堆栈
#define   APP_TASK_TRANSACTION_STK_SIZE		 512	 //借车和还车交易任务堆栈
#define   APP_TASK_CAN_RX_STK_SIZE		 	 512 	 //CAN接收处理任务堆栈
#define   APP_TASK_CAN_CFG_STK_SIZE		 	 512 	 //CAN配置处理任务堆栈

/*系统任务堆栈分配*/

static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_STK Task_Transaction[APP_TASK_TRANSACTION_STK_SIZE];  
static OS_STK Task_Can_Rx_Stk[APP_TASK_CAN_RX_STK_SIZE];
static OS_STK Task_Can_Cfg_Stk[APP_TASK_CAN_CFG_STK_SIZE];


/*任务优先级------------------------------------------------*/

#define APP_TASK_START_PRIO        	7		 /*起始主任务*/
#define TASK_CAN_RX_PRIO			4		/*CAN接收数据处理任务*/
#define TASK_CAN_CFG_PRIO			6		/*CAN配置处理任务*/
#define TASK_TRANSACTION_PRIO		5		/*借车和还车处理任务*/

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

	BSP_Init();  				       /* 系统板级硬件初始化	*/

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
/*-------------------------------------建立系统任务--------------------------------------*/
/*
					1.Task_CAN_RX 		CAN接收处理任务
					2.Task_CAN_IDLE		CAN 低优先级任务处理
					3.Task_Transaction_Process	 借车和还车流程处理任务
					4.Task_Main_500ms			 主任务
*/
/*--------------------------------------------------------------------------------------*/

void Create_Sys_Task(void)
{
	u8 	err;

	/*建立借车和还车流程处理任务*/

	OSTaskCreateExt(Task_Transaction_Process,(void *)0,(OS_STK *)&Task_Transaction[APP_TASK_TRANSACTION_STK_SIZE-1],\
	TASK_TRANSACTION_PRIO,TASK_TRANSACTION_PRIO,(OS_STK *)&Task_Transaction[0],APP_TASK_TRANSACTION_STK_SIZE, \
	(void *)0,OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);	  
		
	
	/*建立CAN接收数据处理任务*/
	OSTaskCreateExt(Task_CAN_RX,(void *)0,(OS_STK *)&Task_Can_Rx_Stk[APP_TASK_CAN_RX_STK_SIZE-1],\
	TASK_CAN_RX_PRIO,TASK_CAN_RX_PRIO,(OS_STK *)&Task_Can_Rx_Stk[0],APP_TASK_CAN_RX_STK_SIZE, \
	(void *)0,OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);	   	/*US1接收数据处理任务*/
		
	
 	/*建立CAN配置处理任务*/
	OSTaskCreateExt(Task_CAN_CFG,(void *)0,(OS_STK *)&Task_Can_Cfg_Stk[APP_TASK_CAN_CFG_STK_SIZE-1],\
	TASK_CAN_CFG_PRIO,TASK_CAN_CFG_PRIO,(OS_STK *)&Task_Can_Cfg_Stk[0],APP_TASK_CAN_CFG_STK_SIZE, \
	(void *)0,OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);	   	/*US1接收数据处理任务*/


	/*创建系统中断事件标志*/
 	SYS_EVENT_FLAG=OSFlagCreate(0,&err);

	CAN_QUEUE=OSQCreate(&can_msg[0],CAN_QUEUE_SIZE);		/*建立CAN 接收消息队列*/
	CFG_QUEUE=OSQCreate(&cfg_msg[0],CFG_QUEUE_SIZE);		/*建立CAN 配置消息队列*/

	CAN_TX_Mutex = OSMutexCreate(9, &err);

	
}

/*---------------Task_Main_500ms 主任务-------------------------*/

static  void  AppTaskStart (void *p_arg)
{
   	u8 count;
	
  	NVIC_Configuration();    	//中断配置

  	GPIO_Config();			 	//GPIO配置

	AFIO_Config(); 				//中断映射			

	EXTI_Config(); 				//外部中断使能

  	USART4_Configuration();	 	//IC串口配置
	USART2_Configuration();	 	//ID串口配置

	Can_Addr = (u32)(~CAN_ID);	//读取CAN ID拨码得到CAN 地址（1- 255）
  	CAN_Config(Can_Addr); 		//CAN硬件配置，配置过滤

	/*------------上电根据有车无车来确定初始化状态-----------*/

	if(Read_Card_ID()) 			//读取是否有合法的电子标签ID
		SYS_STATE = EXIST;
	else
		SYS_STATE = NON_EXIST;

	Volume_Ctrl_Flag = 0;
	Rent_Return_Limit = 0;
	State_Toggle = 0;
	count = 0;

	Create_Sys_Task();			/*建立系统任务*/

	while(1) 					/*Main 500ms 任务*/
	{ 
		count++;
/*------------------指示灯控制-----------------------------------*/
		 
	  	if(count%2)				//绿色LED作为运行指示，500ms闪烁
		 	LED_GREEN_ON;
		else
			LED_GREEN_OFF;

		if(SYS_STATE == SYS_ERR)   //系统错误，红色LED灯亮
			LED_RED_ON;
		else
			LED_RED_OFF;

/*-------------------电子标签扫描----------------------------------*/

#if 0
/*----------------------音量控制程序---------------------------------*/

		if(Volume_Ctrl_Flag == 1)	//白昼定时时间到
		{
		 	 Voice_Volume();		//调节音量变大
			 Volume_Ctrl_Flag = 0;
			 //CAN_Transmit();		//发送校时请求，24h发送一次校时

		}
		else if(Volume_Ctrl_Flag == 2)	//夜晚定时时间到
		{
		 	 Voice_Volume();			//调节音量变小
			 Volume_Ctrl_Flag = 0;
		}

/*--------------------借还车时段限制----------------------------------*/

		if(Rent_Return_Limit == 1)		//白昼定时时间到
		{
		 	 Open_Card_In();			//打开刷卡中断，允许检测用户刷卡
			 Rent_Return_Limit = 0;
		}
		else if(Volume_Ctrl_Flag == 2)	//夜晚禁止借车时间到
		{
		 	 Close_Card_In();;			//关闭刷卡中断，不检测用户刷卡，只有当有人还车的时候才开启
			 Rent_Return_Limit = 0;
		}
	
/*------------------CAN 偷车报警------------------------------------*/

		if(SYS_STATE == RENT_AVAILABLE)	 //如果是可借车状态
		{
			if(Bike_State == NON_EXIST)	 //车不存在了
				Bike_Steal_Alarm();		//偷车报警
		}

/*------------------CAN 长时间未借车报警-----------------------------*/
		
		if(State_Toggle >= 10)			//10次有人借车但未取走车，则说明车锁坏，进行报警,报警后清0
		{
		   	Bike_Spare_Alarm();			//长时间未借车报警
			State_Toggle = 0;
		}
#endif					
		OSTimeDly(500);
	
	}
}
