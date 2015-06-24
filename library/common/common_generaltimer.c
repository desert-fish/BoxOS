#include  "common_generaltimer.h"


//基本的时钟配置，如预分频值和重装载值
void GeneralTimer_BasicConfig(TIM_TypeDef* TIMx,u16 Psc,u16 Arr){
	if			(TIMx==TIM2)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//使能时钟
	else if	(TIMx==TIM3)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//使能时钟
	else if	(TIMx==TIM4)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//使能时钟
	else if	(TIMx==TIM5)RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);//使能时钟
	TIMx->CR1|=0x1<<7;//允许装载缓冲器
	TIMx->PSC=Psc;	//预分频值
	TIMx->ARR=Arr;	//自动重装载值
}

//中断配置函数
void GeneralTimer_InterruptConfig(TIM_TypeDef* TIMx,u8 Item,u16 InterruptID,u8 Priority,u8 Status){
	NVIC_Enable(InterruptID,Priority);
	TIMx->DIER&=~(0x1<<Item);//清除对应位
	TIMx->DIER|=Status<<Item;
}

//配置通道输出信息,默认配置：PWM模式1，输出高电平有效，允许预装载,启动输出
//参数：TIMx 定时器2到5
//			CHx	 通道号码
void GeneralTimer_CHxOutputConfig(TIM_TypeDef* TIMx,u8 CHx,u16 Ccr){
	if(CHx==CH1){
		TIMx->CCMR1&=~(3<<0);	//配置CC1为输出
		TIMx->CCMR1|=(6<<4);	//配置CC1为pwm1模式
		TIMx->CCMR1|=0x1<<3;	//使用CCR1预装载
		TIMx->CCER&=~(1<<1);	//OC1高电平有效
		TIMx->CCR1=Ccr;
		TIMx->CCER|=1<<0;			//开启OC1输出
	}
	else if(CHx==CH2){
		TIMx->CCMR1&=~(3<<8);	//配置CC2为输出
		TIMx->CCMR1|=(6<<12);	//配置CC2为pwm1模式
		TIMx->CCMR1|=0x1<<11;	//使用CCR2预装载
		TIMx->CCER&=~(1<<5);	//OC2高电平有效
		TIMx->CCR2=Ccr;
		TIMx->CCER|=1<<4;			//开启OC2输出
	}
	else if(CHx==CH3){
		TIMx->CCMR2&=~(3<<0);	//配置CC1为输出
		TIMx->CCMR2|=(6<<4);	//配置CC1为pwm1模式
		TIMx->CCMR2|=0x1<<3;	//使用CCR1预装载
		TIMx->CCER&=~(1<<9);	//OC1高电平有效
		TIMx->CCR3=Ccr;
		TIMx->CCER|=1<<8;			//开启OC1输出
	}
	else if(CHx==CH4){
		TIMx->CCMR2&=~(3<<8);	//配置CC2为输出
		TIMx->CCMR2|=(6<<12);	//配置CC2为pwm1模式
		TIMx->CCMR2|=0x1<<11;	//使用CCR2预装载
		TIMx->CCER&=~(1<<13);	//OC2高电平有效
		TIMx->CCR4=Ccr;
		TIMx->CCER|=1<<12;		//开启OC2输出
	}
}

//启动定时器
void GeneralTimer_StartTimer(TIM_TypeDef* TIMx){
		TIMx->CR1|=0x1;	//启动定时器
}

//停止定时器
void GeneralTimer_StopTimer(TIM_TypeDef* TIMx){
		TIMx->CR1&=~(0x1);//关闭定时器
}
//配置通道引脚,设定推挽输出

void GeneralTimer_PinConfig(GPIO_TypeDef* GPIOx,u16 GPIO_Pinx){
	GPIO_InitTypeDef	GPIOxInitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能AFIO时钟
	if			(GPIOx==GPIOA)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA时钟
	else if	(GPIOx==GPIOB)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能GPIOB时钟
	else if	(GPIOx==GPIOC)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能GPIOC时钟
	else if	(GPIOx==GPIOD)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//使能GPIOD时钟
	else if	(GPIOx==GPIOE)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);//使能GPIOE时钟
	else if	(GPIOx==GPIOF)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);//使能GPIOF时钟
	else if	(GPIOx==GPIOG)RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);//使能GPIOG时钟
	GPIOxInitStruct.GPIO_Pin	=GPIO_Pinx;
	GPIOxInitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIOxInitStruct.GPIO_Mode	=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOx,&GPIOxInitStruct);
}
