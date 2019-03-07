#ifndef __BSP_CLI_H_
#define __BSP_CLI_H_

#include "stdint.h"

extern void CLI_Init(uint32_t baud);


extern void UART1_DMA_Send( uint8_t *BufferSRC,uint32_t BufferSize);

#endif