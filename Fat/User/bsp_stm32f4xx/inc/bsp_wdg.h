#ifndef _BSP_WDG_H_
#define _BSP_WDG_H_

#include "stdint.h"

#define IWDG_CLOCK 32 // 独立看门狗时钟频率 32kHz

#define WWDG_PCLK (42000 / 4096) // 窗口看门狗时钟频率 (42000 / 4096)kHz

// 初始化独立看门狗    reset_time_ms : 看门狗复位时间ms
void IWDG_Init(uint16_t reset_time_ms);

// 喂独立看门狗
void IWDG_Feed(void);

// 初始化窗口看门狗    reset_time_ms : 复位时间    top_window_time ：喂狗的上窗口时间 （当top_window_time = reset_time_ms时，可以随时喂狗）
void WWDG_Init(uint8_t reset_time_ms, uint8_t top_window_time);
	
#endif
