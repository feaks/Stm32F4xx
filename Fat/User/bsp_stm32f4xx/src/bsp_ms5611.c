#include "bsp_ms5611.h"
#include "bsp_i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "math_extend.h"

#define MS5611_FILTER_NUM 11

static I2C * I2C_MS5611;

MS5611 ms5611;

static uint8_t T_buf[3], P_buf[3]; // 温度和气压的缓存区

const static double _msl_pressure = 101325;  // 海平面压强 Pa
/* tropospheric properties (0-11km) for standard atmosphere */
const static double T1 = 15.0 + 273.15;	/* temperature at base height in Kelvin */
const static double a  = -6.5 / 1000;	/* temperature gradient in degrees per metre */
const static double g  = 9.80665;	/* gravity constant in m/s/s */
const static double R  = 287.05;	/* ideal gas constant in J/kg/K */

/** 
 * MS5611单字节写入
 *   REG_Address：要写入的寄存器地址
 *   REG_data：要写入的数据
 */
static inline bool MS5611_Write_Byte(uint8_t REG_Address, uint8_t REG_data)
{
	return I2C_Write_REG(I2C_MS5611, MS5611_ADDR, REG_Address, REG_data);
}

/**
 * MS5611单字节读取
 *   REG_Address：要读取的寄存器地址
 */
static inline uint8_t MS5611_Read_Byte(uint8_t REG_Address)
{
	return I2C_Read_REG(I2C_MS5611, MS5611_ADDR, REG_Address);
}

/**
 * MS5611读取n个字节
 *   REG_Address：要读取的寄存器地址
 *   buf：读取后存储数据的buf
 *   len：要读取数据长度
 */
bool MS5611_Read_NByte(uint8_t REG_Address, uint8_t * buf, uint8_t len)
{
  if(!I2C_Start(I2C_MS5611))return false;
	I2C_SendByte(I2C_MS5611, MS5611_ADDR);  //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_MS5611)){I2C_Stop(I2C_MS5611); return false;}
	I2C_SendByte(I2C_MS5611, REG_Address);   
	I2C_WaitAck(I2C_MS5611);
	I2C_Start(I2C_MS5611);
	I2C_SendByte(I2C_MS5611, MS5611_ADDR | 1); // 读操作
	I2C_WaitAck(I2C_MS5611);
	for(uint16_t i=0; i<len; i++)
	{
	  buf[i] = I2C_RadeByte(I2C_MS5611);
		if(i<len-1)
		  I2C_Ack(I2C_MS5611);
	}
	
	I2C_NoAck(I2C_MS5611);
	I2C_Stop(I2C_MS5611);
	return true;
}

// 复位
static inline void MS5611_Reset(void)
{
  MS5611_Write_Byte(CMD_RESET, 1);
}

void MS5611_Read_Prom(void)
{
	uint8_t buffer[2] = { 0, 0 };	
	for (uint8_t i=0; i<PROM_CNT; i++)
	{
		MS5611_Read_NByte(CMD_PROM_RD + i * 2, buffer, 2); // send PROM READ command
		ms5611.C[i] = ((uint16_t)buffer[0]) << 8 | buffer[1];
	}
}

// AD值转换为物理量
bool MS5611_Calculate_Pressure(void)
{
	static TickType_t last_time;
	static TickType_t time;
	static float dt;
	
	static int32_t ms5611_pressure_fifo[MS5611_FILTER_NUM];         	// 中值滤波数组
	static int32_t *p_ms5611_pressure_fifo[MS5611_FILTER_NUM] = { &ms5611_pressure_fifo[0], &ms5611_pressure_fifo[1], &ms5611_pressure_fifo[2], &ms5611_pressure_fifo[3], &ms5611_pressure_fifo[4], &ms5611_pressure_fifo[5], &ms5611_pressure_fifo[6], &ms5611_pressure_fifo[7], &ms5611_pressure_fifo[8], &ms5611_pressure_fifo[9], &ms5611_pressure_fifo[10]};					// 指向中值滤波数组
	static uint8_t index;                                         // 中值滤波索引	
		
	static int32_t _TEMP;
	static int64_t _OFF;
	static int64_t _SENS;
	
	uint32_t D2 = (T_buf[0] << 16) | (T_buf[1] << 8) | T_buf[2];
	uint32_t D1 = (P_buf[0] << 16) | (P_buf[1] << 8) | P_buf[2];

	/* temperature offset (in ADC units) */
	int32_t dT = (int32_t)D2 - ((int32_t)ms5611.C[5] << 8);
	
	/* absolute temperature in centidegrees - note intermediate value is outside 32-bit range */
	_TEMP = 2000 + (((int64_t)dT * ms5611.C[6]) >> 23);
	
	_OFF = ((int64_t)ms5611.C[2] << 16) + (((int64_t)dT * ms5611.C[4]) >> 7);
	_SENS = ((int64_t)ms5611.C[1] << 15) + (((int64_t)dT * ms5611.C[3]) >> 8);
	
	/* MS5611 temperature compensation */
	if (_TEMP < 2000)  // temperature lower than 20degC
	{ 
		int32_t T2 = (int64_t)(dT * dT) >> 31;
		
		int64_t f = ((int64_t)_TEMP - 2000) *  ((int64_t)_TEMP - 2000);
		int64_t OFF2 = 5 * f >> 1;
		int64_t SENS2 = 5 * f >> 2;
		
			if (_TEMP < -1500) { // temperature lower than -15degC
				int64_t f2 = ((int64_t)_TEMP + 1500) * ((int64_t)_TEMP + 1500);
				OFF2 += 7 * f2;
				SENS2 += 11 * f2 >> 1;
			}
			
			_TEMP -= T2;
			_OFF  -= OFF2;
			_SENS -= SENS2;
	}
	
	/* pressure calculation, result in Pa */
	ms5611.pressure_raw = ((((int64_t)D1 * _SENS) >> 21) - _OFF) >> 15;
	ms5611.temperature = _TEMP;
	
	if(ms5611.pressure_raw> 80000 && ms5611.pressure_raw < 120000)
	{
		time = xTaskGetTickCount();
		dt = (time - last_time) * portTICK_RATE_MS * 0.001;
		last_time = time;
		
		ms5611_pressure_fifo[index] = ms5611.pressure_raw;
		
		// 滑动中值滤波
		uint32_t tmp_p;
		for(uint8_t i=0; i<MS5611_FILTER_NUM-1; i++)
		{
			for(uint8_t j=i+1;j<MS5611_FILTER_NUM;j++)
			{
				if(*p_ms5611_pressure_fifo[i] > *p_ms5611_pressure_fifo[j])
				{
					tmp_p = (uint32_t)p_ms5611_pressure_fifo[i];
					p_ms5611_pressure_fifo[i] = p_ms5611_pressure_fifo[j];
					p_ms5611_pressure_fifo[j] = (int32_t* )tmp_p;
				}
			}
		}
		if(++index == MS5611_FILTER_NUM)			index = 0;
		
		ms5611.pressure += TWO_PI * dt * (*p_ms5611_pressure_fifo[MS5611_FILTER_NUM/2+1] - ms5611.pressure); // 低通延时1s
		return true;
	}
	return false;
}

// 计算高度值
void MS5611_Calculate_Altitube(void)
{
	static TickType_t last_time;
	static TickType_t time;
	static float dt;
	
	static float ms5611_altitube_fifo[MS5611_FILTER_NUM];         	// 中值滤波数组
	static uint8_t index = 0;                                         // 中值滤波索引	
		
	time = xTaskGetTickCount();
	dt = (time - last_time) * portTICK_RATE_MS * 0.001;
	last_time = time;
	
	/*
	 * Solve:
	 *
	 *     /        -(aR / g)     \
	 *    | (p / p1)          . T1 | - T1
	 *     \                      /
	 * h = -------------------------------  + h1
	 *                   a
	 */
	ms5611.altitube_raw = (((powf((ms5611.pressure / _msl_pressure), (-(a * R) / g))) * T1) - T1) / a; // 根据气压计算高度值
	
	ms5611.altitube += TWO_PI * dt * (ms5611.altitube_raw - ms5611.altitube) * 0.5f; // 低通延时1s
	
	
	ms5611_altitube_fifo[index] = ms5611.altitube_raw;
	float altitube_sum = 0;
	for(uint8_t i=0; i<MS5611_FILTER_NUM; i++)
	{
		altitube_sum+=ms5611_altitube_fifo[i];
	}
	if(++index == MS5611_FILTER_NUM)			index = 0;
	
	ms5611.altitube2 = altitube_sum / (float)MS5611_FILTER_NUM;

//		
//		mpl3115_distance_fifo[index] = mpl3115.altitube_raw - mpl3115.altitube_offset;
//		if(++index == 10)
//			index = 0;
//		
//		float mpl3115_sum = 0;
//		for(uint8_t i=0; i<10; i++)
//		{
//			mpl3115_sum += mpl3115_distance_fifo[i];
//		}
//		mpl3115.altitube = mpl3115_sum / 10;
}

/**
 * MS5611初始化
 */
void MS5611_Init(void)
{
	I2C_MS5611 = I2C_2;
	vTaskDelay(10/portTICK_RATE_MS);
	MS5611_Reset();
	vTaskDelay(10/portTICK_RATE_MS);
	MS5611_Read_Prom();
}

/**
 * MS5611更新数据
 */
uint8_t MS5611_Update()
{
	MS5611_Write_Byte(CMD_ADC_CONV | CMD_ADC_D2 | CMD_ADC_4096, 1); // 开始温度转换
	vTaskDelay(10/portTICK_RATE_MS);																// 等待转换完成
	MS5611_Read_NByte(CMD_ADC_READ, T_buf, 3); 											// 读取转换的温度		8569150
	MS5611_Write_Byte(CMD_ADC_CONV | CMD_ADC_D1 | CMD_ADC_4096, 1); // 开始气压转换			
	vTaskDelay(10/portTICK_RATE_MS);																// 等待转换完成
	MS5611_Read_NByte(CMD_ADC_READ, P_buf, 3); 											// 读取转换的气压		9085466
	if(MS5611_Calculate_Pressure())																	// 计算气压值
		MS5611_Calculate_Altitube();																	// 计算高度
}

static int8_t ms5611_crc(uint16_t *prom)
{
    int32_t i, j;
    uint32_t res = 0;
    uint8_t zero = 1;
    uint8_t crc = prom[7] & 0xF;
    prom[7] &= 0xFF00;

    // if eeprom is all zeros, we're probably fucked - BUT this will return valid CRC lol
    for (i = 0; i < 8; i++) {
        if (prom[i] != 0)
            zero = 0;
    }
    if (zero)
        return -1;

    for (i = 0; i < 16; i++) {
        if (i & 1)
            res ^= ((prom[i >> 1]) & 0x00FF);
        else
            res ^= (prom[i >> 1] >> 8);
        for (j = 8; j > 0; j--) {
            if (res & 0x8000)
                res ^= 0x1800;
            res <<= 1;
        }
    }
    prom[7] |= crc;
    if (crc == ((res >> 12) & 0xF))
        return 0;

    return -1;
}