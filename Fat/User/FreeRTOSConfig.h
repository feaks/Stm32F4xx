/*
    FreeRTOS V9.0.0rc2 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

/* Ensure stdint is only used by the compiler, and not the assembler. */
#ifdef __ICCARM__
	#include <stdint.h>
	extern uint32_t SystemCoreClock;
#endif

#define configUSE_PREEMPTION			1                                            // 1：抢占式    0：时间片轮转
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0                              // 最佳任务调度方式，采用汇编语句进行调度，不一定所有硬件都支持
#define configUSE_TICKLESS_IDLE                 0                              // IDLE状态时进入低功耗模式，如果系统任务切换过快，则不宜切换
#define configCPU_CLOCK_HZ				( ( unsigned long ) 168000000 )              // 系统时钟频率
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )                      // 任务切换频率  RTOS系统节拍中断的频率。即一秒中断的次数，每次中断RTOS都会进行任务调度。
#define configMAX_PRIORITIES			( 12 )                                       // 最大任务优先级
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 130 )                 // 空闲任务的最小堆栈大小（字）
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 70 * 1024 ) )                 // 内核可用RAM  
#define configMAX_TASK_NAME_LEN			( 16 )                                     // 任务描述字符串长度
#define configUSE_16_BIT_TICKS			0                                          // portTickType位数 16位或32位
#define configIDLE_SHOULD_YIELD			0                                          // 处于空闲状态时是否及时让出CPU给同优先级任务，建议用户任务优先级大于0，并且此值设置为0
#define configUSE_TASK_NOTIFICATIONS   1                                       // 任务通知，默认开启，可以替代信号量，效率更高。开启后每个任务会多分配8字节RAM
#define configUSE_MUTEXES				1                                              // 使用互斥量
#define configUSE_RECURSIVE_MUTEXES		1                                        // 使用递归互斥量
#define configUSE_COUNTING_SEMAPHORES	1                                        // 使用计数信号量
#define configQUEUE_REGISTRY_SIZE		8                                          // 配置可注册的队列数量，用于上位机调试内核
#define configUSE_QUEUE_SETS      0                                            // 使能队列集功能（可以阻塞、挂起到多个队列和信号量）
#define configUSE_TIME_SLICING                  1                              // 时间切片，抢占CPU后是否与同优先级任务轮转时间片
//#define configUSE_NEWLIB_REENTRANT              0                            // ???  是否为每个任务分配一个newlib(一个嵌入式C库)reent结构
#define configENABLE_BACKWARD_COMPATIBILITY     1                              // 向后兼容，定义了之前版本的宏
//#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5                            // 设置每个任务的线程本地存储指针数组大小  vTaskSetThreadLocalStoragePointer()写入值，pvTaskGetThreadLocalStoragePointer()读取值
#define configUSE_APPLICATION_TASK_TAG	0                                      // 为每个任务分配标签

/*Hook function related definitions. */  
#define configUSE_IDLE_HOOK				1                                            // 回调函数，空闲状态回调          void vApplicationIdleHook(void );   不可以使用vTaskDelay()、带有阻塞时间的队列和信号量函数
#define configUSE_TICK_HOOK				0                                            // 回调函数，时间片轮转回调        void vApplicationTickHook( void );   
#define configCHECK_FOR_STACK_OVERFLOW	2                                      // 回调函数，检查堆栈溢出          void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName );   发布时应关闭此功能
#define configUSE_MALLOC_FAILED_HOOK	0                                        // 回调函数，内存分配失败回调      void vApplicationMallocFailedHook(void);  创建任务、队列、信号量时会分配内存

/*Run time and task stats gathering related definitions. */ 
#define configGENERATE_RUN_TIME_STATS	0
															//	extern volatile unsigned longulHighFrequencyTimerTicks;  
															//	#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() ( ulHighFrequencyTimerTicks = 0UL )  
															//	#define portGET_RUN_TIME_COUNTER_VALUE() ulHighFrequencyTimerTicks  
#define configUSE_TRACE_FACILITY		1                                          // 启动可视化跟踪
#define configUSE_STATS_FORMATTING_FUNCTIONS    1                              // 启动状态格式化函数  vTaskList()  任务名、任务状态、优先级、剩余栈、任务序号
																																							 //                     vTaskGetRunTimeStats() 任务名、运行计数、使用率

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0                                            // 启用协程，注：主要用于资源非常受限的嵌入式系统中（RAM非常少），通常不会用于32位微处理器，不建议使用。
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS				1                                              // 启用软件定时器
#define configTIMER_TASK_PRIORITY		( 2 )                                      // 软件定时器任务优先级
#define configTIMER_QUEUE_LENGTH		10                                         // 软件定时器命令队列的长度
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 )         // 软件定时器任务的堆栈深度

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	1
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1
#define INCLUDE_uxTaskGetStackHighWaterMark  1
//#define INCLUDE_xResumeFromISR                  0
//#define INCLUDE_xTaskGetSchedulerState          0  
//#define INCLUDE_xTaskGetCurrentTaskHandle       0  
//#define INCLUDE_xTaskGetIdleTaskHandle          0  
//#define INCLUDE_xTimerGetTimerDaemonTaskHandle  0  
//#define INCLUDE_pcTaskGetTaskName               0  
//#define INCLUDE_eTaskGetState                   0  
//#define INCLUDE_xEventGroupSetBitFromISR        0 
//#define INCLUDE_xTimerPendFunctionCall          0  

/* FreeRTOS+CLI definitions. */
/* Dimensions a buffer into which command outputs can be written. The buffer
 * can be declared in the CLI code itself, to allow multiple command consoles to
 * share the same buffer. For example, an application may allow access to the
 * command interpreter by UART and by Ethernet. Sharing a buffer is done purely
 * to save RAM. Note, however, that the command console itself is not re-entrant,
 * so only one command interpreter interface can be used at any one time. For
 * that reason, no attempt at providing mutual exclusion to the buffer is
 * attempted.
 */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE		400

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0xf

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 	( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) ) //0xf0 // 优先级设置为最低，优先响应系统中断
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY  ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )  //	0x50 // 当优先级大于此优先级时无法在中断中调用系统API，否则可以调用带有ISR的系统API函数。
	// 如果某些中断不想被系统延迟执行，则其优先级需要高于configMAX_SYSCALL_INTERRUPT_PRIORITY，否则可能被系统延迟执行。    // FreeRTOS使用BASEPRI寄存器完成部分中断屏蔽
	
/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }	
	
//#define STR(x)  VAL(x)  
//#define VAL(x)  #x  
//#define configASSERT(x) ((x)?(void) 0 :xAssertCalld(__FILE__ ":" STR(__LINE__) " " #x"\n"))  
	
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */

/***************************************************************************
 * vTaskPrioritySet()    设置任务优先级
 * uxTaskPriorityGet()   获取任务优先级
 * vTaskDelete()         删除任务
 * vTaskSuspend()				 任务挂起
 * vTaskSuspendAll()     挂起全部任务
 * xTaskResumeAll()      恢复全部任务

 * vTaskResume()         任务恢复
 * vTaskResumeFromISR()  任务恢复
 * vTaskDelay()          任务延时   延迟多少个心跳周期，心跳周期由configTICK_RATE_HZ指定，参数可以直接使用 n/portTICK_RATE_MS 定义延迟n ms
 * xTaskGetTickCount()   获取任务滴答数
 * taskYIELD()           主动让出任务时间片
 * 
 * 消息队列 -- xQueueHandle
 *   xQueueCreate()      创建队列 ，会从堆中分配内存，内存不足返回NULL
 *   xQueueSend()        将数据发送到队列尾
 *   xQueueSendToBack()  将数据发送到队列尾
 *   xQueueSendToBack()  将数据发送到队列头
 *   xQueueSendToBackFromISR()       在中断中，将数据发送到队列尾
 *   xQueueSendToFrontFromISR()      在中断中，将数据发送到队列头
 *         注：参数 xTicksToWait：当值为portMAX_DELAY，并且INCLUDE_vTaskSuspend=1时为无限制等待
 *   xQueueReceive()     从队列中取出数据
 *   xQueuePeek()        查看队列中的数据
 *   xQueueReceiveFromISR()           在中断中，从中断中将数据取出
 *   uxQueueMessagesWaiting()         查询队列中当前有效数据的单元个数
 *   uxQueueMessagesWaitingFromISR()  在中断中，查询队列中当前有效数据的单元个数
 
 *    注：使用队列传递指针时，一定要注意数据的有效性
 *    
 *  信号量 -- xSemaphoreHandle
 *    vSemaphoreCreateBinary()         二值信号量创建
 *    xSemaphoreCreateMutex()          创建互斥量
 *    xSemaphoreGiveFromISR()          V操作，唤醒任务
 *    xSemaphoreTake()                 P操作，阻塞任务，除互斥信号量（Recursive Semaphore）外，都可以调用此函数
 *    xSemaphoreCreateCounting()       计数信号量创建
 *    
 *  临界区 (CRITICAL)
 *    taskENTER_CRITICAL();            进入临界区
 *    taskEXIT_CRITICAL();             退出临界区
 *  
 *  内存分配
 *     pvPortMalloc()                  申请内存
 *     pvPortFree()                    释放内存 
 *     uxTaskGetStackHighWaterMark()   查看该任务剩余栈大小
 *  中断
 *     taskDISABLE_INTERRUPTS          禁止RTOS可屏蔽中断
 *      taskENABLE_INTERRUPTS          使能RTOS可屏蔽中断
 *  
 *  注：系统未开始运行之前不能使用系统函数
 */
