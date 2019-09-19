/**
  ******************************************************************************
  * @file    24CXX.c
  * @author  乐易住
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   24CXX系列EPROM驱动
  ******************************************************************************
  * @attention
  * 24CXX系列EPROM驱动，软件模拟I2C实现。
  * 
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "24cxx.h" 
#include "delay.h"

/**
  * @brief  I2C的初始化
  * @param  None
  * @retval None
  */
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7); 	
}

/**
  * @brief  I2C的起始信号
  * @param  None
  * @retval None
  */
void IIC_Start(void)
{
	SDA_OUT();	//sda线输出
	
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	
	delay_us(10);
	
 	IIC_SDA=0;	//START:when CLK is high,DATA change form high to low 
 	
	delay_us(10);
	
	IIC_SCL=0;	//钳住I2C总线，准备发送或接收数据 
}

/**
  * @brief  I2C的停止信号
  * @param  None
  * @retval None
  */
void IIC_Stop(void)
{
	SDA_OUT();	//sda线输出
	
	IIC_SCL=0;
	IIC_SDA=0;	//STOP:when CLK is high DATA change form low to high
	
 	delay_us(10);
	
	IIC_SCL=1; 
	IIC_SDA=1;	//发送I2C总线结束信号
	
	delay_us(10);							   	
}

/**
  * @brief  I2C的停止信号
  * @param  None
  * @retval 1，接收应答失败
  *         0，接收应答成功
  */
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	
	SDA_IN();      //SDA设置为输入  
	
	IIC_SDA=1;

	delay_us(10);
	
	IIC_SCL=1;

	delay_us(10);	 

	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	
	IIC_SCL=0;	//时钟输出0 	   
	
	return 0;  
}

/**
  * @brief  I2C的应答信号
  * @param  None
  * @retval None
  */
void IIC_Ack(void)
{
	IIC_SCL=0;
	
	SDA_OUT();
	
	IIC_SDA=0;
	
	delay_us(10);
	
	IIC_SCL=1;
	
	delay_us(10);
	
	IIC_SCL=0;
}

/**
  * @brief  I2C的非应答信号
  * @param  None
  * @retval None
  */
void IIC_NAck(void)
{
	IIC_SCL=0;
	
	SDA_OUT();
	
	IIC_SDA=1;
	
	delay_us(10);
	
	IIC_SCL=1;
	
	delay_us(10);
	
	IIC_SCL=0;
}

/**
  * @brief  I2C的发送字节
  * @param  txd：要发送的8位数据 首位为1：有应答。0：无应答
  * @retval 
  */
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;
	
	SDA_OUT(); 	    
	
    IIC_SCL=0;	//拉低时钟开始数据传输
    
    for(t=0;t<8;t++)
    {              
      //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
		{
			IIC_SDA = 1;
		}
		else
		{
			IIC_SDA = 0;
		}
			
        txd<<=1; 	  
		
		delay_us(10);   //对TEA5767这三个延时都是必须的
		
		IIC_SCL=1;
		
		delay_us(10); 
		
		IIC_SCL=0;	
		
		delay_us(10);
    }	 
}

/**
  * @brief  I2C的读数据字节
  * @param  ack：是否需要应答
  * @retval 
  */
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	
	SDA_IN();	//SDA设置为输入
	
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
		
        delay_us(10);
		
		IIC_SCL=1;
		
        receive<<=1;
		
        if(READ_SDA)
		{
			receive++;   
    	}
			
		delay_us(10); 
    }
	
    if (!ack)
	{
	    IIC_NAck();//发送nACK
	}
    else
	{
	    IIC_Ack(); //发送ACK   
	}

	SDA_OUT(); 	
	
    return receive;
}

/**
  * @brief  EPROM初始化
  * @param  None
  * @retval None
  */
void AT24CXX_Init(void)
{
	IIC_Init();
}

/**
  * @brief  EPROM读单字节数据
  * @param  ReadAddr：地址
  * @retval 数据
  */
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;
	
    IIC_Start();  
	
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送读命令
		
		if(IIC_Wait_Ack())
		{		
			return 0;
		}

		IIC_Send_Byte(ReadAddr/256);	//发送高地址
		
		if(IIC_Wait_Ack())
		{		
			return 0;
		}

	}
	else
	{
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	}

    IIC_Send_Byte(ReadAddr%256);   //发送低地址
    
	if(IIC_Wait_Ack())
	{		
		return 0;
	}

	//进入接收模式
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           
	
	if(IIC_Wait_Ack())
	{		
		return 0;
	}

    temp=IIC_Read_Byte(0);
	
    IIC_Stop();	//产生一个停止条件	    
    
	return temp;
}

/**
  * @brief  EPROM写单字节数据
  * @param  WriteAddr：地址
  *			DataToWrite：数据
  * @retval 
  */
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		if(IIC_Wait_Ack())
		{		
			return ;
		}

		IIC_Send_Byte(WriteAddr>>8);//发送高地址
 	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
	}
	
	if(IIC_Wait_Ack())
	{		
		return ;
	}
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
    
	if(IIC_Wait_Ack())
	{		
		return ;
	}

	IIC_Send_Byte(DataToWrite);     //发送字节							   

	if(IIC_Wait_Ack())
	{		
		return ;
	}
	
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

/**
  * @brief  EPROM写指定长度数据。该函数用于写入16bit或者32bit的数据
  * @param  WriteAddr：地址
  *			DataToWrite：数据
  *			Len：要写入数据的长度2,4
  * @retval 
  */
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/**
  * @brief  EPROM读指定长度的数据。该函数用于读出16bit或者32bit的数据
  * @param  ReadAddr：地址
  *			Len：要读出数据的长度2,4
  * @retval 数据
  */
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}


