/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2007; Micrium, Inc.; Weston, FL
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
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_GLOBALS
#include "includes.h"
#include "USART.h"
#include "CAN.h"



/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
static volatile ErrorStatus HSEStartUpStatus = SUCCESS;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void SysTick_Config(void);
void NVIC_Configuration(void);
void GPIO_Config(void);
void SPI_Config(void);
void InterruptConfig(void);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
******************************************************************************************************************************
******************************************************************************************************************************
**                                         Global Functions
******************************************************************************************************************************
******************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BSP INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Arguments   : none
*********************************************************************************************************
*/

void  BSP_Init(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
    {
     /* Enable Prefetch Buffer */
     FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

     /* Flash 2 wait state */
     FLASH_SetLatency(FLASH_Latency_2);
    
     /* HCLK = SYSCLK */
     RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
     /* PCLK2 = HCLK */
     RCC_PCLK2Config(RCC_HCLK_Div1); 
 
     /* PCLK1 = HCLK/2 */
     RCC_PCLK1Config(RCC_HCLK_Div2); //CAN 36MHZ
 
     /* PLLCLK = 8MHz * 9 = 72 MHz */
     RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_8);  //64M
 
     /* Enable PLL */ 
     RCC_PLLCmd(ENABLE);
 
     /* Wait till PLL is ready */
     while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
     {

     }
 
     /* Select PLL as system clock source */
     RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
 
     /* Wait till PLL is used as system clock source */
     while(RCC_GetSYSCLKSource() != 0x08)
     {

     }

    }

    /* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */

	//ʹ������ʱ�� 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA 
                           |RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
                           |RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE
						   |RCC_APB2Periph_AFIO  | RCC_APB2Periph_SPI1, ENABLE );

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2   |RCC_APB1Periph_USART2 
                           |RCC_APB1Periph_UART4 |RCC_APB1Periph_I2C1
						   |RCC_APB1Periph_USART3|RCC_APB1Periph_CAN, ENABLE );			//CANʹ��

    SysTick_Config();                                             /* Initialize the uC/OS-II tick interrupt                   */
}


/*
*********************************************************************************************************
*                                     DISABLE ALL INTERRUPTS
*
* Description : This function disables all interrupts from the interrupt controller.
*
* Arguments   : None.
*
* Returns     : None.
*********************************************************************************************************
*/
void  BSP_IntDisAll (void)
{
  CPU_IntDis();
}

/*
******************************************************************************************************************************
******************************************************************************************************************************
**                                         uC/OS-II Timer Functions
******************************************************************************************************************************
******************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       TICKER INITIALIZATION
*
* Description : This function is called to initialize uC/OS-II's tick source (typically a timer generating
*               interrupts every 1 to 100 mS).
*
* Arguments   : none
*
* Note(s)     : 1) The timer is setup for output compare mode BUT 'MUST' also 'freerun' so that the timer
*                  count goes from 0x00000000 to 0xFFFFFFFF to ALSO be able to read the free running count.
*                  The reason this is needed is because we use the free-running count in uC/OS-View.
*********************************************************************************************************
*/

void  SysTick_Config(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;


    RCC_GetClocksFreq(&rcc_clocks);

    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;

    SysTick_SetReload(cnts);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_CounterCmd(SysTick_Counter_Enable);
    SysTick_ITConfig(ENABLE);
}



void GPIO_Config(void)
{
  
	 //4--��������01-00��3---ͨ���������00-11  0--ģ������ɼ���00-00 b--����������� 10-11
	
	GPIOA->CRL = 0x44444b44;	//Usart2(PA2 PA3)
	GPIOA->CRH = 0x444444b4;   	//Usart1(PA9 PA10)

	GPIOB->CRL = 0x44444444;	//IC��ID�жϣ�PB2 PB1��
	GPIOB->CRH = 0x44444444;   	//

	GPIOC->CRL = 0x44444433;	//PC0/PC1�������PC6/PC7 ���������ж�
	GPIOC->CRH = 0x44444b44;   	//Uart4(Pc10 Pc11)

	GPIOD->CRL = 0x44444444;	
	GPIOD->CRH = 0x44444444;   	//PD8-PD15ΪCAN ID���벦��

		
	GPIOE->CRL = 0x44444444;	//
	GPIOE->CRH = 0x44433344;   //PE10��PE11,PE12 LEDָʾ��

}

void AFIO_Config(void)
{

	 /* Selects PB.02 as EXTI Line 2 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource2);

	/* Selects PC.06 as EXTI Line 6 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource6);

	/* Selects PC.07 as EXTI Line 7 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource7);


}


void EXTI_Config(void)
{

	 /* ʹ���ⲿ�ж�2 ���½����ж�*/
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line2 |EXTI_Line6 | EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//ϵͳ�ж�����
void NVIC_Configuration(void)
{ 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//���жϵ�ַ���õ�FLASH��ַ0
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	
	//�����ж�  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	//NVIC_SystemHandlerPriorityConfig(SystemHandler_SysTick,0,0);
	//NVIC_SystemHandlerPriorityConfig(SystemHandler_PSV,3,3);

	//ʹ����IC��ͨѶ�Ĵ���4�ж�
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//ʹ����ID���ӱ�ǩͨѶ�Ĵ���2�ж�
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//ʹ��CAN�����ж�
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN_RX0_IRQChannel;	   //CAN RX�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	   //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			   //�����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//ʹ���ⲿIC���ж� PB2 	EXTI2
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQChannel;	 //ʹ���ⲿ�ж�2�жϣ���ΪIC��ˢ���ж�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//ʹ���ⲿ�����ж� EXTI6�� EXTI7
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;	 //ʹ���ⲿ�ж�9_5�жϣ���Ϊ����ʱ��������������
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void IWDG_Configuration(void)
{
	/* д��0x5555,�����������Ĵ���д�빦�� */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/* ����ʱ�ӷ�Ƶ,40K/256=156HZ(6.4ms)*/
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	/* ι��ʱ�� 5s/6.4MS=781 .ע�ⲻ�ܴ���0xfff*/
	IWDG_SetReload(781);
	
	/* ι��*/
	IWDG_ReloadCounter();
	
	/* ʹ�ܹ���*/
	IWDG_Enable();
}

void RST_WDG(void)
{
	IWDG_ReloadCounter(); //�ֶ�ι��
}
