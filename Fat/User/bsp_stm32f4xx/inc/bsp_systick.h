#ifndef _BSP_SYSTOCK_H_
#define _BSP_SYSTOCK_H_

#include "stdint.h"

void SysTick_Init(uint8_t time_ms);

void SysTick_DelayUS(uint32_t n);

#endif
