#ifndef _TASK_TOUCH_H_
#define _TASK_TOUCH_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern xSemaphoreHandle xTouchBinarySemaphore;

void Task_Touch(void *pvParameters);

#endif
