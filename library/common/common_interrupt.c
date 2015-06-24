/***************************************************
文件描述：文件中声明了一些用于设置外部中断EXTI和嵌套中断NVIC的函数

设置外部中断EXTI的顺序：
1.设置IO口为输入模式
2.使能AFIO时钟
3.清除和重新设置 AFIO-> EXTICR[] 来指定哪个GPIO的Pin_x和第x号外部中断线对应
4.设置 EXTI->IMR 来使能中断线
5.设置 EXTI->EMR 允许中断事件
6.设置 EXTI->FTSR 和 EXTI->RTSR 来选择触发方式

设置NVIC的顺序：
1.设置 SCB->AIRCR 来整个系统的NVIC分组（也就是抢断式优先级占几位，子优先级占几位）
	注意：设置这个需要先写入钥匙
2.设置 NVIC->ISER 寄存器使能需要的中断
3.设置 NVIC->IPR[]抢断式优先级和子优先级（4位）

使用common_interrupt来配置外部中断：
	1.将引脚配置成输入模式
	2.配置外部中断	EXTI_Config(GPIOA,GPIO_Pin_0,FALL,NVIC_GROUP2,8);

使用common_interrupt来配置外设中断：
	1.将外设的中断允许打开
	2.配置系统的中断分组（一个系统只需要配置一次）	NVIC_ConfigGroup(u8 _GroupID)
	3.使能NVIC控制器的中断号码	NVIC_Enable(u8 _InterruptID,u8 _PriorityNum)

注意：由于stm32的原因，使用外部中断配置时，如果是GPIOx.n引脚，会自动配置为EXTIn，例如：GPIOD.3自动配置到EXTI3上
***************************************************/
#include "stm32f10x.h"
#include "common_interrupt.h"
//功能：配置外部中断需要的IO复用
//参数：GPIOx，GPIO_Pin_x
void AFIO_EXTIPortInit(GPIO_TypeDef* _GPIOx,u16 _GPIO_Pin_x){
	u16	_RegAddr	=0;	
	//取得所在寄存器号，每个寄存器能设置4根中断线
	u16 _DataShift	=0;	
	//取得引脚对应寄存器中的偏移位数，例如：GPIO_Pin_3对应的EXTI3的设置位是[15:12]
	u32	_DataICRx	=0x0;
	//中断配置寄存器的值

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//使能AFIO复用时钟
	if			(_GPIO_Pin_x==GPIO_Pin_0 ){_RegAddr= 0/4;_DataShift=( 0%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_1 ){_RegAddr= 1/4;_DataShift=( 1%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_2 ){_RegAddr= 2/4;_DataShift=( 2%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_3 ){_RegAddr= 3/4;_DataShift=( 3%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_4 ){_RegAddr= 4/4;_DataShift=( 4%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_5 ){_RegAddr= 5/4;_DataShift=( 5%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_6 ){_RegAddr= 6/4;_DataShift=( 6%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_7 ){_RegAddr= 7/4;_DataShift=( 7%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_8 ){_RegAddr= 8/4;_DataShift=( 8%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_9 ){_RegAddr= 9/4;_DataShift=( 9%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_10){_RegAddr=10/4;_DataShift=(10%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_11){_RegAddr=11/4;_DataShift=(11%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_12){_RegAddr=12/4;_DataShift=(12%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_13){_RegAddr=13/4;_DataShift=(13%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_14){_RegAddr=14/4;_DataShift=(14%4)*4;}
	else if	(_GPIO_Pin_x==GPIO_Pin_15){_RegAddr=15/4;_DataShift=(15%4)*4;}

	AFIO->EXTICR[_RegAddr]&=~(0xf<<_DataShift);	
	//清除中断线
	if		(_GPIOx==GPIOA)_DataICRx=0x0;
	else if	(_GPIOx==GPIOB)_DataICRx=0x1;
	else if	(_GPIOx==GPIOC)_DataICRx=0x2;
	else if	(_GPIOx==GPIOD)_DataICRx=0x3;
	else if	(_GPIOx==GPIOE)_DataICRx=0x4;
	else if	(_GPIOx==GPIOF)_DataICRx=0x5;
	else if	(_GPIOx==GPIOG)_DataICRx=0x6;
	AFIO->EXTICR[_RegAddr]|= (_DataICRx<<_DataShift);
	//选择中断线对应的GPIO
}

//功能：设置NVIC分组
//参数说明：_GroupID是分组号码。查看CortexM3编程手册120页。取值如下：
//	取值	抢占优先级位数	子优先级位数
//	 0x3			4			0
//	 0x4			3			1
//	 0x5			2			2
//	 0x6			1			3
void NVIC_ConfigGroup(u8 _GroupID){
	u32	_tmp;
    //设置 SCB->AIRCR 来整个系统的NVIC分组（也就是抢断式优先级占几位，子优先级占几位）
	_tmp=SCB->AIRCR;				//取得当前设置
	_tmp&=0x0000F8FF;			//清空之前的分组设置
	_tmp|=(_GroupID<<8);			//设置分组号码
	SCB->AIRCR|=0x05FA0000;		//写入钥匙
	SCB->AIRCR=_tmp;				//写入值
}
//功能：使能NVIC中断
//参数：中断号_InterruptID，stm3210x_ref手册132页查看
//		优先级数（实际上需要参考优先级组设定才能判断谁的优先级高）,取值是0到15
void NVIC_Enable(u8 _InterruptID,u8 _PriorityNum){
	//使能中断
	NVIC->ISER[_InterruptID/32]=(0x1<<(_InterruptID-(_InterruptID/32)*32));
	//设置优先级
	NVIC->IP[_InterruptID]=_PriorityNum<<4;
}
//功能：配置外部中断
//参数说明：_Trigger的取值是RISE（上升沿），FALL（下降沿）和RISEANDFALL（上升和下降沿触发）
//			GroupID是分组号码，详见NVIC_ConfigGroup(u8 _GroupID)的参数说明
//			u8 _PriorityNum是优先级号码
void EXTI_Config(GPIO_TypeDef* _GPIOx,u16 _GPIO_Pin_x,EXTITrigger_TypeDef _Trigger,u8 _PriorityNum){
	EXTI_InitTypeDef	EXTI_InitStructure;
	u8	_InterruptID=0;

	AFIO_EXTIPortInit(_GPIOx,_GPIO_Pin_x);
	EXTI_InitStructure.EXTI_Line = _GPIO_Pin_x;
	//由于GPIO_Pin_x和EXTI_Linex的值是对应的，所以直接赋值（除了GPIO_Pin_ALL以及EXTI_Line16-19）
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = _Trigger;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_ConfigGroup(SYS_NVIC_GROUP);	
	if		(_GPIO_Pin_x==GPIO_Pin_0) _InterruptID=6;
	else if	(_GPIO_Pin_x==GPIO_Pin_1) _InterruptID=7;
	else if	(_GPIO_Pin_x==GPIO_Pin_2) _InterruptID=8;
	else if	(_GPIO_Pin_x==GPIO_Pin_3) _InterruptID=9;
	else if	(_GPIO_Pin_x==GPIO_Pin_4) _InterruptID=10;
	else if	(_GPIO_Pin_x==GPIO_Pin_5) _InterruptID=23;
	else if	(_GPIO_Pin_x==GPIO_Pin_6) _InterruptID=23;
	else if	(_GPIO_Pin_x==GPIO_Pin_7) _InterruptID=23;
	else if	(_GPIO_Pin_x==GPIO_Pin_8) _InterruptID=23;
	else if	(_GPIO_Pin_x==GPIO_Pin_9) _InterruptID=23;
	else if	(_GPIO_Pin_x==GPIO_Pin_10)_InterruptID=40;
	else if	(_GPIO_Pin_x==GPIO_Pin_11)_InterruptID=40;
	else if	(_GPIO_Pin_x==GPIO_Pin_12)_InterruptID=40;
	else if	(_GPIO_Pin_x==GPIO_Pin_13)_InterruptID=40;
	else if	(_GPIO_Pin_x==GPIO_Pin_14)_InterruptID=40;
	else if	(_GPIO_Pin_x==GPIO_Pin_15)_InterruptID=40;
	NVIC_Enable(_InterruptID,_PriorityNum);
}
