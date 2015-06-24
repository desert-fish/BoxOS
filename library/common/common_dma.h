#ifndef COMMON_DMA_H
#define COMMON_DMA_H

#define DMA1_CH1       ((DMA_Channel_TypeDef *) DMA1_Channel1_BASE)
#define DMA1_CH2       ((DMA_Channel_TypeDef *) DMA1_Channel2_BASE)
#define DMA1_CH3       ((DMA_Channel_TypeDef *) DMA1_Channel3_BASE)
#define DMA1_CH4       ((DMA_Channel_TypeDef *) DMA1_Channel4_BASE)
#define DMA1_CH5       ((DMA_Channel_TypeDef *) DMA1_Channel5_BASE)
#define DMA1_CH6       ((DMA_Channel_TypeDef *) DMA1_Channel6_BASE)
#define DMA1_CH7       ((DMA_Channel_TypeDef *) DMA1_Channel7_BASE)
#define DMA2_CH1       ((DMA_Channel_TypeDef *) DMA2_Channel1_BASE)
#define DMA2_CH2       ((DMA_Channel_TypeDef *) DMA2_Channel2_BASE)
#define DMA2_CH3       ((DMA_Channel_TypeDef *) DMA2_Channel3_BASE)
#define DMA2_CH4       ((DMA_Channel_TypeDef *) DMA2_Channel4_BASE)
#define DMA2_CH5       ((DMA_Channel_TypeDef *) DMA2_Channel5_BASE)
//中断使能定义，可以进行或运算
#define DMA_TCIE	0x1<<1	//允许传输完成中断
#define DMA_HTIE	0x1<<2	//允许半传输中断
#define DMA_TEIE	0x1<<3	//允许传输错误中断
#define DMA_IDISABLE	0		//禁止产生中断信号
//中断优先级定义
#define DMA_PRI_LOW			0		//低优先级
#define DMA_PRI_MIDDLE		1		//中优先级
#define DMA_PRI_HIGH		2		//高优先级
#define DMA_PRI_MOSTHIGH	3		//最高优先级
//传输方向定义
#define	DMA_DIR2M			0		//传送到存储器（从外设读取）
#define	DMA_DIR2P			1		//传送到外设（从存储器读取）
//循环模式定义
#define DMA_CIR_EN			0		//循环
#define DMA_CIR_DIS			1		//不循环
//增量模式定义
//										00	存储器地址不增量，外设地址不增量
//										01	存储器地址不增量，外设地址  增量
//										10	存储器地址  增量，外设地址不增量
//										11	存储器地址  增量，外设地址  增量

//USART的DMA信号使能
#define USARTx_DMAT	0x1<<7	//DMA发送信号允许
#define USARTx_DMAR	0x1<<6	//DMA接收信号允许
#define USARTx_DMADISABLE	0	//DMA发送和接收信号禁止

//功能：DMA配置
//参数：
//DMA_Channel_TypeDef* DMA1_Channelx	 DMA通道参见common_dma.h头文件中的定义
//u32 _Cpar								 外设地址寄存器
//u32 _Cmar								 存储地址寄存器
//u16 _Amount							 传输的数量(16位)
//u32 _Priority							 优先级0到3（2位）取值：
//										DMA_PRI_LOW			0		//低优先级
//										DMA_PRI_MIDDLE		1		//中优先级
//										DMA_PRI_HIGH		2		//高优先级
//										DMA_PRI_MOSTHIGH	3		//最高优先级
//u8 _Dir								 传输方向（0：从外设读取，1：从存储器读取）取值：
//										DMA_DIR2M			0		//传送到存储器（从外设读取）
//										DMA_DIR2P			1		//传送到外设（从存储器读取）
//u8 _Circular							 循环模式（0：不循环，1：循环）
//u8 _Increment							 外设和存储器增量模式配置，取值：0,1,2,3
//										00	存储器地址不增量，外设地址不增量
//										01	存储器地址不增量，外设地址  增量
//										10	存储器地址  增量，外设地址不增量
//										11	存储器地址  增量，外设地址  增量
//u8 _DataWidth							 数据宽度配置，取值[3:0]位，其中[3:2]设置存储器数据宽度，[1:0]设置外设数据宽度
//										00	 8位宽度
//										01	16位宽度
//										10	32位宽度
//										11		保留
//										例如：输入值0x09表示存储器用32位数据宽度，外设16位宽度
//u8 _InterruptEnable					中断使能位，用如下参数进行定义：
//										DMA_TCIE	0x1<<1	//允许传输完成中断
//										DMA_HTIE	0x1<<2	//允许半传输中断
//										DMA_TEIE	0x1<<3	//允许传输错误中断
//										DMA_IDISABLE	0		//禁止产生中断信号
//										例如：DMA_TCIE|DMA_HTIE|DMA_TEIE
//u8 M2M								取值0：非存储器到存储器，取值1：存储器到存储器访问
void DMA_Config(DMA_Channel_TypeDef* DMA_CHx,
				u32 _Cpar,
				u32 _Cmar,
				u16 _Amount,
				u8  _Priority,
				u8  _Dir,
				u8  _Circular,
				u8  _Increment,
				u8  _DataWidth,
				u8  _InterruptEnable,
				u8	_M2M){
	DMA_CHx->CPAR	=_Cpar;		//配置外设地址寄存器
	DMA_CHx->CMAR	=_Cmar;		//配置存储地址寄存器
	DMA_CHx->CNDTR	=_Amount;	//配置传输数据的数量
	DMA_CHx->CCR	=0x0;		//清空配置寄存器
	DMA_CHx->CCR	=(_InterruptEnable)|(_Dir<<4)|(_Circular<<5)|(_Increment<<6)|(_DataWidth<<8)|(_Priority<<12|(_M2M<<14));
								//写入配置
}
//功能：DMA传输使能
void DMA_Enable(DMA_Channel_TypeDef* DMA_CHx){
	DMA_CHx->CCR&=~(0x1<<0);		//关闭DMA传输 
	DMA_CHx->CNDTR=100;	//DMA1,传输数据量
	DMA_CHx->CCR|=(0x1<<0);			//开启DMA传输
}
//功能：配置USARTx的DMA信号产生，当每个字节发送或接收结束后向DMA发送一个读或者写请求
//参数：
//USARTx				指定串口
//_DMAEnable			对于DMA信号的配置，取值：
// 						USARTx_DMAT			0x1<<7	DMA发送信号允许
// 						USARTx_DMAR			0x1<<6	DMA接收信号允许
// 						USARTx_DMADISABLE	0		DMA发送和接收信号禁止		
void DMA_USARTxConfig(USART_TypeDef * USARTx,u32 _DMAEnable){
	USARTx->CR3&=~(0x3<<6);			//清除设置位[7:6]
	USARTx->CR3|=_DMAEnable;		//将对应位置1	
}
#endif
