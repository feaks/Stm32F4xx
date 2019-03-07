#include "PID_Position.h"
#include "bsp_ultrasonic.h"
#include "math_extend.h"
#include "AHRS.h"
#include "bsp_rc.h"
#include "throttle.h"
#include "bsp_param.h"

CTRL_Position ctrl_position, ctrl_speed;		// 位置控制、速度控制

Height height;						// 高度数据状态
Horizon horizon;					// 水平数据状态

void PID_Position_Param_Init()
{	
	if(!Param_TakePID(POSITION_X_ADDR,   &ctrl_position.pid.x))
	{
		#ifdef QV250
		ctrl_position.pid.x.kp = 0.0f;
		ctrl_position.pid.x.ki = 0.0f;
		ctrl_position.pid.x.kd = 0.0f;
		#elif defined F450
		ctrl_position.pid.x.kp = 0.0f;
		ctrl_position.pid.x.ki = 0.0f;
		ctrl_position.pid.x.kd = 0.0f;
		#elif defined F550
		ctrl_position.pid.x.kp = 0.0f;
		ctrl_position.pid.x.ki = 0.0f;
		ctrl_position.pid.x.kd = 0.0f;	
		#endif	
	}
	if(!Param_TakePID(POSITION_Y_ADDR,   &ctrl_position.pid.y))
	{
		#ifdef QV250
		ctrl_position.pid.y.kp = 0.0f;
		ctrl_position.pid.y.ki = 0.0f;
		ctrl_position.pid.y.kd = 0.0f; 
		#elif defined F450
		ctrl_position.pid.y.kp = 0.0f;
		ctrl_position.pid.y.ki = 0.0f;
		ctrl_position.pid.y.kd = 0.0f; 	
		#elif defined F550
		ctrl_position.pid.y.kp = 0.0f;
		ctrl_position.pid.y.ki = 0.0f;
		ctrl_position.pid.y.kd = 0.0f; 	
		#endif	
	}
	if(!Param_TakePID(POSITION_Z_ADDR,   &ctrl_position.pid.z))
	{
		#ifdef QV250
		ctrl_position.pid.z.kp = 4.0f;
		ctrl_position.pid.z.ki = 0.1f;
		ctrl_position.pid.z.kd = 0.0f; 
		#elif defined F450
		ctrl_position.pid.z.kp = 2;
		ctrl_position.pid.z.ki = 0.3f;
		ctrl_position.pid.z.kd = 0.5f; 	
		#elif defined F550
		ctrl_position.pid.z.kp = 1.65f;
		ctrl_position.pid.z.ki = 0.02f;
		ctrl_position.pid.z.kd = 0.05f; 
		#endif	
	}
	if(!Param_TakePID(SPEED_X_ADDR,      &ctrl_speed.pid.x))
	{
		#ifdef QV250
		ctrl_speed.pid.x.kp = 0.0f;
		ctrl_speed.pid.x.ki = 0.0f;
		ctrl_speed.pid.x.kd = 0.0f;
		#elif defined F450
		ctrl_speed.pid.x.kp = 0.0f;
		ctrl_speed.pid.x.ki = 0.0f;
		ctrl_speed.pid.x.kd = 0.0f;	
		#elif defined F550
		ctrl_speed.pid.x.kp = 0.0f;
		ctrl_speed.pid.x.ki = 0.0f;
		ctrl_speed.pid.x.kd = 0.0f;
		#endif	
	}
	if(!Param_TakePID(SPEED_Y_ADDR,      &ctrl_speed.pid.y))
	{
		#ifdef QV250
		ctrl_speed.pid.y.kp = 0.0f;
		ctrl_speed.pid.y.ki = 0.0f;
		ctrl_speed.pid.y.kd = 0.0f;
		#elif defined F450
		ctrl_speed.pid.y.kp = 0.0f;
		ctrl_speed.pid.y.ki = 0.0f;
		ctrl_speed.pid.y.kd = 0.0f;	
		#elif defined F550
		ctrl_speed.pid.y.kp = 0.0f;
		ctrl_speed.pid.y.ki = 0.0f;
		ctrl_speed.pid.y.kd = 0.0f;	
		#endif	
	}
	if(!Param_TakePID(SPEED_Z_ADDR,      &ctrl_speed.pid.z))
	{
		#ifdef QV250
		ctrl_speed.pid.z.kp = 2;
		ctrl_speed.pid.z.ki = 0.2f;
		ctrl_speed.pid.z.kd = 0.8f;
		#elif defined F450
		ctrl_speed.pid.z.kp = 0.5f;  
		ctrl_speed.pid.z.ki = 0.003f;
		ctrl_speed.pid.z.kd = 0.5f; 
		#elif defined F550
		ctrl_speed.pid.z.kp = 0.7f;
		ctrl_speed.pid.z.ki = 0.003f;
		ctrl_speed.pid.z.kd = 0.7f;
		#endif	
	}
}

void Position_Ctrl(float dt)  	 // 执行周期30ms左右，程序设计为油门满值，上升速度1m/s
{
	if(mode.location)
	{
		horizon.dt += dt;
	}
	
	// 位置控制
	if(mode.location && horizon.isReady)
	{
		horizon.isReady = false;
		
		/* 计算期望水平位置 */	
		ctrl_position.expect.x += horizon.dt * (0/NORMSCALE*0.5f);
		ctrl_position.expect.y += horizon.dt * (0/NORMSCALE*0.5f);
		
		/* 期望水平位置限幅 */	
		ctrl_position.expect.x = status.position.x + LIMIT(ctrl_position.expect.x - ctrl_position.body.x, -0.5, 0.5);
		ctrl_position.expect.y = status.position.y + LIMIT(ctrl_position.expect.y - ctrl_position.body.y, -0.5, 0.5);

		/* 计算误差 */
		ctrl_position.err.x = ctrl_position.expect.x - status.position.x;
		ctrl_position.err.y = ctrl_position.expect.y - status.position.y;
		
		/* 计算比例项 */
		ctrl_position.err_p.x =  ctrl_position.pid.x.kp * ctrl_position.err.x;
		ctrl_position.err_p.y =  ctrl_position.pid.y.kp * ctrl_position.err.y;
		
		/* 误差滤波 */
		ctrl_position.err_filter.x -= ctrl_position.err_filter.x / 8;
		ctrl_position.err_filter.y -= ctrl_position.err_filter.y / 8;
		
		ctrl_position.err_filter.x += ctrl_position.err.x - ctrl_position.last_err.x;
		ctrl_position.err_filter.y += ctrl_position.err.y - ctrl_position.last_err.y;
		
		/* 计算微分项 */
		ctrl_position.err_d.x = ctrl_position.pid.x.kd * ctrl_position.pid.x.kd * (ctrl_position.err_filter.x/8) / horizon.dt; // -status.speed.x;
		ctrl_position.err_d.y = ctrl_position.pid.y.kd * ctrl_position.pid.y.kd * (ctrl_position.err_filter.y/8) / horizon.dt; // -status.speed.y;
		
		/* 记录历史数据 */
		ctrl_position.last_err.x = ctrl_position.err.x;
		ctrl_position.last_err.y = ctrl_position.err.y;
		
		/* 计算积分项 */
		ctrl_position.err_i.x += ctrl_position.pid.x.ki * ctrl_position.err.x * horizon.dt;
		ctrl_position.err_i.y += ctrl_position.pid.y.ki * ctrl_position.err.y * horizon.dt;
		
		/* 水平位置误差积分限幅 */
		ctrl_position.err_i.x = LIMIT(ctrl_position.err_i.x, -0.05f, 0.05f);
		ctrl_position.err_i.y = LIMIT(ctrl_position.err_i.y, -0.05f, 0.05f);
		
		/* 计算输出量 */
		ctrl_position.out.x = ctrl_position.err_p.x + ctrl_position.err_i.x + ctrl_position.err_d.x;
		ctrl_position.out.y = ctrl_position.err_p.y + ctrl_position.err_i.y + ctrl_position.err_d.y;
		
		horizon.dt = 0;
	}
	
	if(mode.height)
	{
		height.dt += dt;
	}
	
	// 高度控制
	if(mode.height && height.isReady)
	{
		height.isReady = false;
		
		/* 计算期望高度 */
		if(Rudder_Amidships) // 舵量回中
		{
			ctrl_position.expect.z += height.dt * (rc.channel[2].radio_out/NORMSCALE*0.5f); // 回中遥控器
		}
		else
		{
			ctrl_position.expect.z = status.position.z; // 不回中遥控器
		}
		
		/* 期望高度限幅 */	
		ctrl_position.expect.z = status.position.z + LIMIT(ctrl_position.expect.z - status.position.z, -0.5f, 0.5f);

		/* 计算误差 */
		ctrl_position.err.z = ctrl_position.expect.z - status.position.z;
		
		/* 计算比例项 */
		ctrl_position.err_p.z =  ctrl_position.pid.z.kp * ctrl_position.err.z;
		
		/* 计算微分项 */
		ctrl_position.err_d.z = ctrl_position.pid.z.kd * ctrl_position.pid.z.kd * (ctrl_position.err.z - ctrl_position.last_err.z) / height.dt; // -status.speed.z;
		
		/* 记录历史数据 */
		ctrl_position.last_err.z = ctrl_position.err.z;
		
		/* 计算积分项 */
		if(throttle > 200) 
		{
			if(rc.channel[2].radio_out)			ctrl_position.err_i.z = 0;
			else 														ctrl_position.err_i.z += ctrl_position.pid.z.ki * ctrl_position.err.z * height.dt;
		} 
		else 
		{
			ctrl_position.err_i.z = 0;
			ctrl_position.err_d.z = 0;
		}
		
		/* 高度误差积分限幅 */
		ctrl_position.err_i.z = LIMIT(ctrl_position.err_i.z, -0.05f * ctrl_position.pid.z.kp, 0.05f * ctrl_position.pid.z.kp);
		
		/* 计算输出量 */
		ctrl_position.out.z = ctrl_position.err_p.z + ctrl_position.err_i.z + ctrl_position.err_d.z;
		
		height.dt = 0;
	}
}

float Speed_Ctrl(float dt)  // 执行周期2ms
{	
	if(mode.location)
	{
		/* 期望垂直移动速度限幅 */
		ctrl_speed.expect.x = LIMIT(rc.channel[0].radio_out/NORMSCALE, -1, 1); //   //    ctrl_position.out.x
		ctrl_speed.expect.y = LIMIT(rc.channel[1].radio_out/NORMSCALE, -1, 1); //   //    ctrl_position.out.y
		
		/* 计算误差 */
		ctrl_speed.err.x = ctrl_speed.expect.x - ctrl_speed.body.x;
		ctrl_speed.err.y = ctrl_speed.expect.y - ctrl_speed.body.y;
		
		/* 计算比例项 */
		ctrl_speed.err_p.x = ctrl_speed.pid.x.kp * ctrl_speed.err.x;
		ctrl_speed.err_p.y = ctrl_speed.pid.y.kp * ctrl_speed.err.y;
			
		/* 计算微分项 */
		ctrl_speed.err_d.x = ctrl_speed.pid.x.kd * (ctrl_speed.err.x - ctrl_speed.last_err.x) / dt;
		ctrl_speed.err_d.y = ctrl_speed.pid.y.kd * (ctrl_speed.err.y - ctrl_speed.last_err.y) / dt;
		
		/* 记录历史数据 */
		ctrl_speed.last_err.x = ctrl_speed.err.x;
		ctrl_speed.last_err.y = ctrl_speed.err.y;
		
		/* 计算积分项 */
		if(throttle > 200) 
		{
			ctrl_speed.err_i.x += ctrl_speed.pid.x.ki * ctrl_speed.err.x * dt;
			ctrl_speed.err_i.y += ctrl_speed.pid.y.ki * ctrl_speed.err.y * dt;
		} 
		else 
		{
			ctrl_speed.err_i.x = 0;
			ctrl_speed.err_i.y = 0;
		}
		
		/* 垂直移动速度误差积分限幅 */
		ctrl_speed.err_i.x = LIMIT(ctrl_speed.err_i.x, -0.5f * ctrl_speed.pid.x.kp, 0.5f * ctrl_speed.pid.x.kp);
		ctrl_speed.err_i.y = LIMIT(ctrl_speed.err_i.y, -0.5f * ctrl_speed.pid.y.kp, 0.5f * ctrl_speed.pid.y.kp);
		
		/* 计算输出量 */
		ctrl_speed.out.x = ctrl_speed.err_p.x + ctrl_speed.err_i.x + ctrl_speed.err_d.x;
		ctrl_speed.out.y = ctrl_speed.err_p.y + ctrl_speed.err_i.y + ctrl_speed.err_d.y;
	}
	
	if(mode.height)
	{
		/* 期望垂直移动速度限幅 */
		ctrl_speed.expect.z = LIMIT( ctrl_position.out.z, -0.5f, 0.5f);
		
		/* 计算误差 */
		ctrl_speed.err.z = ctrl_speed.expect.z - status.speed.z;
		
		/* 计算比例项 */
		ctrl_speed.err_p.z = ctrl_speed.pid.z.kp * ctrl_speed.err.z;
			
		/* 误差滤波 */
		ctrl_speed.err_filter.z -= ctrl_speed.err_filter.z / 8;
		
		ctrl_speed.err_filter.z += ctrl_speed.err.z - ctrl_speed.last_err.z;
		
		/* 计算微分项 */
		ctrl_speed.err_d.z = ctrl_speed.pid.z.kd * (ctrl_speed.err_filter.z  / 8) / dt;
		
		/* 记录历史数据 */
		ctrl_speed.last_err.z = ctrl_speed.err.z;
		
		/* 计算积分项 */
		if(throttle > 200) 
		{
			ctrl_speed.err_i.z += ctrl_speed.pid.z.ki * ctrl_speed.err.z * dt;
		} 
		else 
		{
			ctrl_speed.err_i.z = 0;
			ctrl_speed.err_d.z = 0;
		}
		
		/* 垂直移动速度误差积分限幅 */
		ctrl_speed.err_i.z = LIMIT(ctrl_speed.err_i.z, -0.2f*ctrl_speed.pid.z.kp, 0.2f*ctrl_speed.pid.z.kp);
		
		/* 计算输出量 */
		ctrl_speed.out.z = ctrl_speed.err_p.z + ctrl_speed.err_i.z + ctrl_speed.err_d.z;
	}
	return ctrl_speed.out.z;
}

float Ctrl(float dt) // 执行周期2ms
{
	Position_Ctrl(dt);
	if(mode.height)
		return Speed_Ctrl(dt);
	else
		return dt * rc.channel[2].radio_out * 0.1f; // dt = 0.002s  油门推到最大值500，10s后实际油门可达到500;
}
