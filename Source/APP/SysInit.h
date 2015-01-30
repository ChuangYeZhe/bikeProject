#ifndef sys_init_h
#define sys_init_h

#include "includes.h"
#include "stm32f10x_gpio.h"


/*系统状态机*/

#define RENT_AVAILABLE  	0			//可借车态
#define RENT_ING  			1			//正在借车态
#define RETURN_AVAILABLE  	2		    //可还车态
#define SYS_ERR  			0xFF		//系统错误状态

/*自行车状态*/

#define EXIST 		1 	//自行车在
#define NON_EXIST 	0	//自行车不在

/*系统事件标志定义-------------------------------------------------------------*/
#define RENT_IC_FLAG 					0x0001	/*用户借车刷卡事件标志*/
#define HUOER_FLAG 						0x0002	/*用户推入车霍尔触发事件标志*/
#define RETURN_IC_FLAG  				0x0004	/*用户还车刷卡事件标志*/


/***********霍尔操作*****************************************/

#define HUO_ER_ENABLE		{EXTI->IMR = EXTI->IMR | 0x00000060;}	 //开霍尔中断PC6/PC7
#define HUO_ER_DISABLE		{EXTI->IMR = EXTI->IMR & 0xFFFFFF9F;}	 //关闭霍尔中断PC6/PC7

#define HUOER0 (GPIOC->IDR & 0x00000040)>>6	//霍尔传感器0, 假设PC6
#define HUOER1 (GPIOC->IDR & 0x00000080)>>7	//霍尔传感器1, 假设PC7

/***********车锁操作*****************************************/

#define BIKE_ULOCK 	 {GPIOC->BSRR = 0x00000001;GPIOC->BSRR = 0x00000002;}	 //开锁	PC0/PC1 = 1
#define BIKE_LOCK 	 {GPIOC->BRR = 0x00000001;GPIOC->BRR = 0x00000002; }	//锁车	PC0/PC1 = 0

/***********LED指示灯*****************************************/
							
#define LED_GREEN_ON 		  GPIOE->BRR = 0x00000400	//PE10 = 0
#define LED_GREEN_OFF		  GPIOE->BSRR = 0x00000400	//PE10 = 1
 
#define LED_RED_ON	 		  GPIOE->BRR = 0x00000800	//PE11 = 0
#define LED_RED_OFF			  GPIOE->BSRR = 0x00000800	//PE11 = 1

#define LED_YELLOW_ON 		  GPIOE->BRR = 0x00001000	//PE12 = 0
#define LED_YELLOW_OFF 		  GPIOE->BSRR = 0x00001000	//PE12 = 1

/********CAN ID*************************************************/

#define CAN_ID 	((GPIOD->IDR & 0x0000FF00)>>8)	 //PD8-PD15 对应CANID拨码开关


/**************************************************************/

#define CAN_QUEUE_SIZE 1   		// 消息队列大小
#define CFG_QUEUE_SIZE 1		//消息队列大小

/*闹钟时间设定结构体*/

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

  	u8 SYS_STATE = RENT_AVAILABLE;	//系统状态，默认为可借车状态
	u8 Bike_State = EXIST;			//自行车是否存在

	Alarm Volume_Daytime;		//白昼音量调大时刻
	Alarm Volume_Nighttime;		//夜晚音量调小时刻
	u8 Volume_Ctrl_Flag;		//音量控制标志0-不动作 1-白昼 2-夜晚 

	Alarm Rent_Daytime;	 		//白昼借车允许时刻
	Alarm Rent_Nighttime;		//夜晚禁止借车时刻
	u8 Rent_Return_Limit;		//借还车时段限制 0-不动作 1-可借可还 2-只能还车

	u8 State_Toggle;			//用来标记系统的状态是否切换过，若借车+1，有人还车-1，
								//若该变量值超过10，即10次有人借车但未取走车，则说明车锁坏，进行报警

	u32 Can_Addr; 				//锁止器CAN地址

/*系统事件标志组定义---------------------------------------------------------*/	

	OS_FLAG_GRP *SYS_EVENT_FLAG;
	
/*消息队列定义-----------------------------------------------------------------*/

	OS_EVENT 	*CAN_QUEUE;					/*CAN消息队列*/
	void		*can_msg[CAN_QUEUE_SIZE];	/*CAN消息队列指针*/

	OS_EVENT 	*CFG_QUEUE;						
	void		*cfg_msg[CFG_QUEUE_SIZE];
			

#else

  	extern u8 SYS_STATE;	//系统状态，默认为可借车状态
	extern u8 Bike_State ; 
	extern u8 Bike_State_Pre;

	extern Alarm Volume_Daytime;		//白昼音量调大时刻
	extern Alarm Volume_Nighttime;		//夜晚音量调小时刻
	extern u8 Volume_Ctrl_Flag;			//音量控制标志0-不动作 1-白昼 2-夜晚 

	extern Alarm Rent_Daytime;	 		//白昼借车允许时刻
	extern Alarm Rent_Nighttime;		//夜晚禁止借车时刻
	extern u8 Rent_Return_Limit;		//借还车时段限制 0-不动作 1-可借可还 2-只能还车

	extern u8 State_Toggle;				//用来标记系统的状态是否切换过，若借车+1，有人还车-1，
										//若该变量值超过10，即10次有人借车但未取走车，则说明车锁坏，进行报警

	extern u32 Can_Addr; 				//锁止器CAN地址

	extern OS_FLAG_GRP 	*SYS_EVENT_FLAG;

	extern OS_EVENT 	*CAN_QUEUE;					/*CAN消息队列*/
	extern void			*can_msg[CAN_QUEUE_SIZE];	/*CAN消息队列指针*/

	extern OS_EVENT 	*CFG_QUEUE;				
	extern void			*cfg_msg[CFG_QUEUE_SIZE];

#endif
#endif

