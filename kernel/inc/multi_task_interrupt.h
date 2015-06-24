//多任务系统中断管理头文件

#ifndef	MULTI_TASK_INTERRUPT_H
#define	MULTI_TASK_INTERRUPT_H
#include "stm32f10x.h"
#include "multi_task.h"
#include "multi_task_interruptIDdef.h"

//定义中断向量表在内存中的起始地址
//需要注意的是向量表起始地址不是任意定的，需要参见权威指南113页的计算方法
#define	ISRTABLE_BASE	0x20000000
#define	ISRTABLE_SIZE	0x130


//定义内存中的中断向量表的宏
#define	ISR(InterruptID)	(u32*)(ISRTABLE_BASE+InterruptID*4+0x40)
//-----------------------------------------------------------------------------------------
//功能：初始化中断向量表，包括移动中断向量表到内存中
//备注：具体的地址在ISRTABLE_BASE宏中定义
void MT_InitISRTable(void);

//功能：设置中断和它的服务程序对应的起始地址
//参数：InterruptID定义:值对应于stm32技术手册132页向量表对应的<位置>号码
//备注：这个函数修改的是内存中的向量表，如果没有切换到内存中的话，设置是不会生效的
void	MT_SetISR(u32 ISRAddr,u32 InterruptID);



#endif
