/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "task_gui.h"
#include "timers.h"
#include "k_module.h"
/* External variables --------------------------------------------------------*/

extern void alphatest(void);
extern void k_StorageInit(void);
extern void MainTask(void);
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void vTimerCallback(void * pvParameters);
/* Private functions ---------------------------------------------------------*/

/*************************************************************************************
* 函 数 名: void Task_GUI( void *pvParameters )                                      *
* 功能说明: 周期2ms的任务                                                            *
* 形    参：pvParameters 是在创建该任务时传递的形参                                  * 
*************************************************************************************\
*/
void Task_GUI(void *pvParameters)                                                                                                                                           
{
	TimerHandle_t xTimerTS;
	/* fat init */
	k_StorageInit();
	
	GUI_Init();												// GUI初始化

	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();   

	GUI_SetLayerVisEx (1, 0);
	GUI_SelectLayer(0);
	
	/* Enable Window Manager Multibuffering */
  WM_MULTIBUF_Enable(1);
	
  /* startip logo */
	k_Startup();
	/* */
	
	/* */
	k_InitMenu();
	
	xTimerTS = xTimerCreate("TS Timer", 30, pdTRUE, (void *)0, vTimerCallback);
	xTimerChangePeriod(xTimerTS, 30 / portTICK_PERIOD_MS, 0);

	while(1)
	{ 	
		GUI_Exec(); /* Do the background work ... Update windows etc.) */
		vTaskDelay(10);
	}	
	
}

void vTimerCallback(void * pvParameters)
{
	k_TouchUpdate();
}