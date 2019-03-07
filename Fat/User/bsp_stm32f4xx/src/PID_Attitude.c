#include "PID_Attitude.h"
#include "bsp_rc.h"
#include "bsp_mpu6050.h"
#include "Throttle.h"
#include "PID_Position.h"
#include "bsp_gps.h"
#include "bsp_mavlink.h"
#include "bsp_param.h"

CTRL_Attitude ctrl_rate, ctrl_stable; 	// 姿态速率、速度控制

void PID_Attitude_Param_Init()					// 设置默认参数
{
	if(!Param_TakePID(STABLE_ROLL_ADDR,  &ctrl_stable.pid.roll))
	{
		#ifdef QV250
		ctrl_stable.pid.roll.kp = 8;
		ctrl_stable.pid.roll.ki = 0.5;
		ctrl_stable.pid.roll.kd = 0.1;
		#elif defined F450
		ctrl_stable.pid.roll.kp = 10;
		ctrl_stable.pid.roll.ki = 3;
		ctrl_stable.pid.roll.kd = 0.3f;
		#elif defined F550
		ctrl_stable.pid.roll.kp = 8;
		ctrl_stable.pid.roll.ki = 1;
		ctrl_stable.pid.roll.kd = 0;
		#endif
	}
	
	if(!Param_TakePID(STABLE_PITCH_ADDR, &ctrl_stable.pid.pitch))
	{
		#ifdef QV250
		ctrl_stable.pid.pitch.kp = 8;
		ctrl_stable.pid.pitch.ki = 0.5;
		ctrl_stable.pid.pitch.kd = 0.1;
		#elif defined F450
		ctrl_stable.pid.pitch.kp = 10;
		ctrl_stable.pid.pitch.ki = 3;
		ctrl_stable.pid.pitch.kd = 0.3f;
		#elif defined F550
		ctrl_stable.pid.pitch.kp = 8;
		ctrl_stable.pid.pitch.ki = 1;
		ctrl_stable.pid.pitch.kd = 0;
		#endif
	}
	if(!Param_TakePID(STABLE_YAW_ADDR,   &ctrl_stable.pid.yaw))
	{
		#ifdef QV250
		ctrl_stable.pid.yaw.kp = 8;
		ctrl_stable.pid.yaw.ki = 0.5;
		ctrl_stable.pid.yaw.kd = 0;
		#elif defined F450
		ctrl_stable.pid.yaw.kp = 10;
		ctrl_stable.pid.yaw.ki = 3;
		ctrl_stable.pid.yaw.kd = 0.3f;
		#elif defined F550
		ctrl_stable.pid.yaw.kp = 8;
		ctrl_stable.pid.yaw.ki = 1;
		ctrl_stable.pid.yaw.kd = 0;
		#endif
	}
	if(!Param_TakePID(RATE_ROLL_ADDR,    &ctrl_rate.pid.roll))
	{
		#ifdef QV250
		ctrl_rate.pid.roll.kp = 35;
		ctrl_rate.pid.roll.ki = 10;
		ctrl_rate.pid.roll.kd = 1;
		#elif defined F450
		ctrl_rate.pid.roll.kp = 35;
		ctrl_rate.pid.roll.ki = 15;
		ctrl_rate.pid.roll.kd = 2;
		#elif defined F550
		ctrl_rate.pid.roll.kp = 40;
		ctrl_rate.pid.roll.ki = 10;
		ctrl_rate.pid.roll.kd = 2;	
		#endif
	}
	if(!Param_TakePID(RATE_PITCH_ADDR,   &ctrl_rate.pid.pitch))
	{
		#ifdef QV250
		ctrl_rate.pid.pitch.kp = 35;
		ctrl_rate.pid.pitch.ki = 10;
		ctrl_rate.pid.pitch.kd = 1;
		#elif defined F450
		ctrl_rate.pid.pitch.kp = 35;
		ctrl_rate.pid.pitch.ki = 15;
		ctrl_rate.pid.pitch.kd = 2;	
		#elif defined F550
		ctrl_rate.pid.pitch.kp = 40;
		ctrl_rate.pid.pitch.ki = 10;
		ctrl_rate.pid.pitch.kd = 2;
		#endif	
	}
	if(!Param_TakePID(RATE_YAW_ADDR,     &ctrl_rate.pid.yaw))
	{
		#ifdef QV250
		ctrl_rate.pid.yaw.kp = 100;
		ctrl_rate.pid.yaw.ki = 10;
		ctrl_rate.pid.yaw.kd = 1;
		#elif defined F450
		ctrl_rate.pid.yaw.kp = 300;
		ctrl_rate.pid.yaw.ki = 30;
		ctrl_rate.pid.yaw.kd = 3;	
		#elif defined F550
		ctrl_rate.pid.yaw.kp = 200;
		ctrl_rate.pid.yaw.ki = 20;
		ctrl_rate.pid.yaw.kd = 2;	
		#endif	
	}
}

void stable_loop(float Dt)
{
	float limit;
	static GPS_POINT *tail;  
	static int index;  
	/* 计算期望角度 */
	if(throttle > 200) 
	{
//		if(mode.location) // 位置控制
//		{
//			local.lat = piont.lat ;
//			local.lon = piont.lon ;
////			if(route_head!=NULL)    
////			{
////				tail = route_head;
////				for(index=0; index<gps_route_num; )
////				{
////					local.lat = route_head->lat;
////					local.lon = route_head->lon;
////					if(gps.lon) // 到达指定点，切换到下一个点
////					{
////						tail = tail->next;
////						index++;
////					}
////				}
////			}
//			ctrl_stable.expect.roll  =  LIMIT(  ctrl_position.out.x,  -MAX_CTRL_ANGLE/5, MAX_CTRL_ANGLE/5 );//MAX_CTRL_ANGLE  * (  rc[0].radio_out / NORMSCALE ); // LIMIT(  ctrl_speed.out.x,  -MAX_CTRL_ANGLE, MAX_CTRL_ANGLE );
//			ctrl_stable.expect.pitch =  LIMIT( -ctrl_position.out.y,  -MAX_CTRL_ANGLE/5, MAX_CTRL_ANGLE/5 );
//		}
//		else  // 姿态控制
//		{
			local.lat = gps.lat;
			local.lon = gps.lon;
			piont.lat = gps.lat;
			piont.lon = gps.lon;
			ctrl_stable.expect.roll  =  MAX_CTRL_ANGLE  * (  rc.channel[0].radio_out / NORMSCALE );
			ctrl_stable.expect.pitch =  MAX_CTRL_ANGLE  * ( -rc.channel[1].radio_out / NORMSCALE );
//		}

		if(rc.channel[3].radio_out)
		{
			ctrl_stable.expect.yaw = status.attitude.yaw + MAX_CTRL_YAW_SPEED *( -rc.channel[3].radio_out / NORMSCALE );
		}
		ctrl_stable.expect.yaw = wrap_around_PI( ctrl_stable.expect.yaw );
	}
	else
	{
		ctrl_stable.expect.roll  = status.attitude.roll;
		ctrl_stable.expect.pitch = status.attitude.pitch;
		ctrl_stable.expect.yaw   = status.attitude.yaw;
	}	

	/* 计算角度误差 */
	ctrl_stable.err.roll  = wrap_around_PI( ctrl_stable.expect.roll  - status.attitude.roll);
	ctrl_stable.err.pitch	= wrap_around_PI( ctrl_stable.expect.pitch - status.attitude.pitch);
	ctrl_stable.err.yaw   = wrap_around_PI( ctrl_stable.expect.yaw   - status.attitude.yaw);
	
	/* 误差限幅 */
	ctrl_stable.err.roll  = LIMIT( ctrl_stable.err.roll,  -MAX_CTRL_ANGLE, MAX_CTRL_ANGLE );
	ctrl_stable.err.pitch = LIMIT( ctrl_stable.err.pitch, -MAX_CTRL_ANGLE, MAX_CTRL_ANGLE );
	ctrl_stable.err.yaw   = LIMIT( ctrl_stable.err.yaw,   -MAX_CTRL_ANGLE, MAX_CTRL_ANGLE );
	
	/* 计算比例项 */
	ctrl_stable.err_p.roll  = ctrl_stable.pid.roll.kp  * ctrl_stable.err.roll;
	ctrl_stable.err_p.pitch = ctrl_stable.pid.pitch.kp * ctrl_stable.err.pitch;
	ctrl_stable.err_p.yaw   = ctrl_stable.pid.yaw.kp   * ctrl_stable.err.yaw;  
	
	/* 计算微分项 */
  ctrl_stable.err_d.roll	= ctrl_stable.pid.roll.kd  * ( ctrl_stable.err.roll  - ctrl_stable.last_err.roll) / Dt;
	ctrl_stable.err_d.pitch	= ctrl_stable.pid.pitch.kd * ( ctrl_stable.err.pitch - ctrl_stable.last_err.pitch) / Dt;
	ctrl_stable.err_d.yaw	  = ctrl_stable.pid.yaw.kd 	 * ( ctrl_stable.err.yaw   - ctrl_stable.last_err.yaw) / Dt;
	
	/* 记录历史数据 */
  ctrl_stable.last_err.roll  = ctrl_stable.err.roll;
	ctrl_stable.last_err.pitch = ctrl_stable.err.pitch;
	ctrl_stable.last_err.yaw   = ctrl_stable.err.yaw;
	
	/* 计算积分项 */
	if(throttle > 200) 
	{
		ctrl_stable.err_i.roll  += ctrl_stable.pid.roll.ki  * ctrl_stable.err.roll  * Dt;
		ctrl_stable.err_i.pitch += ctrl_stable.pid.pitch.ki * ctrl_stable.err.pitch * Dt;
		ctrl_stable.err_i.yaw   += ctrl_stable.pid.yaw.ki   * ctrl_stable.err.yaw   * Dt;
	} 
	else 
	{
		ctrl_stable.err_i.roll  = 0;
		ctrl_stable.err_i.pitch = 0;
		ctrl_stable.err_i.yaw   = 0;
	}
	
	/* 积分限幅 */
	ctrl_stable.err_i.roll  = LIMIT( ctrl_stable.err_i.roll,  -0.4, 0.4 );
	ctrl_stable.err_i.pitch = LIMIT( ctrl_stable.err_i.pitch, -0.4, 0.4 );
	ctrl_stable.err_i.yaw   = LIMIT( ctrl_stable.err_i.yaw,   -0.4, 0.4 );
	
	/* 角度PID输出 */
	ctrl_stable.out.roll	= ctrl_stable.err_p.roll  + ctrl_stable.err_d.roll  + ctrl_stable.err_i.roll;	
	ctrl_stable.out.pitch	= ctrl_stable.err_p.pitch + ctrl_stable.err_d.pitch + ctrl_stable.err_i.pitch;  
	ctrl_stable.out.yaw	  = ctrl_stable.err_p.yaw   + ctrl_stable.err_d.yaw   + ctrl_stable.err_i.yaw;	
}

void rate_loop(float Dt)  // 期望角速度 1.74rad/s  ~ 5.23rad/s
{
	/* 期望角速度限幅 */
	ctrl_rate.expect.roll	 = LIMIT(ctrl_stable.out.roll,  -MAX_CTRL_ASPEED, MAX_CTRL_ASPEED);    //  rc[0].radio_out/250.0f; // 
	ctrl_rate.expect.pitch = LIMIT(ctrl_stable.out.pitch, -MAX_CTRL_ASPEED, MAX_CTRL_ASPEED);    // -rc[1].radio_out/250.0f; //
	ctrl_rate.expect.yaw	 = LIMIT(ctrl_stable.out.yaw,   -MAX_CTRL_YAW_SPEED, MAX_CTRL_YAW_SPEED);    // -rc[3].radio_out/250.0f; //   
	
	/* 计算误差 */
	ctrl_rate.err.roll	=	ctrl_rate.expect.roll  - mpu6050.gyro.y;
	ctrl_rate.err.pitch	=	ctrl_rate.expect.pitch - mpu6050.gyro.x;
	ctrl_rate.err.yaw   = ctrl_rate.expect.yaw   - mpu6050.gyro.z;
	
	/* 计算比例项 */
	ctrl_rate.err_p.roll  = ctrl_rate.pid.roll.kp  * ctrl_rate.err.roll;
	ctrl_rate.err_p.pitch = ctrl_rate.pid.pitch.kp * ctrl_rate.err.pitch;
	ctrl_rate.err_p.yaw   = ctrl_rate.pid.yaw.kp   * ctrl_rate.err.yaw;
	
	/* 误差滤波 */
	ctrl_rate.err_filter.roll  -= ctrl_rate.err_filter.roll  / 8;
	ctrl_rate.err_filter.pitch -= ctrl_rate.err_filter.pitch / 8;
	ctrl_rate.err_filter.yaw   -= ctrl_rate.err_filter.yaw   / 8;
	
	ctrl_rate.err_filter.roll  += ctrl_rate.err.roll  - ctrl_rate.last_err.roll;
	ctrl_rate.err_filter.pitch += ctrl_rate.err.pitch - ctrl_rate.last_err.pitch;
	ctrl_rate.err_filter.yaw   += ctrl_rate.err.yaw   - ctrl_rate.last_err.yaw;
	
	/* 计算微分项 */
	ctrl_rate.err_d.roll  = ctrl_rate.pid.roll.kd  * (ctrl_rate.err_filter.roll  / 8) / Dt;
	ctrl_rate.err_d.pitch = ctrl_rate.pid.pitch.kd * (ctrl_rate.err_filter.pitch / 8) / Dt;
	ctrl_rate.err_d.yaw   = ctrl_rate.pid.yaw.kd   * (ctrl_rate.err_filter.yaw   / 8) / Dt;
	
	/* 记录历史数据 */
	ctrl_rate.last_err.roll  =  ctrl_rate.err.roll;
	ctrl_rate.last_err.pitch =  ctrl_rate.err.pitch;
	ctrl_rate.last_err.yaw   =  ctrl_rate.err.yaw;
	
	/* 计算积分项 */
	if(throttle > 200) 
	{
		ctrl_rate.err_i.roll  += ctrl_rate.pid.roll.ki  * ctrl_rate.err.roll * Dt;
		ctrl_rate.err_i.pitch += ctrl_rate.pid.pitch.ki * ctrl_rate.err.pitch * Dt;
		ctrl_rate.err_i.yaw   += ctrl_rate.pid.yaw.ki   * ctrl_rate.err.yaw * Dt;
	} 
	else 
	{
		ctrl_rate.err_i.roll  = 0;
		ctrl_rate.err_i.pitch = 0;
		ctrl_rate.err_i.yaw   = 0;
	}
	
	/* 角速度误差积分限幅 */
	ctrl_rate.err_i.roll  = LIMIT( ctrl_rate.err_i.roll,  -0.522f*ctrl_rate.pid.roll.kp,  0.522f*ctrl_rate.pid.roll.kp );
	ctrl_rate.err_i.pitch = LIMIT( ctrl_rate.err_i.pitch, -0.522f*ctrl_rate.pid.pitch.kp, 0.522f*ctrl_rate.pid.pitch.kp );
	ctrl_rate.err_i.yaw   = LIMIT( ctrl_rate.err_i.yaw,   -0.522f*ctrl_rate.pid.yaw.kp,   0.522f*ctrl_rate.pid.yaw.kp );
  
	/* 计算输出量 */
	ctrl_rate.out.roll  = ctrl_rate.err_p.roll  + ctrl_rate.err_i.roll  + ctrl_rate.err_d.roll;
	ctrl_rate.out.pitch = ctrl_rate.err_p.pitch + ctrl_rate.err_i.pitch + ctrl_rate.err_d.pitch;
	ctrl_rate.out.yaw   = ctrl_rate.err_p.yaw   + ctrl_rate.err_i.yaw   + ctrl_rate.err_d.yaw;
}

/*********************************** 飞航科技 (END OF FILE) ******************************************/
