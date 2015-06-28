#include "led_driver.h"


//功能：等待LED驱动服务进程
void	Led1_Driver(u32	argc,u32	argv){
	GPIO_InitTypeDef	GPIOxInitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIOxInitStruct.GPIO_Pin	=GPIO_Pin_0;
	GPIOxInitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIOxInitStruct.GPIO_Mode	=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIOxInitStruct);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	MT_ChangeStatus(SYSINFO->PID,PROC_STATUS_SERVICE);		//进入服务状态
	
	while(1){
		argc=PIPE(SYSINFO->PID)->argc;												//从管道获得参数
		while(argc--){
			GPIO_SetBits(GPIOC,GPIO_Pin_0);
			MT_Wait(300000);
			GPIO_ResetBits(GPIOC,GPIO_Pin_0);
			MT_Wait(300000);
		}
		GPIO_SetBits(GPIOC,GPIO_Pin_0);
		MT_ReturnService(0,0);
	}
}
//功能：等待LED驱动服务进程
void	Led2_Driver(u32	argc,u32 argv){
	GPIO_InitTypeDef	GPIOxInitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIOxInitStruct.GPIO_Pin	=GPIO_Pin_0;
	GPIOxInitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIOxInitStruct.GPIO_Mode	=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIOxInitStruct);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	MT_ChangeStatus(SYSINFO->PID,PROC_STATUS_SERVICE);		//进入服务状态
	
	while(1){
		argc=PIPE(SYSINFO->PID)->argc;												//从管道获得参数
		while(argc--){
			GPIO_SetBits(GPIOB,GPIO_Pin_0);
			MT_Wait(300000);
			GPIO_ResetBits(GPIOB,GPIO_Pin_0);
			MT_Wait(300000);
		}
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
		MT_ReturnService(0,0);
	}
}
