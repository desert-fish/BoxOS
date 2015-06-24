//这个文件用来集中放置中断处理函数
#include "stm32f10x.h"
#include "multi_task.h"
#include "common_basictimer.h"
#include "common_generaltimer.h"

//处理0号中断
void EXTI0_IRQHandler(void){
	int i=1800000;
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	while(i--);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);
	EXTI->PR&=0x1;	//清除中断挂起
}
//处理TIM5中断
void TIM5_IRQHandler(void){
	int i=600000;
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	while(i--);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	TIM5->SR&=~(0x1);
}
//处理TIM6中断
void TIM6_IRQHandler(void){
	static u8 i=0;
	if(i==1)
	TIM2->CR1&=0xfffffffe;//关闭定时器
	i++;
	//GeneralTimer_StopTimer(TIM3);	//启动定时器
	//GeneralTimer_StopTimer(TIM4);	//启动定时器
	BasicTimer_InterruptClear(TIM6);
}
//处理TIM7中断
void TIM7_IRQHandler(void){
	int i=600000;
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	while(i--);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	BasicTimer_InterruptClear(TIM7);
}
