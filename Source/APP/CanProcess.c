
/*
*********************************************************************************************************
*                                             CAN协议处理 CODE
*							该文件主要实现CAN数据处理相关操作	 							
*                       
*********************************************************************************************************
*/
#include "SysInit.h"
#include "CAN.h"

/*借还车交易处理任务*/

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

/*CAN 接收处理任务*/
void  Task_CAN_RX (void *p_arg)
{
	u8 err;
	while(1)
	{
	   	OSSemPend(CAN_RX_BUF.RingBufRxSem,0,&err);	//等待CAN接收中断信号量
		/*收到数据进行解析(由师兄添加)*/
	
	}


}

/*CAN 配置接收处理任务*/
void  Task_CAN_CFG (void *p_arg)
{
   while(1)
   {


   }


}
