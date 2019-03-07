#include "FreeRTOS.h"
#include "task.h"
#include "bsp_i2c.h"
#include "bsp_gpio.h"

I2C *I2C_1, *I2C_2, *I2C_3, *I2C_4;

static void I2C_Delay()
{
  __nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop(); __nop();__nop();__nop();__nop();__nop();__nop();__nop();
}

/*******************************************************************************
* Function Name  : I2C_GPIO_Config
* Description    : Configration Simulation IIC GPIO
* Input          : None 
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_GPIO_Config(void)
{	
	I2C_1 = (I2C *)pvPortMalloc(sizeof(I2C));
	I2C_1->SCL_GPIOx = GPIOB;
	I2C_1->SDA_GPIOx = GPIOB;
	I2C_1->SCL_Pin = GPIO_Pin_8;
	I2C_1->SDA_Pin = GPIO_Pin_9;
	GPIO_Initialize(GPIOB, GPIO_Pin_8, GPIO_Mode_OUT, GPIO_OType_OD, GPIO_PuPd_UP);
	GPIO_Initialize(GPIOB, GPIO_Pin_9, GPIO_Mode_OUT, GPIO_OType_OD, GPIO_PuPd_UP);
	
	I2C_2 = (I2C *)pvPortMalloc(sizeof(I2C));
	I2C_2->SCL_GPIOx = GPIOB;
	I2C_2->SDA_GPIOx = GPIOB;
	I2C_2->SCL_Pin = GPIO_Pin_10;
	I2C_2->SDA_Pin = GPIO_Pin_11;
	GPIO_Initialize(GPIOB, GPIO_Pin_10, GPIO_Mode_OUT, GPIO_OType_OD, GPIO_PuPd_UP);
	GPIO_Initialize(GPIOB, GPIO_Pin_11, GPIO_Mode_OUT, GPIO_OType_OD, GPIO_PuPd_UP);
	
	I2C_3 = (I2C *)pvPortMalloc(sizeof(I2C));
	I2C_3->SCL_GPIOx = GPIOA;
	I2C_3->SDA_GPIOx = GPIOC;
	I2C_3->SCL_Pin = GPIO_Pin_8;
	I2C_3->SDA_Pin = GPIO_Pin_9;
	GPIO_Initialize(GPIOA, GPIO_Pin_8, GPIO_Mode_OUT, GPIO_OType_OD, GPIO_PuPd_UP);
	GPIO_Initialize(GPIOC, GPIO_Pin_9, GPIO_Mode_OUT, GPIO_OType_OD, GPIO_PuPd_UP);
	
	I2C_4 = (I2C *)pvPortMalloc(sizeof(I2C));
	I2C_4->SCL_GPIOx = GPIOB;
	I2C_4->SDA_GPIOx = GPIOF;
	I2C_4->SCL_Pin = GPIO_Pin_0;
	I2C_4->SDA_Pin = GPIO_Pin_11;
	GPIO_Initialize(GPIOB, GPIO_Pin_0, GPIO_Mode_OUT, GPIO_OType_OD, GPIO_PuPd_UP);
	GPIO_Initialize(GPIOF, GPIO_Pin_11, GPIO_Mode_OUT, GPIO_OType_OD, GPIO_PuPd_UP);
	
	I2C_Stop(I2C_1); // 给一个停止信号, 复位I2C总线上的所有设备到待机模式
	I2C_Stop(I2C_2);
	I2C_Stop(I2C_3);
	I2C_Stop(I2C_4);
}

/*******************************************************************************
* Function Name  : I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather	 Start
****************************************************************************** */
bool I2C_Start(I2C * i2c)
{
	taskENTER_CRITICAL();
	SDA_H;
	SCL_H;
	I2C_Delay();
	if(!SDA_read){ taskEXIT_CRITICAL(); return false; }	//SDA线为低电平则总线忙,退出
	SDA_L;
	I2C_Delay();
	if(SDA_read) { taskEXIT_CRITICAL(); return false; }	//SDA线为高电平则总线出错,退出
	SDA_L;
	I2C_Delay();
	taskEXIT_CRITICAL();
	return true;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
bool I2C_Stop(I2C * i2c)
{
	taskENTER_CRITICAL();
	SCL_H;
	SDA_L;
	I2C_Delay();
	if(SDA_read) { taskEXIT_CRITICAL(); return false; };	//SDA线为高电平则总线出错,退出
	SDA_H;
	I2C_Delay();
	if(!SDA_read) { taskEXIT_CRITICAL(); return false; };	//SDA线为高电平则总线出错,退出
	SDA_H;
	I2C_Delay();
	taskEXIT_CRITICAL();
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Ack(I2C * i2c)
{	
	taskENTER_CRITICAL();
	SCL_L;
	I2C_Delay();
	SDA_L;
	SCL_H;
	I2C_Delay();
	SCL_L;
	SDA_H;
	I2C_Delay();
	taskEXIT_CRITICAL();
}   
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_NoAck(I2C * i2c)
{	
	taskENTER_CRITICAL();
	SCL_L;
	I2C_Delay();
	SDA_H;
	SCL_H;
	I2C_Delay();
	SCL_L;
	I2C_Delay();
	taskEXIT_CRITICAL();
} 
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
****************************************************************************** */
bool I2C_WaitAck(I2C * i2c) 	 //返回为:=true有ACK,=false无ACK
{
	taskENTER_CRITICAL();
	SCL_L;
	I2C_Delay();
	SDA_H;	
	
	SCL_H;
	I2C_Delay();
	
	if(SDA_read)
	{
      SCL_L;
			I2C_Delay();
		  taskEXIT_CRITICAL();
      return false;
	}
	SCL_L;
	I2C_Delay();
	taskEXIT_CRITICAL();
	return true;
}
/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_SendByte(I2C * i2c, uint8_t SendByte) //数据从高位到低位
{
	taskENTER_CRITICAL();
	uint8_t i=8;
	while(i--)
	{
		SCL_L;
		I2C_Delay();
		if(SendByte&0x80)
			SDA_H;  
		else 
			SDA_L;   
		SendByte<<=1;
		SCL_H;
		I2C_Delay();
	}
	taskEXIT_CRITICAL();
}  
/*******************************************************************************
* Function Name  : I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
****************************************************************************** */
uint8_t I2C_RadeByte(I2C * i2c)  //数据从高位到低位
{ 
	  taskENTER_CRITICAL();
    uint8_t i=8;
    uint8_t ReceiveByte=0;			
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
			I2C_Delay();
			SCL_H;
			I2C_Delay();
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
		taskEXIT_CRITICAL();
    return ReceiveByte;
} 

// 写寄存器
bool I2C_Write_REG(I2C * i2c, uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data)		     //void
{
	taskENTER_CRITICAL();
	if(!I2C_Start(i2c)) { taskEXIT_CRITICAL(); return false; }
	I2C_SendByte(i2c, SlaveAddress);   //发送设备地址+写信号
	if(!I2C_WaitAck(i2c)){I2C_Stop(i2c); taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(i2c, REG_Address );   //设置起始地址
	I2C_WaitAck(i2c);	
	I2C_SendByte(i2c, REG_data);
	I2C_WaitAck(i2c);   
	I2C_Stop(i2c); 
	vTaskDelay(2);
	taskEXIT_CRITICAL();
	return true;
}

// 读寄存器
uint8_t I2C_Read_REG(I2C * i2c, uint8_t SlaveAddress, uint8_t REG_Address)
{
	taskENTER_CRITICAL();
	uint8_t REG_data;     	
	if(!I2C_Start(i2c)) {taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(i2c, SlaveAddress);
	if(!I2C_WaitAck(i2c)){I2C_Stop(i2c); taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(i2c, REG_Address);   
	I2C_WaitAck(i2c);
	I2C_Start(i2c);
	I2C_SendByte(i2c, SlaveAddress+1);
	I2C_WaitAck(i2c);

	REG_data = I2C_RadeByte(i2c);
	I2C_NoAck(i2c);
	I2C_Stop(i2c);
	taskEXIT_CRITICAL();
	return REG_data;
}

// 检查设备是否存在
bool I2C_CheckDevice(I2C * i2c, uint8_t SlaveAddress)
{
	taskENTER_CRITICAL();
	bool ack = true; // 默认为true，如有任何异常则返回false
	ack &= I2C_Start(i2c); // 可能总线忙返回false
	I2C_SendByte(i2c, SlaveAddress);
	ack &= I2C_WaitAck(i2c); // 可能设备无应答返回false
	ack &= I2C_Stop(i2c); // 可能总线忙返回false
	taskEXIT_CRITICAL();
	return ack;
}
