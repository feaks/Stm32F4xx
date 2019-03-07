#include "task_init.h"
#include "main.h"
#include "task_gui.h"
#include "task_touch.h"
#include "task_fat.h"

extern K_ModuleItem_Typedef  picture_player_board;
extern K_ModuleItem_Typedef  vedio_player_board;

/* The default IP and MAC address used by the demo.  The address configuration
defined here will be used if ipconfigUSE_DHCP is 0, or if ipconfigUSE_DHCP is
1 but a DHCP server could not be contacted.  See the online documentation for
more information.  http://www.FreeRTOS.org/tcp */
static const uint8_t ucIPAddress[ 4 ] = { 192, 168, 31, 226 };
static const uint8_t ucNetMask[ 4 ] = { 255, 255, 255, 0 };
static const uint8_t ucGatewayAddress[ 4 ] = { 192, 168, 31, 1 };
static const uint8_t ucDNSServerAddress[ 4 ] = { 192, 168, 31, 1 };

/* Default MAC address configuration. */
static const uint8_t ucMACAddress[ 6 ] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };


TaskHandle_t Task_MavLinkHandler, Task_LogHandler, Task_ShowStackHandler, Task_GUIHandler;
TaskHandle_t Task_TouchHandler,Task_FATHandler;
xQueueHandle xMavlinkReceiveQueue;

static void AppTaskCreate (void)
{
	xTaskCreate(Task_GUI,"Task_GUI",4*1024, NULL, 0,   &Task_GUIHandler );    // 100	
//	xTaskCreate(Task_Fat,"Task_GUI",2048, NULL, 2,   &Task_FATHandler );
}
extern void Hmac(void);
void Task_Init(void *pvParameters)
{

	k_BspInit(); 
  	
	/* Add Modules*/
  k_ModuleInit();
	
 /* Link modules */ 
  k_ModuleAdd(&picture_player_board); 
	k_ModuleAdd(&vedio_player_board); 
	

	/***NOTE*** Tasks that use the network are created in the network event hook 
    when the network is connected and ready for use (see the definition of 
    vApplicationIPNetworkEventHook() below).  The address values passed in here 
    are used if ipconfigUSE_DHCP is set to 0, or if ipconfigUSE_DHCP is set to 1 
    but a DHCP server cannot be contacted. */ 
//	FreeRTOS_IPInit( ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress );
	
	AppTaskCreate();

	vTaskDelete(NULL);
}
