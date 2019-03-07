#ifndef _BSP_NOR_H_
#define _BSP_NOR_H_

// NOR Flash 型号 S29GL128P10TFI01        容量16M字节，16bit访问，100ns速度，物理地址0x6400 0000
// 注：16MFlash地址实际是0x6400 0000~0x64FF FFFF，在应用中使用的是16bit访问，所以可访问的地址是0x6400 0000~0x647F FFFF
#define NOR_FLASH_ADDR  	((uint32_t)0x64000000)

#define NOR_SECTOR_SIZE		0x10000				// 扇区大小			65536字*16bit/字   (128 * 1024 / 2)=65536
#define NOR_SECTOR_COUNT	0x80					// 扇区数量			128个
#define NOR_PAGE_SIZE			0x20					// 页大小 			32字*16bit/字
#define NOR_PAGE_COUNT		0x40000				// 页数量   ‭		262144‬个 
#define NOR_FLASH_SIZE		0x800000			// Flash大小		8M字*16bit/字
#define NOR_PAGE_IN_SECTOR	0x800				// 每个扇区中2048页

/*
	制造商ID：Spansion   0x01

	S29GL01GP	01 7E 28 01		1 Gigabit		 128M字节
	S29GL512P	01 7E 23 01		512 Megabit		64M字节
	S29GL256P	01 7E 22 01		256 Megabit		32M字节
	S29GL128P	01 7E 21 01		128 Megabit		16M字节
*/
typedef enum
{
	S29GL128P = 0x017E2101,
	S29GL256P = 0x017E2201,
	S29GL512P = 0x017E2301
} NOR_CHIP_ID;

/* NOR Status */
typedef enum
{
	NOR_SUCCESS = 0,
	NOR_SUSPEND = 1,
	NOR_ABORTS  = 2,
	NOR_TIMEOUT = 3,
	NOR_ERROR = 4
} NOR_STATUS;

void NOR_Init();

#endif
