#include "bsp_sflash.h"

#define CMD_AAI       0xAD  	// AAI 连续编程指令(FOR SST25VF016B)
#define CMD_DISWR	  	0x04		// 禁止写, 退出AAI状态
#define CMD_EWRSR	  	0x50		// 允许写状态寄存器的命令
#define CMD_WRSR      0x01  	// 写状态寄存器命令
#define CMD_WREN      0x06		// 写使能命令
#define CMD_READ      0x03  	// 读数据区命令
#define CMD_RDSR      0x05		// 读状态寄存器命令
#define CMD_RDID      0x9F		// 读器件ID命令
#define CMD_SE        0x20		// 擦除扇区命令
#define CMD_BE        0xC7		// 批量擦除命令

#define WIP_FLAG      0x01		// 状态寄存器中的正在编程标志（WIP) */

#define DUMMY_BYTE            ((uint8_t)0x00)

#define SFLASH_SPI		SPI1

static uint32_t sflash_spi_lost;

SFLASH sflash;

static void SFLASH_ReadInfo(void); // 读芯片ID
static void SFLASH_WriteDisable(void); // 向器件发送禁止写命令
static void SFLASH_WriteEnable(void); // 向器件发送写使能命令
static void SFLASH_WriteStatus(uint8_t value); // 向器件写状态寄存器
static void SFLASH_WaitForWriteEnd(void); 	// 采用循环查询的方式等待器件内部写操作完成
static uint8_t SFLASH_ReadWriteByte(uint8_t Data); // SPI_FLASH读写一个字节

/********************************************************************
 *  STM32F4XX 时钟计算
 *  	HCLK = 168M
 *  	PCLK1 = HCLK / 4 = 42M
 *  	PCLK2 = HCLK / 2 = 84M
 *  
 *  	SPI2、SPI3	在PCLK1，时钟42M
 *  	SPI1        在PCLK2，时钟84M
 *  
 *  	STM32F4 支持的最大SPI时钟为 37.5 Mbits/S，因此需要分频
 ********************************************************************/
void SFLASH_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* 配置片选口线为推挽输出模式 */
	SFLASH_CS(1);		/* 片选置高，不选中 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1); // PB3:CLK
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1); // PB4:MISO
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1); // PB5:MOSI

	SPI_Cmd(SFLASH_SPI, DISABLE); // 先禁止SPI
		
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 数据方向：2线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	// STM32的SPI工作模式：主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	// 数据位长度：8位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	// 时钟上升沿采样数据
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; // 时钟的第2个边沿采样数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	// 片选控制方式：软件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // 时钟分频系数 4分频=84/4=21M	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // 数据位传输次序：高位先传
	SPI_InitStructure.SPI_CRCPolynomial = 7; // CRC多项式寄存器，复位后为7。未使用
	SPI_Init(SFLASH_SPI, &SPI_InitStructure); 	  
 
	SPI_Cmd(SFLASH_SPI, ENABLE); // 使能SPI
	
	SFLASH_ReadInfo(); // 自动识别芯片型号
	
	SFLASH_WriteDisable(); // 发送禁止写入的命令,即使能软件写保护
	
	SFLASH_WaitForWriteEnd(); // 等待串行Flash内部写操作完成
	
	SFLASH_WriteStatus(0); // 解除所有BLOCK的写保护
}

// 读芯片ID
static void SFLASH_ReadInfo(void)
{
	SFLASH_CS(0);
	SFLASH_ReadWriteByte(CMD_RDID);
	uint8_t id1 = SFLASH_ReadWriteByte(DUMMY_BYTE);
	uint8_t id2 = SFLASH_ReadWriteByte(DUMMY_BYTE);
	uint8_t id3 = SFLASH_ReadWriteByte(DUMMY_BYTE);
	SFLASH_CS(0);

	sflash.chip_id = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

	switch(sflash.chip_id)
	{
		case SST25VF016B_ID:
			strcpy(sflash.chip_name, "SST25VF016B");
			sflash.total_size = 2 * 1024 * 1024;   // 总容量 = 2M
			sflash.page_size = 4 * 1024;           // 页面大小 = 4K
			break;

		case MX25L1606E_ID:
			strcpy(sflash.chip_name, "MX25L1606E");
			sflash.total_size = 2 * 1024 * 1024;   // 总容量 = 2M
			sflash.page_size = 4 * 1024;           // 页面大小 = 4K
			break;

		case W25Q64BV_ID:
			strcpy(sflash.chip_name, "W25Q64BV");
			sflash.total_size = 8 * 1024 * 1024;   // 总容量 = 8M
			sflash.page_size = 4 * 1024;           // 页面大小 = 4K
			break;

		default:
			strcpy(sflash.chip_name, "Unknow Flash");
			sflash.total_size = 2 * 1024 * 1024;
			sflash.page_size = 4 * 1024;
			break;
	}
}

// 向器件发送禁止写命令
static void SFLASH_WriteDisable(void)
{
	SFLASH_CS(0);												// 使能片选
	SFLASH_ReadWriteByte(CMD_DISWR);		// 发送命令
	SFLASH_CS(1);												// 禁能片选
}

// 向器件发送写使能命令
static void SFLASH_WriteEnable(void)
{
	SFLASH_CS(0);												// 使能片选
	SFLASH_ReadWriteByte(CMD_WREN);			// 发送命令
	SFLASH_CS(1);												// 禁能片选
}

// 向器件写状态寄存器
static void SFLASH_WriteStatus(uint8_t value)
{
	if(sflash.chip_id == SST25VF016B_ID)
	{
		SFLASH_CS(0);												// 使能片选
		SFLASH_ReadWriteByte(CMD_EWRSR);		// 发送命令，允许写状态寄存器
		SFLASH_CS(1);												// 禁能片选
	}

	SFLASH_CS(0);													// 使能片选
	SFLASH_ReadWriteByte(CMD_WRSR);				// 发送命令，写状态寄存器
	SFLASH_ReadWriteByte(value);					// 发送数据，状态寄存器的值
	SFLASH_CS(1);													// 禁能片选
}

// 采用循环查询的方式等待器件内部写操作完成
static void SFLASH_WaitForWriteEnd(void)
{
	SFLASH_CS(0);												// 使能片选
	SFLASH_ReadWriteByte(CMD_RDSR);			// 发送命令，读状态寄存器
	while((SFLASH_ReadWriteByte(DUMMY_BYTE) & WIP_FLAG) == SET);	// 判断状态寄存器的忙标志位
	SFLASH_CS(1);												// 禁能片选
}

// SPI_FLASH读写一个字节
static uint8_t SFLASH_ReadWriteByte(uint8_t Data)
{		 		
    uint8_t retry=0;
    while (SPI_I2S_GetFlagStatus(SFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET) { if(retry++>200) { sflash_spi_lost++; return 0xFF;} } // 发送缓存标志位为空
    SPI_I2S_SendData(SPI1, Data); // 通过外设SPI1发送一个数据
    retry=0;
    while (SPI_I2S_GetFlagStatus(SFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET) { if(retry++>200) { sflash_spi_lost++; return 0xFF;} } //接收缓存标志位不为空
    return SPI_I2S_ReceiveData(SPI1); // 通过SPI1返回接收数据
}

int SFLASH_main(void)
{
	SFLASH_Init();
	while(1);
}