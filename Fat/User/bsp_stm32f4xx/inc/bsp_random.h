#ifndef _BSP_RANDOM_H_
#define _BSP_RANDOM_H_

#include "stdint.h"

// 随机数生成器初始化
void Random_Init();
	
// 阻塞的方式获取随机数，随机数生成时间很短，一般需要2个时钟周期，所以一般是使用阻塞的方式读取随机数
uint32_t Random_GetNumber();

#endif
