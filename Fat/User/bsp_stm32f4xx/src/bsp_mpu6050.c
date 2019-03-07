#include "FreeRTOS.h"
#include "task.h"
#include "bsp_mpu6050.h"
#include "bsp_gpio.h"
#include "bsp_i2c.h"
#include "bsp_param.h"
#include "config.h"
#include "filter.h"

MPU6050 mpu6050 = {.enabled = true, .present = true, .health = true};

static I2C * I2C_MPU6050;

static uint8_t mpu6050_buffer[14];

static biquadFilter_t gyroFilterLPF[3];
static biquadFilter_t gyroFilterNotch_1[3];
static biquadFilter_t gyroFilterNotch_2[3];

static biquadFilter_t accFilterLPF[3];
static biquadFilter_t accFilterNotch[3];

#define gyro_lpf_hz 							70
#define gyro_notch_hz_1  					170
#define gyro_notch_cutoff_1 			125
#define gyro_notch_hz_2  					85
#define gyro_notch_cutoff_2 			43

#define acc_lpf_hz 								15
#define acc_notch_hz							0
#define acc_notch_cutoff					1

//extern xQueueHandle logQueue;

/** MPU6050单字节写入
 *   REG_Address：要写入的寄存器地址
 *   REG_data：要写入的数据
 */
bool MPU6050_Write_Byte(uint8_t REG_Address, uint8_t REG_data)
{
	return I2C_Write_REG(I2C_MPU6050, MPU6050_ADDR, REG_Address, REG_data);
}

/**
 * MPU6050单字节读取
 *   REG_Address：要读取的寄存器地址
 */
uint8_t MPU6050_Read_Byte(uint8_t REG_Address)
{
	return I2C_Read_REG(I2C_MPU6050, MPU6050_ADDR, REG_Address);
}

/**
 * MPU6050读取n个字节
 *   REG_Address：要读取的寄存器地址
 *   buf：读取后存储数据的buf
 *   len：要读取数据长度
 */
bool MPU6050_Read_NByte(uint8_t REG_Address, uint8_t* buf, uint8_t len)
{
  if(!I2C_Start(I2C_MPU6050))return false;
	I2C_SendByte(I2C_MPU6050, MPU6050_ADDR);  //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_MPU6050)){I2C_Stop(I2C_MPU6050); return false;}
	I2C_SendByte(I2C_MPU6050, REG_Address);   
	I2C_WaitAck(I2C_MPU6050);
	I2C_Start(I2C_MPU6050);
	I2C_SendByte(I2C_MPU6050, MPU6050_ADDR | 1); // 读操作
	I2C_WaitAck(I2C_MPU6050);
	for(uint16_t i=0; i<len; i++)
	{
	  buf[i] = I2C_RadeByte(I2C_MPU6050);
		if(i<len-1)
		  I2C_Ack(I2C_MPU6050);
	}
	
	I2C_NoAck(I2C_MPU6050);
	I2C_Stop(I2C_MPU6050);
	return true;
}

/**
 * 读取MPU6050原始数据
 */
bool MPU6050_Read(Vector3i* acc, Vector3i* gyro)
{
  MPU6050_Read_NByte(MPU6050_RA_ACCEL_XOUT_H, mpu6050_buffer, 14);
	if(mpu6050_buffer[0] | mpu6050_buffer[1] | mpu6050_buffer[2] | mpu6050_buffer[3] | mpu6050_buffer[4] | mpu6050_buffer[5])
	{
		acc->x  =  (int16_t)(mpu6050_buffer[0]  << 8 | mpu6050_buffer[1]);
		acc->y  =  (int16_t)(mpu6050_buffer[2]  << 8 | mpu6050_buffer[3]);	
		acc->z  =  (int16_t)(mpu6050_buffer[4]  << 8 | mpu6050_buffer[5]);
		gyro->x =  (int16_t)(mpu6050_buffer[8]  << 8 | mpu6050_buffer[9]);
		gyro->y	=	 (int16_t)(mpu6050_buffer[10] << 8 | mpu6050_buffer[11]);
		gyro->z =  (int16_t)(mpu6050_buffer[12] << 8 | mpu6050_buffer[13]);
		return true;
	} 
	else
	{
		// xQueueSendToBack(logQueue, "MPU6050 Read Data Failed!!!\r\n", LOG_WAIT); // all zero data - probably a IIC bus error
		return false;
	}	
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_initialize(void)
*功　　能:	  初始化 	MPU6050 以进入可用状态。
*******************************************************************************/

void MPU6050_Init()
{
	static bool calibration = DEFAULT_MPU6050_CALIBRATE;
	
	I2C_MPU6050 = I2C_1;
	
	vTaskDelay(10/portTICK_RATE_MS);
	
	uint8_t revision;
	uint8_t buffer[6] = {0};
	
	MPU6050_Read_NByte(MPU6050_RA_XA_OFFS_H, buffer, 6);
	revision = ((buffer[5] & 0x01) << 2) | ((buffer[3] & 0x01) << 1) | (buffer[1] & 0x01);
	if (revision) {
		/* Congrats, these parts are better. */
		if (revision == 1) {
			mpu6050.resolution = MPU6050_ACCEL_LSB * 0.5f;
		} else if (revision == 2) {
			mpu6050.resolution = MPU6050_ACCEL_LSB;
		} else if ((revision == 3) || (revision == 7)) {
			mpu6050.resolution = MPU6050_ACCEL_LSB;
		} else {
			mpu6050.health = false; // MPU6050异常
		}
	} else {
		uint8_t productId = MPU6050_Read_Byte(MPU6050_RA_PRODUCT_ID);
		revision = productId & 0x0F;
		if (!revision) {
			mpu6050.health = false; // MPU6050异常
		} else if (revision == 4) {
			mpu6050.resolution = MPU6050_ACCEL_LSB * 0.5f;
		} else {
			mpu6050.resolution = MPU6050_ACCEL_LSB;
		}
	}	
		
	/* 初始化 */
	uint8_t data, count=0;
	
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		MPU6050_Write_Byte(MPU6050_RA_PWR_MGMT_1, 0x80); // 复位设备
		vTaskDelay(5/portTICK_RATE_MS);
		data = MPU6050_Read_Byte(MPU6050_RA_PWR_MGMT_1);
	} while(data != MPU6050_PWR1_SLEEP && count++ < 200); // 复位后处于sleep模式
	
	vTaskDelay(100/portTICK_RATE_MS);
	
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		MPU6050_Write_Byte(MPU6050_RA_PWR_MGMT_1, MPU6050_CLOCK_PLL_ZGYRO); // 关闭睡眠模式，使用Z轴陀螺仪作为参考时钟
		vTaskDelay(5/portTICK_RATE_MS);
		data = MPU6050_Read_Byte(MPU6050_RA_PWR_MGMT_1);
	} while(data != MPU6050_CLOCK_PLL_ZGYRO && count++ < 200);
	
	vTaskDelay(5/portTICK_RATE_MS);
	
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		// 采样频率=陀螺仪输出频率/(1+SMPLRT_DIV)  当DLPF is disabled(DLPF_CFG=0 or 7),陀螺仪输出频率=8kHz; 当DLPF is enabled(寄存器26)，陀螺仪输出频率=1kHz
		MPU6050_Write_Byte(MPU6050_RA_SMPLRT_DIV, 1000/(1000) - 1); // 此处DLPF is enabled，陀螺仪输出频率=1kHz，不进行分频，直接使用1kHz。
		vTaskDelay(5/portTICK_RATE_MS);
		data = MPU6050_Read_Byte(MPU6050_RA_SMPLRT_DIV);
	} while(data != 1000/(1000) - 1 && count++ < 200);
	
	vTaskDelay(15/portTICK_RATE_MS);
	
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		MPU6050_Write_Byte(MPU6050_RA_CONFIG, MPU6050_DLPF_BW_42); // 设置低通滤波频率，当滤波频率超过90Hz时无明显滤波效果
		vTaskDelay(5/portTICK_RATE_MS);
		data = MPU6050_Read_Byte(MPU6050_RA_CONFIG);
	} while(data != MPU6050_DLPF_BW_42 && count++ < 200);

	vTaskDelay(5/portTICK_RATE_MS);
	
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		MPU6050_Write_Byte(MPU6050_RA_GYRO_CONFIG, MPU6050_GYRO_FS); // 陀螺仪最大量程 +-2000度每秒
		vTaskDelay(5/portTICK_RATE_MS);
		data = MPU6050_Read_Byte(MPU6050_RA_GYRO_CONFIG);
	} while(data != MPU6050_GYRO_FS && count++ < 200);
	
	vTaskDelay(5/portTICK_RATE_MS);
		
	do
	{
		vTaskDelay(5/portTICK_RATE_MS);
		MPU6050_Write_Byte(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACCEL_AFS); // 加速度度最大量程 +-8G;
		vTaskDelay(5/portTICK_RATE_MS);
		data = MPU6050_Read_Byte(MPU6050_RA_ACCEL_CONFIG);
	} while(data != MPU6050_ACCEL_AFS && count++ < 200);
	
	if(count >= 200) // 硬件已损坏
	{
		mpu6050.present = false;
		mpu6050.health = false;
	}
	else
	{
		vTaskDelay(100/portTICK_RATE_MS);
		
		/* 初始化校准 */
		if(calibration)
		{
			calibration = false;
			MPU6050_Calibration();
		}
		else 
		{
			mpu6050.health &= Param_TakeAccelOffset(&mpu6050.acc_offset);
			mpu6050.health &= Param_TakeGyroOffset(&mpu6050.gyro_offset);
			mpu6050.acc_scale.x = 1.0;
			mpu6050.acc_scale.y = 1.0;
			mpu6050.acc_scale.z = 1.0;
			mpu6050.gyro_scale.x = 1.0;
			mpu6050.gyro_scale.y = 1.0;
			mpu6050.gyro_scale.z = 1.0;
			// xQueueSendToBack(logQueue, "mpu6050 read param success . \r\n", LOG_WAIT);
		}
	}
	MPU6050_Filter_Init();
	
	mpu6050.calibrating = false;
}

/**************************实现函数********************************************
*函数原型:	void MPU6050_calibration()
*功　　能:	校准加速度计和陀螺仪
*******************************************************************************/
void MPU6050_Calibration(void)
{
	if(!mpu6050.calibrating) 
	{
		mpu6050.calibrating = true;
		mpu6050.health = false;
		                                    
		bool acc_converged = false;         // 加速度聚合状态
		bool gyro_converged = false;        // 角速度聚合状态
		Vector3f acc_best_avg = {0};        // 50次测量中最优加速度
		Vector3f gyro_best_avg = {0};       // 50次测量中最优角速度
		Vector3f new_acc_offset = {0};      // 加速度最终需要修正的偏移量
		Vector3f new_gyro_offset = {0};     // 角速度最终需要修正的偏移量
	
    Vector3f acc_last_average = {0};    // 上50次测量的角速度平均值
		Vector3f gyro_last_average = {0};   // 上50次测量的角速度平均值
		Vector3l acc_raw_sum = {0};         // 加速度原始值累计和
		Vector3l gyro_raw_sum = {0};        // 角速度原始值累积和
		Vector3i acc_start = {0};           // 每50次数据读取前加速度状态
		Vector3i gyro_start = {0};          // 每50次数据读取前角速度状态
		Vector3f acc_avg = {0};             // 50次累计加速度均值
		Vector3f gyro_avg = {0};            // 50次累计角速度均值
                                        
		uint8_t sum_cnt=0;                  // 50次的累加计数器
		float acc_best_diff = 0;            // 加速度最优方差
		float gyro_best_diff = 0;           // 角速度最优方差
		float acc_diff_norm = 0;            // 加速度方差
		float gyro_diff_norm = 0;           // 角速度方差
		
    // xQueueSendToBack(logQueue, "MPU6050  calibrating...\r\n", LOG_WAIT);
		vTaskDelay(5/portTICK_RATE_MS);
		
		for(int count =0; count<MPU6050_CALIB_CYCLES; count++) 
		{	
			/* 累计50次坐标和 */
			acc_raw_sum.x = acc_raw_sum.y = acc_raw_sum.z = gyro_raw_sum.x = gyro_raw_sum.x = gyro_raw_sum.y = gyro_raw_sum.z = 0;
			/* 记录起始坐标 */
			do {
	      // xQueueSendToBack(logQueue, "*", LOG_WAIT);
				vTaskDelay(5/portTICK_RATE_MS);
			} while(!MPU6050_Read(&acc_start,&gyro_start));
			for(sum_cnt=0; sum_cnt<50; sum_cnt++)
			{
				if(MPU6050_Read(&mpu6050.acc_raw, &mpu6050.gyro_raw))
				{
					acc_raw_sum.x += mpu6050.acc_raw.x;
					acc_raw_sum.y += mpu6050.acc_raw.y;
					acc_raw_sum.z += mpu6050.acc_raw.z;
					gyro_raw_sum.x += mpu6050.gyro_raw.x;
					gyro_raw_sum.y += mpu6050.gyro_raw.y;
					gyro_raw_sum.z += mpu6050.gyro_raw.z;
				}
				vTaskDelay(5/portTICK_RATE_MS);
			}

			if(sqrtf3(mpu6050.acc_raw.x-acc_start.x, mpu6050.acc_raw.y-acc_start.y, mpu6050.acc_raw.z-acc_start.z) > (240 / MPU6500_MAX_ACCEL_G)) continue; // 采样50次后允许加速度方差最大值（实际测量值）
			
			acc_avg.x = acc_raw_sum.x / (float)sum_cnt;
			acc_avg.y = acc_raw_sum.y / (float)sum_cnt;
			acc_avg.z = acc_raw_sum.z / (float)sum_cnt;
			gyro_avg.x = gyro_raw_sum.x / (float)sum_cnt;
			gyro_avg.y = gyro_raw_sum.y / (float)sum_cnt;
			gyro_avg.z = gyro_raw_sum.z / (float)sum_cnt;
			
   		acc_diff_norm = sqrtf3((acc_last_average.x - acc_avg.x), (acc_last_average.y - acc_avg.y),( acc_last_average.z - acc_avg.z));
			gyro_diff_norm = sqrtf3((gyro_last_average.x - gyro_avg.x), (gyro_last_average.y - gyro_avg.y), (gyro_last_average.z - gyro_avg.z));
					
			if (count == 0) 
			{
				acc_best_diff = acc_diff_norm;
				acc_best_avg.x = acc_avg.x;
				acc_best_avg.y = acc_avg.y;
				acc_best_avg.z = acc_avg.z;
				gyro_best_diff = gyro_diff_norm;
				gyro_best_avg.x = gyro_avg.x;
				gyro_best_avg.y = gyro_avg.y;
				gyro_best_avg.z = gyro_avg.z;
			} 
			else 
			{
				if (acc_diff_norm < 1)  // 加速度方差
				{
					acc_last_average.x = (acc_avg.x * 0.5f) + (acc_last_average.x * 0.5f);
					acc_last_average.y = (acc_avg.y * 0.5f) + (acc_last_average.y * 0.5f);
					acc_last_average.z = (acc_avg.z * 0.5f) + (acc_last_average.z * 0.5f);
					if(!acc_converged || (sqrtf3(acc_last_average.x,acc_last_average.y,acc_last_average.z) < sqrtf3(new_acc_offset.x,new_acc_offset.y,new_acc_offset.z))) 
					{
						acc_converged = true;
						new_acc_offset.x = acc_last_average.x;
						new_acc_offset.y = acc_last_average.y;
						new_acc_offset.z = acc_last_average.z;
					}
				}
				else if (acc_diff_norm < acc_best_diff)
				{
					acc_best_diff = acc_diff_norm;
					acc_best_avg.x = (acc_avg.x * 0.5f) + (acc_last_average.x * 0.5f);
					acc_best_avg.y = (acc_avg.y * 0.5f) + (acc_last_average.y * 0.5f);
					acc_best_avg.z = (acc_avg.z * 0.5f) + (acc_last_average.z * 0.5f);
				}
				acc_last_average.x = acc_avg.x;
				acc_last_average.y = acc_avg.y;
				acc_last_average.z = acc_avg.z;
				
				if (gyro_diff_norm < 0.1)  // 角速度方差
				{
					gyro_last_average.x = (gyro_avg.x * 0.5f) + (gyro_last_average.x * 0.5f);
					gyro_last_average.y = (gyro_avg.y * 0.5f) + (gyro_last_average.y * 0.5f);
					gyro_last_average.z = (gyro_avg.z * 0.5f) + (gyro_last_average.z * 0.5f);
					if(!gyro_converged || (sqrtf3(gyro_last_average.x,gyro_last_average.y,gyro_last_average.z) < sqrtf3(new_gyro_offset.x,new_gyro_offset.y,new_gyro_offset.z))) 
					{
						gyro_converged = true;
						new_gyro_offset.x = gyro_last_average.x;
						new_gyro_offset.y = gyro_last_average.y;
						new_gyro_offset.z = gyro_last_average.z;
					}
				}
				else if (gyro_diff_norm < gyro_best_diff)
				{
					gyro_best_diff = gyro_diff_norm;
					gyro_best_avg.x = (gyro_avg.x * 0.5f) + (gyro_last_average.x * 0.5f);
					gyro_best_avg.y = (gyro_avg.y * 0.5f) + (gyro_last_average.y * 0.5f);
					gyro_best_avg.z = (gyro_avg.z * 0.5f) + (gyro_last_average.z * 0.5f);
				}
				gyro_last_average.x = gyro_avg.x;
				gyro_last_average.y = gyro_avg.y;
				gyro_last_average.z = gyro_avg.z;
			}
		}
		
		if (!acc_converged) {
			// xQueueSendToBack(logQueue, "acc did not converge\r\n", LOG_WAIT);
			mpu6050.acc_offset.x = acc_best_avg.x;
			mpu6050.acc_offset.y = acc_best_avg.y;
			mpu6050.acc_offset.z = acc_best_avg.z - mpu6050.resolution;
		} else {
			// xQueueSendToBack(logQueue, "acc calibrate success\r\n", LOG_WAIT);
			mpu6050.acc_offset.x = new_acc_offset.x;
			mpu6050.acc_offset.y = new_acc_offset.y;
			mpu6050.acc_offset.z = new_acc_offset.z - mpu6050.resolution;
		}
		if (!gyro_converged) {
			// xQueueSendToBack(logQueue, "gyro did not converge\r\n", LOG_WAIT);
			mpu6050.gyro_offset.x = gyro_best_avg.x;
			mpu6050.gyro_offset.y = gyro_best_avg.y;
			mpu6050.gyro_offset.z = gyro_best_avg.z;				
		} else {
			// xQueueSendToBack(logQueue, "gyro calibrate success\r\n", LOG_WAIT);
			mpu6050.gyro_offset.x = new_gyro_offset.x;
			mpu6050.gyro_offset.y = new_gyro_offset.y;
			mpu6050.gyro_offset.z = new_gyro_offset.z;		
		}
		Param_SaveAccelOffset(&mpu6050.acc_offset);
		Param_SaveGyroOffset(&mpu6050.gyro_offset);
		mpu6050.calibrating = false;
		mpu6050.health = true;
	}
	LED1_ON;
}

void MPU6050_Filter_Init(void)
{
	// 二阶低通滤波
	biquadFilterInitLPF(&gyroFilterLPF[0], gyro_lpf_hz, 2 / portTICK_RATE_MS * MS2S);
	biquadFilterInitLPF(&gyroFilterLPF[1], gyro_lpf_hz, 2 / portTICK_RATE_MS * MS2S);
	biquadFilterInitLPF(&gyroFilterLPF[2], gyro_lpf_hz, 2 / portTICK_RATE_MS * MS2S);
	
	// 二阶陷波滤波
	biquadFilterInitNotch(&gyroFilterNotch_1[0], 2 / portTICK_RATE_MS * MS2S, gyro_notch_hz_1, gyro_notch_cutoff_1);
	biquadFilterInitNotch(&gyroFilterNotch_1[1], 2 / portTICK_RATE_MS * MS2S, gyro_notch_hz_1, gyro_notch_cutoff_1);
	biquadFilterInitNotch(&gyroFilterNotch_1[2], 2 / portTICK_RATE_MS * MS2S, gyro_notch_hz_1, gyro_notch_cutoff_1);
	
	// 二阶陷波滤波
	biquadFilterInitNotch(&gyroFilterNotch_2[0], 2 / portTICK_RATE_MS * MS2S, gyro_notch_hz_2, gyro_notch_cutoff_2);
	biquadFilterInitNotch(&gyroFilterNotch_2[1], 2 / portTICK_RATE_MS * MS2S, gyro_notch_hz_2, gyro_notch_cutoff_2);
	biquadFilterInitNotch(&gyroFilterNotch_2[2], 2 / portTICK_RATE_MS * MS2S, gyro_notch_hz_2, gyro_notch_cutoff_2);
	
	biquadFilterInitLPF(&accFilterLPF[0], acc_lpf_hz, 2 / portTICK_RATE_MS * MS2S);
	biquadFilterInitLPF(&accFilterLPF[1], acc_lpf_hz, 2 / portTICK_RATE_MS * MS2S);
	biquadFilterInitLPF(&accFilterLPF[2], acc_lpf_hz, 2 / portTICK_RATE_MS * MS2S);
	
	biquadFilterInitNotch(&accFilterNotch[0], 2 / portTICK_RATE_MS * MS2S, acc_notch_hz, acc_notch_cutoff);
	biquadFilterInitNotch(&accFilterNotch[1], 2 / portTICK_RATE_MS * MS2S, acc_notch_hz, acc_notch_cutoff);
	biquadFilterInitNotch(&accFilterNotch[2], 2 / portTICK_RATE_MS * MS2S, acc_notch_hz, acc_notch_cutoff);
}

void MPU6050_Update(float dt)
{
	static Vector3i mpu6050_acc_fifo[FILTER_NUM], mpu6050_gyro_fifo[FILTER_NUM];
	static uint8_t index;
	
	if(!mpu6050.calibrating) 
	{
		if(MPU6050_Read(&mpu6050.acc_raw, &mpu6050.gyro_raw))
		{
			// 滑动滤波
			mpu6050_acc_fifo[index].x = mpu6050.acc_raw.x - mpu6050.acc_offset.x;
			mpu6050_acc_fifo[index].y = mpu6050.acc_raw.y - mpu6050.acc_offset.y;
			mpu6050_acc_fifo[index].z = mpu6050.acc_raw.z - mpu6050.acc_offset.z;			
			mpu6050_gyro_fifo[index].x = mpu6050.gyro_raw.x - mpu6050.gyro_offset.x;
			mpu6050_gyro_fifo[index].y = mpu6050.gyro_raw.y - mpu6050.gyro_offset.y;
			mpu6050_gyro_fifo[index].z = mpu6050.gyro_raw.z - mpu6050.gyro_offset.z;
			
			if(++index == FILTER_NUM)
				index = 0;
			
			Vector3l acc_sum = {0}, gyro_sum = {0};
			for(uint8_t i=0; i<FILTER_NUM; i++)
			{
				acc_sum.x += mpu6050_acc_fifo[i].x;
				acc_sum.y += mpu6050_acc_fifo[i].y;
				acc_sum.z += mpu6050_acc_fifo[i].z;
				gyro_sum.x += mpu6050_gyro_fifo[i].x;
				gyro_sum.y += mpu6050_gyro_fifo[i].y;
				gyro_sum.z += mpu6050_gyro_fifo[i].z;
			}
			
			mpu6050.acc.x += TWO_PI * dt * (acc_sum.x / (float)FILTER_NUM / mpu6050.resolution - mpu6050.acc.x) * 50; // 低通延时20ms
			mpu6050.acc.y += TWO_PI * dt * (acc_sum.y / (float)FILTER_NUM / mpu6050.resolution - mpu6050.acc.y) * 50;
			mpu6050.acc.z += TWO_PI * dt * (acc_sum.z / (float)FILTER_NUM / mpu6050.resolution - mpu6050.acc.z) * 50;
			
			mpu6050.gyro.x = gyro_sum.x / (float)FILTER_NUM / MPU6050_GYRO_LSB * DEG2RAD;
			mpu6050.gyro.y = gyro_sum.y / (float)FILTER_NUM / MPU6050_GYRO_LSB * DEG2RAD;
			mpu6050.gyro.z = gyro_sum.z / (float)FILTER_NUM / MPU6050_GYRO_LSB * DEG2RAD;
			
			#if 0
			// 角速度二阶低通滤波
			float gyro_x = biquadFilterApply(&gyroFilterLPF[0], (mpu6050.gyro_raw.x - mpu6050.gyro_offset.x) / MPU6050_GYRO_LSB * DEG2RAD);
			float gyro_y = biquadFilterApply(&gyroFilterLPF[1], (mpu6050.gyro_raw.y - mpu6050.gyro_offset.y) / MPU6050_GYRO_LSB * DEG2RAD);
			float gyro_z = biquadFilterApply(&gyroFilterLPF[2], (mpu6050.gyro_raw.z - mpu6050.gyro_offset.z) / MPU6050_GYRO_LSB * DEG2RAD);
			
			// 角速度二阶陷波滤波
			if(gyro_notch_hz_1)
			{
				gyro_x = biquadFilterApply(&gyroFilterNotch_1[0], gyro_x);
				gyro_y = biquadFilterApply(&gyroFilterNotch_1[1], gyro_y);
				gyro_z = biquadFilterApply(&gyroFilterNotch_1[2], gyro_z);
			}
				
			// 角速度二阶陷波滤波
			if(gyro_notch_hz_2)
			{
				gyro_x = biquadFilterApply(&gyroFilterNotch_2[0], gyro_x);
				gyro_y = biquadFilterApply(&gyroFilterNotch_2[1], gyro_y);
				gyro_z = biquadFilterApply(&gyroFilterNotch_2[2], gyro_z);
			}
			
			// 角速度赋值
			mpu6050.gyro.x = gyro_x;
			mpu6050.gyro.y = gyro_y;
			mpu6050.gyro.z = gyro_z;

			// 加速度二阶低通
			float acc_x = biquadFilterApply(&accFilterLPF[0], (mpu6050.acc_raw.x - mpu6050.acc_offset.x) / mpu6050.resolution);
			float acc_y = biquadFilterApply(&accFilterLPF[1], (mpu6050.acc_raw.y - mpu6050.acc_offset.y) / mpu6050.resolution);
			float acc_z = biquadFilterApply(&accFilterLPF[2], (mpu6050.acc_raw.z - mpu6050.acc_offset.z) / mpu6050.resolution);
			 
			// 加速度二阶陷波滤波
			if(acc_notch_hz)
			{
				acc_x = biquadFilterApply(&accFilterNotch[0], acc_x);
				acc_y = biquadFilterApply(&accFilterNotch[1], acc_y);
				acc_z = biquadFilterApply(&accFilterNotch[2], acc_z);
			}
			 
			// 加速度赋值
			mpu6050.acc.x = acc_x;
			mpu6050.acc.y = acc_y;
			mpu6050.acc.z = acc_z;

			#endif
		}
	}
}
