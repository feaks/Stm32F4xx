#include "Throttle.h"
#include "bsp_rc.h"
#include "bsp_mpl3115.h"
#include "AHRS.h"
#include "math_extend.h"
#include "PID_Position.h"
#include "main.h"
	
float throttle; 						// 油门

float throttle_ctrl(float dt)
{
	static float compensation = 0;
	
	if(!rc.channel[2].enable)		
	{
		throttle = 0;
		
		status.acc_offset.x -= status.acc_offset.x * 0.005f;
		status.acc_offset.y -= status.acc_offset.y * 0.005f;
		status.acc_offset.z -= status.acc_offset.z * 0.005f;
		status.acc_offset.x += status.acc.x;
		status.acc_offset.y += status.acc.y;
		status.acc_offset.z += status.acc.z;
		
		status.speed.x = 0;
		status.speed.y = 0;
    status.speed.z = 0;
		
		if(height.source == 2) // 如果定高数据来源于气压计，则锁定时需要修正气压计告诉偏移量（此情况只有超声波故障时执行）
			mpl3115.altitube_offset = (mpl3115.altitube_raw + mpl3115.altitube_offset) * 0.5f;
	}
	else
	{
		if(Rudder_Amidships) // 舵量回中
		{
			throttle += Ctrl(dt); // 定高模式
  		// throttle = rc.channel[2].radio_out * 2;
		}
		else // 舵量不回中
		{
			if(mav.base_mode == MAV_MODE_AUTO_ARMED) // 油门控制
			{
				throttle = rc.channel[2].radio_out * 2 + compensation;
			}
			else // 定高控制
			{
				throttle += Ctrl(dt);
				compensation = throttle - rc.channel[2].radio_out * 2;
			}
		}
	}

	throttle = LIMIT(throttle, 0, MAX_THR);
	return throttle;
}