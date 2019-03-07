#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "main.h"

#include "task_init.h"

#include "config.h"

#include "stdio.h"
#include "bsp_sram.h"
/*********************************
**						函数声明					 *
*********************************/
static void SystemInitialize(void);
static void InitTaskCreate(void);
static void AppTaskCreate(void);

/*********************************
**						变量声明					**
*********************************/
volatile uint32_t minCycles, idleCounter, totalCycles, cycles;
volatile static uint32_t *DWT_CYCCNT  = (volatile uint32_t *)0xE0001004;//当前PC采样器周期计数寄存器
volatile static uint32_t *DWT_CONTROL = (volatile uint32_t *)0xE0001000;
volatile static uint32_t *SCB_DEMCR   = (volatile uint32_t *)0xE000EDFC;

float idlePercent; 
static uint32_t oldIdleCycleCount;
static uint32_t idleCycleCount = 0;
	
extern xSemaphoreHandle xTouchBinarySemaphore;

/*********************************
**	函 数 名: main						  **
**	功能说明: 标准c程序入口。	  **
**  形    参：无								**
**	返 回 值: 无								**
*********************************/
int main(void)
{	
	/* 系统初始化 */
  FSMC_SRAM_Init();
	SystemInitialize();

	/* 创建任务 */
	InitTaskCreate();

	/* 启动调度，开始执行任务 */ 
	vTaskStartScheduler();

	/* 
	 If all is well we will never reach here as the scheduler will now be
	 running.  If we do reach here then it is likely that there was insufficient
	 heap available for the idle task to be created. 
	*/
	while (1)
	{
			
	}
}

static void SystemInitialize(void)
{

	vSemaphoreCreateBinary(xTouchBinarySemaphore);
	
	*SCB_DEMCR = *SCB_DEMCR | 0x01000000;
	*DWT_CONTROL = *DWT_CONTROL | 1;	// enable the counter
	
	minCycles = 0xffff;
	
	SRAM_main();

}

/****************************************************************
**	函 数 名: AppTaskCreate																		 **
**	功能说明: 创建应用任务																		 **
**	形    参：无																							 **
**	返 回 值: 无																							 **
****************************************************************/
static void InitTaskCreate (void)
{
	xTaskCreate(    Task_Init,    // Pointer to the function that implements the task. 
                    "Task_Init",  // Text name for the task.  This is to facilitate debugging only. 
                    500,          // Stack depth in words.                                          
                    NULL,         // We are not using the task parameter.                           
                    11,           // This task will run at priority 26.                             
                    NULL );       // We are not using the task handle. */
}

void calculateIdlePercent()
{
		idlePercent = 100.0f * (idleCycleCount-oldIdleCycleCount) * minCycles / totalCycles;
    oldIdleCycleCount = idleCycleCount;
    totalCycles = 0;
}

/**
 * 系统IDLE状态时运行
 */
void vApplicationIdleHook( void )
{
	/* This hook function does nothing but increment a counter. */
     idleCycleCount++; 
	
	__nop();
	__nop();
	__nop();
	__nop();

	cycles = *DWT_CYCCNT;
	*DWT_CYCCNT = 0;		    // reset the counter
	
	totalCycles += cycles;
	if (cycles < minCycles)
		minCycles = cycles;
}

void vApplicationTickHook( void )
{
  /* 这里不可以调用不带“FromISR”结尾的FreeRTOS API函数 */
}

signed char *name;
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName )
{
	name = pcTaskName;
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
	printf("file : %s    line : %d", (char*)file, line);
  while (1)
  {
  }
}
#endif
 
/*************************************** 飞航科技 (END OF FILE) ******************************************/
