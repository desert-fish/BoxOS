#include "common_usart.h"
//定义了一些串口相关的函数

//功能：配置串口(使用默认配置),目前只用了USART1，需要增加串口的话就添加代码
//		配置的内容：1.外设时钟使能	2.GPIO的配置	3.USARTx寄存器设置
//默认配置：8位数据位，一位停止位，115200波特率
void USARTx_ConfigDefault(USART_TypeDef* USARTx){
	if(USARTx==USART1){						//如果配置的是USART1
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		//PA9->TX，PA10->RX
		GPIOA->CRH&=0XFFFFF00F;
		GPIOA->CRH|=0X000008B0;
		//激活USART1
		USART1->CR1|=(0x1<<13);
		//定义字长8位
		USART1->CR1&=~(0x1<<12);
		//设置停止位个数为1位
		USART1->CR2&=~(0x3<<12);
		//设置波特率115200
		USART1->BRR&=0xffff0000;
		USART1->BRR|=((39<<4)|(1<<0));
	}
}
//功能：发送使能
void USARTx_SendEnable(USART_TypeDef* USARTx){
	//使能发送
	USARTx->CR1|=(0x1<<3);
}
//功能：发送禁止
void USARTx_SendDisable(USART_TypeDef* USARTx){
	//禁止发送
	USARTx->CR1&=~(0x1<<3);
}
//功能:接收使能
void USARTx_ReceiveEnable(USART_TypeDef* USARTx){
	//使能接收
	USARTx->CR1|=(0x1<<2);
}
//功能:接收禁止
void USARTx_ReceiveDisable(USART_TypeDef* USARTx){
	//禁止接收
	USARTx->CR1&=~(0x1<<2);
}
//功能：配置中断使能
//参数：u32 _InterruptFlag	指定中断的允许，如果不填写就会清除位
//说明：如果TXE和TC目前为1，若调用USARTx_InterruptConfig(0)会把他们都清除为0；
//		如果调用USARTx_InterruptConfig(USART_TXEIE)会把TCIE清除为0
//#define	USART_TXEIE		0x1<<7	//发送缓冲区为空的中断
//#define USART_TCIE		0x1<<6	//发送完成中断
//#define USART_RXNEIE	0x1<<5	//接收缓冲区非空中断

void USARTx_ITConfig(USART_TypeDef* USARTx,u32 _InterruptFlag){
	USARTx->CR1&=~(0x3<<6);
	USARTx->CR1|=_InterruptFlag;	//设置中断允许标志
	if(_InterruptFlag){				//如果不为0，就打开NVIC
		if(USARTx==USART1){
			NVIC_ConfigGroup(SYS_NVIC_GROUP);
			NVIC_Enable(37,8);	//USART1在37号中断，优先级设置为2		
		}	
	}	
}
//功能：发送字符串
//参数：char *	字符串首地址
void USARTx_SendString(USART_TypeDef* USARTx,char* _String){
	while(*_String){					//没有执行到字符串结尾
		while(!(USARTx->SR&(0x1<<6)));	//等待发送完成
		USARTx->DR=*_String;
		_String++;
	}
}
//功能：发送一个数字
//参数：u32 _Number	要发送的数字
//		u8  _Base	发送的进制形式，取值：
//#define	NUM_BASE_HEX	16	//16进制形式
//#define NUM_BASE_DEC	10	//10进制形式
//#define NUM_BASE_BIN	2	// 2进制形式
//说明：16进制的输出是全部输出的，包括前边的0，例如：000000fa
//		10进制的输出会忽略前边的0
//		 2进制的输出会保留所有位
void USARTx_SendNumberASCII(USART_TypeDef* USARTx,u32 _Number,u8 _Base){
	int i=0;
	u32	_Temp=0;
	u32	_HeadZeroFlag=0;			//标记开头的0，用来区分开头和中间的0
	if(NUM_BASE_HEX==_Base){		//实现方法是从左向右依次转换四位数
		for(i=28;i>=0;i-=4){
			while(!(USARTx->SR&(0x1<<6)));	//等待发送完成
			_Temp=_Number&(0xf<<i);	//取得当前要转换的四位
			_Temp>>=i;				//右移到0位对齐
			if(_Temp<10){			//如果是10以下的数字就加上'0'进行转换
				USARTx->DR='0'+_Temp;
			}
			else{					//如果是10以上的数字就加上'a'减去10
				USARTx->DR='a'+_Temp-10;
			}	
		}
	}
	else if(NUM_BASE_DEC==_Base){
		if(0==_Number){				   		//处理显示0的特殊情况
			while(!(USARTx->SR&(0x1<<6)));	//等待发送完成
			USARTx->DR='0';
			return;							//结束显示
		}
		for(i=1000000000;i>0;i/=10){		//从十亿开始除
			while(!(USARTx->SR&(0x1<<6)));	//等待发送完成
			_Temp=_Number/i;
			if(_Temp || _HeadZeroFlag){		//去除掉开头的0
				USARTx->DR='0'+_Temp;
				_HeadZeroFlag=1;			//标记已经存在左边的有效位，不在舍去0	
			}
			_Number-=_Temp*i;
		}
	}
	else if(NUM_BASE_BIN==_Base){
		for(i=31;i>=0;i--){		//每循环一次，i都会右移一位
			while(!(USARTx->SR&(0x1<<6)));	//等待发送完成
			if((0x1<<i)&_Number){
				USARTx->DR='1';
			}
			else{
				USARTx->DR='0';
			}
		}
	}
}
//功能：等待接收字符串
//参数：char*	_String
//说明：当接收到\n,\r或者\0时结束接收
void USARTx_ReceiveString(USART_TypeDef* USARTx,char* _String){
	do{
		while(!(USARTx->SR&(0x1<<5)));	//等待接收数据
		*_String=USARTx->DR;			//储存数据
		_String++;						//增量地址
	}while('\0'!=*(_String-1) && '\n'!=*(_String-1) && '\r'!=*(_String-1));
	*(_String-1)='\0';					//写入结束标志
}
//功能：等待接收ASCII数字
//返回值：接收到的数字
//参数：u8  _Base	接收的进制形式，取值：
//#define	NUM_BASE_HEX	16	//16进制形式
//#define NUM_BASE_DEC	10	//10进制形式
//#define NUM_BASE_BIN	2	// 2进制形式
//说明：数字是以ASCII码的形式接收的，例如："32"，当出现'\0'或者'\n'或者'\r'时认为数字结束
//		同时，这个函数接收的只能是正数
u32	USARTx_ReceiveNumberASCII(USART_TypeDef* USARTx,u8 _Base){
	u8	_Temp=0;
	u32	_ReceiveNumber=0;
	while(1){
		while(!(USARTx->SR&(0x1<<5)));	//等待接收数据
		_Temp=USARTx->DR;
		if('\0'==_Temp || '\n'==_Temp || '\r'==_Temp ){
			return _ReceiveNumber;		//终止数据接收
		}
		else{
			if(_Base==NUM_BASE_HEX){	//以16进制处理
				_ReceiveNumber<<=4;			//乘以16
				if(_Temp>='0' && _Temp<='9')		//数字在0到9之间
					_ReceiveNumber+=(_Temp-'0');
				else if(_Temp>='A' && _Temp<='F')	//数字在A到F之间
					_ReceiveNumber+=(_Temp-'A');
				else if(_Temp>='a' && _Temp<='f')	//数字在a到f之间
					_ReceiveNumber+=(_Temp-'a');
			}
			else if(_Base==NUM_BASE_DEC){
				_ReceiveNumber*=10;			//乘以10
				_ReceiveNumber+=(_Temp-'0');
			}
			else if(_Base==NUM_BASE_BIN){
				_ReceiveNumber<<=1;			//乘以2
				_ReceiveNumber+=(_Temp-'0');
			}
		}
	}
}
//配置USARTx的DMA
//参数：u32 _MemoryAddr		内存地址
//		u16	_Size			发送或者接收的数据量大小
//		u8	_Operate		发送或者接收，取值：
//#define USART_DMA_SEND		0x1<<7
//#define USART_DMA_RECEIVE		0x1<<6
void USARTx_DMAConfig(USART_TypeDef* USARTx,u32 _MemoryAddr,u16 _Size,u32 _Operate,u32 _ITConfig){
	#define IT_PRIORITY	3	//中断优先级
	DMA_InitTypeDef 	DMAInitStruct;
	//DMA1和串口的配置
	//时钟使能（AHB时钟线上的DMA）,USART1到3是DMA1，USART4是DMA2
	if(USARTx==USART1 || USARTx==USART2 || USARTx==USART3)
		RCC->AHBENR|=(0x1<<0);
	//else if(USARTx==USART4)
	//	RCC->AHBENR|=(0x1<<1);
	//USART作为外设
	DMAInitStruct.DMA_PeripheralBaseAddr=(u32)&USARTx->DR;
	//设定存储器地址
	DMAInitStruct.DMA_MemoryBaseAddr=_MemoryAddr;
	if(_Operate==USART_DMA_SEND){
		//传送数据到外设
		DMAInitStruct.DMA_DIR=DMA_DIR_PeripheralDST;
	}
	else if(_Operate==USART_DMA_RECEIVE){
		//从外设取得数据
		DMAInitStruct.DMA_DIR=DMA_DIR_PeripheralSRC;
	}
	//设定缓冲大小
	DMAInitStruct.DMA_BufferSize=_Size;
	//外设不增量
	DMAInitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	//内存地址增量
	DMAInitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
	//外设单位为字节
	DMAInitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	//内存地址单位为字节
	DMAInitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	//普通模式（非循环模式）
	DMAInitStruct.DMA_Mode=DMA_Mode_Normal;
	//中等优先级
	DMAInitStruct.DMA_Priority=DMA_Priority_Medium;
	//非内存到内存模式
	DMAInitStruct.DMA_M2M=DMA_M2M_Disable;
	//写入配置
	if(USARTx==USART1){
		if(_Operate==USART_DMA_SEND){
			DMA_Init(DMA1_Channel4,&DMAInitStruct);
			//配置通道中断
			DMA1_Channel4->CCR&=~(0x6<<1);
			DMA1_Channel4->CCR|=(_ITConfig<<1);
			//配置分组
			if(_ITConfig){
				DMA1->IFCR=(0xf<<12);				//清除中断标志
				NVIC_ConfigGroup(SYSTEM_NVIC_GROUP);
				NVIC_Enable(14,IT_PRIORITY);
			}
			//启动通道4
			DMA_Cmd(DMA1_Channel4,ENABLE);			//开启通道4
			//允许USART1的DMAT
			USART1->CR3|=(0x1<<7);
		}
		else if(_Operate==USART_DMA_RECEIVE){
			DMA_Init(DMA1_Channel5,&DMAInitStruct);
			//配置通道中断
			DMA1_Channel5->CCR&=(0x3<<1);
			DMA1_Channel5->CCR|=(_ITConfig<<1);
			//配置分组
			if(_ITConfig){
				NVIC_ConfigGroup(SYSTEM_NVIC_GROUP);
				NVIC_Enable(15,IT_PRIORITY);
			}
			//启动通道4
			DMA_Cmd(DMA1_Channel5,ENABLE);			//开启通道5
			//允许USART1的DMAR
			USART1->CR3|=(0x1<<6);
		}
	}
}
