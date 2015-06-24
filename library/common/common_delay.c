#include "common_errordefine.h"
#include "common_delay.h"
void DelayInit(){
	SysTick->CTRL&=0xFFFFFFFB;
	//在AHB进行8分频（9M）后把时钟提供到SysTick，所以每9K记录下1ms
	SysTick->CTRL&=~(0x1<<1);	
	//不产生中断
}
int Delayms(u32 _delayms){
	if(9000*_delayms > 0x00ffffff)
		return ERROR_OUTOFRANGE;
	//超出计时器范围错误
	SysTick->VAL=0x0;
	//清除计时器值
	SysTick->LOAD=9000*_delayms;
	//设置装载值
	SysTick->CTRL|=(u32)(0x1<<0);
	//使能时钟滴答
	while(!(SysTick->CTRL&(0x1<<16)));
	//当计时器状态值不为1时，保持循环
	SysTick->CTRL&=~(0x1<<0);
	//关闭计时器
	return 0;
}
