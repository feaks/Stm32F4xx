
#include "bsp_hash.h"

void BSP_hash_sha1_Init(void)
{
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_HASH , ENABLE);
}

