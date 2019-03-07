#include "bsp_spi.h"

long spi_lost;

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

void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	GPIO_Initialize(GPIOA, GPIO_Pin_4, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP); // L3GD20H_CS初始化
	GPIO_Initialize(GPIOB, GPIO_Pin_2, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP); // LSM303D_CS初始化
	GPIO_Initialize(GPIOD, GPIO_Pin_11, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP); // MPU6000_CS初始化
	GPIO_Initialize(GPIOE, GPIO_Pin_8, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP); // MS5611_CS初始化
		
	L3GD20H_CS(1); 
	LSM303D_CS(1);	
	MPU6000_CS(1); 
	MS5611_CS(1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1); // PA5:CLK
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1); // PA6:MISO
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); // PA7:MOSI

	SPI_Cmd(SPI1, DISABLE);

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);
		
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
	SPI_InitStructure.SPI_CRCPolynomial = 7;	
	SPI_Init(SPI1, &SPI_InitStructure); 	  

	SPI_Cmd(SPI1, ENABLE);	
}

// SPI1读写一个字节
uint8_t SPI1_ReadWriteByte(uint8_t Data)
{		 		
    uint8_t retry=0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) { if(retry++>200) { spi_lost++; return 0xFF;} } // 发送缓存标志位为空
    SPI_I2S_SendData(SPI1, Data); // 通过外设SPI1发送一个数据
    retry=0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) { if(retry++>200) { spi_lost++; return 0xFF;} } //接收缓存标志位不为空
    return SPI_I2S_ReceiveData(SPI1); // 通过SPI1返回接收数据
}