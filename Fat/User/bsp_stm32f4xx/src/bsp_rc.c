#include "bsp_rc.h"
#include "bsp_param.h"
#include "bsp_tim_pwm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_gpio.h"
#include "Throttle.h"
#include "bsp_mavlink.h"

#include "task_mavlink.h"
#include "main.h"

Mode mode;			// 控制模式

bool Rudder_Amidships = true; // 舵量回中

RC rc = {	.present = true, .enabled = true, .health = true};

void RC_Calibration(void)
{
	bool first_record_comlete[CHANNEL_MAX] = {false};
	rc.calibrating = true;

	for(uint16_t j=0; j<CALIBRATING_RC_CYCLES; j++)
	{
		for(uint8_t i=0; i<CHANNEL_MAX; i++)
		{
			if(CAPTURE_STA & 1<<i) 
			{
				CAPTURE_STA &= ~(1<<i);
				rc.present = true;
				
				// 记录最大值及最小值
				if(!first_record_comlete[i])
				{
					rc.para[i].radio_max = rc.para[i].radio_min = rc.para[i].radio_mid = Rc_PWMCount[i];	
					first_record_comlete[i] = true;
				} 
				else
				{
					rc.para[i].radio_max = MAX(Rc_PWMCount[i], rc.para[i].radio_max);
					rc.para[i].radio_min = MIN(Rc_PWMCount[i], rc.para[i].radio_min);
				}
				// 确定两端的线性比例系数
				rc.para[i].scale_high = NORMSCALE / (float)(rc.para[i].radio_max - (rc.para[i].radio_mid + DEAD_ZOOM));
				rc.para[i].scale_low =  NORMSCALE / (float)((rc.para[i].radio_mid - DEAD_ZOOM) - rc.para[i].radio_min);
			}
		}
		vTaskDelay(20/portTICK_RATE_MS);
	}
	rc.health = true;
	
	Rudder_Amidships = ABS(rc.para[2].radio_mid - rc.para[2].radio_min) > LOCK_ZOOM;
	
	RC_Para* para[CHANNEL_MAX];
	for(uint8_t i=0; i<CHANNEL_MAX; i++)
	{
		para[i] = &rc.para[i];
		if(para[i]->radio_min > 1100 || para[i]->radio_min < 900 || para[i]->radio_max < 1900 || para[i]->radio_max > 2100)
		{
			rc.health = false;
		}
	}
	if(rc.health)
	{
		Param_SaveRcPara(para);
	}
	else
	{
		Param_TakeRcPara(para);
	}
	rc.calibrating = false;
}

void RC_Init()
{
  if(DEFAULT_RC_CALIBRATE)
	{
		RC_Calibration();
	}
	else
	{
		RC_Para* para[CHANNEL_MAX];
		for(uint8_t i=0; i<CHANNEL_MAX; i++)
		{
				para[i] = &rc.para[i];
		}
		rc.health = Param_TakeRcPara(para);
		Rudder_Amidships = ABS(rc.para[2].radio_mid - rc.para[2].radio_min) > LOCK_ZOOM;
  }
	
	for(int i=0; i<CHANNEL_MAX; i++)
	{
		rc.channel[i].control_in = rc.para[i].radio_mid;
		rc.channel[i].enable = false;
	}
	
//	bool success;
//	uint32_t count = 0;
//	do
//	{
//		success = true;
//		RC_Read(0.002);
//		for(uint8_t i=0; i<CHANNEL_MAX; i++)
//		{
//			if(rc.channel[i].radio_in==0 || rc.channel[i].radio_out != 0)
//				success = false;
//		}
//		if(++count%250 == 0)
//		{
//			LED2_TOGGLE;
//		}
//		vTaskDelay(2/portTICK_RATE_MS);
//	} while(!success);
}

void RC_Read(float dt)
{ 
	// 判断解锁
	if(rc.calibrating)
	{
		LED2_OFF;
		for(int i=0; i<CHANNEL_MAX; i++)
		{
			rc.channel[i].enable = false;
			throttle = 0;
		}
	}
	else 
	{
		for(int i=0; i<CHANNEL_MAX; i++)
		{
			if(CAPTURE_STA & 1<<i) 
			{
				CAPTURE_STA &= ~(1<<i);
				rc.lost[i] = 0;
				if(Rc_PWMCount[i] > 900 && Rc_PWMCount[i] < 2100)
				{
					rc.health = true;
					
					rc.channel[i].radio_in = Rc_PWMCount[i];
					rc.channel[i].control_in += TWO_PI * dt * (rc.channel[i].radio_in - rc.channel[i].control_in) * 10; // 低通滤波 延迟100ms
						
					if(rc.channel[i].control_in > rc.para[i].radio_mid + DEAD_ZOOM)           rc.channel[i].radio_out = (rc.channel[i].control_in - (rc.para[i].radio_mid + DEAD_ZOOM)) * rc.para[i].scale_high;
					else if(rc.channel[i].control_in < rc.para[i].radio_mid - DEAD_ZOOM)      rc.channel[i].radio_out = (rc.channel[i].control_in - (rc.para[i].radio_mid - DEAD_ZOOM)) * rc.para[i].scale_low;
					else                                                              				rc.channel[i].radio_out = 0;			
				}
				else
				{
					rc.health = false;
				}	
			}
			else
			{
				if(rc.lost[i]++ >= 250)
				{
					rc.lost[i] = 250;
					rc.health = false;
					
					LED2_OFF;
					for(int i=0; i<CHANNEL_MAX; i++)
					{
						rc.channel[i].enable = false;
						rc.channel[i].radio_out = 0;
						throttle = 0;
					}
				}
			}
		}
	
		if(mav.system_status == MAV_STATE_STANDBY && rc.channel[2].control_in < rc.para[2].radio_min + LOCK_ZOOM  && rc.channel[3].control_in > rc.para[3].radio_max - LOCK_ZOOM) // 右下角，解锁
		{
			mav.system_status = MAV_STATE_ACTIVE;
			LED2_ON;
		}
	  else if(rc.channel[2].control_in < rc.para[2].radio_min + LOCK_ZOOM  && rc.channel[3].control_in < rc.para[3].radio_min + LOCK_ZOOM) // 左下角，上锁
		{
			LED2_OFF;
			for(int i=0; i<CHANNEL_MAX; i++)
			{
				rc.channel[i].enable = false;
				throttle = 0;
			}
			
			if(mav.system_status != MAV_STATE_CALIBRATING) 
			{
				mav.system_status = MAV_STATE_STANDBY;
				if(rc.channel[1].control_in > rc.para[1].radio_max - LOCK_ZOOM  && rc.channel[0].control_in < rc.para[0].radio_min + LOCK_ZOOM) // 右手左上角，校准加速计
				{
					mavlink_message_t msg;
					mavlink_command_long_t command_long;
					command_long.command = MAV_CMD_PREFLIGHT_CALIBRATION;
					command_long.param1 = 1;
					memcpy(&msg.payload64, &command_long, sizeof(command_long));
					xQueueSendToBack(xMavlinkReceiveQueue, &msg, MAVLINK_WAIT);
				}
				else if(rc.channel[1].control_in > rc.para[1].radio_max - LOCK_ZOOM  && rc.channel[0].control_in < rc.para[0].radio_min + LOCK_ZOOM) // 右手右上角，校准磁力计
				{
					mavlink_message_t msg;
					mavlink_command_long_t command_long;
					command_long.command = MAV_CMD_PREFLIGHT_CALIBRATION;
					command_long.param2 = 1;
					memcpy(&msg.payload64, &command_long, sizeof(command_long));
					xQueueSendToBack(xMavlinkReceiveQueue, &msg, MAVLINK_WAIT);
				}
			}
		}
		
		if((mav.system_status == MAV_STATE_ACTIVE) && rc.channel[2].control_in>rc.para[2].radio_mid-DEAD_ZOOM && rc.channel[2].control_in<rc.para[2].radio_mid+DEAD_ZOOM) // 解锁并且油门回中
		{
			for(int i=0; i<CHANNEL_MAX; i++)
			{
				rc.channel[i].enable = true;
			}
		}
		
		mode.height = 1;
		mode.location = rc.channel[4].control_in > (rc.para[4].radio_max + rc.para[4].radio_min)/2;
		
		if(rc.channel[4].control_in > (rc.para[4].radio_max + rc.para[4].radio_min)/2)		mav.base_mode = MAV_MODE_AUTO_DISARMED | (mav.system_status == MAV_STATE_ACTIVE) << 7;
		else 																																							mav.base_mode = MAV_MODE_GUIDED_DISARMED | (mav.system_status == MAV_STATE_ACTIVE) << 7;
	}	
}

/*********************************** 飞航科技 (END OF FILE) ******************************************/
