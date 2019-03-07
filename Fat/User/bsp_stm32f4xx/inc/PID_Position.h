#ifndef _PID_POSITION_H_
#define _PID_POSITION_H_

#define ULTRA_SPEED 		 300    // mm/s
#define ULTRA_MAX_HEIGHT 2000   // mm		最大定高高度
#define ULTRA_INT        100    // mm    高度积分幅度

#include "stdbool.h"
#include "stdint.h"
#include "math_extend.h"

typedef struct 
{
	bool isReady;					// 数据状态
	uint8_t source;				// 数据来源    1：超声波    2：气压计
	float dt;							// 高度变化间隔时间
} Height;

typedef struct 
{
	bool isReady;					// 数据状态
	uint8_t source;				// 数据来源    1：光流 		2：GPS    
	float dt;							// 水平位置变化间隔时间
} Horizon;

typedef struct
{
	PID x;
	PID y;
	PID z;
} PID_Position;

typedef struct
{
	Position body;	 					 // 机体当前位置
	Position expect;           // 期望值
	Position err;              // 本次误差
	Position err_filter;       // 本次误差
	Position err_p;            // 误差比例
	Position err_i;            // 误差积分
	Position err_d;            // 误差微分
	Position last_err;         // 上次误差，用于计算误差微分
	Position out;              // 输出
	PID_Position 	   pid;
} CTRL_Position;

extern CTRL_Position ctrl_position, ctrl_speed;		// 位置控制、速度控制

extern Height  height;						// 高度数据状态
extern Horizon horizon;					// 水平数据状态

void PID_Position_Param_Init();

void Position_Ctrl(float dt);

float Speed_Ctrl(float dt);

float Ctrl(float dt);

#endif

