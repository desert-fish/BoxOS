#ifndef COMMON_USART_H
#define COMMON_USAER_H
#include "stm32f10x.h"
#include "common_interrupt.h"
//定义了一些串口相关的函数

//功能：配置串口(使用默认配置),目前只用了USART1，需要增加串口的话就添加代码
//		配置的内容：1.外设时钟使能	2.GPIO的配置	3.USARTx寄存器设置
//默认配置：8位数据位，一位停止位，115200波特率
void USARTx_ConfigDefault(USART_TypeDef* USARTx);
//功能：发送使能
void USARTx_SendEnable(USART_TypeDef* USARTx);
//功能：发送禁止
void USARTx_SendDisable(USART_TypeDef* USARTx);
//功能:接收使能
void USARTx_ReceiveEnable(USART_TypeDef* USARTx);
//功能:接收禁止
void USARTx_ReceiveDisable(USART_TypeDef* USARTx);
//功能：配置中断使能
//参数：u32 _InterruptFlag	指定中断的允许，如果不填写就会清除位
//说明：如果TXE和TC目前为1，若调用USARTx_InterruptConfig(0)会把他们都清除为0；
//		如果调用USARTx_InterruptConfig(USART_TXEIE)会把TCIE清除为0
#define	USART_TXEIE		0x1<<7	//发送缓冲区为空的中断
#define USART_TCIE		0x1<<6	//发送完成中断
#define USART_RXNEIE	0x1<<5	//接收缓冲区非空中断

void USARTx_ITConfig(USART_TypeDef* USARTx,u32 _InterruptFlag);
//功能：发送字符串
//参数：char *	字符串首地址
void USARTx_SendString(USART_TypeDef* USARTx,char* _String);
//功能：发送一个数据
//参数：u32 _Number	要发送的数字
//		u8  _Base	发送的进制形式，取值：
#define	NUM_BASE_HEX	16	//16进制形式
#define NUM_BASE_DEC	10	//10进制形式
#define NUM_BASE_BIN	2	// 2进制形式
//说明：16进制的输出是全部输出的，包括前边的0，例如：000000fa
//		10进制的输出会忽略前边的0
//		 2进制的输出会保留所有位
void USARTx_SendNumberASCII(USART_TypeDef* USARTx,u32 _Number,u8 _Base);
//功能：等待接收字符串
//参数：char*	_String
//说明：当接收到\n,\r或者\0时结束接收
void USARTx_ReceiveString(USART_TypeDef* USARTx,char* _String);
//功能：等待接收ASCII数字
//返回值：接收到的数字
//参数：u32 _Number	要接收的数字
//		u8  _Base	接收的进制形式，取值：
//#define	NUM_BASE_HEX	16	//16进制形式
//#define NUM_BASE_DEC	10	//10进制形式
//#define NUM_BASE_BIN	2	// 2进制形式
//说明：数字是以ASCII码的形式接收的，例如："32"，当出现'\0'或者'\n'或者'\r'时认为数字结束
//		同时，这个函数接收的只能是正数
u32	USARTx_ReceiveNumberASCII(USART_TypeDef* USARTx,u8 _Base);
//配置USARTx的DMA
//参数：u32 _MemoryAddr		内存地址
//		u16	_Size			发送或者接收的数据量大小
//		u32	_Operate		发送或者接收，取值：
#define USART_DMA_SEND		0x1<<7
#define USART_DMA_RECEIVE	0x1<<6
//		u32	_ITConfig		中断配置
#define USART_DMA_TCIE		0x1<<1	//传输完成中断
#define USART_DMA_HTIE		0x1<<2	//半传输中断
#define USART_DMA_ITDISABLE	0x0		//不用中断
void USARTx_DMAConfig(USART_TypeDef* USARTx,u32 _MemoryAddr,u16 _Size,u32 _Operate,u32 _ITConfig);
#endif
