#include "stm32f10x.h"
#include "common_delay.h"
#include "common_interrupt.h"
#include "common_usart.h"
#include "common_basictimer.h"
#include "common_generaltimer.h"
#include "multi_task.h"

#include "led_driver.h"

//-------------------------------------
//一些硬件驱动的PID 号码，这些号码固定不变
#define LED1_PID 2	//红灯
#define LED2_PID 3	//绿灯
#define MOTOR_PID 4	//马达驱动
//-------------------------------------
#define ERROR_SHOW(n)  MT_CallService(LED1_PID,n,0)
#define NORMAL_SHOW(n) MT_CallService(LED2_PID,n,0)

int proc1(void){
	GPIO_InitTypeDef	GPIOxInitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIOxInitStruct.GPIO_Pin	=GPIO_Pin_1;
	GPIOxInitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIOxInitStruct.GPIO_Mode	=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIOxInitStruct);
	GPIO_SetBits(GPIOC,GPIO_Pin_1);
	GPIOxInitStruct.GPIO_Pin	=GPIO_Pin_2;
	GPIOxInitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIOxInitStruct.GPIO_Mode	=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIOxInitStruct);
	GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIOxInitStruct.GPIO_Pin	=GPIO_Pin_5;
	GPIOxInitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIOxInitStruct.GPIO_Mode	=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIOxInitStruct);
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	
	
	GPIOxInitStruct.GPIO_Pin	=GPIO_Pin_4;
	GPIOxInitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIOxInitStruct.GPIO_Mode	=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIOxInitStruct);
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
	
//普通定时器计时
	//BasicTimer_InterruptConfig(TIM7,ENABLE,6);
	//BasicTimer_ConfigStart(TIM7,36000-1,2000);
	//BasicTimer_InterruptConfig(TIM6,ENABLE,6);
	//BasicTimer_ConfigStart(TIM6,36000-1,2000);
	//EXTI_Config(GPIOA,GPIO_Pin_0,FALL,2);
	GeneralTimer_BasicConfig(TIM2,360-1,40);
	GeneralTimer_BasicConfig(TIM3,360-1,40);
	//GeneralTimer_InterruptConfig(TIM5,UIE,50,6,ENABLE);
	GeneralTimer_CHxOutputConfig(TIM2,CH2,20);
	GeneralTimer_PinConfig(GPIOA,GPIO_Pin_1);//配置复用推挽输出
	GeneralTimer_CHxOutputConfig(TIM3,CH1,20);
	GeneralTimer_PinConfig(GPIOA,GPIO_Pin_6);//配置复用推挽输出
	//MT_Wait(2000000);
	//配置基本定时器进行计时
	//BasicTimer_InterruptConfig(TIM6,ENABLE,3);
	//BasicTimer_Config(TIM6,36-1,4800);
	GeneralTimer_StartTimer(TIM2);	//启动定时器
	GeneralTimer_StartTimer(TIM3);	//启动定时器
	
	//USART
	//USARTx_ConfigDefault(USART1);
	//USARTx_SendEnable(USART1);
	while(1){
		ERROR_SHOW(3);
		MT_Wait(1000000);
	}
	return 0;
}
void proc2(void){
	while(1){
		USARTx_SendString(USART1,"2");
		MT_Wait(1000000);
	}
}
int main(){
	MT_ChangeSP(0);
	MT_InitSystem();
	MT_AddTask((u32)Led1_Driver,LED1_PID,0,0);
	MT_AddTask((u32)Led2_Driver,LED2_PID,0,0);	
	//MT_AddTask((u32)Motor_Driver,MOTOR_PID,0,0);	
	MT_AddTask((u32)proc1,0,0,0);
	MT_AddTask((u32)proc2,0,0,0);
	MT_StartSysTick();
	while(1);
}
