#ifndef __TASK_DHCP_H_
#define __TASK_DHCP_H_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "bsp_lan8720.h"
#include "netconf.h"



void task_DHCP( void *pvParameters );


#endif
