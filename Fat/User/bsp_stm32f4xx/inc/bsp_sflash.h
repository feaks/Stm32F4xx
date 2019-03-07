#ifndef _BSP_SFLASH_H_
#define _BSP_SFLASH_H_

#include "stm32f4xx_gpio.h"
		
#define SFLASH_CS(x)   		if(x) GPIO_SetBits(GPIOF, GPIO_Pin_8); \
													else  GPIO_ResetBits(GPIOF, GPIO_Pin_8);

// 定义串行Flash ID
enum
{
	SST25VF016B_ID = 0xBF2541,
	MX25L1606E_ID  = 0xC22015,
	W25Q64BV_ID    = 0xEF4017
};

typedef struct
{
	uint32_t chip_id;			// 芯片ID
	uint32_t total_size;	// 总容量
	uint16_t page_size;		// 页面大小
	char chip_name[16];		// 芯片型号字符串，主要用于显示
} SFLASH;


void SFLASH_Init(void);

uint8_t SFLASH_ReadWriteByte(uint8_t Data);

#endif
