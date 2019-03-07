#include "bsp_param.h"
#include "bsp_flash.h"

#if EEPROM_SIZE

#else	// EEPROM_SIZE == 0 使用Flash存储参数

Param param;

void Param_Init()
{
	STMFLASH_Read(FLASH_PARAM_ADDR, (uint32_t*)&param, sizeof(Param)/4);
}

void Param_Save()
{
	STMFLASH_Erase_SECTOR(FLASH_PARAM_ADDR, sizeof(Vector3f)/4);
	STMFLASH_Write(FLASH_PARAM_ADDR, (uint32_t*)&param, sizeof(Param)/4);
}

#endif