#include "sys.h"
#include "usart.h"	  
#include "timer.h"
#include "main.h"


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}

void uart_init(void)
{
#if EN_USART1_RX
	uart1_init(115200);
#endif

#if EN_USART2_RX
	uart2_init(115200);
#endif

#if EN_USART3_RX
	uart3_init(115200);
#endif
	
#if EN_USART4_RX
	uart4_init(115200);
#endif
	
#if EN_USART5_RX
	uart5_init(115200);
#endif

}

 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART1_RX_STA=0;       //接收状态标记	  
u8  USART1_RX_TIMER;

void USART1_Timer_CallBack()
{
	USART1_RX_STA|=1<<15;	//标记接收完成
}

void uart1_init(u32 bound)
{    
 	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
    USART1_RX_STA = 0;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART1_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{
			if(USART1_RX_STA==0)				
			{
				Set_Timer_Msg(TIMER_MSG_UART1, 3, USART1_Timer_CallBack);
			}
			
			if(USART1_RX_STA < USART_REC_LEN-1)	//还可以接收数据
			{
				USART1_RX_BUF[USART1_RX_STA++] = res;	//记录接收到的值	 
			}else 
			{
				USART1_RX_STA |= 1<<15; 			//强制标记接收完成
			} 
		}
	 } 
} 


void USART1_Send_Data(u8 *buf,u8 len)
{
	u8 t;

  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART1, buf[t]);
	}
}


#endif	

#if EN_USART2_RX   //如果使能了接收
//串口2中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART2_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART2_RX_STA;       //接收状态标记	  
u8  USART2_RX_TIMER;

void USART2_Timer_CallBack()
{
	USART2_RX_STA|=1<<15;	//标记接收完成
}
  
void uart2_init(u32 bound)
{
  	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
    USART2_RX_STA = 0;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART1，GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

	//USART2_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//复位串口2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//停止复位

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART2, ENABLE);                    //使能串口2

}

void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	u8 res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{
		res =USART_ReceiveData(USART2);	//读取接收到的数据

		//DEBUG_PRINTF("R 2");
		
		if((USART2_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{
			if(USART2_RX_STA==0)				//使能定时器7的中断 
			{
				Set_Timer_Msg(TIMER_MSG_UART2, 3, USART2_Timer_CallBack);
			}
			
			if(USART2_RX_STA < USART_REC_LEN-1)	//还可以接收数据
			{
				USART2_RX_BUF[USART2_RX_STA++] = res;	//记录接收到的值	 
			}else 
			{
				USART2_RX_STA |= 1<<15; 			//强制标记接收完成
			} 
		}
	 } 
} 

void USART2_Send_Data(u8 *buf,u8 len)
{
	u8 t;

  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
	}
}
#endif	

#if EN_USART3_RX
u8 USART3_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART3_RX_STA;       //接收状态标记	  
u8  USART3_RX_TIMER;

void USART3_Timer_CallBack()
{
	USART3_RX_STA|=1<<15;	//标记接收完成
}

//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void uart3_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //串口3时钟使能

	USART_DeInit(USART3);  //复位串口3
	//USART3_TX   PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB10

	//USART3_RX	  PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB11

	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART3, &USART_InitStructure); //初始化串口	3
	USART_Cmd(USART3, ENABLE);                    //使能串口 

	//使能接收中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断   

	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	USART3_RX_STA=0;		//清零
}


void USART3_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res = USART_ReceiveData(USART3);
		
		if((USART3_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{
			if(USART3_RX_STA==0)				//使能定时器7的中断 
			{
				Set_Timer_Msg(TIMER_MSG_UART3, 3, USART3_Timer_CallBack);
			}
			
			if(USART3_RX_STA < USART_REC_LEN-1)	//还可以接收数据
			{
				USART3_RX_BUF[USART3_RX_STA++] = res;	//记录接收到的值	 
			}else 
			{
				USART3_RX_STA |= 1<<15;				//强制标记接收完成
				Clear_Timer_Msg(TIMER_MSG_UART3);
			} 
		}
	}  				 											 
}   

void USART3_Send_Data(u8 *buf,u8 len)
{
	u8 t;

  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART3, buf[t]);
	}
}

#endif

#if EN_USART4_RX
u8 USART4_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART4_RX_STA;       //接收状态标记	  
u8  USART4_RX_TIMER;

void USART4_Timer_CallBack()
{
	USART4_RX_STA |= 1<<15;	//标记接收完成
}

//初始化IO 串口4
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void uart4_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	// GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); //串口4时钟使能

	USART_DeInit(UART4);  //复位串口4
	//USART4_TX   PC10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PB10

	//USART4_RX	  PC11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PB11
    
    	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    
    
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
  	USART_Init(UART4, &USART_InitStructure); //初始化串口	4

	//使能接收中断
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断  
    USART_Cmd(UART4, ENABLE);                    //使能串口 
  
	USART4_RX_STA=0;		//清零
}


void UART4_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res =USART_ReceiveData(UART4);
		
		if((USART4_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{
			if(USART4_RX_STA==0)				//使能定时器7的中断 
			{
				Set_Timer_Msg(TIMER_MSG_UART4, 3, USART4_Timer_CallBack);
			}
			
			if(USART4_RX_STA < USART_REC_LEN-1)	//还可以接收数据
			{
				USART4_RX_BUF[USART4_RX_STA++] = res;	//记录接收到的值	 
			}else 
			{
				USART4_RX_STA |= 1<<15;				//强制标记接收完成
			} 
		}
	}  				 											 
}   

void USART4_Send_Data(u8 *buf,u8 len)
{
	u8 t;

	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	  
		USART_SendData(UART4, buf[t]);
	}
}

void USART4_Set_BaudRate(u32 baud)
{
	float tmp;
	u16 mantissa;  // 整数部分
	u16 fraction;  // 小数部分
	u32 pclk2 = 36;

	USART_Cmd(UART4, DISABLE); 

	tmp = (float) (pclk2*1000000)/(baud*16); // 计算得到USARTDIV

	mantissa = tmp; // 得到整数部分
	fraction = (tmp-mantissa)*16;  // 得到小数部分

	// mantissa部分需左移
	mantissa = mantissa << 4;
	//因为USART_BRR低4位是小数部分
	UART4->BRR = mantissa + fraction;

	USART_Cmd(UART4, ENABLE);
}


#endif

#if EN_USART5_RX
u8 USART5_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART5_RX_STA;       //接收状态标记	  
u8  USART5_RX_TIMER;

void USART5_Timer_CallBack()
{
	USART5_RX_STA|=1<<15;	//标记接收完成
}

//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void uart5_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE); //串口3时钟使能

	USART_DeInit(UART5);  //复位串口5
	//USART5_TX   PC12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC12
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC12

	//USART5_RX	  PD2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化PD2

	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(UART5, &USART_InitStructure); //初始化串口	3
	USART_Cmd(UART5, ENABLE);                    //使能串口 

	//使能接收中断
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启中断   

	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	USART5_RX_STA=0;		//清零
}


void UART5_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res = USART_ReceiveData(UART5);
		
		if((USART5_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{
			if(USART5_RX_STA==0)				//使能定时器7的中断 
			{
				Set_Timer_Msg(TIMER_MSG_UART3, 3, USART5_Timer_CallBack);
			}
			
			if(USART5_RX_STA < USART_REC_LEN-1)	//还可以接收数据
			{
				USART5_RX_BUF[USART5_RX_STA++] = res;	//记录接收到的值	 
			}else 
			{
				USART5_RX_STA |= 1<<15;				//强制标记接收完成
				Clear_Timer_Msg(TIMER_MSG_UART5);
			} 
		}
	}  				 											 
}   

void USART5_Send_Data(u8 *buf,u8 len)
{
	u8 t;

  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);	  
		USART_SendData(UART5, buf[t]);
	}
}

#endif




