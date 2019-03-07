#include "task_misc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <stdio.h>
#include <string.h>

#include "task_init.h"

#include "config.h"

#include "stm32f4xx_conf.h"

xQueueHandle logQueue;

void Task_Log(void *pvParameters)
{ 
	uint8_t buf[LOG_MAX_LENGTH];
  while(1)
	{
	  xQueueReceive( logQueue, buf, portMAX_DELAY );
		printf("%s", buf);
		memset(buf, 0, LOG_MAX_LENGTH);
	}
	 vTaskDelete(NULL);
}

int a[9];
void Task_ShowStack(void *pvParameters)
{
  TickType_t pxPreviousWakeTime = xTaskGetTickCount();	
	while(1)
	{
		vTaskDelayUntil(&pxPreviousWakeTime,3000 /portTICK_RATE_MS );
		assert_param(
					 (uxTaskGetStackHighWaterMark(Task_2msHandler) > 20) \
				&& (uxTaskGetStackHighWaterMark(Task_5msHandler) > 20) \
				&& (uxTaskGetStackHighWaterMark(Task_10msHandler) > 20) \
				&& (uxTaskGetStackHighWaterMark(Task_30msHandler) > 20) \
				&& (uxTaskGetStackHighWaterMark(Task_50msHandler) > 20) \
				&& (uxTaskGetStackHighWaterMark(Task_1000msHandler) > 20) \
				&& (uxTaskGetStackHighWaterMark(Task_MavLinkHandler) > 20) \
				&& (uxTaskGetStackHighWaterMark(Task_LogHandler) > 20) \
				&& (uxTaskGetStackHighWaterMark(Task_ShowStackHandler) > 20) \
		);
		
			a[0] = (int)uxTaskGetStackHighWaterMark(Task_2msHandler);
			a[1] = (int)uxTaskGetStackHighWaterMark(Task_5msHandler);
			a[2] = (int)uxTaskGetStackHighWaterMark(Task_10msHandler);
			a[3] = (int)uxTaskGetStackHighWaterMark(Task_30msHandler);
			a[4] = (int)uxTaskGetStackHighWaterMark(Task_50msHandler);
			a[5] = (int)uxTaskGetStackHighWaterMark(Task_1000msHandler);
			a[6] = (int)uxTaskGetStackHighWaterMark(Task_MavLinkHandler);
			a[7] = (int)uxTaskGetStackHighWaterMark(Task_LogHandler);
			a[8] = (int)uxTaskGetStackHighWaterMark(Task_ShowStackHandler);
//		printf("%d  %d  %d  %d  %d  %d  %d  %d  %d  %lld\r\n",\
//		(int)uxTaskGetStackHighWaterMark(Task_2msHandler), \
//		(int)uxTaskGetStackHighWaterMark(Task_5msHandler), \
//		(int)uxTaskGetStackHighWaterMark(Task_10msHandler), \
//		(int)uxTaskGetStackHighWaterMark(Task_30msHandler), \
//		(int)uxTaskGetStackHighWaterMark(Task_50msHandler), \
//		(int)uxTaskGetStackHighWaterMark(Task_1000msHandler), \
//    (int)uxTaskGetStackHighWaterMark(Task_MavLinkHandler), \
//		(int)uxTaskGetStackHighWaterMark(Task_LogHandler), \
//		(int)uxTaskGetStackHighWaterMark(Task_ShowStackHandler), \
//		ulIdleCycleCount);
	};
  vTaskDelete(NULL);
}