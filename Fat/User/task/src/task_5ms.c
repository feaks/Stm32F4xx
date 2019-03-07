#include "task_5ms.h"

#include "FreeRTOS.h"
#include "task.h"

#include "PID_Attitude.h"

#include "config.h"

/*************************************************************************************
*	函 数 名: void Task_5ms( void *pvParameters )                                      *
*	功能说明: 周期5ms的任务	                                                           *
*	形    参：pvParameters 是在创建该任务时传递的形参                                  *
*	返 回 值: 无                                                                       *
*************************************************************************************/
void Task_5ms( void *pvParameters )
{
	float dt;
	TickType_t pxPreviousWakeTime = xTaskGetTickCount();
	TickType_t last_time = pxPreviousWakeTime;
	while(1)
	{		
		vTaskDelayUntil(&pxPreviousWakeTime, 5 / portTICK_RATE_MS );
		dt = (pxPreviousWakeTime - last_time) * portTICK_RATE_MS * MS2S;
		last_time = pxPreviousWakeTime;

		AHRS(dt); // 姿态更新

		stable_loop(dt); //外环控制
	}
	vTaskDelete(NULL);
}