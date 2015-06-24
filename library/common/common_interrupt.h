/***************************************************
文件描述：文件中声明了一些用于设置外部中断EXTI和嵌套中断NVIC的函数

设置外部中断EXTI的顺序：
1.设置IO口为输入模式
2.使能AFIO时钟
3.清除和重新设置 AFIO-> EXTICR[] 来指定哪个GPIO的Pin_x和第x号外部中断线对应
4.设置 EXTI->IMR 来使能中断线
5.设置 EXTI->EMR 允许中断事件
6.设置 EXTI->FTSR 和 EXTI->RTSR 来选择触发方式

设置NVIC的顺序：
1.设置 SCB->AIRCR 来整个系统的NVIC分组（也就是抢断式优先级占几位，子优先级占几位）
	注意：设置这个需要先写入钥匙
2.设置 NVIC->ISER 寄存器使能需要的中断
3.设置 NVIC->IPR[]抢断式优先级和子优先级（4位）

使用common_interrupt来配置外部中断：
	EXTI_Config(GPIOA,GPIO_Pin_0,FALL,NVIC_GROUP2,8);

***************************************************/
#ifndef COMMON_INTERRUPT_H
#define COMMON_INTERRUPT_H
#include "stm32f10x.h"
#include "common_system.h"
#define	RISE		EXTI_Trigger_Rising			//定义上升沿触发
#define FALL		EXTI_Trigger_Falling		//定义下降沿触发
#define RISEANDFALL	EXTI_Trigger_Rising_Falling	//同时设置上升和下降沿触发
//NVIC_GROUPx表示抢占优先级有x位，子优先级4-x位
#define NVIC_GROUP1	0x6
#define NVIC_GROUP2	0x5
#define NVIC_GROUP3	0x4
#define NVIC_GROUP4	0x3

#define SYSTEM_NVIC_GROUP	NVIC_GROUP2			//整个系统公用的的分组设置		
//功能：配置外部中断需要的IO复用
//参数：GPIOx，GPIO_Pin_x
void AFIO_EXTIPortInit(GPIO_TypeDef* _GPIOx,u16 _GPIO_Pin_x);
//功能：设置NVIC分组
//参数说明：_GroupID是分组号码。查看CortexM3编程手册120页。取值如下：
//	取值	抢占优先级位数	子优先级位数
//	 0x3			4			0
//	 0x4			3			1
//	 0x5			2			2
//	 0x6			1			3
void NVIC_ConfigGroup(u8 _GroupID);
//功能：使能NVIC中断
//参数：中断号_InterruptID，stm3210x_ref手册132页查看
//		优先级数（实际上需要参考优先级组设定才能判断谁的优先级高）,取值是0到15
void NVIC_Enable(u8 _InterruptID,u8 _PriorityNum);
//功能：配置外部中断
//参数说明：_Trigger的取值是RISE（上升沿），FALL（下降沿）和RISEANDFALL（上升和下降沿触发）
//			GroupID是分组号码，详见NVIC_ConfigGroup(u8 _GroupID)的参数说明
//			u8 _PriorityNum是优先级号码
void EXTI_Config(GPIO_TypeDef* _GPIOx,u16 _GPIO_Pin_x,EXTITrigger_TypeDef _Trigger,u8 _PriorityNum);


#endif
