
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"

#include "task_init.h"



static void AppTaskCreate (void);

int main( void )
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	AppTaskCreate();
	
  vTaskStartScheduler();
	
	for(;;)
	{
		
	}
}


static void AppTaskCreate (void)
{
	/* Create one task. */
    xTaskCreate(    task_init,     /* Pointer to the function that implements the task.              */
                    "task_init",   /* Text name for the task.  This is to facilitate debugging only. */
                    200,        /* Stack depth in words.                                          */
                    NULL,       /* We are not using the task parameter.                           */
                    1,          /* This task will run at priority 1.                              */
                    NULL );     /* We are not using the task handle.                              */

}

