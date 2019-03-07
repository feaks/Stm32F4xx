#ifndef _TASK_INIT_H_
#define _TASK_INIT_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern TaskHandle_t Task_2msHandler, Task_5msHandler, Task_10msHandler, Task_30msHandler, Task_50msHandler, Task_1000msHandler, Task_MavLinkHandler, Task_LogHandler, Task_ShowStackHandler;

void Task_Init(void *pvParameters);
	
#endif
