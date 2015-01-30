/*
*********************************************************************************************************
*                                             借车 CODE
*							该文件主要实现借车处理相关操作	 							
*                         	当系统状态机为“可借车状态”时，Task_Transaction_Process会调用该函数
*********************************************************************************************************
*/
#include "SysInit.h"
#include "USART.h"
#include "CAN.h"
#include "IC.h"
#include "RTC.h"

/*用户刷IC卡中断*/
void EXTI2_IRQHandler(void)
{
	u8 err;

	OSIntEnter();

	if(EXTI_GetITStatus(EXTI_Line2)!=RESET)
    {
		if(SYS_STATE == RENT_AVAILABLE)	 //可借车状态，刷卡中断
			OSFlagPost(SYS_EVENT_FLAG,RENT_IC_FLAG,OS_FLAG_SET,&err);	 	//post借车刷卡事件

		if(SYS_STATE == RETURN_AVAILABLE) //可还车状态，刷卡中断
			OSFlagPost(SYS_EVENT_FLAG,RETURN_IC_FLAG,OS_FLAG_SET,&err);	 	//post还车刷卡标志

	  	EXTI_ClearITPendingBit(EXTI_Line2);
    }

	OSIntExit();

}

/*************借车处理函数**********************/

void Task_Bike_Rent(void)
{
	u8 	i,err,isValid,tranState;
	u8 buf[16] = {0};
	OSFlagPend(SYS_EVENT_FLAG,RENT_IC_FLAG,OS_FLAG_WAIT_SET_ALL+OS_FLAG_CONSUME,0,&err);//pend借车事件,该事件由外部刷卡中断触发
	
	if(Block_Read(0) == 0)			//发送读Block0指令,获取4字节的唯一代码CSN
		goto RentFail;

	for(i = 0; i < 4; i++)	   		//从接收缓冲区获取4字节的唯一代码CSN
		CSN[i] = IC_Buf.RingBufRx[2+i];

	if(Block_Read(4) == 0)			//发送读Block4指令,获取城市代码、发行流水、卡认证码
		goto RentFail;

	/*以下由师兄添加代码*/

	if(Block_Read(8) == 0)
		goto RentFail;
	isValid = IC_Buf.RingBufRx[3+0]; //黑名单
	if(isValid == 1)
	{
	// 该卡为黑卡
		goto RentFail;
	}
	tranState = IC_Buf.RingBufRx[3+3];
	if(tranState == TRAN_STATE_RETRUN)
	{
	//换车状态，说明可以再次借车
	   memcpy(buf,&IC_Buf.RingBufRx[3],4);	 //将原有信息写入
	   //入口站点号
	   buf[4+0] = 0;
	   buf[4+1] = 0;
	   buf[4+2] = 0;
	   //入口中端编号
	   buf[4+3] = 0;
	   buf[4+4] = 0;
	   buf[4+5] = 0;
	   //交易时间
	   get_clock((TIMER*)&buf[9]);
	   //将交易信息写入卡内
	   if(Block_Write(8,buf) == 0)
		goto RentFail;	//写卡失败

	}
	//发送CAN命令，查询卡是否合法
	//CAN_Transmit();
	//hart_module=(CanRxBUF *)OSQPend(CAN_QUEUE,1000,&err);
	


	

	
	SYS_STATE = RETURN_AVAILABLE;  //借车成功，系统状态改为可还车状态
	HUO_ER_ENABLE;					//开霍尔中断

RentFail:

	SYS_STATE = RENT_AVAILABLE;	  //借车失败，系统状态改为可借车状态
}
