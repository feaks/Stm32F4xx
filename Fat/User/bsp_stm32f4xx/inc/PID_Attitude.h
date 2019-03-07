/*
*********************************************************************************************************
*	                                  
*	模块名称 : PID控制
*	文件名称 : PID.h
*	版    本 : V1.0
*	说    明 : PID控制
* 日		期 ：2016-2-20   
*		
*	Copyright(c)2016 武汉飞航科技有限公司 
*	技术讨论QQ群：363450228 淘宝官网：https://shop149667832.taobao.com
* 以下所有内容版权均属武汉飞航科技所有，未经允许不得用于商业用途。
* 欢迎各位使用并推广本程序，修改内容时必须保留飞航科技的版权声明。
*********************************************************************************************************
*/
#ifndef _PID_H_
#define	_PID_H_

#include "config.h"
#include "math_extend.h"
#include "ahrs.h"

#define MAX_CTRL_ANGLE			0.45f										// 遥控能达到的最大角度 0.45rad = 25.78°
#define MAX_CTRL_ASPEED 	 	3.5f									  // ROL,PIT允许的最大控制角速度
#define MAX_CTRL_YAW_SPEED 	0.9f									  // YAW允许的最大控制角度差  0.9rad = 51°

typedef struct
{
	PID roll;
	PID pitch;
	PID yaw;
} PID_Attitude;

typedef struct
{
	Attitude expect;           // 期望值
	Attitude err;              // 本次误差
	Attitude err_p;            // 误差比例
	Attitude err_i;            // 误差积分
	Attitude err_filter;       // 本次误差
	Attitude err_d;            // 误差微分
	Attitude last_err;         // 上次误差，用于计算误差微分
	Attitude last_detle;       // 上2次误差，用于计算误差微分
	Attitude last_detle2;      // 上3次误差，用于计算误差微分
	Attitude out;              // 输出
	PID_Attitude 	   pid;
} CTRL_Attitude;

extern CTRL_Attitude ctrl_rate, ctrl_stable; 	// 内环控制、外环控制

void speed_loop(float dt);
void stable_loop(float);
void rate_loop(float);
void PID_Attitude_Param_Init(void);

#endif
   
/*********************************** 飞航科技 (END OF FILE) ******************************************/
