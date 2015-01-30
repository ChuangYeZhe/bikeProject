
#define CAN_RX
#include "includes.h"
#include "CAN.h" 
  /*************Ӧ�ó������***********************
   OSMutexPend(ResouceMutex,0,&err);
   *************ռ�ù�����Դ***********************
   OSMutexPost(ResouceMutex);
   *************Ӧ�ó������***********************/ 

/***************************************************
*	���ܣ�CAN��ص�IO�ĳ�ʼ��
***************************************************/
static void CAN_GPIO_Config(void)
{

 	GPIO_InitTypeDef GPIO_InitStructure;
   	GPIO_PinRemapConfig(GPIO_Remap1_CAN, ENABLE );	 			  //Remap CAN��PB8��PB9

	/* Configure CAN pin: RX */									  // PB8
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // ��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Configure CAN pin: TX */									 // PB9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);

   	
	
}

/*********************************************************************
*	���ܣ�CAN��������ʼ��
*********************************************************************/
static void CAN_Mode_Config(void)
{
   	CAN_InitTypeDef        CAN_InitStructure;

	/*CAN�Ĵ�����ʼ��*/
	CAN_DeInit();
	CAN_StructInit(&CAN_InitStructure);
	/*CAN��Ԫ��ʼ��*/
	CAN_InitStructure.CAN_TTCM=DISABLE;			   //MCR-TTCM  �ر�ʱ�䴥��ͨ��ģʽʹ��
    CAN_InitStructure.CAN_ABOM=ENABLE;			   //MCR-ABOM  �Զ����߹��� 
    CAN_InitStructure.CAN_AWUM=ENABLE;			   //MCR-AWUM  ʹ���Զ�����ģʽ
    CAN_InitStructure.CAN_NART=DISABLE;			   //MCR-NART  ��ֹ�����Զ��ش�	  DISABLE-�Զ��ش�
    CAN_InitStructure.CAN_RFLM=DISABLE;			   //MCR-RFLM  ����FIFO ����ģʽ  DISABLE-���ʱ�±��ĻḲ��ԭ�б���  
    CAN_InitStructure.CAN_TXFP=DISABLE;			   //MCR-TXFP  ����FIFO���ȼ� DISABLE-���ȼ�ȡ���ڱ��ı�ʾ�� 
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;  //��������ģʽ
    CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;		   //BTR-SJW ����ͬ����Ծ��� 1��ʱ�䵥Ԫ
    CAN_InitStructure.CAN_BS1=CAN_BS1_13tq;		   //BTR-TS1 ʱ���1 ռ����12��ʱ�䵥Ԫ
    CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;		   //BTR-TS1 ʱ���2 ռ����3��ʱ�䵥Ԫ
    CAN_InitStructure.CAN_Prescaler =4;		       //BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 32/(1+12+3)/4=0.5Mbps

	/*������
	75%     when ������ > 800K
	80%     when ������ > 500K
	87.5%   when ������ <= 500K
	��1+13��/(1+13+2��= 78.5%
	*/

	CAN_Init(&CAN_InitStructure);
}

/********************************************************
*	���ܣ�CAN����������
********************************************************/
static void CAN_Filter_Config(u32 ID)
{
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/*CAN��������ʼ��*/
	CAN_FilterInitStructure.CAN_FilterNumber=0;						//��������0
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//�����ڱ�ʶ������λģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//������λ��Ϊ����32λ��
	/* ʹ�ܱ��ı�ʾ�����������ձ�ʾ�������ݽ��бȶԹ��ˣ���չID�������µľ����������ǵĻ��������FIFO0�� */

    CAN_FilterInitStructure.CAN_FilterIdHigh= (((u32)ID<<3)&0xFFFF0000)>>16;				//Ҫ���˵�ID��λ 
    CAN_FilterInitStructure.CAN_FilterIdLow= (((u32)ID<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //Ҫ���˵�ID��λ 
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh= 0xFFFF;				//��������16λÿλ����ƥ��
    CAN_FilterInitStructure.CAN_FilterMaskIdLow= 0xFFFF;				//��������16λÿλ����ƥ��
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FilterFIFO0 ;	//��������������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;				//ʹ�ܹ�����
	CAN_FilterInit(&CAN_FilterInitStructure);
	/*CANͨ���ж�ʹ��*/
	CAN_ITConfig(CAN_IT_FMP0, ENABLE);
}


/****************************************
*	���ܣ�CAN��ʼ��
****************************************/
void CAN_Config(u32 ID)
{
  CAN_GPIO_Config();
  CAN_Mode_Config();
  CAN_Filter_Config(ID);   
}


/****************************************
*	���ܣ����ͱ�������
****************************************/  
void CAN_SendMsg(u8 *buf,u8 len)
{
	CanTxMsg TxMessage;			  			 //���ͽṹ��
	u8 n = 0;
	u8 err,i;
	
	OSMutexPend(CAN_TX_Mutex,0,&err); 		//��ȡ����������֤һ֡������������
											//�����Ϸ���Уʱ�������ȴ���
	
	while( n < len)
	{
	
		TxMessage.ExtId=0x1234;				    //վ�������ʹ�õ���չID 0x1234
		TxMessage.IDE=CAN_ID_EXT;				 //��չģʽ
		TxMessage.RTR=CAN_RTR_DATA;				 //���͵�������

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
		CAN_Transmit(&TxMessage);  				//��������
	}
	OSMutexPost(CAN_TX_Mutex);	
}

/******************************************************
*	���ܣ����ݽ����ж�
******************************************************/
void USB_LP_CAN_RX0_IRQHandler(void)
{
	u8 i;

	CanRxMsg RxMessage;				        //���ջ�����

	OSIntEnter();

	CAN_Receive(CAN_FIFO0, &RxMessage);	 	/*�������ж�������*/

	for (i = 0; i < RxMessage.DLC; i++)
	{
	
		CAN_RX_BUF.BufRx[CAN_RX_BUF.W_Index] = RxMessage.Data[i]; //�����ݴ洢�����λ�����
		//CAN_RX_BUF.W_Index++;
	    CAN_RX_BUF.W_Index >= CAN_RX_BUF.R_Index ? 0:CAN_RX_BUF.W_Index++;
		//if(CAN_RX_BUF.W_Index >= CAN_RX_BUF.R_Index)
			//CAN_RX_BUF.W_Index = 0;

	}
	OSSemPost(CAN_RX_BUF.RingBufRxSem);				//post�ź���������������
	
	OSIntExit();
}



/**************************END OF FILE************************************/
