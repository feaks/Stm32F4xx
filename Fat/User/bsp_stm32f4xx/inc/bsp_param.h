#ifndef _BSP_PARAM_H_
#define	_BSP_PARAM_H_

#include "math_extend.h"
#include "bsp_rc.h"

#if EEPROM_SIZE

#include "bsp_eeprom.h"

#define ACC_OFFSET_ADDR   0     // 0 ~ 11
#define GYRO_OFFSET_ADDR  32    // 32 ~ 43
#define MAG_OFFSET_ADDR   64    // 64 ~ 65
#define MAG_SCALE_ADDR    96    // 96 ~ 107
#define RC_ADDR           128   // 128 ~ 255 

#define STABLE_ROLL_ADDR  (256)
#define STABLE_PITCH_ADDR (256+32)
#define STABLE_YAW_ADDR   (256+64)

#define RATE_ROLL_ADDR   	(256+96)
#define RATE_PITCH_ADDR   (256+128)
#define RATE_YAW_ADDR    	(256+160)
	
#define POSITION_X_ADDR  	(256+192)
#define POSITION_Y_ADDR 	(256+224)
#define POSITION_Z_ADDR 	(256+256)

#define SPEED_X_ADDR   		(256+288)
#define SPEED_Y_ADDR 			(256+320)
#define SPEED_Z_ADDR 			(256+352)

#define isZero(x)					(x==0)
static inline void Param_SavePID(uint16_t address, PID * pid)
{
  EEPROM_Write(address, (uint8_t *)pid, sizeof(PID));
}

static inline bool Param_TakePID(uint16_t address, PID * pid)
{
	PID temp; 
	bool result = false;
  EEPROM_Read(address, (uint8_t*)&temp, sizeof(PID));
	if(!isnan(temp.kp) && !isnan(temp.ki) && !isnan(temp.kd) \
		&& !isZero(temp.kp))
	{
		pid->kp = temp.kp;
		pid->ki = temp.ki;
		pid->kd = temp.kd;
		result = true;
	}
	return result;
}

static inline void Param_SaveAccelOffset(Vector3f* acc_offset)
{
  EEPROM_Write(ACC_OFFSET_ADDR, (uint8_t*)acc_offset, sizeof(Vector3f));
}

static inline bool Param_TakeAccelOffset(Vector3f* acc_offset)
{
	bool result = true;
  EEPROM_Read(ACC_OFFSET_ADDR, (uint8_t*)acc_offset, sizeof(Vector3f));
	if(isnan(acc_offset->x) || isnan(acc_offset->y) || isnan(acc_offset->z)) 
	{
		acc_offset->x = 0;
		acc_offset->y = 0;
		acc_offset->z = 0;
		result = false;
	}
	return result;
}

static inline void Param_SaveGyroOffset(Vector3f* gyro_offset)
{
  EEPROM_Write(GYRO_OFFSET_ADDR, (uint8_t*)gyro_offset, sizeof(Vector3f));
}

static inline bool Param_TakeGyroOffset(Vector3f* gyro_offset)
{
	bool result = true;
  EEPROM_Read(GYRO_OFFSET_ADDR, (uint8_t*)gyro_offset, sizeof(Vector3f));
	if(isnan(gyro_offset->x) || isnan(gyro_offset->y) || isnan(gyro_offset->z)) 
	{
		gyro_offset->x = 0;
		gyro_offset->y = 0;
		gyro_offset->z = 0;
	  result = false;
	}
	return result;
}

static inline void Param_SaveMagOffset(Vector3f* mag_offset)
{
  EEPROM_Write(MAG_OFFSET_ADDR, (uint8_t*)mag_offset, sizeof(Vector3f));
}

static inline bool Param_TakeMagOffset(Vector3f* mag_offset)
{
	bool result = true;
  EEPROM_Read(MAG_OFFSET_ADDR, (uint8_t*)mag_offset, sizeof(Vector3f));
	if(isnan(mag_offset->x) || isnan(mag_offset->y) || isnan(mag_offset->z)) 
	{
		mag_offset->x = 0;
		mag_offset->y = 0;
		mag_offset->z = 0;
		result = false;
	}
	return result;
}

static inline void Param_SaveMagScale(Vector3f* mag_scale)
{
  EEPROM_Write(MAG_SCALE_ADDR, (uint8_t*)mag_scale, sizeof(Vector3f));
}

static inline bool Param_TakeMagScale(Vector3f * mag_scale)
{
	bool result = true;
  EEPROM_Read(MAG_SCALE_ADDR, (uint8_t*)mag_scale, sizeof(Vector3f));
	if(isnan(mag_scale->x) || isnan(mag_scale->y) || isnan(mag_scale->z) \
		|| isZero(mag_scale->x) || isZero(mag_scale->y) || isZero(mag_scale->z))
	{
		mag_scale->x = 1;
		mag_scale->y = 1;
		mag_scale->z = 1;
		result = false;
	}
	return result;
}

static inline void Param_SaveRcPara(RC_Para * rc[])
{
	for(uint8_t i=0; i<CHANNEL_MAX; i++)
	{
		EEPROM_Write(RC_ADDR + 16*i, (uint8_t*)rc[i], sizeof(RC_Para));
	}
}

static inline bool Param_TakeRcPara(RC_Para * rc[])
{
	bool result = true;
	for(uint8_t i=0; i<CHANNEL_MAX; i++)
	{
		EEPROM_Read(RC_ADDR + 16*i, (uint8_t*)rc[i], sizeof(RC_Para));
		if(isnan(rc[i]->scale_high) || isnan(rc[i]->scale_low) || isnan(rc[i]->radio_min) || isnan(rc[i]->radio_mid) || isnan(rc[i]->radio_max) || isnan(rc[i]->tmp) \
			|| isZero(rc[i]->scale_high) || isZero(rc[i]->scale_low) || isZero(rc[i]->radio_min) || isZero(rc[i]->radio_mid) || isZero(rc[i]->radio_max))
		{
			rc[i]->scale_high = 1;
			rc[i]->scale_low = 1;
			rc[i]->radio_min = 1000;
			rc[i]->radio_mid = 1500;
			rc[i]->radio_max = 2000;
			rc[i]->tmp = 0;
			result = false;
		}
	}
	return result;
}
#else

#define FLASH_PARAM_ADDR   0x08040000

typedef struct  
{
	Vector3f acc_offset;
	Vector3f gyro_offset;
	Vector3f mag_offset;
	Vector3f mag_scale;
	Vector3f acc_error;
	Vector3f gyro_error;
	RC_Para  rc[CHANNEL_MAX];
} Param;

extern Param param;

void Param_Init();
void Param_Save();

static inline void Param_SaveAccelOffset(Vector3f * acc_offset)
{
  param.acc_offset.x = acc_offset->x;
	param.acc_offset.y = acc_offset->y;
	param.acc_offset.z = acc_offset->z;
	Param_Save();
}

static inline bool Param_TakeAccelOffset(Vector3f * acc_offset)
{
	bool result = true;
  acc_offset->x = param.acc_offset.x;
	acc_offset->y = param.acc_offset.y;
	acc_offset->z = param.acc_offset.z;
	if(isnan(acc_offset->x) || isnan(acc_offset->y) || isnan(acc_offset->z)) 
	{
		acc_offset->x = 0;
		acc_offset->y = 0;
		acc_offset->z = 0;
		result = false;
	}
	return result;
}

static inline void Param_SaveGyroOffset(Vector3f * gyro_offset)
{
  param.gyro_offset.x = gyro_offset->x;
	param.gyro_offset.y = gyro_offset->y;
	param.gyro_offset.z = gyro_offset->z;
	Param_Save();
}

static inline bool Param_TakeGyroOffset(Vector3f * gyro_offset)
{
	bool result = true;
	gyro_offset->x = param.gyro_offset.x;
	gyro_offset->y = param.gyro_offset.y;
	gyro_offset->z = param.gyro_offset.z;
	if(isnan(gyro_offset->x) || isnan(gyro_offset->y) || isnan(gyro_offset->z)) 
	{
		gyro_offset->x = 0;
		gyro_offset->y = 0;
		gyro_offset->z = 0;
		result = false;
	}
	return result;
}

static inline void Param_SaveMagOffset(Vector3f * mag_offset)
{
  param.mag_offset.x = mag_offset->x;
	param.mag_offset.y = mag_offset->y;
	param.mag_offset.z = mag_offset->z;
	Param_Save();
}

static inline bool Param_TakeMagOffset(Vector3f * mag_offset)
{
	bool result = true;
  mag_offset->x = param.mag_offset.x;
	mag_offset->y = param.mag_offset.y;
	mag_offset->z = param.mag_offset.z;
	if(isnan(mag_offset->x) || isnan(mag_offset->y) || isnan(mag_offset->z)) 
	{
		mag_offset->x = 0;
		mag_offset->y = 0;
		mag_offset->z = 0;
		result = false;
	}
	return result;
}

static inline void Param_SaveMagScale(Vector3f * mag_scale)
{
  param.mag_scale.x = mag_scale->x;
	param.mag_scale.y = mag_scale->y;
	param.mag_scale.z = mag_scale->z;
	Param_Save();
}

static inline bool Param_TakeMagScale(Vector3f * mag_scale)
{
	bool result = true;
  mag_scale->x = param.mag_scale.x;
	mag_scale->y = param.mag_scale.y;
	mag_scale->z = param.mag_scale.z;
	if(isnan(mag_scale->x) || isnan(mag_scale->y) || isnan(mag_scale->z)) 
	{
		mag_scale->x = 1;
		mag_scale->y = 1;
		mag_scale->z = 1;
		result = false;
	}
	return result;
}

static inline void Param_SaveRcPara(RC_Para * rc[])
{
	for(uint8_t i=0; i<CHANNEL_MAX; i++)
	{
		param.rc[i].scale_high = rc[i]->scale_high;
		param.rc[i].scale_low  = rc[i]->scale_low;
		param.rc[i].radio_min  = rc[i]->radio_min;
		param.rc[i].radio_mid  = rc[i]->radio_mid;
		param.rc[i].radio_max  = rc[i]->radio_max;
		param.rc[i].tmp 			 = rc[i]->tmp;
	}
	Param_Save();
}

static inline bool Param_TakeRcPara(RC_Para * rc[])
{
	bool result = true;
	for(uint8_t i=0; i<CHANNEL_MAX; i++)
	{
		rc[i]->scale_high = param.rc[i].scale_high;
		rc[i]->scale_low  = param.rc[i].scale_low;
		rc[i]->radio_min  = param.rc[i].radio_min;
		rc[i]->radio_mid  = param.rc[i].radio_mid;
		rc[i]->radio_max  = param.rc[i].radio_max;
		rc[i]->tmp 				= param.rc[i].tmp;
		if(isnan(rc[i]->scale_high) || isnan(rc[i]->scale_low) || isnan(rc[i]->radio_min) || isnan(rc[i]->radio_mid) || isnan(rc[i]->radio_max) || isnan(rc[i]->tmp))
		{
			rc[i]->scale_high = 1;
			rc[i]->scale_low = 1;
			rc[i]->radio_min = 1000;
			rc[i]->radio_mid = 1500;
			rc[i]->radio_max = 2000;
			rc[i]->tmp = 0;
			result = false;
		}
	}
	return result;
}

#endif // EEPROM_SIZE

#endif // _BSP_PARAM_H_

/*********************************** 飞航科技 (END OF FILE) ***************************************/
