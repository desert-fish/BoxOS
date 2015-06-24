//通用定时器函数库TIM2-TIM5
#ifndef COMMON_GENERALTIMER_H
#define	COMMON_GENERALTIMER_H

#include "stm32f10x.h"
#include "common_interrupt.h"

//基本的时钟配置，如启动APB2的时钟，预分频值和重装载值
void GeneralTimer_BasicConfig(TIM_TypeDef* TIMx,u16 Psc,u16 Arr);

//中断配置函数
//参数；定时器TIMx
//	Item：中断项目，值为项目在CCER中的偏移位置,可取值：
#define UIE		0
//#define CC1IE	1 暂时用不到
//InterruptID 中断号码，查手册的 中断表对应
//Priority TIMx的中断优先级取值0到15
//Status 更改后的状态
/*
用于输出PWM信号的示例：
	GeneralTimer_BasicConfig(TIM2,36000-1,500);
	GeneralTimer_InterruptConfig(TIM2,UIE,28,6,ENABLE);
	GeneralTimer_CHxOutputConfig(TIM2,CH2,300);
	GeneralTimer_PinConfig(GPIOA,GPIO_Pin_0);//配置复用推挽输出
	GeneralTimer_StartTimer(TIM2);	//启动定时器
*/
void GeneralTimer_InterruptConfig(TIM_TypeDef* TIMx,u8 Item,u16 InterruptID,u8 Priority,u8 Status);


//配置通道输出信息,默认配置：PWM模式1，输出高电平有效，允许预装载,启动输出
//参数：TIMx 定时器2到5
//			CHx	 通道号码
#define CH1	1
#define CH2	2
#define CH3 3
#define CH4 4
//Ccr 高低电平分割点
void GeneralTimer_CHxOutputConfig(TIM_TypeDef* TIMx,u8 CHx,u16 Ccr);

//启动定时器
void GeneralTimer_StartTimer(TIM_TypeDef* TIMx);

//停止定时器
void GeneralTimer_StopTimer(TIM_TypeDef* TIMx);

//配置通道引脚,设定推挽输出
void GeneralTimer_PinConfig(GPIO_TypeDef* GPIOx,u16 GPIO_Pinx);
#endif
