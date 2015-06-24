//多任务函数声明头文件
//这个文件中函数的定义在multi_task.c和multi_task.s中
//文件中以下划线结尾的函数表示函数被移动到了内存中
//这类函数必须在进行了MoveToRAM()后才能正常调用
#ifndef	MULTI_TASK_H
#define MULTI_TASK_H

#include "stm32f10x.h"
#include "multi_task_interrupt.h"

//-----------------------------------------------------------------------------------
//定义任务切换被移动到RAM中的起始位置
//#define MTMOVEADDR			0x20001500
//extern	u32	MT_POSITIONSTART;		//程序块在Flash中的起始地址
//-----------------------------------------------------------------------------------
//系统数据定义（汇编文件开头的数据定义要求和这个一致）
#define SYSINFO_BASE		0x20001000	//系统信息区起始地址
#define PROCINFO_BASE		0x20001040	//进程信息表的起始地址
#define PROCSTACK_BASE	0x20010000	//设定的进程堆栈地址，这是所有进程堆栈的起点，堆栈是向下生长的满栈
#define PROCSTACK_SIZE	0x00000500	//设定为每个进程分配的堆栈大小
#define PIPE_BASE				0x20001240	//定义管道区域的起始地址
#define SWITCHTIME			1000				//定义系统的任务切换时间

//数据结构定义（数据结构涉及到汇编代码，不得删除已有成员，添加成员只能添加在尾部）
//定义系统信息结构体
#define SYSINFO_SIZE		16
typedef struct{
	u32	PID;			//当前执行的进程信息
	u32	PAMOUNT;		//总的进程数目
	u32	PMASK;			//记录了31个进程号的使用情况，位x=1表示pid=x的进程号已经被使用
	u32 SYSTIME;		//记录系统的运行时间（微秒），用来唤醒等待中的进程
}_SYSINFO;

//进程信息结构体
#define PROCINFO_SIZE 16				//定义进程信息大小
//定义进程的状态
#define PROC_STATUS_NORMAL	0x1	//进程设定为正常状态
#define PROC_STATUS_SLEEP		0x2	//进程正在休眠，等待被其他程序唤醒
#define PROC_STATUS_WAIT		0x3 //进程等待一定时间被唤醒
#define	PROC_STATUS_LEISURE	0x4	//进程只有在空闲时候才被运行
#define	PROC_STATUS_SERVICE	0x5 //进程处于等待服务状态（例如驱动），当其他程序进行调用时才唤醒
#define	PROC_STATUS_WAITRET	0x6	//进程处于等待服务返回状态（进程1用服务方式调用进程2时，进程1自动进入等待返回状态）
typedef struct{
	u32	PID;				//进程ID
	u32	Status;			//进程状态
	u32	PSP;				//堆栈地址
	u32	WeekUpTime;	//当状态为WAIT状态时，任务切换程序会检查系统时间是不是大于唤醒时间，如果大于就会唤醒程序
}_PROCINFO;

#define	PIPE_SIZE	12	//每个进程的管道区域大小
//管道定义（用于服务进程调用时候的参数传递）
typedef	struct{
	u32	argc;				//参数1，通常表示参数数量
	u32	argv;				//参数2，通常是参数区指针
	u32	return_pid;	//返回的pid地址
}_PIPE;
//---------------------------------------------------------------------------------------------
//定义一个系统信息访问的途径
#define SYSINFO	((_SYSINFO*)(SYSINFO_BASE))
	
//定义一个进程信息访问的途径
//因为pid是从1开始的，所以要进行pid-1
#define PROCINFO(pid)	((_PROCINFO*)(PROCINFO_BASE+(pid-1)*PROCINFO_SIZE))

//定义一个管道访问的途径
#define	PIPE(pid)	((_PIPE*)(PIPE_BASE+(pid-1)*PIPE_SIZE))
//---------------------------------------------------------------------------------------------

//功能：完成系统的初始化
void MT_InitSystem(void);

//函数名：void ChangeSP(u32	_BackUp)
//参数	：_BackUp，没有实际意义，只是为了防止堆栈切换造成的r0数据丢失
//功能	：初始化MSP和PSP堆栈，切换堆栈为PSP
//备注	：初始化值在函数区域中定义
void	MT_ChangeSP(u32 _BackUp);

//功能	: 将r4-r11备份到进程堆栈中
void MT_BackUpProc(u32 psp_addr);

//功能	：将进程堆栈中的数据恢复到寄存器中
void MT_RecoverProc(u32 psp_addr);

//功能：SysTick初始化
//注意：这个函数不会启动定时器
void MT_InitSysTick(u32 usCount);

//功能：SysTick启动
//注意：这个函数不会进行初始化，直接启动定时器
void MT_StartSysTick(void);

//功能：SysTick关闭
void MT_StopSysTick(void);

//系统信息区初始化
void MT_SysInfoInit(void);

//功能：添加任务，并初始化任务堆栈，pid为0时自动分配pid
u32  MT_AddTask(u32 TaskAddr,u32 pid,u32 argc,u32 argv);

//功能：结束自身进程
void MT_ExitTask(void);

//功能：结束其他进程（需要输入进程号码）
void MT_KillTask(u32 pid);

//功能：修改进程的状态
void MT_ChangeStatus(u32 pid,u32 Status);

//功能：等待一定的时间（在这段时间CPU会交还给其他进程，当时间到了的时候会重新唤醒程序）
void MT_Wait(u32 usCount);

//功能：进行服务调用
void MT_CallService(u32	pid,u32 argc,u32 argv);

//功能：进行服务返回
void MT_ReturnService(u32	argc,u32	argv);

//功能：软件触发任务切换
void	MT_TriggerSwitchTask(void);

//功能：立刻切换到某个进程上去执行（会自动将目标进程状态修改为NORMAL状态）
void	MT_SwitchToProc(u32 pid);

//功能：在中断状态下进行任务切换
//参数：当pid==0时，系统自动切换下一个进程，pid!=0时，切换为进程pid
//注意：这个函数不会主动更改目标进程状态，如果目标进程可能为非可执行状态，应该使用MT_SwitchToProc(u32 pid)切换
void MT_SwitchTask(u32 pid);

#endif
