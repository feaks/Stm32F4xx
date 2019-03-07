#include "task_30ms.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "bsp_Ultrasonic.h"
#include "bsp_mpl3115.h"

#include "PID_Position.h"

#include "config.h"

/*************************************************************************************
*	函 数 名: void Task_30ms( void *pvParameters )                                     *
*	功能说明: 周期30ms的任务		                                                       *
*	形    参：pvParameters 是在创建该任务时传递的形参                                  *
*	返 回 值: 无                                                                       *
*************************************************************************************/
void Task_30ms( void *pvParameters )
{
	vSemaphoreCreateBinary(xUltrasonicReceiveBinarySemaphore);
	xSemaphoreTake(xUltrasonicReceiveBinarySemaphore, portMAX_DELAY );
	float dt;
	TickType_t pxPreviousWakeTime = xTaskGetTickCount();
	TickType_t last_time = pxPreviousWakeTime;
	while(1)
	{
		vTaskDelayUntil(&pxPreviousWakeTime,  30 / portTICK_RATE_MS );
		/**
		 * 气压计定高
		 *		1.超声波数据丢失
		 *    2.上次定高数据来自气压计，并且当前高度大于1.5米
		 *    3.上次定高数据来自超声波，并且当前高度大于2米
		 */
		if(!ultra.health || (height.source == 2 && ultra.distance > 1.5) || (height.source == 1 && ultra.distance > 2)) 
		{
			MPL3115_Update(dt);
			Ultra_Read();
			xSemaphoreTake(xUltrasonicReceiveBinarySemaphore, ULTRASONIC_WAIT );
			dt = (pxPreviousWakeTime - last_time) * portTICK_RATE_MS * MS2S;
			last_time = pxPreviousWakeTime;	
			Ultra_Calculate_Height(dt);
		}
		/**
		 * 超声波定高
		 *		1.上次定高数据来自超声波，并且当前高度小于2米
		 *		2.上次定高数据来自气压计，并且当前高度小于1.5米
		 *		3.上次定高数据未知，并且当前高度小于2米，尝试使用超声波定高
		 */
		else if((((height.source == 0) || (height.source == 1)) && ultra.distance < 2) || (height.source == 2 && ultra.distance < 1.5))
		{
			Ultra_Read();
			xSemaphoreTake(xUltrasonicReceiveBinarySemaphore, ULTRASONIC_WAIT );
			dt = (pxPreviousWakeTime - last_time) * portTICK_RATE_MS * MS2S;
			last_time = pxPreviousWakeTime;	
			Ultra_Calculate_Height(dt);
			Ultra_Fusion_Height(dt);
			MPL3115_Read_Altitube(dt); // 实时更新气压计与超声波误差
		}
	}
	vTaskDelete(NULL);		
}