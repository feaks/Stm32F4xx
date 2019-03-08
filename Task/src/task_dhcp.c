#include "task_dhcp.h"

#include "Ali_iot.h"

extern struct netif gnetif;

void task_DHCP( void *pvParameters )
{
		uint8_t times1=0;
		uint8_t state=0;
		while(1)
		{
			vTaskDelay(500/portTICK_PERIOD_MS);
			dhcp_fine_tmr();
			times1++;
			if(times1 == 120)
			{
				times1 =0;
				dhcp_coarse_tmr();
			}
			
			if(gnetif.ip_addr.addr !=0 && state==0)
			{
				state =1;
				Ali_Mqtt_Connect();
			}
		}
}
