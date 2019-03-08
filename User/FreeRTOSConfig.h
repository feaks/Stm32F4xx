
#ifndef __FREERTOSCONFIG_H_
#define __FREERTOSCONFIG_H_

#ifdef  __cplusplus
    extern "C" {
#endif

	#define configUSE_PREEMPTION		1	//抢占式调度
	#define configUSE_IDLE_HOOK			0	//空闲任务
	#define configUSE_TICK_HOOK			0	//心跳任务
	#define configCPU_CLOCK_HZ			( ( unsigned long ) 168000000 )		//时钟主频
	#define configTICK_RATE_HZ			( ( portTickType ) 1000 )	//切换频率
	#define configMAX_PRIORITIES		( 5 )	//任务优先级数 0 1 2 3 4
	#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 128 )	//任务最小堆栈
	#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 30 * 1024 ) )	//任务总堆栈
	#define configMAX_TASK_NAME_LEN		( 16 )	//给任务起说明 最大为 15个字节
	#define configUSE_TRACE_FACILITY	1
	#define configUSE_16_BIT_TICKS		0 
	#define configIDLE_SHOULD_YIELD		1

	#define configUSE_MUTEXES           1

	#define configUSE_CO_ROUTINES 		0
	#define configMAX_CO_ROUTINE_PRIORITIES ( 16 )


	#define INCLUDE_vTaskPrioritySet		1
	#define INCLUDE_uxTaskPriorityGet		1
	#define INCLUDE_vTaskDelete				1
	#define INCLUDE_vTaskCleanUpResources	0
	#define INCLUDE_vTaskSuspend			1
	#define INCLUDE_vTaskDelayUntil			1
	#define INCLUDE_vTaskDelay				1


	#define configKERNEL_INTERRUPT_PRIORITY 		255


	#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	191


	#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY	15

	#define vPortSVCHandler SVC_Handler
	#define xPortPendSVHandler PendSV_Handler
	#define xPortSysTickHandler SysTick_Handler	//滴答定时器

#ifdef  __cplusplus
}  
#endif

#endif
