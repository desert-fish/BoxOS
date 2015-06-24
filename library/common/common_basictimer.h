//基本定时器控制函数库
#ifndef COMMON_TIMER6_7_H
#define COMMON_TIMER6_7_H

#include "stm32f10x.h"
#include "common_interrupt.h"

//中断配置函数
//参数：TIMx可取TIM6或者TIM7
//Status可以取ENABLE或者DISABLE
//_PriorityNum取0-15的优先级，详见NVIC寄存器
void BasicTimer_InterruptConfig(TIM_TypeDef* TIMx,u8 Status,u8 _PriorityNum);

//中断清除
//参数：TIMx可取TIM6或者TIM7
void BasicTimer_InterruptClear(TIM_TypeDef* TIMx);

//配置和启动时钟
void BasicTimer_Config(TIM_TypeDef* TIMx,u32 psc,u32 arr);
#endif
