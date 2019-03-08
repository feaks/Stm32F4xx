

#include "task_init.h"

#include "bsp_uart.h"

#include "bsp_hash.h"

#include "bsp_random.h"
								
void task_init( void *pvParameters )
{
	UART1_Init(115200);
	Random_Init();
	BSP_hash_sha1_Init();
	ETH_BSP_Config();
	LwIP_Init();
  for(;;)
  {
		xTaskCreate(task_DHCP, "DHCP", configMINIMAL_STACK_SIZE * 8, NULL,4, NULL);
		
		vTaskDelete(NULL);
  }
}

