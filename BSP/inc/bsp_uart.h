
#ifndef __BSP_UART_H_
#define __BSP_UART_H_


#ifdef  __cplusplus
	extern "C" {
#endif


	#include "stm32f4xx.h"
	
	void UART1_Init(uint32_t baud);

	
#ifdef  __cplusplus
	}
#endif


#endif
