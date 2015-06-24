#include "stm32f10x.h"
#include "common_delay.h"
#include "common_interrupt.h"
#include "common_usart.h"
#include "common_basictimer.h"
#include "common_generaltimer.h"
#include "multi_task.h"

#include "led_driver.h"

//-------------------------------------
//一些硬件驱动的PID 号码，这些号码固定不变
#define LED1_PID 2	//红灯
#define LED2_PID 3	//绿灯
//-------------------------------------
#define ERROR_SHOW(n)  MT_CallService(LED1_PID,n,0)
#define NORMAL_SHOW(n) MT_CallService(LED2_PID,n,0)

int proc1(void){
	while(1){
		NORMAL_SHOW(1);
		MT_Wait(700000);
	}
}
void proc2(void){
	while(1){
		ERROR_SHOW(1);
		MT_Wait(500000);
	}
}
int main(){
	MT_ChangeSP(0);
	MT_InitSystem();
	MT_AddTask((u32)Led1_Driver,LED1_PID,0,0);
	MT_AddTask((u32)Led2_Driver,LED2_PID,0,0);		
	MT_AddTask((u32)proc1,0,0,0);
	MT_AddTask((u32)proc2,0,0,0);
	MT_StartSysTick();
	while(1);
}
