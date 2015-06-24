//实现多任务系统的中断管理

#include "multi_task_interrupt.h"

//功能：初始化中断向量表，包括移动中断向量表到内存中
//备注：具体的地址在ISRTABLE_BASE宏中定义
void MT_InitISRTable(void){
	int i=0;
	for(i=0;i<ISRTABLE_SIZE;i+=4){
		*(u32*)(ISRTABLE_BASE+i)=*(u32*)(i);	//将地址0处的向量表拷贝到RAM中去
	}
	NVIC_SetVectorTable(NVIC_VectTab_RAM,ISRTABLE_BASE-0x20000000);
	return;
}

//功能：设置中断和它的服务程序对应的起始地址
//参数：InterruptID定义:值对应于stm32技术手册132页向量表对应的<位置>号码
//备注：这个函数修改的是内存中的向量表，如果没有切换到内存中的话，设置是不会生效的
void	MT_SetISR(u32 ISRAddr,u32 InterruptID){
	*ISR(InterruptID)=ISRAddr;		//修改内存中的中断服务程序地址
	return;
}
