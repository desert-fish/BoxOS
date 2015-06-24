#include "multi_task.h"

//功能：这个函数作为一个进程在系统空闲时运行防止无法计算可用进程陷入死循环
void MT_LeisureProtect(void){
	MT_ChangeStatus(SYSINFO->PID,PROC_STATUS_LEISURE);
	while(1);
}

//功能：完成系统的初始化
void MT_InitSystem(void){
	MT_SysInfoInit();												//初始化系统信息区
	MT_InitISRTable();
	MT_InitSysTick(SWITCHTIME);
	MT_AddTask((u32)MT_LeisureProtect,0,0,0);//添加空闲守护进程
}

//功能：SysTick初始化
//注意：这个函数不会启动定时器
void MT_InitSysTick(u32 usCount){
	//在AHB进行8分频（9M）后把时钟提供到SysTick，所以每9K记录下1ms
	SysTick->CTRL&=0xFFFFFFFB;
	//定时系统产生中断
	SysTick->CTRL|=(0x1<<1);
	//清除计时器值
	SysTick->VAL=0x0;
	//设置装载值
	SysTick->LOAD=9*usCount;
	return;
}

//功能	：系统信息区初始化
void MT_SysInfoInit(void){
	SYSINFO->PID=0;
	SYSINFO->PAMOUNT=0;
	SYSINFO->PMASK=0;
	SYSINFO->SYSTIME=0;
	return;
}

//功能：添加任务，并初始化任务堆栈
//返回：新加入的进程的PID号码，如果为0表示添加失败
u32 MT_AddTask(u32 TaskAddr,u32 pid,u32 R0,u32 R1){
  //检查输入的pid是不是已经被使用
  if(pid && (SYSINFO->PMASK&(0x1<<pid)))return 0;	//返回0表示添加失败
  //寻找最小的可用PID号码
  if(!pid)while(SYSINFO->PMASK&(0x1<<++pid));			//如果pid为0时自动分配pid
	//设定PID号码
	PROCINFO(pid)->PID=pid;
	//设定进程状态
	PROCINFO(pid)->Status=PROC_STATUS_NORMAL;				//设定为正常状态
	//设定进程堆栈指针，因为每次会恢复16个字的数据，所以先将PSP-64
	PROCINFO(pid)->PSP=PROCSTACK_BASE-(pid-1)*PROCSTACK_SIZE-64;
	//预先修改进程堆栈中保存的PC值
	*(u32*)(PROCINFO(pid)->PSP+4*(16-2))=TaskAddr;
	//预先修改进程堆栈中保存的xPSR值（否则发生fault错误）
	*(u32*)(PROCINFO(pid)->PSP+4*(16-1))=0x01000000;
	//保证函数在return之后会正常结束进程
	*(u32*)(PROCINFO(pid)->PSP+4*(16-3))=(u32)MT_ExitTask;
	//将第一个参数传入R0的备份区域
	*(u32*)(PROCINFO(pid)->PSP+4*(16-8))=R0;
	//将第二个参数传入R1的备份区域
	*(u32*)(PROCINFO(pid)->PSP+4*(16-7))=R1;
	//增加总的进程数目
	SYSINFO->PAMOUNT++;
	SYSINFO->PMASK|=(0x1<<pid);
	return pid;
}

//功能：结束自身进程
void MT_ExitTask(void){
 	MT_StopSysTick();
  SYSINFO->PMASK&=~(0x1<<SYSINFO->PID);
	MT_TriggerSwitchTask();	//手动悬挂SysTick中断
}

//功能：结束其他进程（需要输入进程号码）
void MT_KillTask(u32 pid){
  SYSINFO->PMASK&=~(0x1<<pid);
	return;
}

//功能：修改进程的状态
void MT_ChangeStatus(u32 pid,u32 Status){
  PROCINFO(pid)->Status=Status;
	MT_TriggerSwitchTask();	//手动悬挂SysTick中断
	return;
}
//功能：等待一定的时间（在这段时间CPU会交还给其他进程，当时间到了的时候会重新唤醒程序）
//		这个函数的目的是防止进程在等待中占用CPU
void MT_Wait(u32 usCount){
 	MT_StopSysTick();
  	PROCINFO(SYSINFO->PID)->WeekUpTime=(SYSINFO->SYSTIME+usCount);	//修改自己的唤醒时间
  	MT_ChangeStatus(SYSINFO->PID,PROC_STATUS_WAIT);									//修改状态
  return;
}

//功能：进行服务调用
void MT_CallService(u32	pid,u32 argc,u32 argv){
	//等待目标进程进入服务状态
	while(PROCINFO(pid)->Status!=PROC_STATUS_SERVICE);
	//向管道写入参数
	PIPE(pid)->argc=argc;
	PIPE(pid)->argv=argv;
	PIPE(pid)->return_pid=SYSINFO->PID;
	MT_StopSysTick();		//防止中断造成进程破坏
	PROCINFO(pid)->Status=PROC_STATUS_NORMAL;	//将目标进程修改为正常状态
	MT_ChangeStatus(SYSINFO->PID,PROC_STATUS_WAITRET);
}

//功能：进行服务返回
void MT_ReturnService(u32	argc,u32	argv){
	//向目标进程管道写入参数
	PIPE(PIPE(SYSINFO->PID)->return_pid)->argc=argc;
	PIPE(PIPE(SYSINFO->PID)->return_pid)->argv=argv;
	MT_StopSysTick();		//防止中断造成进程破坏
	PROCINFO(PIPE(SYSINFO->PID)->return_pid)->Status=PROC_STATUS_NORMAL;	//将目标进程修改为正常状态
	MT_ChangeStatus(SYSINFO->PID,PROC_STATUS_SERVICE);
	return;
}

//功能：软件触发任务切换
void	MT_TriggerSwitchTask(void){
	//SCB->ICSR|=(0x1<<26);		//手动悬挂SysTick中断
	__asm("svc	#1");
	return;
}

//功能：立刻切换到某个进程上去执行（会自动将目标进程状态修改为NORMAL状态）
void	MT_SwitchToProc(u32 pid){
	MT_StopSysTick();													//防止中断造成进程破坏
	PROCINFO(pid)->Status=PROC_STATUS_NORMAL;	//更改目标进程状态
	MT_SwitchTask(pid);												//进程切换
	return;
}

//功能：在中断状态下进行任务切换
void	MT_SwitchTask(u32	pid){
	u32	PID_backup;
	u32	LeisureEnable=0;
	u32	PID_Ready=0;
	//备份现场
	if(SYSINFO->PID!=0){											//如果进入前在执行进程，就将数据压栈,并存储堆栈指针
		MT_BackUpProc((u32)(&PROCINFO(SYSINFO->PID)->PSP));
	}
	//增加系统时间
	if(!(SysTick->CTRL&(0x1<<16))){						//判断中断是不是定时器自动产生的
		SYSINFO->SYSTIME+=SWITCHTIME*9-SysTick->VAL;
	}
	else{
		SYSINFO->SYSTIME+=SWITCHTIME;
	}
	MT_StopSysTick();													//停止定时器
	//判断PID是否可用
	if(pid!=0 && SYSINFO->PMASK&(0x1<<pid)){	//如果提供的pid为可用pid
		SYSINFO->PID=pid;
	}
	else{
	//PID计算
		PID_backup=SYSINFO->PID;
		if(SYSINFO->PID==31)SYSINFO->PID=0;				//限制PID的范围
			do{
				while(!(SYSINFO->PMASK&(0x1<<(++SYSINFO->PID)))){
					if(SYSINFO->PID==31)SYSINFO->PID=0;						//限制PID的范围
					if(SYSINFO->PID==PID_backup)LeisureEnable=1;	//如果搜索了一圈的话，就允许执行空闲等待的进程,放在这里是为了防止当前进程已经被清除造成依然无法进入空闲进程
				}
				if(SYSINFO->PID==PID_backup)LeisureEnable=1;	//如果搜索了一圈的话，就允许执行空闲等待的进程
				//处理普通状态的进程
				if(PROCINFO(SYSINFO->PID)->Status==PROC_STATUS_NORMAL){
						PID_Ready=1;
				}
				//处理等待状态的进程
				if(PROCINFO(SYSINFO->PID)->Status==PROC_STATUS_WAIT){
					if(PROCINFO(SYSINFO->PID)->WeekUpTime<=SYSINFO->SYSTIME){
						PROCINFO(SYSINFO->PID)->Status=PROC_STATUS_NORMAL;
						PID_Ready=1;
					}
				}
				//处理空闲执行状态进程
				if(PROCINFO(SYSINFO->PID)->Status==PROC_STATUS_LEISURE){
					if(LeisureEnable){
						PID_Ready=1;
					}
				}
			}while(!PID_Ready);
	}
	//启动定时器
	MT_StartSysTick();
	//恢复现场
	MT_RecoverProc(PROCINFO(SYSINFO->PID)->PSP);
	return;
}
