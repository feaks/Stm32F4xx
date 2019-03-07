#include "task_mavlink.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bsp_gpio.h"
#include "bsp_adc.h"
#include "bsp_uart.h"
#include "bsp_i2c.h"
#include "bsp_eeprom.h"
#include "bsp_rc.h"
#include "bsp_tim_pwm.h"
#include "bsp_mpu6050.h"
#include "bsp_hmc5883l.h"
#include "bsp_Ultrasonic.h"
#include "bsp_mpl3115.h"
#include "bsp_ms5611.h"
#include "bsp_optical_flow.h"
#include "bsp_mavlink.h"
#include "bsp_usb_vcp.h"

#include "PID_Attitude.h"
#include "PID_Position.h"

#include "config.h"

#include "main.h"

static bool sending = true;
	
void Task_MavLink( void *pvParameters )
{
	static mavlink_message_t msg;
	
	uint32_t t_now;
	TickType_t pxPreviousWakeTime = xTaskGetTickCount();
	TickType_t last_time = pxPreviousWakeTime;
	

	
  while(1)
	{
		vTaskDelay(1 / portTICK_RATE_MS );
		if(sending)
			xSemaphoreTake(xMavlinkSendBinarySemaphore, portMAX_DELAY );

		sending = false;
		
		t_now = xTaskGetTickCount() * portTICK_RATE_MS;

		if(t_now - mavlink_status.last_receive_time < 2000)
		{
			static uint32_t t_last_msg_26, t_last_msg_27, t_last_msg_30, t_last_msg_29, tlast_msg_186, t_last_msg_34, t_last_msg_35, t_last_msg_187, t_last_msg_2, t_last_msg_0, t_last_msg_24, t_last_msg_36, t_last_msg_186, t_last_msg_188;

			sending = true;
			if(mavlink.ack[COMMAND_ACK]) // 返回应答
			{
				mavlink_msg_command_ack_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, mavlink.mavlink_command_ack.command, mavlink.mavlink_command_ack.result);
				mavlink.ack[COMMAND_ACK] = false;
			}
			else if(mavlink.ack[SET_PID]) // 返回PID值
			{
				mavlink_msg_set_pid_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, mavlink.mavlink_set_pid.number, mavlink.mavlink_set_pid.PID_P, mavlink.mavlink_set_pid.PID_I, mavlink.mavlink_set_pid.PID_D);
				mavlink.ack[SET_PID] = false;
			}
			else if(t_now - t_last_msg_30 >= MAVLINK_MESSAGE_30) // 三轴角度、角速度 30ms
			{
				t_last_msg_30 = t_now;
				mavlink_msg_attitude_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, status.attitude.roll, status.attitude.pitch, -status.attitude.yaw, mpu6050.gyro.x, mpu6050.gyro.y, -mpu6050.gyro.x);
			}
			else if(t_now - t_last_msg_26 >= MAVLINK_MESSAGE_26) // 9DOF 物理量 30ms
			{
				t_last_msg_26 = t_now;
				mavlink_msg_scaled_imu_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mpu6050.acc.x*G2MG, mpu6050.acc.y*G2MG, mpu6050.acc.z*G2MG, mpu6050.gyro.x*RAD2MILLIRAD, mpu6050.gyro.y*RAD2MILLIRAD, mpu6050.gyro.z*RAD2MILLIRAD, hmc5883l.mag.x*GS2MGS/HMC5583l_GAIN, hmc5883l.mag.y*GS2MGS/HMC5583l_GAIN, hmc5883l.mag.z*GS2MGS/HMC5583l_GAIN);
			} 
			else if(!rc.calibrating && t_now - t_last_msg_34 >= MAVLINK_MESSAGE_34) // 遥控输入值 40ms
			{
				t_last_msg_34 = t_now;
				mavlink_msg_rc_channels_scaled_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, 0x01, (int16_t)rc.channel[0].radio_out * 20, (int16_t)rc.channel[1].radio_out * 20, (int16_t)rc.channel[2].radio_out * 20, (int16_t)rc.channel[3].radio_out * 20, (int16_t)rc.channel[4].radio_out * 20, (int16_t)rc.channel[5].radio_out * 20, (int16_t)rc.channel[6].radio_out * 20, (int16_t)rc.channel[7].radio_out * 20, 100);
			}
			else if(rc.calibrating && t_now - t_last_msg_35 >= MAVLINK_MESSAGE_35 ) // 遥控输入原始值 40ms
			{
				t_last_msg_35 = t_now;
				mavlink_msg_rc_channels_raw_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, 0x01, Rc_PWMCount[0], Rc_PWMCount[1], Rc_PWMCount[2], Rc_PWMCount[3], Rc_PWMCount[4], Rc_PWMCount[5], Rc_PWMCount[6], Rc_PWMCount[7], 100);
			}
			else if(t_now - t_last_msg_36 > MAVLINK_MESSAGE_36) // 电机输出
			{
				t_last_msg_36 = t_now;
				mavlink_msg_servo_output_raw_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, 0x01, motor.motor_pwm.motor_1/PWM_RADIO, motor.motor_pwm.motor_2/PWM_RADIO, motor.motor_pwm.motor_3/PWM_RADIO, motor.motor_pwm.motor_4/PWM_RADIO, 0, 0, 0, 0);
			}
			else if(hmc5883l.calibrating && t_now - t_last_msg_187 >= MAVLINK_MESSAGE_187) // 磁力计校准数据
			{
				t_last_msg_187 = t_now;
				mavlink_msg_calibration_magmetometer_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, hmc5883l.mag_min.x, hmc5883l.mag_max.x, hmc5883l.mag_min.y, hmc5883l.mag_max.y, hmc5883l.mag_min.z, hmc5883l.mag_max.z);
			}
			else if(mavlink.mavlink_get_pid.number != 0 && mavlink.mavlink_get_pid.number != 0xFFFFFFFF && t_now - t_last_msg_188 >= MAVLINK_MESSAGE_188) // PID OUT
			{
				t_last_msg_188 = t_now;
				switch(mavlink.mavlink_get_pid.number)
				{
					case STABLE_ROLL:		mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_stable.expect.roll,  status.attitude.roll,  ctrl_stable.err.roll,  ctrl_stable.err_filter.roll,  ctrl_stable.err_p.roll,  ctrl_stable.err_i.roll,  ctrl_stable.err_d.roll,  ctrl_stable.out.roll);	break;
					case STABLE_PITCH:	mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_stable.expect.pitch, status.attitude.pitch, ctrl_stable.err.pitch, ctrl_stable.err_filter.pitch, ctrl_stable.err_p.pitch, ctrl_stable.err_i.pitch, ctrl_stable.err_d.pitch, ctrl_stable.out.pitch);	break;
					case STABLE_YAW:		mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_stable.expect.yaw,   status.attitude.yaw,   ctrl_stable.err.yaw,   ctrl_stable.err_filter.yaw,   ctrl_stable.err_p.yaw,   ctrl_stable.err_i.yaw,   ctrl_stable.err_d.yaw,   ctrl_stable.out.yaw);	  break;
					case RATE_ROLL:			mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_rate.expect.roll,    mpu6050.gyro.y, 				ctrl_rate.err.roll,    ctrl_rate.err_filter.roll,    ctrl_rate.err_p.roll,    ctrl_rate.err_i.roll,    ctrl_rate.err_d.roll,    ctrl_rate.out.roll);	  break;
					case RATE_PITCH:		mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_rate.expect.pitch,   mpu6050.gyro.x, 				ctrl_rate.err.pitch,   ctrl_rate.err_filter.pitch,   ctrl_rate.err_p.pitch,   ctrl_rate.err_i.pitch,   ctrl_rate.err_d.pitch,   ctrl_rate.out.pitch);	  break;
					case RATE_YAW:			mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_rate.expect.yaw,     mpu6050.gyro.z, 				ctrl_rate.err.yaw,     ctrl_rate.err_filter.yaw,     ctrl_rate.err_p.yaw,     ctrl_rate.err_i.yaw,     ctrl_rate.err_d.yaw,     ctrl_rate.out.yaw);		  break;
					case POSITION_X:		mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_position.expect.x,   status.position.x, 		ctrl_position.err.x, 	 ctrl_position.err_filter.x,   ctrl_position.err_p.x,   ctrl_position.err_i.x, 	 ctrl_position.err_d.x, 	ctrl_position.out.x);	  break;
					case POSITION_Y:		mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_position.expect.y,   status.position.y, 		ctrl_position.err.y, 	 ctrl_position.err_filter.y,   ctrl_position.err_p.y,   ctrl_position.err_i.y, 	 ctrl_position.err_d.y, 	ctrl_position.out.y);	  break;
					case POSITION_Z:		mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_position.expect.z,   status.position.z, 		ctrl_position.err.z, 	 ctrl_position.err_filter.z,   ctrl_position.err_p.z,   ctrl_position.err_i.z, 	 ctrl_position.err_d.z, 	ctrl_position.out.z);	  break;
					case SPEED_X:				mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_speed.expect.x,      status.speed.x, 				ctrl_speed.err.x, 		 ctrl_speed.err_filter.x, 		 ctrl_speed.err_p.z,      ctrl_speed.err_i.x, 		 ctrl_speed.err_d.x, 			ctrl_speed.out.x);		  break;
					case SPEED_Y:				mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_speed.expect.y,      status.speed.y, 				ctrl_speed.err.y, 		 ctrl_speed.err_filter.y, 		 ctrl_speed.err_p.z,      ctrl_speed.err_i.y, 		 ctrl_speed.err_d.y, 			ctrl_speed.out.y);		  break;
					case SPEED_Z:				mavlink_msg_debugging_pid_parameters_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mavlink.mavlink_get_pid.number, ctrl_speed.expect.z,      status.speed.z, 				ctrl_speed.err.z, 		 ctrl_speed.err_filter.z, 		 ctrl_speed.err_p.z,      ctrl_speed.err_i.z, 		 ctrl_speed.err_d.z, 			ctrl_speed.out.z);		  break;
					default : sending = false;
				}
			}
			else if(t_now - t_last_msg_27 >= MAVLINK_MESSAGE_27) // 9DOF 原始值 500ms
			{
				t_last_msg_27 = t_now;
				mavlink_msg_raw_imu_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mpu6050.acc_raw.x, mpu6050.acc_raw.y, mpu6050.acc_raw.z, mpu6050.gyro_raw.x, mpu6050.gyro_raw.y, mpu6050.gyro_raw.z, hmc5883l.mag_raw.x, hmc5883l.mag_raw.y, hmc5883l.mag_raw.z);
			}
			else if(t_now - t_last_msg_29 >= MAVLINK_MESSAGE_29) // 气压高度 500ms
			{
				t_last_msg_29 = t_now;
				mavlink_msg_scaled_pressure_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, mpl3115.altitube_raw, 101325, mpl3115.temperature_raw);
			}
			else if(t_now - t_last_msg_186 >= MAVLINK_MESSAGE_186) // 超声波高度 500ms
			{
				t_last_msg_186 = t_now;
				mavlink_msg_scaled_ultrasonic_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, ultra.distance*M2MM);
			}
			else if(t_now - t_last_msg_2 >= MAVLINK_MESSAGE_2) // 电池 1000ms
			{
				t_last_msg_2 = t_now;
				// uint32_t onboard_control_sensors_present, uint32_t onboard_control_sensors_enabled, uint32_t onboard_control_sensors_health, uint16_t load, uint16_t voltage_battery, int16_t current_battery, int8_t battery_remaining, uint16_t drop_rate_comm, uint16_t errors_comm, uint16_t errors_count1, uint16_t errors_count2, uint16_t errors_count3, uint16_t errors_count4
				mavlink_msg_sys_status_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, 
				(hmc5883l.present ? MAV_SYS_STATUS_SENSOR_3D_MAG : 0) | (mpu6050.present ? MAV_SYS_STATUS_SENSOR_3D_GYRO : 0) | (mpu6050.present ? MAV_SYS_STATUS_SENSOR_3D_ACCEL : 0) | (ultra.present ? MAV_SYS_STATUS_SENSOR_LASER_POSITION : 0) | (mpl3115.present ? MAV_SYS_STATUS_SENSOR_ABSOLUTE_PRESSURE : 0) | (motor.enable ? MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS : 0) | (rc.present ? MAV_SYS_STATUS_SENSOR_RC_RECEIVER : 0), 
				(hmc5883l.enabled ? MAV_SYS_STATUS_SENSOR_3D_MAG : 0) | (mpu6050.enabled ? MAV_SYS_STATUS_SENSOR_3D_GYRO : 0) | (mpu6050.enabled ? MAV_SYS_STATUS_SENSOR_3D_ACCEL : 0) | (ultra.enabled ? MAV_SYS_STATUS_SENSOR_LASER_POSITION : 0) | (mpl3115.enabled ? MAV_SYS_STATUS_SENSOR_ABSOLUTE_PRESSURE : 0) | (motor.enable ? MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS : 0) | (rc.enabled ? MAV_SYS_STATUS_SENSOR_RC_RECEIVER : 0),
				(hmc5883l.health  ? MAV_SYS_STATUS_SENSOR_3D_MAG : 0) | (mpu6050.health  ? MAV_SYS_STATUS_SENSOR_3D_GYRO : 0) | (mpu6050.health  ? MAV_SYS_STATUS_SENSOR_3D_ACCEL : 0) | (ultra.health  ? MAV_SYS_STATUS_SENSOR_LASER_POSITION : 0) | (mpl3115.health  ? MAV_SYS_STATUS_SENSOR_ABSOLUTE_PRESSURE : 0) | (motor.enable ? MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS : 0) | (rc.health  ? MAV_SYS_STATUS_SENSOR_RC_RECEIVER : 0),
				(uint16_t)(1000 - idlePercent*10), (uint16_t)(battery.voltage*1000), (int16_t)(battery.current*100), 0, mavlink_status.signal_percent, 0, 0, 0, 0, 0); // battery.voltage*10 为电压V
			}
			else if(t_now - t_last_msg_0 >= MAVLINK_MESSAGE_0) // 心跳包
			{
				t_last_msg_0 = t_now;
				mavlink_msg_heartbeat_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_GENERIC, mav.base_mode, mav.custom_mode, mav.system_status);
			}
			else if(t_now - t_last_msg_24 >= MAVLINK_MESSAGE_24) // GPS
			{
				t_last_msg_24 = t_now;
				mavlink_msg_gps_raw_int_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, t_now, gps.state,  gps.lat*10000000, gps.lon*10000000,  gps.alt,  gps.hdop, 0,  gps.speed,  gps.degree,  gps.num_of_sv);
			}
			else 
			{
				sending = false;
			}
			if(sending) 
			{
				if(usb_vcp.usbd_status == USBD_Status_Configured || usb_vcp.usbd_status == USBD_Status_Connected)
				{
					mavlink_send_with_usb(&msg);
				}
				else
				{
					mavlink_send_with_dma(&msg);
				}
			}
		}
		else
		{
			
		}
	}
	vTaskDelete(NULL);
}