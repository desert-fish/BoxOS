#include "common_basictimer.h"


//中断配置函数
//参数：TIMx可取TIM6或者TIM7
//Status可以取ENABLE或者DISABLE
void BasicTimer_InterruptConfig(TIM_TypeDef* TIMx,u8 Status,u8 _PriorityNum){
	if(Status){
		NVIC_ConfigGroup(SYS_NVIC_GROUP);
		if(TIMx==TIM6){
			NVIC_Enable(54,_PriorityNum);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);//使能时钟
		}
		else if(TIMx==TIM7){
			NVIC_Enable(55,_PriorityNum);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);//使能时钟
		}
		TIMx->DIER|=0x1;		//允许产生中断信号
	}
	else{
		TIMx->DIER&=~(0x1);	//禁止产生中断信号
	}
}

//中断清除
//参数：TIMx可取TIM6或者TIM7
void BasicTimer_InterruptClear(TIM_TypeDef* TIMx){
	TIMx->SR&=~(0x1);//清除中断信号
}

//配置和启动时钟
void BasicTimer_Config(TIM_TypeDef* TIMx,u32 Psc,u32 Arr){
	if(TIMx==TIM6){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);//使能时钟
	}
	else if(TIMx==TIM7){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);//使能时钟
	}
	TIMx->PSC=Psc;			//分频
	TIMx->ARR=Arr;			//周期
}
