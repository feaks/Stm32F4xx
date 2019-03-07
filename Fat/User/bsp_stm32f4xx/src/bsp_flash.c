#include "bsp_flash.h"
#include "stm32f4xx_flash.h"

/**
 * 描述：
 *    读取指定地址的字
 *
 * 参数：
 *  read_addr:读地址 
 * 
 * 返回值：
 *    读取到的数据
 */
uint32_t STMFLASH_ReadWord(uint32_t read_addr)
{
	return *(volatile uint32_t*)read_addr; 
}

/**
 * 描述：
 *    从指定地址所在扇区擦除至指定长度的截止地址所在扇区
 *    特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数
 *             写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
 *             写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
 *             没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
 *    该函数对OTP区域也有效!可以用来写OTP区!
 *    OTP区域地址范围:0X1FFF7800~0X1FFF7A0F
 * 
 * 参数：
 *    EraseAddr：起始地址(此地址必须为4的倍数!!)
 *    NumToErase：字(32位)数(就是要擦数的32位数据的个数.) 
 * 
 * 返回值：
 *     0：成功擦除
 *    -1：写入非法地址
 *    -2：擦除失败
 */

int STMFLASH_Erase_SECTOR(uint32_t EraseAddr, uint32_t NumToErase)
{
	int result = 0;
	
	if(EraseAddr<STM32_FLASH_BASE || EraseAddr%4) return -1;	// 非法地址
	
	FLASH_Unlock(); // 解锁
  
	FLASH_DataCacheCmd(DISABLE); // FLASH擦除期间,必须禁止数据缓存
	
	uint32_t endAddr = EraseAddr + NumToErase*4;
	uint16_t FLASH_Sector = FLASH_Sector_0;
	uint32_t nextEraseAddr = EraseAddr;
	if(EraseAddr < 0x1FFF0000 && EraseAddr >= ADDR_FLASH_SECTOR_0 && endAddr < 0x1FFF0000) // 只有主存储区,才需要执行擦除操作!!
	{
		while(EraseAddr < endAddr) // 扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if(EraseAddr < ADDR_FLASH_SECTOR_1) { FLASH_Sector = FLASH_Sector_0; nextEraseAddr = ADDR_FLASH_SECTOR_1; }
			else if(EraseAddr < ADDR_FLASH_SECTOR_2) { FLASH_Sector = FLASH_Sector_1; nextEraseAddr = ADDR_FLASH_SECTOR_2; }
			else if(EraseAddr < ADDR_FLASH_SECTOR_3) { FLASH_Sector = FLASH_Sector_2; nextEraseAddr = ADDR_FLASH_SECTOR_3; }
			else if(EraseAddr < ADDR_FLASH_SECTOR_4) { FLASH_Sector = FLASH_Sector_3; nextEraseAddr = ADDR_FLASH_SECTOR_4; }
			else if(EraseAddr < ADDR_FLASH_SECTOR_5) { FLASH_Sector = FLASH_Sector_4; nextEraseAddr = ADDR_FLASH_SECTOR_5; }
			else if(EraseAddr < ADDR_FLASH_SECTOR_6) { FLASH_Sector = FLASH_Sector_5; nextEraseAddr = ADDR_FLASH_SECTOR_6; }
			else if(EraseAddr < ADDR_FLASH_SECTOR_7) { FLASH_Sector = FLASH_Sector_6; nextEraseAddr = ADDR_FLASH_SECTOR_7; }
			else if(EraseAddr < ADDR_FLASH_SECTOR_8) { FLASH_Sector = FLASH_Sector_7; nextEraseAddr = ADDR_FLASH_SECTOR_8; }
			else if(EraseAddr < ADDR_FLASH_SECTOR_9) { FLASH_Sector = FLASH_Sector_8; nextEraseAddr = ADDR_FLASH_SECTOR_9; }
			else if(EraseAddr < ADDR_FLASH_SECTOR_10) { FLASH_Sector = FLASH_Sector_9; nextEraseAddr = ADDR_FLASH_SECTOR_10; }
			else if(EraseAddr < ADDR_FLASH_SECTOR_11) { FLASH_Sector = FLASH_Sector_10; nextEraseAddr = ADDR_FLASH_SECTOR_11; }
			else { FLASH_Sector = FLASH_Sector_11; nextEraseAddr = 0x1FFF0000; }
			
			// VCC=2.7~3.6V之间!!
			if(FLASH_EraseSector(FLASH_Sector, VoltageRange_3) != FLASH_COMPLETE) { result = -2;  break; } // 发生错误了 
			else  																																{ EraseAddr = nextEraseAddr; }
		}
		
		FLASH_DataCacheCmd(ENABLE);	// FLASH擦除结束,开启数据缓存
	
		FLASH_Lock(); // 上锁
	} 
	else 
	{
		result = -1;
	}
	return result;
}

/**
 * 描述：
 *    注：一定先确定写入的位置已被清空
 *    从指定地址开始写入指定长度的数据
 *    特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数
 *             写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
 *             写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
 *             没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
 *    该函数对OTP区域也有效!可以用来写OTP区!
 *    OTP区域地址范围:0X1FFF7800~0X1FFF7A0F
 * 
 * 参数：
 *    WriteAddr：起始地址(此地址必须为4的倍数!!)
 *    pBuffer：数据指针
 *    NumToWrite：字(32位)数(就是要写入的32位数据的个数.) 
 * 
 * 返回值：
 *     0：成功写入
 *    -1：写入非法地址
 *    -2：写入失败
 */
int STMFLASH_Write(uint32_t WriteAddr, uint32_t *pBuffer, uint32_t NumToWrite)	
{ 
	int result = 0;
  
	if(WriteAddr<STM32_FLASH_BASE || WriteAddr%4) return -1;	// 非法地址
	
	FLASH_Unlock(); // 解锁
  
	FLASH_DataCacheCmd(DISABLE); // FLASH擦除期间,必须禁止数据缓存
	
	uint32_t add_addr = WriteAddr; // 写入的起始地址
	uint32_t end_addr = WriteAddr + NumToWrite*4;	// 写入的结束地址
	
	while(WriteAddr < end_addr) // 写数据
	{
		if(FLASH_ProgramWord(WriteAddr, *pBuffer) != FLASH_COMPLETE) // 写入数据
		{ 
			result = -2;
			break; // 写入异常
		}
		WriteAddr += 4;
		pBuffer++;
	} 
	
  FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	
	FLASH_Lock();//上锁
	
	return result;
} 

/**
 * 描述：
 *    获取某个地址所在的flash扇区
 *
 * 参数：
 *  ReadAddr:起始地址
 *  pBuffer:数据指针
 *  NumToRead:字(4位)数
 */
void STMFLASH_Read(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead)   	
{
	for(uint32_t i=0; i<NumToRead; i++)
	{
		pBuffer[i] = STMFLASH_ReadWord(ReadAddr); // 读取4个字节.
		ReadAddr += 4; // 偏移4个字节.	
	}
}
