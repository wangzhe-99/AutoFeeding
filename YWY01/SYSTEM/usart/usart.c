#include "sys.h"
#include "usart.h"	  
#include "timer.h"
#include "main.h"


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
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

 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART1_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART1_RX_STA=0;       //����״̬���	  
u8  USART1_RX_TIMER;

void USART1_Timer_CallBack()
{
	USART1_RX_STA|=1<<15;	//��ǽ������
}

void uart1_init(u32 bound)
{    
 	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
    USART1_RX_STA = 0;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART1_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{
			if(USART1_RX_STA==0)				
			{
				Set_Timer_Msg(TIMER_MSG_UART1, 3, USART1_Timer_CallBack);
			}
			
			if(USART1_RX_STA < USART_REC_LEN-1)	//�����Խ�������
			{
				USART1_RX_BUF[USART1_RX_STA++] = res;	//��¼���յ���ֵ	 
			}else 
			{
				USART1_RX_STA |= 1<<15; 			//ǿ�Ʊ�ǽ������
			} 
		}
	 } 
} 


void USART1_Send_Data(u8 *buf,u8 len)
{
	u8 t;

  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART1, buf[t]);
	}
}


#endif	

#if EN_USART2_RX   //���ʹ���˽���
//����2�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART2_RX_STA;       //����״̬���	  
u8  USART2_RX_TIMER;

void USART2_Timer_CallBack()
{
	USART2_RX_STA|=1<<15;	//��ǽ������
}
  
void uart2_init(u32 bound)
{
  	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
    USART2_RX_STA = 0;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART1��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

	//USART2_RX	  GPIOA.3��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//��λ����2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//ֹͣ��λ

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2

}

void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
		res =USART_ReceiveData(USART2);	//��ȡ���յ�������

		//DEBUG_PRINTF("R 2");
		
		if((USART2_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{
			if(USART2_RX_STA==0)				//ʹ�ܶ�ʱ��7���ж� 
			{
				Set_Timer_Msg(TIMER_MSG_UART2, 3, USART2_Timer_CallBack);
			}
			
			if(USART2_RX_STA < USART_REC_LEN-1)	//�����Խ�������
			{
				USART2_RX_BUF[USART2_RX_STA++] = res;	//��¼���յ���ֵ	 
			}else 
			{
				USART2_RX_STA |= 1<<15; 			//ǿ�Ʊ�ǽ������
			} 
		}
	 } 
} 

void USART2_Send_Data(u8 *buf,u8 len)
{
	u8 t;

  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
	}
}
#endif	

#if EN_USART3_RX
u8 USART3_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART3_RX_STA;       //����״̬���	  
u8  USART3_RX_TIMER;

void USART3_Timer_CallBack()
{
	USART3_RX_STA|=1<<15;	//��ǽ������
}

//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void uart3_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //����3ʱ��ʹ��

	USART_DeInit(USART3);  //��λ����3
	//USART3_TX   PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PB10

	//USART3_RX	  PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��PB11

	USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART3, &USART_InitStructure); //��ʼ������	3
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 

	//ʹ�ܽ����ж�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�   

	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	USART3_RX_STA=0;		//����
}


void USART3_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		res = USART_ReceiveData(USART3);
		
		if((USART3_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{
			if(USART3_RX_STA==0)				//ʹ�ܶ�ʱ��7���ж� 
			{
				Set_Timer_Msg(TIMER_MSG_UART3, 3, USART3_Timer_CallBack);
			}
			
			if(USART3_RX_STA < USART_REC_LEN-1)	//�����Խ�������
			{
				USART3_RX_BUF[USART3_RX_STA++] = res;	//��¼���յ���ֵ	 
			}else 
			{
				USART3_RX_STA |= 1<<15;				//ǿ�Ʊ�ǽ������
				Clear_Timer_Msg(TIMER_MSG_UART3);
			} 
		}
	}  				 											 
}   

void USART3_Send_Data(u8 *buf,u8 len)
{
	u8 t;

  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART3, buf[t]);
	}
}

#endif

#if EN_USART4_RX
u8 USART4_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART4_RX_STA;       //����״̬���	  
u8  USART4_RX_TIMER;

void USART4_Timer_CallBack()
{
	USART4_RX_STA |= 1<<15;	//��ǽ������
}

//��ʼ��IO ����4
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void uart4_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	// GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); //����4ʱ��ʹ��

	USART_DeInit(UART4);  //��λ����4
	//USART4_TX   PC10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PB10

	//USART4_RX	  PC11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PB11
    
    	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
    
    
	USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
  	USART_Init(UART4, &USART_InitStructure); //��ʼ������	4

	//ʹ�ܽ����ж�
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�����ж�  
    USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ��� 
  
	USART4_RX_STA=0;		//����
}


void UART4_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		res =USART_ReceiveData(UART4);
		
		if((USART4_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{
			if(USART4_RX_STA==0)				//ʹ�ܶ�ʱ��7���ж� 
			{
				Set_Timer_Msg(TIMER_MSG_UART4, 3, USART4_Timer_CallBack);
			}
			
			if(USART4_RX_STA < USART_REC_LEN-1)	//�����Խ�������
			{
				USART4_RX_BUF[USART4_RX_STA++] = res;	//��¼���յ���ֵ	 
			}else 
			{
				USART4_RX_STA |= 1<<15;				//ǿ�Ʊ�ǽ������
			} 
		}
	}  				 											 
}   

void USART4_Send_Data(u8 *buf,u8 len)
{
	u8 t;

	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	  
		USART_SendData(UART4, buf[t]);
	}
}

void USART4_Set_BaudRate(u32 baud)
{
	float tmp;
	u16 mantissa;  // ��������
	u16 fraction;  // С������
	u32 pclk2 = 36;

	USART_Cmd(UART4, DISABLE); 

	tmp = (float) (pclk2*1000000)/(baud*16); // ����õ�USARTDIV

	mantissa = tmp; // �õ���������
	fraction = (tmp-mantissa)*16;  // �õ�С������

	// mantissa����������
	mantissa = mantissa << 4;
	//��ΪUSART_BRR��4λ��С������
	UART4->BRR = mantissa + fraction;

	USART_Cmd(UART4, ENABLE);
}


#endif

#if EN_USART5_RX
u8 USART5_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART5_RX_STA;       //����״̬���	  
u8  USART5_RX_TIMER;

void USART5_Timer_CallBack()
{
	USART5_RX_STA|=1<<15;	//��ǽ������
}

//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void uart5_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE); //����3ʱ��ʹ��

	USART_DeInit(UART5);  //��λ����5
	//USART5_TX   PC12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC12
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PC12

	//USART5_RX	  PD2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);  //��ʼ��PD2

	USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(UART5, &USART_InitStructure); //��ʼ������	3
	USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ��� 

	//ʹ�ܽ����ж�
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�����ж�   

	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	USART5_RX_STA=0;		//����
}


void UART5_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		res = USART_ReceiveData(UART5);
		
		if((USART5_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{
			if(USART5_RX_STA==0)				//ʹ�ܶ�ʱ��7���ж� 
			{
				Set_Timer_Msg(TIMER_MSG_UART3, 3, USART5_Timer_CallBack);
			}
			
			if(USART5_RX_STA < USART_REC_LEN-1)	//�����Խ�������
			{
				USART5_RX_BUF[USART5_RX_STA++] = res;	//��¼���յ���ֵ	 
			}else 
			{
				USART5_RX_STA |= 1<<15;				//ǿ�Ʊ�ǽ������
				Clear_Timer_Msg(TIMER_MSG_UART5);
			} 
		}
	}  				 											 
}   

void USART5_Send_Data(u8 *buf,u8 len)
{
	u8 t;

  	for(t=0;t<len;t++)		//ѭ����������
	{		   
		while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);	  
		USART_SendData(UART5, buf[t]);
	}
}

#endif




