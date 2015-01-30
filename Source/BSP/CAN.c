
#define CAN_RX
#include "includes.h"
#include "CAN.h" 
  /*************应用程序代码***********************
   OSMutexPend(ResouceMutex,0,&err);
   *************占用共享资源***********************
   OSMutexPost(ResouceMutex);
   *************应用程序代码***********************/ 

/***************************************************
*	功能：CAN相关的IO的初始化
***************************************************/
static void CAN_GPIO_Config(void)
{

 	GPIO_InitTypeDef GPIO_InitStructure;
   	GPIO_PinRemapConfig(GPIO_Remap1_CAN, ENABLE );	 			  //Remap CAN到PB8和PB9

	/* Configure CAN pin: RX */									  // PB8
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Configure CAN pin: TX */									 // PB9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);

   	
	
}

/*********************************************************************
*	功能：CAN控制器初始化
*********************************************************************/
static void CAN_Mode_Config(void)
{
   	CAN_InitTypeDef        CAN_InitStructure;

	/*CAN寄存器初始化*/
	CAN_DeInit();
	CAN_StructInit(&CAN_InitStructure);
	/*CAN单元初始化*/
	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  关闭时间触发通信模式使能
    CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  自动离线管理 
    CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  使用自动唤醒模式
    CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  禁止报文自动重传	  DISABLE-自动重传
    CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文  
    CAN_InitStructure.CAN_TXFP=DISABLE;			   //MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符 
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  //正常工作模式
    CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		   //BTR-SJW 重新同步跳跃宽度 1个时间单元
    CAN_InitStructure.CAN_BS1=CAN_BS1_13tq;		   //BTR-TS1 时间段1 占用了12个时间单元
    CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		   //BTR-TS1 时间段2 占用了3个时间单元
    CAN_InitStructure.CAN_Prescaler =4;		       //BTR-BRP 波特率分频器  定义了时间单元的时间长度 32/(1+12+3)/4=0.5Mbps

	/*采样点
	75%     when 波特率 > 800K
	80%     when 波特率 > 500K
	87.5%   when 波特率 <= 500K
	（1+13）/(1+13+2）= 78.5%
	*/

	CAN_Init(&CAN_InitStructure);
}

/********************************************************
*	功能：CAN过滤器配置
********************************************************/
static void CAN_Filter_Config(u32 ID)
{
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/*CAN过滤器初始化*/
	CAN_FilterInitStructure.CAN_FilterNumber=0;						//过滤器组0
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//工作在标识符屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//过滤器位宽为单个32位。
	/* 使能报文标示符过滤器按照标示符的内容进行比对过滤，扩展ID不是如下的就抛弃掉，是的话，会存入FIFO0。 */

    CAN_FilterInitStructure.CAN_FilterIdHigh= (((u32)ID<<3)&0xFFFF0000)>>16;				//要过滤的ID高位 
    CAN_FilterInitStructure.CAN_FilterIdLow= (((u32)ID<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要过滤的ID低位 
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0xFFFF;				//过滤器高16位每位必须匹配
    CAN_FilterInitStructure.CAN_FilterMaskIdLow= 0xFFFF;				//过滤器低16位每位必须匹配
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FilterFIFO0 ;	//过滤器被关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;				//使能过滤器
	CAN_FilterInit(&CAN_FilterInitStructure);
	/*CAN通信中断使能*/
	CAN_ITConfig(CAN_IT_FMP0, ENABLE);
}


/****************************************
*	功能：CAN初始化
****************************************/
void CAN_Config(u32 ID)
{
  CAN_GPIO_Config();
  CAN_Mode_Config();
  CAN_Filter_Config(ID);   
}


/****************************************
*	功能：发送报文内容
****************************************/  
void CAN_SendMsg(u8 *buf,u8 len)
{
	CanTxMsg TxMessage;			  			 //发送结构体
	u8 n = 0;
	u8 err,i;
	
	OSMutexPend(CAN_TX_Mutex,0,&err); 		//获取互斥锁，保证一帧数据完整发完
											//不被上发的校时、报警等打乱
	
	while( n < len)
	{
	
		TxMessage.ExtId=0x1234;				    //站点管理箱使用的扩展ID 0x1234
		TxMessage.IDE=CAN_ID_EXT;				 //扩展模式
		TxMessage.RTR=CAN_RTR_DATA;				 //发送的是数据

		if( (len -n) >= 8)
		{
		  	TxMessage.DLC = 8;	 	
		}
		else
		{
			TxMessage.DLC = len -n;
		
		}

		for( i = 0; i < TxMessage.DLC; i++)
		{
			TxMessage.Data[i]=buf[n];
			n++;
		
		}
		CAN_Transmit(&TxMessage);  				//发送数据
	}
	OSMutexPost(CAN_TX_Mutex);	
}

/******************************************************
*	功能：数据接收中断
******************************************************/
void USB_LP_CAN_RX0_IRQHandler(void)
{
	u8 i;

	CanRxMsg RxMessage;				        //接收缓冲区

	OSIntEnter();

	CAN_Receive(CAN_FIFO0, &RxMessage);	 	/*从邮箱中读出报文*/

	for (i = 0; i < RxMessage.DLC; i++)
	{
	
		CAN_RX_BUF.BufRx[CAN_RX_BUF.W_Index] = RxMessage.Data[i]; //将数据存储到环形缓冲区
		//CAN_RX_BUF.W_Index++;
	    CAN_RX_BUF.W_Index >= CAN_RX_BUF.R_Index ? 0:CAN_RX_BUF.W_Index++;
		//if(CAN_RX_BUF.W_Index >= CAN_RX_BUF.R_Index)
			//CAN_RX_BUF.W_Index = 0;

	}
	OSSemPost(CAN_RX_BUF.RingBufRxSem);				//post信号量，给接收任务
	
	OSIntExit();
}



/**************************END OF FILE************************************/
