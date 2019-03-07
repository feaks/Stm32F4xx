#ifndef _BSP_HMC5883L_H
#define _BSP_HMC5883L_H

#include "math_extend.h"
#include <stdbool.h>

//****************************************
// 定义HMC5883L内部地址
//****************************************
// HMC58X3 register map. For details see HMC58X3 datasheet
#define HMC58X3_R_CONFA	 	0
#define HMC58X3_R_CONFB 	1
#define HMC58X3_R_MODE 		2
#define HMC58X3_R_XM 			3
#define HMC58X3_R_XL 			4
#define HMC58X3_R_YM 			7  //!< Register address for YM.
#define HMC58X3_R_YL 			8  //!< Register address for YL.
#define HMC58X3_R_ZM 			5  //!< Register address for ZM.
#define HMC58X3_R_ZL 			6  //!< Register address for ZL.

#define HMC58X3_R_STATUS 	9
#define HMC58X3_R_IDA 		10
#define HMC58X3_R_IDB 		11
#define HMC58X3_R_IDC 		12

#define HMC5583l_ADDR         			        0x3C

#define CALIBRATING_MAG_CYCLES              3000  // n*20ms
#define HMC5583l_GAIN												1090	// 高斯增益

#define FILTER_NUM 			10				//滑动平均滤波数值个数

typedef struct 
{
	Vector3i  mag_raw;			// 采样值
	Vector3f 	mag_offset;		// 偏移值
	Vector3f 	mag_scale;		// 比例缩放	
  Vector3f 	mag;			    // 纠正后的值
	Vector3i  mag_max;			// 磁力计最大值
	Vector3i  mag_min;			// 磁力计最小值
	bool calibrating;       // 是否在校准中
	bool present;						// 是否存在
	bool enabled;						// 是否使能
	bool health;						// 是否可用
} HMC5883L;

extern HMC5883L hmc5883l;

void HMC5883L_Self_Test(void);
void HMC5883L_Init(void);
void HMC5883L_Calibration(void);
void HMC5883L_Update(float dt);

#endif

/*********************************** 飞航科技 (END OF FILE) ******************************************/
