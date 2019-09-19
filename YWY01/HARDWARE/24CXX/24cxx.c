/**
  ******************************************************************************
  * @file    24CXX.c
  * @author  ����ס
  * @version V1.0.0
  * @date    2018/1/5
  * @brief   24CXXϵ��EPROM����
  ******************************************************************************
  * @attention
  * 24CXXϵ��EPROM���������ģ��I2Cʵ�֡�
  * 
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "24cxx.h" 
#include "delay.h"

/**
  * @brief  I2C�ĳ�ʼ��
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
  * @brief  I2C����ʼ�ź�
  * @param  None
  * @retval None
  */
void IIC_Start(void)
{
	SDA_OUT();	//sda�����
	
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	
	delay_us(10);
	
 	IIC_SDA=0;	//START:when CLK is high,DATA change form high to low 
 	
	delay_us(10);
	
	IIC_SCL=0;	//ǯסI2C���ߣ�׼�����ͻ�������� 
}

/**
  * @brief  I2C��ֹͣ�ź�
  * @param  None
  * @retval None
  */
void IIC_Stop(void)
{
	SDA_OUT();	//sda�����
	
	IIC_SCL=0;
	IIC_SDA=0;	//STOP:when CLK is high DATA change form low to high
	
 	delay_us(10);
	
	IIC_SCL=1; 
	IIC_SDA=1;	//����I2C���߽����ź�
	
	delay_us(10);							   	
}

/**
  * @brief  I2C��ֹͣ�ź�
  * @param  None
  * @retval 1������Ӧ��ʧ��
  *         0������Ӧ��ɹ�
  */
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	
	SDA_IN();      //SDA����Ϊ����  
	
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
	
	IIC_SCL=0;	//ʱ�����0 	   
	
	return 0;  
}

/**
  * @brief  I2C��Ӧ���ź�
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
  * @brief  I2C�ķ�Ӧ���ź�
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
  * @brief  I2C�ķ����ֽ�
  * @param  txd��Ҫ���͵�8λ���� ��λΪ1����Ӧ��0����Ӧ��
  * @retval 
  */
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;
	
	SDA_OUT(); 	    
	
    IIC_SCL=0;	//����ʱ�ӿ�ʼ���ݴ���
    
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
		
		delay_us(10);   //��TEA5767��������ʱ���Ǳ����
		
		IIC_SCL=1;
		
		delay_us(10); 
		
		IIC_SCL=0;	
		
		delay_us(10);
    }	 
}

/**
  * @brief  I2C�Ķ������ֽ�
  * @param  ack���Ƿ���ҪӦ��
  * @retval 
  */
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	
	SDA_IN();	//SDA����Ϊ����
	
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
	    IIC_NAck();//����nACK
	}
    else
	{
	    IIC_Ack(); //����ACK   
	}

	SDA_OUT(); 	
	
    return receive;
}

/**
  * @brief  EPROM��ʼ��
  * @param  None
  * @retval None
  */
void AT24CXX_Init(void)
{
	IIC_Init();
}

/**
  * @brief  EPROM�����ֽ�����
  * @param  ReadAddr����ַ
  * @retval ����
  */
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;
	
    IIC_Start();  
	
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //���Ͷ�����
		
		if(IIC_Wait_Ack())
		{		
			return 0;
		}

		IIC_Send_Byte(ReadAddr/256);	//���͸ߵ�ַ
		
		if(IIC_Wait_Ack())
		{		
			return 0;
		}

	}
	else
	{
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	}

    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
    
	if(IIC_Wait_Ack())
	{		
		return 0;
	}

	//�������ģʽ
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           
	
	if(IIC_Wait_Ack())
	{		
		return 0;
	}

    temp=IIC_Read_Byte(0);
	
    IIC_Stop();	//����һ��ֹͣ����	    
    
	return temp;
}

/**
  * @brief  EPROMд���ֽ�����
  * @param  WriteAddr����ַ
  *			DataToWrite������
  * @retval 
  */
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		if(IIC_Wait_Ack())
		{		
			return ;
		}

		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
 	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}
	
	if(IIC_Wait_Ack())
	{		
		return ;
	}
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
    
	if(IIC_Wait_Ack())
	{		
		return ;
	}

	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   

	if(IIC_Wait_Ack())
	{		
		return ;
	}
	
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

/**
  * @brief  EPROMдָ���������ݡ��ú�������д��16bit����32bit������
  * @param  WriteAddr����ַ
  *			DataToWrite������
  *			Len��Ҫд�����ݵĳ���2,4
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
  * @brief  EPROM��ָ�����ȵ����ݡ��ú������ڶ���16bit����32bit������
  * @param  ReadAddr����ַ
  *			Len��Ҫ�������ݵĳ���2,4
  * @retval ����
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


