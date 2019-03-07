/*
*********************************************************************************************************
*	                                  
*	模块名称 : 气压计驱动
*	文件名称 : bsp_mpl3115.c
*	版    本 : V1.0
*	说    明 : FreeRTOS系统
* 日		期 ：2015-12-15   
*		
*	2015.12.15 武汉飞航科技有限公司 
*
*********************************************************************************************************
*/
#include "bsp_mpl3115.h"
#include "bsp_i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_Ultrasonic.h"
#include "ahrs.h"
#include "math_extend.h"
#include "PID_Position.h"

MPL3115 mpl3115 = {.enabled = true};

static I2C * I2C_MPL3115;

static uint8_t mpl3115_buffer[5];

/** MPL3115单字节写入
 *   REG_Address：要写入的寄存器地址
 *   REG_data：要写入的数据
 */
bool MPL3115_Write_Byte(uint8_t REG_Address, uint8_t REG_data)
{
	return I2C_Write_REG(I2C_MPL3115, MPL3115_ADDR, REG_Address, REG_data);
}

/**
 * MPL3115单字节读取
 *   REG_Address：要读取的寄存器地址
 */
uint8_t MPL3115_Read_Byte(uint8_t REG_Address)
{
	return I2C_Read_REG(I2C_MPL3115, MPL3115_ADDR, REG_Address);
}

/**
 * MPL3115读取n个字节
 *   REG_Address：要读取的寄存器地址
 *   buf：读取后存储数据的buf
 *   len：要读取数据长度
 */
bool MPL3115_Read_Nbyte(uint8_t REG_Address, uint8_t * buf, uint8_t len)
{
  if(!I2C_Start(I2C_MPL3115))return false;
	I2C_SendByte(I2C_MPL3115, MPL3115_ADDR);  //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_MPL3115)){I2C_Stop(I2C_MPL3115); return false;}
	I2C_SendByte(I2C_MPL3115, REG_Address);   
	I2C_WaitAck(I2C_MPL3115);
	I2C_Start(I2C_MPL3115);
	I2C_SendByte(I2C_MPL3115, MPL3115_ADDR | 1); // 读操作
	I2C_WaitAck(I2C_MPL3115);
	for(uint16_t i=0; i<len; i++)
	{
	  buf[i] = I2C_RadeByte(I2C_MPL3115);
		if(i<len-1)
		  I2C_Ack(I2C_MPL3115);
	}
	
	I2C_NoAck(I2C_MPL3115);
	I2C_Stop(I2C_MPL3115);
	return true;
}

bool MPL3115_Read(MPL3115* mpl3115)
{
	MPL3115_Read_Nbyte(OUT_P_MSB_REG, mpl3115_buffer, 3); // 5 没有读取温度
	// mpl3115->pressure_raw 	 = (((uint32_t)mpl3115_buffer[0]) << 16 | ((uint32_t) mpl3115_buffer[1]) <<  8 | ((uint32_t)mpl3115_buffer[2])) / 64.0f;
	// mpl3115->temperature_raw = mpl3115_buffer[3] + (mpl3115_buffer[4] >> 4) / 16.0f;
	mpl3115->altitube_raw = ((int32_t)mpl3115_buffer[0] << 24 | (int32_t)mpl3115_buffer[1] << 16 | (int32_t)mpl3115_buffer[2] << 8) / 65536.0f;
}

/**
 * MPL3115初始化
 */
void MPL3115_Init( void )
{
	I2C_MPL3115 = I2C_2;
}

// 仅读取高度原始值，并不用于计算
void MPL3115_Read_Altitube(float dt)
{
	static TickType_t last_time;
	static TickType_t time;
	time = xTaskGetTickCount();
	if(!(MPL3115_Read_Byte(CTRL_REG1) & 0x02)) // 测量完成
	{
		mpl3115.lost = 0;
		mpl3115.health = true;
		mpl3115.present = true;
		
		MPL3115_Read(&mpl3115);
		
		mpl3115.altitube_offset = mpl3115.altitube_raw - status.position.z; // 实时计算气压计与当前位置的偏移量
		
		mpl3115.altitube = status.position.z;
		
		MPL3115_Write_Byte(CTRL_REG1, 0xBA); // 开始测量
		
		last_time = time;
	}
	else if(time - last_time > 600) // 读取超时
	{
		if(++mpl3115.lost >= 10)
		{
			mpl3115.lost = 10;
			mpl3115.health = false;
			mpl3115.present = false;
		}
		
		MPL3115_Write_Byte(CTRL_REG1, 0xBA); // 重新开始测量	
	}
}

/**
 * MPL3115更新数据
 */
void MPL3115_Update(float dt)
{
	static TickType_t last_time;
	static TickType_t time;
	
	static float last_pz = 0; // 记录上次位置，用于微分修改速度
	static float last_sz = 0;	// 记录上次速度，用于计算位置
		
	time = xTaskGetTickCount();
	
	if(!(MPL3115_Read_Byte(CTRL_REG1) & 0x02)) // 测量完成
	{
		MPL3115_Read(&mpl3115);
		
		MPL3115_Write_Byte(CTRL_REG1, 0xBA); // 开始测量
		
		mpl3115.altitube = mpl3115.altitube_raw - mpl3115.altitube_offset;
		
		last_time = time;
	}	
	else if(time - last_time > 600) // 读取超时
	{
		MPL3115_Write_Byte(CTRL_REG1, 0xBA); // 重新开始测量	
	}
	
	status.position.z += (last_sz + status.speed.z) * 0.5f * dt; // 速度积分求位置
	
	status.position.z += TWO_PI * dt * (mpl3115.altitube - status.position.z) * 0.005f; // 气压计修正位置   // 0.15f
	
	float speed = (status.position.z  - last_pz) / dt; // 速度等于高度的微分
	
	if(speed < 1) // 速度小于1米时补偿
		status.speed.z += TWO_PI * dt * ( speed - status.speed.z ) * 0.1f; // 高度变化速度修正当前速度  // 0.15f
	
	last_pz = status.position.z; // 记录上次位置
	
	height.isReady = true;
	height.source = 2;			// 数据来源气压计
}

/*********************************** 飞航科技 (END OF FILE) ******************************************/
