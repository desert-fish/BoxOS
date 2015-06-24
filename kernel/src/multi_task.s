	AREA    MultiTask, CODE, READONLY
;文件使用注意：
;因为很多的（如系统信息区地址）在C文件和S文件都被定义，所以修改时注意同步修改
;
;定义初始化数据
;（！）标记表示要求和C定义保持同步
;MSP_INIT		equ	0x20000f00	;MSP初始化值
PSP_INIT		equ	0x2000a000	;PSP初始化值
;
;
;函数名	：void MT_BackUpProc(u32 psp_addr);
;功能	: 将r4-r11备份到进程堆栈中
MT_BackUpProc
	EXPORT	MT_BackUpProc
	mrs	r1,psp
	stmdb	r1!,{r4-r11}
	;将进程堆栈地址保存到系统信息表中
	str	r1,[r0]
	bx	lr
;
;函数名	：void MT_RecoverProc(u32 psp_addr);
;功能	：将进程堆栈中的数据恢复到寄存器中
MT_RecoverProc
	EXPORT	MT_RecoverProc
	ldmia	r0!,{r4-r11}	;将r4-r11弹出堆栈
	msr		psp,r0			;更新psp
	bx	lr
;
;函数名	：void ChangeSP(u32 _BackUp)
;功能	：堆栈初始化和切换
MT_ChangeSP
	EXPORT	MT_ChangeSP
	ldr	r0,=PSP_INIT
	msr	psp,r0
;	ldr	r0,=MSP_INIT
;	msr	msp,r0
	mrs	r0,control
	orr	r0,r0,#0x2
	msr	control,r0
	bx	lr
;
;
;函数名	：void MT_StartSysTick(void)
;功能	:启动定时器
MT_StartSysTick
	EXPORT	MT_StartSysTick
	push	{r0,r1}
	ldr	r0,=0xE000E010
	ldr	r1,[r0]
	orr	r1,r1,#0x1
	str	r1,[r0]
	pop		{r0,r1}
	bx	lr
;
;
;函数名	：void MT_StopSysTick(void)
;功能	:停止定时器
MT_StopSysTick
	EXPORT	MT_StopSysTick
	push	{r0,r1}
	ldr	r0,=0xE000E010
	ldr	r1,[r0]
	and	r1,r1,#0xfffffffe
	str	r1,[r0]
	pop		{r0,r1}
	bx	lr
;
	END
