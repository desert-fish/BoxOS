//LED的驱动文件
#ifndef	LED_DRIVER_H
#define	LED_DRIVER_H

#include "stm32f10x.h"
#include "multi_task.h"

//功能：等待LED驱动服务进程
void	Led1_Driver(u32	argc,u32 argv);

//功能：等待LED驱动服务进程
void	Led2_Driver(u32	argc,u32 argv);

#endif
