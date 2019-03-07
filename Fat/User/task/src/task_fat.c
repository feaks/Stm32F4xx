#include "task_fat.h"

#include "main.h"
#include "mqtt.h"

#define aaTopic  "this is aa topic"
#define stm32Topic  "this is stm32 topic"
extern void reconnect_subscriber(void);
extern void publish_msg(struct mqtt_client* client,char * topic,char *msg,uint32_t len);
extern void simple_publish(void);
/*************************************************************************************
* 函 数 名: void Task_Touch( void *pvParameters )                                    *
* 功能说明: 周期2ms的任务                                                            *
* 形    参：pvParameters 是在创建该任务时传递的形参                                  *
* 返 回 值: 无                                                                       *
*************************************************************************************/

void Task_Fat(void *pvParameters)                                                                                                                                           
{
	TickType_t pxPreviousWakeTime = xTaskGetTickCount();
	TickType_t last_time = pxPreviousWakeTime;

	while( FreeRTOS_IsNetworkUp()== pdFALSE)
	{
		printf("wait net up\n");
		vTaskDelay(1000);
	}
	simple_publish();
//	reconnect_subscriber();
	while(1)
	{	
//		publish_msg(NULL,"aa",aaTopic,strlen(aaTopic) + 1);
//		publish_msg(NULL,"stm32",stm32Topic,strlen(stm32Topic) + 1);
		vTaskDelayUntil(&pxPreviousWakeTime, 3000 / portTICK_RATE_MS );
	}
	vTaskDelete(NULL);
}
