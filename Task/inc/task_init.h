#ifndef __TASK_INIT_H_
#define __TASK_INIT_H_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "bsp_lan8720.h"
#include "netconf.h"


#include "task_dhcp.h"


void task_init( void *pvParameters );



#endif
