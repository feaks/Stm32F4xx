#include "task_touch.h"

#include "bsp_touch.h"

#include "WindowDLG.h"

#include "math_extend.h"

#include "stdio.h"
xSemaphoreHandle xTouchBinarySemaphore;
#include "stm32f4xx_hash.h"
/*************************************************************************************
* 函 数 名: void Task_Touch( void *pvParameters )                                    *
* 功能说明: 周期2ms的任务                                                            *
* 形    参：pvParameters 是在创建该任务时传递的形参                                  *
* 返 回 值: 无                                                                       *
*************************************************************************************/
void Task_Touch(void *pvParameters)                                                                                                                                           
{
	float dt;
	TickType_t pxPreviousWakeTime = xTaskGetTickCount();
	TickType_t last_time = pxPreviousWakeTime;
	GUI_MTOUCH_EVENT Event;
	GUI_MTOUCH_INPUT Input[5];
	uint8_t *pcWriteBuffer =  (uint8_t *)pvPortMalloc(250);
	if(pcWriteBuffer != NULL)
		memset(pcWriteBuffer,0,250);
	while(1)
	{
		// xSemaphoreTake(xTouchBinarySemaphore, portMAX_DELAY );
		
//		vTaskList(pcWriteBuffer);
//		printf("任务名   任务状态  优先级   剩余栈 任务序号\r\n");
//		printf("%s\r\n", pcWriteBuffer); 
		ts.Scan();				// 扫描触摸点
		
		GUI_TOUCH_Exec(); // 执行触摸操作
		
		if(ts.number>0)
		{
			Event.LayerIndex = 0;
			Event.NumPoints = ts.number;

			for(uint8_t i = 0; i<ts.number; i++)
			{
				Input[i].x = ts.point[i].x;
				Input[i].y = ts.point[i].y;
				Input[i].Flags = ts.point[i].flags;
				Input[i].Id = ts.point[i].id;
			}

			GUI_MTOUCH_StoreEvent(&Event, Input);
		}
		vTaskDelayUntil(&pxPreviousWakeTime, 500 / portTICK_RATE_MS );
	}
	vTaskDelete(NULL);
}

















