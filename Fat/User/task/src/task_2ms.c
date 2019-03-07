#include "task_2ms.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bsp_gpio.h"
#include "bsp_rc.h"
#include "bsp_tim_pwm.h"
#include "bsp_mpu6050.h"
#include "bsp_hmc5883l.h"

#include "PID_Attitude.h"

#include "Throttle.h"

#include "config.h"

/*************************************************************************************
* 函 数 名: void Task_2ms( void *pvParameters )                                      *
* 功能说明: 周期2ms的任务                                                            *
* 形    参：pvParameters 是在创建该任务时传递的形参                                  *
* 返 回 值: 无                                                                       *
*************************************************************************************/
void Task_2ms(void *pvParameters)                                                                                                                                           
{
	float dt;
	TickType_t pxPreviousWakeTime = xTaskGetTickCount();
	TickType_t last_time = pxPreviousWakeTime;
	while(1)
	{
		vTaskDelayUntil(&pxPreviousWakeTime, 2 / portTICK_RATE_MS );
		dt = (pxPreviousWakeTime - last_time) * portTICK_RATE_MS * MS2S;
		last_time = pxPreviousWakeTime;
		
		MPU6050_Update(dt);
		
		float throttle = throttle_ctrl(dt);	// 油门控制
		
		rate_loop(dt);			// 内环角速度控制。

		MOTOR_Ctrl(throttle); // 电机输出

		RC_Read( dt);		// 遥控器通道数据处理 ，输入：执行周期，接收机pwm捕获的数据。
	}
	vTaskDelete(NULL);
}
