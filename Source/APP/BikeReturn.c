
/*
*********************************************************************************************************
*                                             还车 CODE
*							该文件主要实现还车动作的触发及处理相关操作	 							
*                         	主要实现是当霍尔传感器检测到变化，即触发相关中断及还车处理流程
*********************************************************************************************************
*/

#include "SysInit.h"
#include "USART.h"
#include "CAN.h"
#include "IC.h"
#include "RTC.h"

#define Exti_HUOER0  0x00000040
#define Exti_HUOER1  0x00000080

/*霍尔中断使用PC6和PC7,对应中断6和7*/
void EXTI9_5_IRQHandler(void)
{
	u8 err;

	OSIntEnter();

 	if(EXTI_GetITStatus(EXTI_Line6)!=RESET)
    {

        HUO_ER_DISABLE;	   	//先关闭外部霍尔中断，防止扰动持续触发
		OSFlagPost(SYS_EVENT_FLAG,HUOER_FLAG,OS_FLAG_SET,&err);	 	//post霍尔触发事件

	  	EXTI_ClearITPendingBit(EXTI_Line6);
    }
	else if(EXTI_GetITStatus(EXTI_Line7)!=RESET)
	{

		HUO_ER_DISABLE;	   	//先关闭外部霍尔中断，防止扰动持续触发
		OSFlagPost(SYS_EVENT_FLAG,HUOER_FLAG,OS_FLAG_SET,&err);	 	//post霍尔触发事件
	  	EXTI_ClearITPendingBit(EXTI_Line7);
	
	}

	OSIntExit();
}

/*******************************还车处理函数*************************************/
/*
		当系统状态机为“可还车状态”时，Task_Transaction_Process会调用该函数
*/

/*********************************************************************************/

void Task_Bike_Return(void)
{
	u8 err;

	OSFlagPend(SYS_EVENT_FLAG,HUOER_FLAG,OS_FLAG_WAIT_SET_ALL+OS_FLAG_CONSUME,0,&err);//等待霍尔触发还车事件
		
	//为防止信号扰动，前面已经关闭了中断，此处通过读IO电平来进一步确认是否真的有车推入

	OSTimeDly(500);	//延时500ms

	if( (HUOER0 == 0) | (HUOER1 == 0))	  //低电平表示有车，高电平表示无车
	{
		//若霍尔真的已经检测到车推入，则读取车的ID判断是否合法
	
	}
	/*师兄添加还车流程*/

   	SYS_STATE = RENT_AVAILABLE;  //还车成功，系统状态改为可借车状态
	HUO_ER_DISABLE;				//关霍尔中断

}

