#include "task_50ms.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bsp_gps.h"

#include "config.h"
#include "math_extend.h"

#include "bsp_mavlink.h"
#include "bsp_usb_vcp.h"

/*************************************************************************************
*	函 数 名: void Task_50ms( void *pvParameters )                                     *
*	功能说明: 周期50ms的任务		                                                       *
*	形    参：pvParameters 是在创建该任务时传递的形参                                  *
*	返 回 值: 无                                                                       *
*************************************************************************************/
void Task_50ms(void *pvParameters)
{
	float dt;
	TickType_t pxPreviousWakeTime = xTaskGetTickCount();
	TickType_t last_time = pxPreviousWakeTime;
	while(1)
	{
		vTaskDelayUntil(&pxPreviousWakeTime,  50 / portTICK_RATE_MS );
		dt = (pxPreviousWakeTime - last_time) * portTICK_RATE_MS * MS2S;
		last_time = pxPreviousWakeTime;
		if(usb_vcp.usbd_status == USBD_Status_Configured || usb_vcp.usbd_status == USBD_Status_Connected)
		{
			mavlink_parse_usb_message();
		}
		else
		{
			mavlink_parse_message();
		}
		
    // px4flow_read();
		nmea_parse(); // GPS数据解析
		
		if(local.lat != 0 && local.lon != 0)
		{
			GPS_Fusion_Position(dt);
		}
	}
	vTaskDelete(NULL);
}