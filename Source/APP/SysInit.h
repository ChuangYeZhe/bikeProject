#ifndef sys_init_h
#define sys_init_h

#include "includes.h"
#include "stm32f10x_gpio.h"


/*ϵͳ״̬��*/

#define RENT_AVAILABLE  	0			//�ɽ賵̬
#define RENT_ING  			1			//���ڽ賵̬
#define RETURN_AVAILABLE  	2		    //�ɻ���̬
#define SYS_ERR  			0xFF		//ϵͳ����״̬

/*���г�״̬*/

#define EXIST 		1 	//���г���
#define NON_EXIST 	0	//���г�����

/*ϵͳ�¼���־����-------------------------------------------------------------*/
#define RENT_IC_FLAG 					0x0001	/*�û��賵ˢ���¼���־*/
#define HUOER_FLAG 						0x0002	/*�û����복���������¼���־*/
#define RETURN_IC_FLAG  				0x0004	/*�û�����ˢ���¼���־*/


/***********��������*****************************************/

#define HUO_ER_ENABLE		{EXTI->IMR = EXTI->IMR | 0x00000060;}	 //�������ж�PC6/PC7
#define HUO_ER_DISABLE		{EXTI->IMR = EXTI->IMR & 0xFFFFFF9F;}	 //�رջ����ж�PC6/PC7

#define HUOER0 (GPIOC->IDR & 0x00000040)>>6	//����������0, ����PC6
#define HUOER1 (GPIOC->IDR & 0x00000080)>>7	//����������1, ����PC7

/***********��������*****************************************/

#define BIKE_ULOCK 	 {GPIOC->BSRR = 0x00000001;GPIOC->BSRR = 0x00000002;}	 //����	PC0/PC1 = 1
#define BIKE_LOCK 	 {GPIOC->BRR = 0x00000001;GPIOC->BRR = 0x00000002; }	//����	PC0/PC1 = 0

/***********LEDָʾ��*****************************************/
							
#define LED_GREEN_ON 		  GPIOE->BRR = 0x00000400	//PE10 = 0
#define LED_GREEN_OFF		  GPIOE->BSRR = 0x00000400	//PE10 = 1
 
#define LED_RED_ON	 		  GPIOE->BRR = 0x00000800	//PE11 = 0
#define LED_RED_OFF			  GPIOE->BSRR = 0x00000800	//PE11 = 1

#define LED_YELLOW_ON 		  GPIOE->BRR = 0x00001000	//PE12 = 0
#define LED_YELLOW_OFF 		  GPIOE->BSRR = 0x00001000	//PE12 = 1

/********CAN ID*************************************************/

#define CAN_ID 	((GPIOD->IDR & 0x0000FF00)>>8)	 //PD8-PD15 ��ӦCANID���뿪��


/**************************************************************/

#define CAN_QUEUE_SIZE 1   		// ��Ϣ���д�С
#define CFG_QUEUE_SIZE 1		//��Ϣ���д�С

/*����ʱ���趨�ṹ��*/

typedef struct 
{
	u8 hour;
	u8 minute;
	u8 second;
	u8 year;
	u8 month;
	u8 day;
	u8 week;
} TIMER;

typedef struct 
{
	u8 hour;
	u8 minute;
	u8 second;

}Alarm;




#ifdef SYS_INIT

  	u8 SYS_STATE = RENT_AVAILABLE;	//ϵͳ״̬��Ĭ��Ϊ�ɽ賵״̬
	u8 Bike_State = EXIST;			//���г��Ƿ����

	Alarm Volume_Daytime;		//������������ʱ��
	Alarm Volume_Nighttime;		//ҹ��������Сʱ��
	u8 Volume_Ctrl_Flag;		//�������Ʊ�־0-������ 1-���� 2-ҹ�� 

	Alarm Rent_Daytime;	 		//����賵����ʱ��
	Alarm Rent_Nighttime;		//ҹ���ֹ�賵ʱ��
	u8 Rent_Return_Limit;		//�軹��ʱ������ 0-������ 1-�ɽ�ɻ� 2-ֻ�ܻ���

	u8 State_Toggle;			//�������ϵͳ��״̬�Ƿ��л��������賵+1�����˻���-1��
								//���ñ���ֵ����10����10�����˽賵��δȡ�߳�����˵�������������б���

	u32 Can_Addr; 				//��ֹ��CAN��ַ

/*ϵͳ�¼���־�鶨��---------------------------------------------------------*/	

	OS_FLAG_GRP *SYS_EVENT_FLAG;
	
/*��Ϣ���ж���-----------------------------------------------------------------*/

	OS_EVENT 	*CAN_QUEUE;					/*CAN��Ϣ����*/
	void		*can_msg[CAN_QUEUE_SIZE];	/*CAN��Ϣ����ָ��*/

	OS_EVENT 	*CFG_QUEUE;						
	void		*cfg_msg[CFG_QUEUE_SIZE];
			

#else

  	extern u8 SYS_STATE;	//ϵͳ״̬��Ĭ��Ϊ�ɽ賵״̬
	extern u8 Bike_State ; 
	extern u8 Bike_State_Pre;

	extern Alarm Volume_Daytime;		//������������ʱ��
	extern Alarm Volume_Nighttime;		//ҹ��������Сʱ��
	extern u8 Volume_Ctrl_Flag;			//�������Ʊ�־0-������ 1-���� 2-ҹ�� 

	extern Alarm Rent_Daytime;	 		//����賵����ʱ��
	extern Alarm Rent_Nighttime;		//ҹ���ֹ�賵ʱ��
	extern u8 Rent_Return_Limit;		//�軹��ʱ������ 0-������ 1-�ɽ�ɻ� 2-ֻ�ܻ���

	extern u8 State_Toggle;				//�������ϵͳ��״̬�Ƿ��л��������賵+1�����˻���-1��
										//���ñ���ֵ����10����10�����˽賵��δȡ�߳�����˵�������������б���

	extern u32 Can_Addr; 				//��ֹ��CAN��ַ

	extern OS_FLAG_GRP 	*SYS_EVENT_FLAG;

	extern OS_EVENT 	*CAN_QUEUE;					/*CAN��Ϣ����*/
	extern void			*can_msg[CAN_QUEUE_SIZE];	/*CAN��Ϣ����ָ��*/

	extern OS_EVENT 	*CFG_QUEUE;				
	extern void			*cfg_msg[CFG_QUEUE_SIZE];

#endif
#endif

