#ifndef MPU6050_H_
#define MPU6050_H_

//****************************************
// ??MPU6050????
//****************************************
#include "math_extend.h"
#include <stdbool.h>

#define MPU6050_ADDR         			  MPU6050_ADDRESS_AD0_HIGH

#define MPU6050_ADDRESS_AD0_LOW     0xD0 // address pin low (GND), default for InvenSense evaluation board
#define MPU6050_ADDRESS_AD0_HIGH    0xD2 // address pin high (VCC)
#define MPU6050_DEFAULT_ADDRESS     MPU6050_ADDRESS_AD0_HIGH

#define MPU6050_RA_XG_OFFS_TC       0x00 //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_YG_OFFS_TC       0x01 //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_ZG_OFFS_TC       0x02 //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_X_FINE_GAIN      0x03 //[7:0] X_FINE_GAIN
#define MPU6050_RA_Y_FINE_GAIN      0x04 //[7:0] Y_FINE_GAIN
#define MPU6050_RA_Z_FINE_GAIN      0x05 //[7:0] Z_FINE_GAIN
#define MPU6050_RA_XA_OFFS_H        0x06 //[15:0] XA_OFFS
#define MPU6050_RA_XA_OFFS_L_TC     0x07
#define MPU6050_RA_YA_OFFS_H        0x08 //[15:0] YA_OFFS
#define MPU6050_RA_YA_OFFS_L_TC     0x09
#define MPU6050_RA_ZA_OFFS_H        0x0A //[15:0] ZA_OFFS
#define MPU6050_RA_ZA_OFFS_L_TC     0x0B
#define MPU6050_RA_PRODUCT_ID	      0x0C
#define MPU6050_RA_XG_OFFS_USRH     0x13 //[15:0] XG_OFFS_USR
#define MPU6050_RA_XG_OFFS_USRL     0x14
#define MPU6050_RA_YG_OFFS_USRH     0x15 //[15:0] YG_OFFS_USR
#define MPU6050_RA_YG_OFFS_USRL     0x16
#define MPU6050_RA_ZG_OFFS_USRH     0x17 //[15:0] ZG_OFFS_USR
#define MPU6050_RA_ZG_OFFS_USRL     0x18
#define MPU6050_RA_SMPLRT_DIV       0x19 //??????,???:0x07(125Hz)
#define MPU6050_RA_CONFIG           0x1A //??????,???:0x06(5Hz)
#define MPU6050_RA_GYRO_CONFIG      0x1B //??????????,???:0x18(???,2000deg/s)
#define MPU6050_RA_ACCEL_CONFIG     0x1C //?????????????????,???:0x01(???,2G,5Hz)
#define MPU6050_RA_FF_THR           0x1D
#define MPU6050_RA_FF_DUR           0x1E
#define MPU6050_RA_MOT_THR          0x1F
#define MPU6050_RA_MOT_DUR          0x20
#define MPU6050_RA_ZRMOT_THR        0x21
#define MPU6050_RA_ZRMOT_DUR        0x22
#define MPU6050_RA_FIFO_EN          0x23
#define MPU6050_RA_I2C_MST_CTRL     0x24
#define MPU6050_RA_I2C_SLV0_ADDR    0x25
#define MPU6050_RA_I2C_SLV0_REG     0x26
#define MPU6050_RA_I2C_SLV0_CTRL    0x27
#define MPU6050_RA_I2C_SLV1_ADDR    0x28
#define MPU6050_RA_I2C_SLV1_REG     0x29
#define MPU6050_RA_I2C_SLV1_CTRL    0x2A
#define MPU6050_RA_I2C_SLV2_ADDR    0x2B
#define MPU6050_RA_I2C_SLV2_REG     0x2C
#define MPU6050_RA_I2C_SLV2_CTRL    0x2D
#define MPU6050_RA_I2C_SLV3_ADDR    0x2E
#define MPU6050_RA_I2C_SLV3_REG     0x2F
#define MPU6050_RA_I2C_SLV3_CTRL    0x30
#define MPU6050_RA_I2C_SLV4_ADDR    0x31
#define MPU6050_RA_I2C_SLV4_REG     0x32
#define MPU6050_RA_I2C_SLV4_DO      0x33
#define MPU6050_RA_I2C_SLV4_CTRL    0x34
#define MPU6050_RA_I2C_SLV4_DI      0x35
#define MPU6050_RA_I2C_MST_STATUS   0x36
#define MPU6050_RA_INT_PIN_CFG      0x37
#define MPU6050_RA_INT_ENABLE       0x38
#define MPU6050_RA_DMP_INT_STATUS   0x39
#define MPU6050_RA_INT_STATUS       0x3A
#define MPU6050_RA_ACCEL_XOUT_H     0x3B
#define MPU6050_RA_ACCEL_XOUT_L     0x3C
#define MPU6050_RA_ACCEL_YOUT_H     0x3D
#define MPU6050_RA_ACCEL_YOUT_L     0x3E
#define MPU6050_RA_ACCEL_ZOUT_H     0x3F
#define MPU6050_RA_ACCEL_ZOUT_L     0x40
#define MPU6050_RA_TEMP_OUT_H       0x41
#define MPU6050_RA_TEMP_OUT_L       0x42
#define MPU6050_RA_GYRO_XOUT_H      0x43
#define MPU6050_RA_GYRO_XOUT_L      0x44
#define MPU6050_RA_GYRO_YOUT_H      0x45
#define MPU6050_RA_GYRO_YOUT_L      0x46
#define MPU6050_RA_GYRO_ZOUT_H      0x47
#define MPU6050_RA_GYRO_ZOUT_L      0x48
#define MPU6050_RA_EXT_SENS_DATA_00 0x49
#define MPU6050_RA_EXT_SENS_DATA_01 0x4A
#define MPU6050_RA_EXT_SENS_DATA_02 0x4B
#define MPU6050_RA_EXT_SENS_DATA_03 0x4C
#define MPU6050_RA_EXT_SENS_DATA_04 0x4D
#define MPU6050_RA_EXT_SENS_DATA_05 0x4E
#define MPU6050_RA_EXT_SENS_DATA_06 0x4F
#define MPU6050_RA_EXT_SENS_DATA_07 0x50
#define MPU6050_RA_EXT_SENS_DATA_08 0x51
#define MPU6050_RA_EXT_SENS_DATA_09 0x52
#define MPU6050_RA_EXT_SENS_DATA_10 0x53
#define MPU6050_RA_EXT_SENS_DATA_11 0x54
#define MPU6050_RA_EXT_SENS_DATA_12 0x55
#define MPU6050_RA_EXT_SENS_DATA_13 0x56
#define MPU6050_RA_EXT_SENS_DATA_14 0x57
#define MPU6050_RA_EXT_SENS_DATA_15 0x58
#define MPU6050_RA_EXT_SENS_DATA_16 0x59
#define MPU6050_RA_EXT_SENS_DATA_17 0x5A
#define MPU6050_RA_EXT_SENS_DATA_18 0x5B
#define MPU6050_RA_EXT_SENS_DATA_19 0x5C
#define MPU6050_RA_EXT_SENS_DATA_20 0x5D
#define MPU6050_RA_EXT_SENS_DATA_21 0x5E
#define MPU6050_RA_EXT_SENS_DATA_22 0x5F
#define MPU6050_RA_EXT_SENS_DATA_23 0x60
#define MPU6050_RA_MOT_DETECT_STATUS    0x61
#define MPU6050_RA_I2C_SLV0_DO      0x63
#define MPU6050_RA_I2C_SLV1_DO      0x64
#define MPU6050_RA_I2C_SLV2_DO      0x65
#define MPU6050_RA_I2C_SLV3_DO      0x66
#define MPU6050_RA_I2C_MST_DELAY_CTRL   0x67   
#define MPU6050_RA_SIGNAL_PATH_RESET    0x68
#define MPU6050_RA_MOT_DETECT_CTRL      0x69
#define MPU6050_RA_USER_CTRL        0x6A
#define MPU6050_RA_PWR_MGMT_1       0x6B //????,???:0x00(????)
#define MPU6050_RA_PWR_MGMT_2       0x6C
#define MPU6050_RA_BANK_SEL         0x6D
#define MPU6050_RA_MEM_START_ADDR   0x6E
#define MPU6050_RA_MEM_R_W          0x6F
#define MPU6050_RA_DMP_CFG_1        0x70
#define MPU6050_RA_DMP_CFG_2        0x71
#define MPU6050_RA_FIFO_COUNTH      0x72
#define MPU6050_RA_FIFO_COUNTL      0x73
#define MPU6050_RA_FIFO_R_W         0x74
#define MPU6050_RA_WHO_AM_I         0x75 //IIC?????(????0x68,??)  0x6B?

// 
#define MPU6050_PWR1_SLEEP          	  0x40					
#define MPU6050_PWR1_CYCLE 		      		0x20				
#define MPU6050_PWR1_TEMP_DIS 		  		0x08				
#define MPU6050_CLOCK_INTERNAL          0x00   // 内部8M晶振      
#define MPU6050_CLOCK_PLL_XGYRO         0x01   // 参考X轴陀螺仪
#define MPU6050_CLOCK_PLL_YGYRO         0x02   // 参考y轴陀螺仪
#define MPU6050_CLOCK_PLL_ZGYRO         0x03   // 参考z轴陀螺仪
#define MPU6050_CLOCK_PLL_EXT32K        0x04   // 外部32.768KHz
#define MPU6050_CLOCK_PLL_EXT19M        0x05   // 外部19.2MHz
#define MPU6050_CLOCK_KEEP_RESET        0x07

// Register 26 ?Configuration
#define MPU6050_CFG_DLPF_CFG  
#define MPU6050_DLPF_BW_256         0x00  // 当陀螺仪频率=8kHz时有效，
#define MPU6050_DLPF_BW_188         0x01  // Acc:delay=2.0ms   Gyro:delay=1.9ms
#define MPU6050_DLPF_BW_98          0x02  // Acc:delay=3.0ms   Gyro:delay=2.8ms
#define MPU6050_DLPF_BW_42          0x03  // Acc:delay=4.9ms   Gyro:delay=4.8ms
#define MPU6050_DLPF_BW_20          0x04  // Acc:delay=8.5ms   Gyro:delay=8.3ms
#define MPU6050_DLPF_BW_10          0x05  // Acc:delay=13.8ms  Gyro:delay=13.4ms
#define MPU6050_DLPF_BW_5           0x06  // Acc:delay=19.0ms  Gyro:delay=18.6ms

// REGISTER 27 - GYROSCOPE CONFIGURATION
#define MPU6050_GYRO_XG_ST              0x80    // Self Test Response= Gyroscope Output with Self Test Enabled  - Gyroscope Output with Self Test Disabled
#define MPU6050_GYRO_YG_ST              0x40
#define MPU6050_GYRO_ZG_ST              0x20
#define MPU6050_GYRO_FS_250             0x00    // +-250°/s
#define MPU6050_GYRO_FS_500             0x08    // +-500°/s
#define MPU6050_GYRO_FS_1000            0x10    // +-1000°/s
#define MPU6050_GYRO_FS_2000            0x18    // +-2000°/s

// Register 28 - Accelerometer Configuration
#define MPU6050_GYRO_XA_ST              0x80    // Self-test response = Sensor output with self-test enabled - Sensor output without self-test enabled
#define MPU6050_GYRO_YA_ST              0x40
#define MPU6050_GYRO_ZA_ST              0x20
#define MPU6050_ACCEL_AFS_2             0x00    // +-2g
#define MPU6050_ACCEL_AFS_4             0x08    // +-4g
#define MPU6050_ACCEL_AFS_8             0x10    // +-8g
#define MPU6050_ACCEL_AFS_16            0x18    // +-16g

// REGISTER 106 - USER CONTROL
#define MPU6050_USERCTRL_I2C_MST_EN     0x20    // 该位置1：使能I2C主机模式。 该位清0：辅助I2C总线（AUX_DA和AUX_CL）逻辑上由主I2C总线（SDA和SCL）驱动

#define MPU6050_GYRO_FS    MPU6050_GYRO_FS_2000
#define MPU6050_ACCEL_AFS  MPU6050_ACCEL_AFS_8

// 加速度精度值
#if MPU6050_ACCEL_AFS == MPU6050_ACCEL_AFS_2
  #define MPU6050_ACCEL_LSB 16384
	#define MPU6500_MAX_ACCEL_G 2
#elif MPU6050_ACCEL_AFS == MPU6050_ACCEL_AFS_4
  #define MPU6050_ACCEL_LSB 8192
	#define MPU6500_MAX_ACCEL_G 4
#elif MPU6050_ACCEL_AFS == MPU6050_ACCEL_AFS_8
  #define MPU6050_ACCEL_LSB 4096
	#define MPU6500_MAX_ACCEL_G 8
#elif MPU6050_ACCEL_AFS == MPU6050_ACCEL_AFS_16
  #define MPU6050_ACCEL_LSB 2048
	#define MPU6500_MAX_ACCEL_G 16
#endif

// 角速度精度值
#if MPU6050_GYRO_FS == MPU6050_GYRO_FS_250
  #define MPU6050_GYRO_LSB 131.072
#elif MPU6050_GYRO_FS == MPU6050_GYRO_FS_500
  #define MPU6050_GYRO_LSB 65.526
#elif MPU6050_GYRO_FS == MPU6050_GYRO_FS_1000
  #define MPU6050_GYRO_LSB 32.768
#elif MPU6050_GYRO_FS == MPU6050_GYRO_FS_2000
  #define MPU6050_GYRO_LSB 16.384
#endif

#define MPU6050_CALIB_CYCLES     120  // 校准时间持续30s
#define MPU6050_CALIB_AVG_NUM     50  

#define FILTER_NUM                10

typedef struct
{
	Vector3i acc_raw;     // 加速度原始数据
	Vector3i gyro_raw;    // 角速度原始数据
  Vector3f acc_offset;  // 加速度计误差修正的偏移量
	Vector3f gyro_offset;	// 角速度误差修正的偏移量
	Vector3f acc_scale;   // 加速度放大比例
	Vector3f gyro_scale;  // 角速度放大比例
  Vector3f acc;         // 滤波后加速度
  Vector3f gyro;        // 滤波后角速度

	bool calibrating;       // 是否在校准中
	bool present;						// 是否存在
	bool enabled;						// 是否使能
	bool health;						// 是否可用
	
	uint32_t resolution;		// 灵敏度
} MPU6050;

extern MPU6050 mpu6050;

void MPU6050_Self_Test(void);
void MPU6050_Init(void);
void MPU6050_Filter_Init(void);
void MPU6050_Calibration(void);
void MPU6050_Update(float dt);

#endif /* MPU6050_H_ */
