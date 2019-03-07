/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.40 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_X_OS.C
Purpose     : This file provides emWin Interface with FreeRTOS
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "GUI.h"
    
/* FreeRTOS include files */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h "
    void GUI_ErrorPrint(const char *s);
/*********************************************************************
*
* Global data
*/
static SemaphoreHandle_t osMutex;
static SemaphoreHandle_t osSemaphore;
/*********************************************************************
*
* Timing:
* GUI_X_GetTime()
* GUI_X_Delay(int)

Some timing dependent routines require a GetTime
and delay function. Default time unit (tick), normally is
1 ms.
*/

int GUI_X_GetTime(void)
{
	return ((int) xTaskGetTickCount()); // 获取系统时间
}

void GUI_X_Delay(int ms)
{
	TickType_t pxPreviousWakeTime = xTaskGetTickCount();
	vTaskDelayUntil(&pxPreviousWakeTime, ms / portTICK_RATE_MS ); // 系统延时
}

/*********************************************************************
*
* GUI_X_Init()
*
* Note:
* GUI_X_Init() is called from GUI_Init is a possibility to init
* some hardware which needs to be up and running before the GUI.
* If not required, leave this routine blank.
*/

void GUI_X_Init(void) {
	
	GUI_SetOnErrorFunc(GUI_ErrorPrint);
}


/*********************************************************************
*
* GUI_X_ExecIdle
*
* Note:
* Called if WM is in idle state
*/

void GUI_X_ExecIdle(void) {
	GUI_X_Delay(1); // GUI空闲
}

/*********************************************************************
*
* Multitasking:
*
* GUI_X_InitOS()
* GUI_X_GetTaskId()
* GUI_X_Lock()
* GUI_X_Unlock()
*
* Note:
* The following routines are required only if emWin is used in a
* true multi task environment, which means you have more than one
* thread using the emWin API.
* In this case the
* #define GUI_OS 1
* needs to be in GUIConf.h
*/

/* Init OS */
void GUI_X_InitOS(void)
{
  /* Create the Mutex used by the two threads */
  osMutex = xSemaphoreCreateMutex(); // 创建互斥锁
	configASSERT (osMutex != NULL);
	  
  /* Create the Semaphore used by the two threads */
	vSemaphoreCreateBinary(osSemaphore); // 创建信号量
	configASSERT (osSemaphore != NULL);  
	
	
}

void GUI_X_Unlock(void)
{ 
	if( xSemaphoreGive(osMutex) != pdTRUE) // 释放互斥锁
	{
			while(xSemaphoreGive(osMutex) != pdTRUE)
			{
				
			}
	}
}

void GUI_X_Lock(void)
{
//	if(osMutex == NULL)
//  {
//    GUI_X_InitOS();
//  }
	if( xSemaphoreTake(osMutex , portMAX_DELAY) != pdTRUE) // 获取互斥锁
	{
			while(1);
	}
}

/* Get Task handle */
U32 GUI_X_GetTaskId(void) 
{ 
  return ((U32) xTaskGetCurrentTaskHandle()); // 获取当前任务句柄
}


void GUI_X_WaitEvent (void) 
{
 while( xSemaphoreTake(osSemaphore, portMAX_DELAY) != pdTRUE ); // 等待直到获取到信号量
}

void GUI_X_WaitEventTimed(int Period)
{
	while( xSemaphoreTake(osSemaphore, Period) != pdTRUE ); // 等待直到获取到信号量，或者等待时间结束
}

void GUI_X_SignalEvent (void) 
{
	xSemaphoreGive(osSemaphore); // 释放信号量
}

/*********************************************************************
*
* Logging: OS dependent

Note:
Logging is used in higher debug levels only. The typical target
build does not use logging and does therefor not require any of
the logging routines below. For a release build without logging
the routines below may be eliminated to save some space.
(If the linker is not function aware and eliminates unreferenced
functions automatically)

*/
static const char *log;
static const char *warn;
static const char *error;
#include "stdio.h"
void GUI_X_Log (const char *s) 
{
	log = s;
	printf("GUI Log: %s\n",s);
}
void GUI_X_Warn (const char *s)
{
	warn = s;
	printf("GUI Warm: %s\n",s);
}
void GUI_X_ErrorOut(const char *s) 
{
	printf("GUI ERR: %s\n",s);
}
void GUI_ErrorPrint(const char *s) 
{
	printf("GUI ERR: %s\n",s);
}

/*************************** End of file ****************************/
