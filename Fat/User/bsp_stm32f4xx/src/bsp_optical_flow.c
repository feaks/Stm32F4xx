#include "bsp_optical_flow.h"

#include "bsp_i2c.h"
#include "stdbool.h"

PX4_FRAME px4flow_frame;
PX4_INTRGRAL_FRAME px4flow;

static I2C * I2C_PX4FLOW;

void PX4FLOW_Init(void)
{
  I2C_PX4FLOW = I2C_3;
}

/** PX4FLOW单字节写入
 *   REG_Address：要写入的寄存器地址
 *   REG_data：要写入的数据
 */
bool PX4FLOW_Write_Byte(uint8_t REG_Address, uint8_t REG_data)
{
	return I2C_Write_REG(I2C_PX4FLOW, PX4FLOW_ADDR, REG_Address, REG_data);
}

/**
 * PX4FLOW单字节读取
 *   REG_Address：要读取的寄存器地址
 */
uint8_t PX4FLOW_Read_Byte(uint8_t REG_Address)
{
	return I2C_Read_REG(I2C_PX4FLOW, PX4FLOW_ADDR, REG_Address);
}


/**
 * PX4FLOW读取n个字节
 *   REG_Address：要读取的寄存器地址
 *   buf：读取后存储数据的buf
 *   len：要读取数据长度
 */
bool PX4FLOW_Read_NByte(uint8_t REG_Address, uint8_t * buf, uint8_t len)
{
  if(!I2C_Start(I2C_PX4FLOW))return false;
	I2C_SendByte(I2C_PX4FLOW, PX4FLOW_ADDR);  //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_PX4FLOW)){I2C_Stop(I2C_PX4FLOW); return false;}
	I2C_SendByte(I2C_PX4FLOW, REG_Address);   
	I2C_WaitAck(I2C_PX4FLOW);
	I2C_Start(I2C_PX4FLOW);
	I2C_SendByte(I2C_PX4FLOW, PX4FLOW_ADDR | 1); // 读操作
	I2C_WaitAck(I2C_PX4FLOW);
	for(uint16_t i=0; i<len; i++)
	{
	  //buf[i] = I2C_RadeByte(I2C_PX4FLOW);
		uint8_t a = I2C_RadeByte(I2C_PX4FLOW);
		if(i < sizeof(PX4_FRAME))
		{
		  ((uint8_t*)&px4flow_frame)[i] = a;
		}
		else 
		{
			((uint8_t*)&px4flow)[i-sizeof(PX4_FRAME)] = a;
		}
		
		if(i<len-1)
		  I2C_Ack(I2C_PX4FLOW);
	}
	
	I2C_NoAck(I2C_PX4FLOW);
	I2C_Stop(I2C_PX4FLOW);
	return true;
}
//float x_sum,y_sum,a=0.003,b=0.0015;
void px4flow_read(void)
{
	//PX4FLOW_Read_NByte(0x00, (uint8_t *)&px4flow_frame, sizeof(PX4_FRAME));
	
  PX4FLOW_Read_NByte(0x0, (uint8_t *)&px4flow, 20+sizeof(PX4_FRAME));
//	if(px4flow.quality>0 && px4flow.sonar_timestamp)
//	{
//	x_sum += px4flow_frame.flow_comp_m_x*0.00005-a;
//	y_sum += px4flow_frame.flow_comp_m_y*0.00005-b;
//	}
//	x_sum+=px4flow.pixel_flow_x_integral*0.000005;
//	y_sum+=px4flow.pixel_flow_y_integral*0.000005;
}
void px4flow_data (void)
{


}
