#ifndef _BSP_RC_H_
#define	_BSP_RC_H_

#include "config.h"
#include "stdbool.h"
#include "stdint.h"
#include "mavlink.h"

#define DEAD_ZOOM 			10.0f
#define LOCK_ZOOM 			20.0f     // 解锁范围

#define NORMSCALE 			500.0f
#define CALIBRATING_RC_CYCLES              1000  // n*20ms

typedef struct {
	float          scale_high;    // 高段比例
	float          scale_low;     // 低段比例
	uint16_t       radio_min;     // 输入最小值
	uint16_t       radio_mid;     // 输入中值
	uint16_t       radio_max;     // 输入最大值	
	uint16_t       tmp;           // 	
} RC_Para;                      // 遥控器校准参数

typedef struct { 
	int16_t       radio_in;	  		// pwm输入值
	int16_t				enable;					// 通道使能
	float       	control_in; 		// 低通滤波后的输入值
	float         radio_out;	    // 处理后的值 +-500
} RC_Channel;

typedef struct {
	RC_Channel 		channel[CHANNEL_MAX]; // 遥控输入通道   1横滚，2俯仰，3油门，4航向 范围
	RC_Para       para[CHANNEL_MAX];    // 遥控器基本参数
	uint8_t 			lost[CHANNEL_MAX];		// 遥控读数丢失次数
	// bool secure_moto_lock; 							// false: moto is locked/disarmed,  true: moto is unlocked/armed
	bool calibrating;       // 是否在校准中
	bool present;						// 是否存在
	bool enabled;						// 是否使能
	bool health;						// 是否可用
} RC;

typedef struct {
	bool height;			// true:定高模式    false:非定高模式
	bool location;		// true:位置控制		false:姿态控制
} Mode;

extern RC rc;
extern Mode mode;                    				// 飞行模式
extern CUSTOM_MODE custom_mode;
extern bool Rudder_Amidships; // 舵量回中

void RC_Init();
void RC_Read(float Dt);
void RC_Calibration(void);

#endif
