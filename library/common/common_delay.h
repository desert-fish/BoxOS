#ifndef DELAY_H
#define DELAY_H
#include "stm32f10x.h"
#include "common_errordefine.h"
//初始化SysTick
void	DelayInit(void);
//毫秒计数，如果计数值超过范围会返回ERROR_OUTOFRANGE
int		Delayms(u32 _delayms);
#endif
