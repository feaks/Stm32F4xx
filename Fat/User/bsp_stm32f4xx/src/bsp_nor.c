#include "bsp_nor.h"
#include "stm32f4xx_gpio.h"

#define ADDR_SHIFT(address) 			(NOR_FLASH_ADDR + 2*(address)) // 计算指定地址实际存储地址

#define PAGE_AT_SECTOR(page)			((page)/NOR_SECTOR_SIZE)		// 计算该页所在扇区编号
#define PAGE_AT_ADDRESS(page)			((page)*NOR_PAGE_SIZE)			// 计算该页所在地址
#define SECTOR_AT_ADDRESS(sector)	((sector)*NOR_SECTOR_SIZE)	// 计算该扇区所在的地址

#define NOR_WRITE(address, data)  (*(volatile uint16_t *)ADDR_SHIFT(address) = (data)) // 按字（16bit）方式写入1个字
#define NOR_READ(address)					(*(volatile uint16_t *)ADDR_SHIFT(address)) // 按字（16bit）方式读取1个字
#define NOR_Pointer(address)			((volatile uint16_t *)ADDR_SHIFT(address))  // 按字（16bit）方式获取指向该字的指针

// 判断忙时的执行语句循环次数
#define BlockErase_Timeout    ((uint32_t)0x00A00000)
#define ChipErase_Timeout     ((uint32_t)0x30000000)
#define Program_Timeout       ((uint32_t)0x00001400)

// PD6是NOR Flash输出到STM32的忙信号，通过GPIO查询方式判断忙
#define NOR_IS_BUSY()	(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == RESET)

static uint8_t NOR_GetStatus(uint32_t Timeout);

/*************************************************************************************
 * NOR FLASH 的 GPIO ：                                                              *
 *	A0  -> PF0/FSMC_A0                            DQ0  -> PD14/FSMC_D0               *
 *	A1  -> PF1/FSMC_A1                            DQ1  -> PD15/FSMC_D1               *
 *	A2  -> PF2/FSMC_A2                            DQ2  -> PD0/FSMC_D2                *
 *	A3  -> PF3FSMC_A3                             DQ3  -> PD1/FSMC_D3                *
 *	A4  -> PF4/FSMC_A4                            DQ4  -> PE7/FSMC_D4                *
 *	A5  -> PF5/FSMC_A5                            DQ5  -> PE8/FSMC_D5                *
 *	A6  -> PF12/FSMC_A6                           DQ6  -> PE9/FSMC_D6                *
 *	A7  -> PF13/FSMC_A7                           DQ7  -> PE10/FSMC_D7               *
 *	A8  -> PF14/FSMC_A8                           DQ8  -> PE11/FSMC_D8               *
 *	A9  -> PF15/FSMC_A9                           DQ9  -> PE12/FSMC_D9               *
 *	A10 -> PG0/FSMC_A10                           DQ10 -> PE13/FSMC_D10              *
 *	A11 -> PG1/FSMC_A11                           DQ11 -> PE14/FSMC_D11              *
 *	A12 -> PG2/FSMC_A12                           DQ12 -> PE15/FSMC_D12              *
 *	A13 -> PG3/FSMC_A13                           DQ13 -> PD8/FSMC_D13               *
 *	A14 -> PG4/FSMC_A14                           DQ14 -> PD9/FSMC_D14               *
 *	A15 -> PG5/FSMC_A15                           DQ15/A-1 -> PD10/FSMC_D15          *
 *	A16 -> PD11/FSMC_CLE/FSMC_A16				  				          												 *
 *	A17 -> PD12/FSMC_ALE/FSMC_A17                 RY/BY -> PD6/FSMC_NWAIT            *
 *	A18 -> PD13/FSMC_A18                          UB    -> PE1/FSMC_NBL1  					 *
 *	A19 -> PE3/FSMC_A19                           CE    -> PG9/FSMC_NE2		           *
 *  A20 -> PE4/FSMC_A20														OE    -> PD4/FSMC_NOE				       *
 *	A21 -> PE5/FSMC_A21														WE    -> PD5/FSMC_NWE		           *
 *  A22 -> PE6/FSMC_A22                                                              *
 ************************************************************************************/
void NOR_Init()
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTimingInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	// 使能GPIO时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);

	// 使能 FSMC 时钟
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	// GPIOD复用配置
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// GPIOE复用配置
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	// GPIOF复用配置
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	// GPIOG复用配置
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_9;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	// PD6 作为忙信号，配置为GPIO输入模式，软件查询
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		// 输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// FSMC配置
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 0x06; // 0x05正常 0x04出错
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0x01;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 0x0C;			// 0x0B正常 0x0A 出错
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_B;

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM2;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	// Enable FSMC Bank1_SRAM2 Bank
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);
}

/*************************************************************************************
*	函 数 名: NOR_ReadID                                                    		       *
*	功能说明: 读取NOR Flash的器件ID                                         		       *
*	形    参: 无                                                            		       *
*	返 回 值: 器件ID,32Bit, 高8bit 是厂商码, 低24bit是器件ID                		       *
*************************************************************************************/      
uint32_t NOR_ReadID(void)
{
	uint32_t uiID;
	uint8_t code, id1, id2, id3;

	NOR_WRITE(0x0555, 0x00AA);
	NOR_WRITE(0x02AA, 0x0055);
	NOR_WRITE(0x0555, 0x0090);

	code = NOR_READ(0x0000); // 厂商码
	id1 = NOR_READ(0x0001); // Id1
	id2 = NOR_READ(0x000E); // Id2
	id3 = NOR_READ(0x000F); // Id3

	uiID = ((uint32_t)code << 24) | ((uint32_t)id1 << 16)  | ((uint32_t)id2 << 8) | id3;

	NOR_WRITE(0x0000, 0x00F0);		// 退出ID模式

	return uiID;
}

/*************************************************************************************
*	函 数 名: NOR_WriteWord                                                 		       *
*	功能说明: 字编程。编程前执行解锁命令序列，编程完毕后自动退到读取模式。  		       *
*						字编程可以是随机地址。编程前需要保证存储单元是全0xFF状态。    		       *
*						可以重复编程相同的数据。                                      		       *
*	形    参: address : 偏移地址[0, NOR_FLASH_SIZE];																	 *
*						word    : 数据 16bit                                          		       *
*                                                                         		       *
*	返 回 值: NOR_SUCCESS=0, NOR_SUSPEND=1,	NOR_ABORTS=2, NOR_TIMEOUT=3, NOR_ERROR=4   *
*************************************************************************************/
uint8_t NOR_WriteWord(uint32_t address, uint16_t word)
{
	NOR_WRITE(0x0555, 0x00AA);
	NOR_WRITE(0x02AA, 0x0055);
	NOR_WRITE(0x0555, 0x00A0);
	NOR_WRITE(address, word);

	return (NOR_GetStatus(Program_Timeout));
}

/*************************************************************************************
*	函 数 名: NOR_WritePage.                                             	             *
*	功能说明: 页面内编程（32字一个页面）。编程前需要保证存储单元是全0xFF状态。       	 *
*						可以重复编程相同的数据。                                                 *
*	形    参: page	  : 偏移地址[0-NOR_PAGE_COUNT]*NOR_PAGE_SIZE             	 				 *
*						data	 	: 数据存放在此缓冲区                                          	 *
*						length  : 数据长度，双字节为1个单位. 值域:[1-32]                      	 *
*                                                                                    *
*	返 回 值: NOR_SUCCESS=0, NOR_SUSPEND=1,	NOR_ABORTS=2, NOR_TIMEOUT=3, NOR_ERROR=4   *
*************************************************************************************/
uint8_t NOR_WritePage(uint32_t address, uint16_t *data, uint8_t length)
{
	/****************************************************************************
	* pdf 表7.7 写入缓冲器编程                                                	*
	*	写入缓冲器编程允许系统在一个编程操作中写入最多32 个字。                		*
	*	与标准的“ 字” 编程算法相比，这可以有效地加快字编程速度。                 	*
	****************************************************************************/	
	if (length > 32 || address%NOR_PAGE_SIZE!=0 || (address+length) > NOR_FLASH_SIZE)
	{
		return NOR_ERROR;
	}

	/* 解锁命令序列 */
	NOR_WRITE(0x0555, 0x00AA);
	NOR_WRITE(0x02AA, 0x0055);

	/* Write Write Buffer Load Command */
	NOR_WRITE(address, 0x0025);
	NOR_WRITE(address, (length - 1));
	
	/*  Load Data into NOR Buffer */
	for(uint8_t i=0; i<length; i++)
	{
		NOR_WRITE(address+i, *data++);
	}

	NOR_WRITE(address, 0x29);

	return (NOR_GetStatus(Program_Timeout));
}

/*************************************************************************************
*	函 数 名: NOR_EraseSector                                                          *
*	功能说明: 擦除NOR Flash指定的扇区                                                  *
*	形    参: address ：扇区地址                                                       *
*	返 回 值: NOR_SUCCESS=0, NOR_SUSPEND=1,	NOR_ABORTS=2, NOR_TIMEOUT=3, NOR_ERROR=4   *
*************************************************************************************/
uint8_t NOR_EraseSector(uint32_t address)
{
	if (address%NOR_SECTOR_SIZE!=0)
	{
		return NOR_ERROR;
	}
	NOR_WRITE(0x0555, 0x00AA); // 解锁
	NOR_WRITE(0x02AA, 0x0055); // 解锁
	NOR_WRITE(0x0555, 0x0080); // 设置指令
	NOR_WRITE(0x0555, 0x00AA); // 解锁
	NOR_WRITE(0x02AA, 0x0055); // 解锁
	NOR_WRITE(address, 0x30); // 扇区擦除命令

	return (NOR_GetStatus(BlockErase_Timeout));
}

/*************************************************************************************
*	函 数 名: NOR_EraseChip                                                            *
*	功能说明: 擦除NOR Flash整个芯片                                                    *
*	形    参: 无                                                                       *
*	返 回 值: NOR_SUCCESS=0, NOR_SUSPEND=1,	NOR_ABORTS=2, NOR_TIMEOUT=3, NOR_ERROR=4   *
*************************************************************************************/
uint8_t NOR_EraseChip(void)
{
	NOR_WRITE(0x0555, 0x00AA); // 解锁
	NOR_WRITE(0x02AA, 0x0055); // 解锁
	NOR_WRITE(0x0555, 0x0080); // 设置指令
	NOR_WRITE(0x0555, 0x00AA); // 解锁
	NOR_WRITE(0x02AA, 0x0055); // 解锁
	NOR_WRITE(0x0555, 0x0010); // 扇区擦除命令

	return (NOR_GetStatus(ChipErase_Timeout));
}

/*************************************************************************************
* 函数名：NOR_Supend（未测试）                                                       *
* 功能说明：擦除挂起：命令允许系统中断扇区擦除操作，然后从未被选择擦除的任何扇区     *
*           读取数据或向其编程数据。如果擦除操作已经挂起则操作无效。                 *
						编程挂起：命令允许系统中断嵌入式编程操作或 “ 写入缓冲器 ” 编程操作，     *
            以便能够从未挂起的扇区中读取数据。在编程过程中写入编程挂起命令后，       *
            设备在最长 15 µs （典型值 5 µs）内中止编程操作，并更新状态位。           *
*	形    参: 无                                                                       *
*	返 回 值: 无                                                                       *
*************************************************************************************/
static void NOR_Supend(void)
{
	NOR_WRITE(0x0000, 0x00B0); // 擦除挂起命令
}

/*************************************************************************************
* 函数名：NOR_Resume（未测试）                                                       *
* 功能说明：擦除恢复：恢复扇区擦除操作，当前擦除未挂起状态下此命令无效。						 *
*           读取数据或向其编程数据。                                                 *
*           编程恢复：写入编程恢复命令后，设备返回编程。                             *
*	形    参: 无                                                                       *
*	返 回 值: 无                                                                       *
*************************************************************************************/
static void NOR_Resume(void)
{
	NOR_WRITE(0x0000, 0x0030); // 擦除恢复命令
}

/*************************************************************************************
*	函 数 名: NOR_Reset                                                                *
*	功能说明: 复位NOR，退到读状态                                                      *
*	形    参: 无                                                                       *
*	返 回 值: 无                                                                       *
*************************************************************************************/
static void NOR_Reset(void)
{
	NOR_WRITE(0x0555, 0x00AA);
	NOR_WRITE(0x02AA, 0x0055);
	NOR_WRITE(0x0000, 0x00F0 );
}

/*************************************************************************************
*	函 数 名: NOR_GetStatus                                                            *
*	功能说明: 读取NOR的操作状态                                                        *
*	形    参: 无                                                                       *
*	返 回 值: NOR_SUCCESS=0, NOR_SUSPEND=1,	NOR_ABORTS=2, NOR_TIMEOUT=3, NOR_ERROR=4   *
*************************************************************************************/
static uint8_t NOR_GetStatus(uint32_t Timeout)
{
	uint16_t val1 = 0x00, val2 = 0x00, val3 = 0x00;
	uint32_t timeout = Timeout;

	/* 等待NOR输出忙信号，高电平时等待。避免NOR的忙信号还未反映过来导致CPU提前认为不忙了 */
	while (!NOR_IS_BUSY() && timeout)
	{
		timeout--;
	}

	/* 等待NOR忙信号结束，低电平时等待 */
	timeout = Timeout;
	while(NOR_IS_BUSY() && timeout)
	{
		timeout--;
	}

	/**********************************************************************
	* 通过读取DQ6, DQ5, DQ2, DQ1的数据位判断Nor Flash内部操作状态。       *
	*  - DQ 6 编程时跳变                                                  *
	*  - DQ 6 和 DQ 2 在擦除时跳变                                        *
	*  - DQ 2 在擦除挂起时跳变                                            *
	*  - DQ 1 在编程错误时置1                                             *
	*  - DQ 5 在超时时置1                                                 *
	**********************************************************************/
	while (Timeout)
	{
		Timeout--;
		// 读取val1, val2 val3
		val1 = NOR_READ(0x0000);
		val2 = NOR_READ(0x0000);
		val3 = NOR_READ(0x0000);

		// 如果 val1和val2，val2和val3，DQ6都在跳变，写入或擦除正在执行
		if (((val1 & 0x0040) != (val2 & 0x0040)) && ((val2 & 0x0040) != (val3 & 0x0040)))
		{
			// 如果是写入缓冲器编程（页编程），并且val1 DQ1为1
			if(val1 & 0x0002) // 判断DQ1置1
			{
				NOR_Reset();
				return NOR_ABORTS; // 返回编程中止
			}
			else if(val1 & 0x0020) // 判断DQ5置1
			{
				NOR_Reset();
				return NOR_TIMEOUT; // 返回超时
			}
		}
		else
		{
			// 读取val1, val2
			val1 = NOR_READ(0x0000);
			val2 = NOR_READ(0x0000);
			
			// 如果val1, val2中DQ2跳变
			if((val1 & 0x0004) != (val2 & 0x0004))
			{
				return NOR_SUSPEND; // 返回挂起
			}
			else
			{
				return NOR_SUCCESS; // 操作完成
			}
		}
	}
	NOR_Reset();
	return NOR_TIMEOUT; // 返回超时
}

int NOR_main(void)
{
	static uint8_t erase_status, byte_status, word_status, page_status;
	static uint16_t word, byte, page[32];
	static uint32_t uuid;
	uint16_t page_buffer[NOR_PAGE_SIZE] = {0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 
																				 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F};
	static volatile uint16_t * pPage;
	NOR_Init();

	#if 0
	/* =================================读取芯片Id================================= */
	uuid = NOR_ReadID();

	/* =========================擦除整片16M，大约需要45s=========================== */
	erase_status = NOR_EraseChip();
	
	/* =======================擦除扇区====写入字====读取字========================= */
	// 擦除最后一个扇区
	erase_status = NOR_EraseSector(SECTOR_AT_ADDRESS(NOR_SECTOR_COUNT-1)); // 擦除指定扇区（0~127）128k，大约需要430ms，如果先前有数据则擦除相对较快
	
	// Nor Flash中最后一个字写入0x1234
	word_status = NOR_WriteWord(NOR_FLASH_SIZE-1, 0x1234);
	
	// 读取Nor Flash中最后一个字
	word = NOR_READ(NOR_FLASH_SIZE-1);

	/* =======================擦除扇区====写入页====读取页========================= */
	// 擦除最后一个扇区
	erase_status = NOR_EraseSector(SECTOR_AT_ADDRESS(NOR_SECTOR_COUNT-1));
	
	// Nor Flash写入一页数据
	page_status = NOR_WritePage(PAGE_AT_ADDRESS(NOR_PAGE_COUNT-1), page_buffer, sizeof(page_buffer)/2);
	
	// 读取写入的一页数据
	for(uint8_t i=0; i<sizeof(page_buffer)/2; i++)
	{
		page[i] = NOR_READ(PAGE_AT_ADDRESS(NOR_PAGE_COUNT-1) + i);
	}
	
	/* =====================擦除扇区====写入扇区====读取扇区======================= */
	// 擦除最后一个扇区
	erase_status = NOR_EraseSector(SECTOR_AT_ADDRESS(NOR_SECTOR_COUNT-1));
	
	// Nor Flash写入一个扇区数据（全部写入0，时间大约2.4s）
	for(uint16_t i=0; i<NOR_PAGE_IN_SECTOR; i++)
	{
		page_buffer[0]=i;
		page_status = NOR_WritePage(SECTOR_AT_ADDRESS(NOR_SECTOR_COUNT-1) + i*NOR_PAGE_SIZE, page_buffer, NOR_PAGE_SIZE);
	}
	
	// 读取写入的一扇区数据
	for(uint32_t i=0; i<NOR_PAGE_IN_SECTOR; i++)
	{
		pPage = NOR_Pointer(SECTOR_AT_ADDRESS(NOR_SECTOR_COUNT-1) + i*NOR_PAGE_SIZE);
		for(uint8_t j=0; j<NOR_PAGE_SIZE; j++)
		{
			page[j] = NOR_READ(SECTOR_AT_ADDRESS(NOR_SECTOR_COUNT-1) + i*NOR_PAGE_SIZE+j);
		}
	}
	
	#endif
	while(1);
}
