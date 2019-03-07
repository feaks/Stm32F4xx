#include "task_10ms.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bsp_hmc5883l.h"

#include "config.h"

/*************************************************************************************
*	函 数 名: void Task_10ms( void *pvParameters )                                     *
*	功能说明: 周期10ms的任务	                                                         *
*	形    参：pvParameters 是在创建该任务时传递的形参                                  *
*	返 回 值: 无                                                                       *
**************************************************************************************/
void Task_10ms( void *pvParameters )
{	
	float dt;
	TickType_t pxPreviousWakeTime = xTaskGetTickCount();
	TickType_t last_time = pxPreviousWakeTime;
	while(1)
	{
		vTaskDelayUntil(&pxPreviousWakeTime, 10 / portTICK_RATE_MS );
		dt = (pxPreviousWakeTime - last_time) * portTICK_RATE_MS * MS2S;
		last_time = pxPreviousWakeTime;
		
		HMC5883L_Update(dt);			//获取电子罗盘数据	
	}
	vTaskDelete(NULL);
}
