#include "bsp_hmc5883l.h"
#include "bsp_param.h"
#include "bsp_i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "config.h"
#include "bsp_gpio.h"
#include "bsp_mavlink.h"

HMC5883L hmc5883l = {.enabled = true};

static I2C * I2C_HMC5883L;

static uint8_t hmc5883l_buffer[6];

//extern xQueueHandle xMavlinkReceiveQueue;
//extern xQueueHandle logQueue;

/** HMC5883l单字节写入
 *   REG_Address：要写入的寄存器地址
 *   REG_data：要写入的数据
 */
bool HMC5883L_Write_Byte(uint8_t REG_Address, uint8_t REG_data)
{
	return I2C_Write_REG(I2C_HMC5883L, HMC5583l_ADDR, REG_Address, REG_data);
}

/** HMC5883L单字节读取
 *   REG_Address：要读取的寄存器地址
 */
uint8_t HMC5883L_Read_Byte(uint8_t REG_Address)
{
	return I2C_Read_REG(I2C_HMC5883L, HMC5583l_ADDR, REG_Address);
}

/** HMC5883L读取n个字节
 *   REG_Address：要读取的寄存器地址
 *   buf：读取后存储数据的buf
 *   len：要读取数据长度
 */
bool HMC5883L_Read_NByte(uint8_t REG_Address, uint8_t * buf, uint8_t len)
{
  if(!I2C_Start(I2C_HMC5883L))return false;
	I2C_SendByte(I2C_HMC5883L, HMC5583l_ADDR);  //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_HMC5883L)){I2C_Stop(I2C_HMC5883L); return false;}
	I2C_SendByte(I2C_HMC5883L, REG_Address);   
	I2C_WaitAck(I2C_HMC5883L);
	I2C_Start(I2C_HMC5883L);
	I2C_SendByte(I2C_HMC5883L, HMC5583l_ADDR | 1); // 读操作
	I2C_WaitAck(I2C_HMC5883L);
	for(uint16_t i=0; i<len; i++)
	{
	  buf[i] = I2C_RadeByte(I2C_HMC5883L);
		I2C_Ack(I2C_HMC5883L);
	}
	I2C_NoAck(I2C_HMC5883L);
	I2C_Stop(I2C_HMC5883L);
	return true;
}

/**
 * 读取HMC5883L原始数据
 */
bool HMC5883L_Read(Vector3i* mag)
{
	HMC5883L_Read_NByte(HMC58X3_R_XM, hmc5883l_buffer, 6);
	mag->x = hmc5883l_buffer[0] << 8 | hmc5883l_buffer[1];
	mag->y = hmc5883l_buffer[4] << 8 | hmc5883l_buffer[5];
	mag->z = hmc5883l_buffer[2] << 8 | hmc5883l_buffer[3];

	// 5983读取温度
	// int16_t temp16 = (HMC5883L_Read_Byte(HMC58X3_R_XM) << 8 | HMC5883L_Read_Byte(HMC58X3_R_XL)); // 5983读取温度
	// float temperature = 25 + (temp16 / (16 * 8.0f));
	return (!(mag->x==0 && mag->y==0 && mag->z==0) && (mag->x<2047 && mag->y<2047 && mag->z<2047) && ( mag->x>-2048 && mag->y>-2048 && mag->z>-2048));
}

// 初始化HMC5883L，HMC5883L输出最高频率为75Hz，调用周期应大于13.33ms
void HMC5883L_Init(void)
{
	static bool self_test = DEFAULT_HMC5883L_SELF_TEST;
	
	hmc5883l.present = true;
	hmc5883l.health = true;	
	
	I2C_HMC5883L = I2C_1;
	
	/* 自测 */
	if(self_test)
	{
		self_test = false;
	  HMC5883L_Self_Test();
	}
	else
	{
		hmc5883l.health &= Param_TakeMagScale(&hmc5883l.mag_scale);
	}
	
	/* 初始化 */
	uint8_t data, count=0;
	
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		HMC5883L_Write_Byte(HMC58X3_R_CONFA, 0x78); // 采样平均数为8，输出速率75Hz，正常测量模式
		vTaskDelay(5/portTICK_RATE_MS);
		data = HMC5883L_Read_Byte(HMC58X3_R_CONFA);
	} while(data != 0x78 && count++ < 200);
	
	vTaskDelay(100/portTICK_RATE_MS);
	
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		HMC5883L_Write_Byte(HMC58X3_R_CONFB, 0x20); // 周围磁场强度+-1.3Ga，高斯增益1090（地磁场强度大约0.5~0.6高斯）
		vTaskDelay(5/portTICK_RATE_MS);
		data = HMC5883L_Read_Byte(HMC58X3_R_CONFB); 
	} while(data != 0x20 && count++ < 200);
	
	vTaskDelay(100/portTICK_RATE_MS);
	
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		HMC5883L_Write_Byte(HMC58X3_R_MODE, 0x00); // 连续测量模式
		vTaskDelay(5/portTICK_RATE_MS);
		data = HMC5883L_Read_Byte(HMC58X3_R_MODE);
	} while(data != 0x00 && count++ < 200);
	
	if(count >= 200) // 硬件已损坏
	{
		hmc5883l.present = false;
		hmc5883l.health = false;	
	}
	else
	{
		vTaskDelay(500/portTICK_RATE_MS); // 等待初始化完成
		
		// discard 100 samples to let the sensor settle
		for(uint8_t i=0; i<100; i++)
		{
			if(HMC5883L_Read_Byte(HMC58X3_R_STATUS) & 0x1)
			{
				HMC5883L_Read(&hmc5883l.mag_raw);  
			}
			vTaskDelay(10 / portTICK_RATE_MS);
		}
		
		// xQueueSendToBack(logQueue, "HMC5883L Init Succeed!!!\r\n", LOG_WAIT);
		
		/* 初始化校准 */
		if(DEFAULT_HMC5883L_CALIBRATE)
		{
			HMC5883L_Calibration();
		}
		else 
		{
			hmc5883l.health &= Param_TakeMagOffset(&hmc5883l.mag_offset);
			// xQueueSendToBack(logQueue, "hmc5883l read param success . \r\n", LOG_WAIT);
		}
	}
	hmc5883l.calibrating = false;
}

// 磁力计校准
void HMC5883L_Calibration(void)
{
	if(!hmc5883l.calibrating) 
	{
		LED4_ON;
		hmc5883l.health = false;
		hmc5883l.calibrating = true;		
		
		hmc5883l.mag_max.x = hmc5883l.mag_max.y =	hmc5883l.mag_max.z = hmc5883l.mag_min.x = hmc5883l.mag_min.y = hmc5883l.mag_min.z = 0;

 		// xQueueSendToBack(logQueue, "HMC5883L  calibrating...\r\n", LOG_WAIT);
		vTaskDelay(5/portTICK_RATE_MS);
		
		for(int count=0; count<CALIBRATING_MAG_CYCLES; count++) 
		{	
			if(HMC5883L_Read_Byte(HMC58X3_R_STATUS) & 0x1) // 查看数据是否已准备好
	    {
				if(HMC5883L_Read(&hmc5883l.mag_raw))
				{
					if(count == 0)
					{
						hmc5883l.mag_max.x = hmc5883l.mag_min.x = hmc5883l.mag_raw.x;
						hmc5883l.mag_max.y = hmc5883l.mag_min.y = hmc5883l.mag_raw.y;
						hmc5883l.mag_max.z = hmc5883l.mag_min.z = hmc5883l.mag_raw.z;
					}
					else
					{
						hmc5883l.mag_max.x = MAX(hmc5883l.mag_raw.x, hmc5883l.mag_max.x);
						hmc5883l.mag_max.y = MAX(hmc5883l.mag_raw.y, hmc5883l.mag_max.y);
						hmc5883l.mag_max.z = MAX(hmc5883l.mag_raw.z, hmc5883l.mag_max.z);
						hmc5883l.mag_min.x = MIN(hmc5883l.mag_raw.x, hmc5883l.mag_min.x);
						hmc5883l.mag_min.y = MIN(hmc5883l.mag_raw.y, hmc5883l.mag_min.y);
						hmc5883l.mag_min.z = MIN(hmc5883l.mag_raw.z, hmc5883l.mag_min.z);		
						hmc5883l.mag.x = (hmc5883l.mag_max.x - hmc5883l.mag_min.x) * 0.5f * hmc5883l.mag_scale.x;
						hmc5883l.mag.y = (hmc5883l.mag_max.y - hmc5883l.mag_min.y) * 0.5f * hmc5883l.mag_scale.y;
						hmc5883l.mag.z = (hmc5883l.mag_max.z - hmc5883l.mag_min.z) * 0.5f * hmc5883l.mag_scale.z;
					}
				}
			}
			vTaskDelay(20 / portTICK_RATE_MS);
		}
		
		hmc5883l.mag_offset.x = (int16_t)((hmc5883l.mag_max.x + hmc5883l.mag_min.x) * 0.5f);
		hmc5883l.mag_offset.y = (int16_t)((hmc5883l.mag_max.y + hmc5883l.mag_min.y) * 0.5f);
		hmc5883l.mag_offset.z = (int16_t)((hmc5883l.mag_max.z + hmc5883l.mag_min.z) * 0.5f);
		
		if ((-FLT_EPSILON < hmc5883l.mag_offset.x && hmc5883l.mag_offset.x < FLT_EPSILON ) && \
                   (-FLT_EPSILON < hmc5883l.mag_offset.y && hmc5883l.mag_offset.y < FLT_EPSILON ) && \
		               (-FLT_EPSILON < hmc5883l.mag_offset.z && hmc5883l.mag_offset.z < FLT_EPSILON ))
		{
			hmc5883l.mag_offset.x = 0;
			hmc5883l.mag_offset.y = 0;
			hmc5883l.mag_offset.z = 0;
			Param_SaveMagOffset(&hmc5883l.mag_offset);			//保存数据
			hmc5883l.health = false;		
			// xQueueSendToBack(logQueue, "HMC5883L calibration failed. \r\n", LOG_WAIT);
		}
	  else 
		{
		  Param_SaveMagOffset(&hmc5883l.mag_offset);			//保存数据
			hmc5883l.health = true;		
			// xQueueSendToBack(logQueue, "HMC5883L  calibration success .\r\n", LOG_WAIT);
		}	
		hmc5883l.calibrating = false;
		LED4_OFF;
	}
}

// 更新磁力计信息
void HMC5883L_Update(float dt)
{	
	if(!hmc5883l.calibrating && HMC5883L_Read_Byte(HMC58X3_R_STATUS) & 0x1) // 不是在校准状态，并且数据已准备好
	{
		if(HMC5883L_Read(&hmc5883l.mag_raw))
		{
			hmc5883l.mag.x = (hmc5883l.mag_raw.x - hmc5883l.mag_offset.x) * hmc5883l.mag_scale.x; // 磁力计x轴
			hmc5883l.mag.y = (hmc5883l.mag_raw.y - hmc5883l.mag_offset.y) * hmc5883l.mag_scale.y; // 磁力计y轴
			hmc5883l.mag.z = (hmc5883l.mag_raw.z - hmc5883l.mag_offset.z) * hmc5883l.mag_scale.z; // 磁力计z轴
		}		
  } 	
}

// 地磁计自检 Self Test Response= Gyroscope Output with Self Test Enabled  - Gyroscope Output with Self Test Disabled
void HMC5883L_Self_Test()
{
	// 比例系数=期望值（660*1.16）/该增益下磁场实际值
	/* expected axis scaling. The datasheet says that 766 will
	 * be places in the X and Y axes and 713 in the Z
	 * axis. Experiments show that in fact 766 is placed in X,
	 * and 713 in Y and Z. This is relative to a base of 660
	 * LSM/Ga, giving 1.16 and 1.08 */
	const Vector3f expected_cal = { 1.16f, 1.08f, 1.16f };
	
	bool init_flag = true;  
	uint8_t good_count = 0;
	Vector3f sum_excited = {0};
	Vector3i raw = {0};

	hmc5883l.calibrating = true;
	hmc5883l.health = false;
	
	// xQueueSendToBack(logQueue, "HMC5883L Self Test Start!!! \r\n", LOG_WAIT);
	
		/* 初始化 */
	uint8_t data, count=0;
	
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		HMC5883L_Write_Byte(HMC58X3_R_CONFA, 0x71);  // 采样平均数为8，输出速率15Hz，正偏置测量模式
		vTaskDelay(5/portTICK_RATE_MS);
		data = HMC5883L_Read_Byte(HMC58X3_R_CONFA);
	} while(data != 0x71 && count++ < 200);
	
	vTaskDelay(100/portTICK_RATE_MS);
	
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		HMC5883L_Write_Byte(HMC58X3_R_CONFB, 0x60);	// 周围磁场强度+-2.5Ga，高斯增益660（地磁场强度大约0.5~0.6高斯）
		vTaskDelay(5/portTICK_RATE_MS);
		data = HMC5883L_Read_Byte(HMC58X3_R_CONFB);
	} while(data != 0x60 && count++ < 200);
	
	vTaskDelay(100/portTICK_RATE_MS);
	
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		HMC5883L_Write_Byte(HMC58X3_R_MODE, 0x00); // 连续测量模式
		vTaskDelay(5/portTICK_RATE_MS);
		data = HMC5883L_Read_Byte(HMC58X3_R_MODE);
	} while(data != 0x00 && count++ < 200);
	
	if(count >= 200) // 硬件已损坏
	{
		hmc5883l.present = false;
	}
	else
	{
		vTaskDelay(500/portTICK_RATE_MS); // 等待初始化完成
		
		// discard 10 samples to let the sensor settle
		for(uint8_t i=0; i<10; i++)
		{
			if(HMC5883L_Read_Byte(HMC58X3_R_STATUS) & 0x1)
			{
				HMC5883L_Read(&raw);  
			}
			vTaskDelay(70 / portTICK_RATE_MS);
		}
		
		/* read the sensor up to 150x, stopping when we have 50 good values */
		for (uint8_t i = 0; i < 150 && good_count < 50; i++) {
			if(HMC5883L_Read_Byte(HMC58X3_R_STATUS) & 0x1)
			{
				if(HMC5883L_Read(&raw))
				{
					Vector3f cal = {fabsf(expected_cal.x * 660 / raw.x), fabsf(expected_cal.y * 660 / raw.y), fabsf(expected_cal.z * 660 / raw.z)};

					if (cal.x > 0.7f && cal.x < 1.35f && cal.y > 0.7f && cal.y < 1.35f && cal.z > 0.7f && cal.z < 1.35f) 
					{
						good_count++;
						sum_excited.x += cal.x;
						sum_excited.y += cal.y;
						sum_excited.z += cal.z;
					}
				}
			}
			// xQueueSendToBack(logQueue, "*", LOG_WAIT);
			vTaskDelay(70 / portTICK_RATE_MS);
		}
		
		if(good_count > 5)
		{
			/* set scaling in device */
			hmc5883l.mag_scale.x = sum_excited.x / good_count;
			hmc5883l.mag_scale.y = sum_excited.y / good_count;
			hmc5883l.mag_scale.z = sum_excited.z / good_count;
			
			if ((-FLT_EPSILON + 1.0f < hmc5883l.mag_scale.x && hmc5883l.mag_scale.x < FLT_EPSILON + 1.0f) &&
				(-FLT_EPSILON + 1.0f < hmc5883l.mag_scale.y && hmc5883l.mag_scale.y < FLT_EPSILON + 1.0f) &&
				(-FLT_EPSILON + 1.0f < hmc5883l.mag_scale.z && hmc5883l.mag_scale.z < FLT_EPSILON + 1.0f)) 
			{
				// xQueueSendToBack(logQueue, "HMC5883L scale calculate failed. \r\n", LOG_WAIT);
				hmc5883l.mag_scale.x = 1;
				hmc5883l.mag_scale.y = 1;
				hmc5883l.mag_scale.z = 1;
				Param_SaveMagScale(&hmc5883l.mag_scale);
				vTaskDelay(5/portTICK_RATE_MS);
			}
			else 
			{
				// xQueueSendToBack(logQueue, "HMC5883L scale calculate success. \r\n", LOG_WAIT);
				Param_SaveMagScale(&hmc5883l.mag_scale);
				vTaskDelay(5/portTICK_RATE_MS);
			}			
		}
		else
		{
			// xQueueSendToBack(logQueue, "HMC5883L scale calculate failed. \r\n", LOG_WAIT);
			vTaskDelay(5/portTICK_RATE_MS);
		}
	}
}
/*********************************** 飞航科技 (END OF FILE) ******************************************/
