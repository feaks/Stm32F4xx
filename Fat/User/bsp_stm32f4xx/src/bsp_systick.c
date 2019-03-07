#include "bsp_systick.h"
#include "stm32f4xx.h"

// systick : 0~99.86ms   99.86 = 0xFFFFFF / (168M / 1000)
void SysTick_Init(uint8_t time_ms)
{
	/**
	 * 设置嘀嗒定时器是1ms中断：
	 * 1. 函数SysTick_Config()是属于CMSIS里面的一个函数，实现配置如下
	 *    - 函数的参数是重载寄存器reload要赋予的初值
	 *    - 配置嘀嗒定时器的优先级是最低优先级15
	 *    - 配置HCLK作为系统的时钟源
	 *    - 使能嘀嗒定时器中断
	 *    - 启动嘀嗒定时器计数
	 * 
	 * 2. 用户可以在调用函数SysTick_Config()后通过函数 SysTick_CLKSourceConfig()
	 *    更改嘀嗒定时器的时钟源为HCLK/8。SysTick_CLKSourceConfig()在文件misc.c里面
	 * 
	 * 3. 用户可以在调用函数SysTick_Config()后通过函数 NVIC_SetPriority()修改优先级，
	 *    函数NVIC_SetPriority()在文件core_cm4.h文件里面
	 * 
	 * 4. 通过下面的公式调整时基
	 *    Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)
	 *    Reload Value 是函数SysTick_Config()的参数，但是不能超过0xFFFFFF
   **/
	SysTick_Config(SystemCoreClock / 1000 * time_ms);
	/**
	 * 1s计数 SystemCoreClock
	 * 1ms计数 SystemCoreClock / 1000
	 * 滴答定时器最大计数0xFFFFFF
	 * 所以滴答定时器中断最大间隔为 99.86 = 0xFFFFFF / (168M / 1000);
	 */
}

// FreeRTOS中已经使用并处理滴答定时器，此项目中不能再定义
//void SysTick_Handler(void)
//{

//}

// 延迟n us
void SysTick_DelayUS(uint32_t n)
{
	uint32_t ticks;
	uint32_t told;
	uint32_t tnow;
	uint32_t tcnt = 0;
	uint32_t reload;
	reload = SysTick->LOAD;
	ticks = n * (SystemCoreClock / 1000000); /* 需要的节拍数 */
	tcnt = 0;
	told = SysTick->VAL; /* 刚进入时的计数器值 */
	while (1)
	{
		tnow = SysTick->VAL;
		if (tnow != told)
		{
			/* SYSTICK是一个递减的计数器 */
			if (tnow < told)
			{
				tcnt += told - tnow;
			}
			/* 重新装载递减 */
			else
			{
				tcnt += reload - tnow + told;
			}
			told = tnow;
			/* 时间超过/等于要延迟的时间,则退出 */
			if (tcnt >= ticks)
			{
				break;
			}
		}
	}
}
