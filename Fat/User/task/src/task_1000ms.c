#include "task_1000ms.h"

#include "FreeRTOS.h"
#include "task.h"
#include "bsp_gpio.h"
#include "bsp_adc.h"
#include "bsp_wdg.h"
#include "bsp_rc.h"

#include "config.h"

#include "bsp_mavlink.h"

#include "main.h"

extern void calculateIdlePercent();

/*************************************************************************************
*	函 数 名: void Task_1000ms( void *pvParameters )                                   *
*	功能说明: 周期1000ms的任务		                                                     *
*	形    参：pvParameters 是在创建该任务时传递的形参                                  *
*	返 回 值: 无                                                                       *
*************************************************************************************/
void Task_1000ms(void *pvParameters)
{
	TickType_t pxPreviousWakeTime = xTaskGetTickCount();
	while(1)
	{
		vTaskDelayUntil(&pxPreviousWakeTime, 1000 / portTICK_RATE_MS );
		
		LED1_TOGGLE;
		
		Get_Battery();	// 电池信息
		
		IWDG_Feed();
		
		calculateIdlePercent();
	}
	vTaskDelete(NULL);		
}
