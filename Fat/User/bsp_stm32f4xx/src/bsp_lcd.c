#include "bsp_lcd.h"
#include "bsp_lcd_NT35510.h"
#include "bsp_lcd_SSD1963.h"

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_fsmc.h"

#include "FreeRTOS.h"
#include "task.h"
  
LCD_PARAM lcd_param;	// LCD设备参数
LCD 			lcd;				// LCD控制函数

void LCD_WR_DATA(volatile uint16_t data)
{
	data = data;
	LCD_ADDR->LCD_RAM = data;
}

// 发送写GRAM指令
void LCD_WR_CMD()
{
	LCD_ADDR->LCD_REG = lcd_param.wramcmd;
}		

void LCD_Init(DIRECTION direction, uint16_t width, uint16_t height)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMReadWriteTimingInitStructure; 
	FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMWriteTimingInitStructure;
	
	// 使能GPIO时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);
	
	// 使能 FSMC 时钟
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
	
	// PB15 推挽输出控制背光
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; // LCD_BL
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	// GPIOD复用配置
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC); // FSMC_D2
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC); // FSMC_D3
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC); // FSMC_NOE
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC); // FSMC_NWE
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC); // FSMC_D13
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC); // FSMC_D14
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC); // FSMC_D15
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC); // FSMC_D0
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC); // FSMC_D1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// GPIOE复用配置
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC); // FSMC_D4
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC); // FSMC_D5
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC); // FSMC_D6
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC); // FSMC_D7
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC); // FSMC_D8
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC); // FSMC_D9
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC); // FSMC_D10
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC); // FSMC_D11
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC); // FSMC_D12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	// GPIOF复用配置
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);  // FSMC_A6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
 
	// GPIOG复用配置
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12 , GPIO_AF_FSMC);  // FSMC_NE4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	// FSMC配置
	FSMC_NORSRAMReadWriteTimingInitStructure.FSMC_AddressSetupTime = 0x0F; // 地址建立时间（ADDSET）为16个HCLK  1/168M=6ns*16=96ns
  FSMC_NORSRAMReadWriteTimingInitStructure.FSMC_AddressHoldTime = 0x00; // 地址保持时间（ADDHLD），模式A未使用
  FSMC_NORSRAMReadWriteTimingInitStructure.FSMC_DataSetupTime = 60; // 数据保存时间为60个HCLK=6*60=360ns
  FSMC_NORSRAMReadWriteTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  FSMC_NORSRAMReadWriteTimingInitStructure.FSMC_CLKDivision = 0x00;
  FSMC_NORSRAMReadWriteTimingInitStructure.FSMC_DataLatency = 0x00;
  FSMC_NORSRAMReadWriteTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A; // 访问模式A

	FSMC_NORSRAMWriteTimingInitStructure.FSMC_AddressSetupTime = 0x09; // 地址建立时间（ADDSET）为9个HCLK  1/168M=6ns*9=54ns
  FSMC_NORSRAMWriteTimingInitStructure.FSMC_AddressHoldTime = 0x00; // 地址保持时间（ADDHLD），模式A未使用
  FSMC_NORSRAMWriteTimingInitStructure.FSMC_DataSetupTime = 8; // 数据保存时间为8个HCLK=6*8=48ns
  FSMC_NORSRAMWriteTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  FSMC_NORSRAMWriteTimingInitStructure.FSMC_CLKDivision = 0x00;
  FSMC_NORSRAMWriteTimingInitStructure.FSMC_DataLatency = 0x00;
  FSMC_NORSRAMWriteTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	 //Ä£Ê½A 

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4; // NE4 BANK1
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM; // 存储区类型SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b; // 存储器数据宽度16位
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable; // 突发访问模式禁止
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low; // 等待信号优先级低
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable; // 异步等待禁止
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable; // 包裹模式禁止 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; // 在等待状态钱等待信号激活
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	// 写操作使能
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable; // 等待信号禁止
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 扩展模式使能，读写可以使用不同时序
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; // 突发写禁止
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMReadWriteTimingInitStructure; // 读写时序
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMWriteTimingInitStructure;  // 写时序

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  // FSMC初始化

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // NE4_BANK1使能	
	
	vTaskDelay(2/portTICK_PERIOD_MS); // 至少延迟2ms等待硬件初始化完成
	
	// LCD控制函数初始化
	lcd.Check               = NT35510_Check;
	lcd.Init                = NT35510_Init;
	lcd.SetDisplayDirection = NT35510_SetDisplayDirection;
	lcd.FlipHorizontal      = NT35510_FlipHorizontal;
	lcd.SetCursor           = NT35510_SetCursor;
	lcd.DrawPoint           = NT35510_DrawPoint;
	lcd.ReadPoint           = NT35510_ReadPoint;
	lcd.FillRect            = NT35510_FillRect;
	lcd.DrawHLine           = NT35510_DrawHLine;
	lcd.Clear               = NT35510_Clear;
	lcd.Init();
	vTaskDelay(2/portTICK_PERIOD_MS); 
	
	// 检查LCD是否正确
	while(!lcd.Check()) ;
//		while(1);
	
	vTaskDelay(2/portTICK_PERIOD_MS); 
	// LCD初始化
	lcd.Init();
	 vTaskDelay(2/portTICK_PERIOD_MS); 
	// 设置LCD方向
	lcd.SetDisplayDirection(direction, width, height);
	vTaskDelay(2/portTICK_PERIOD_MS); 	
	// 点亮LCD背光灯
	LCD_BL_ON;
	// LCD清屏


	lcd.Clear(LCD_WHITE);
	
//	// LCD填充矩形	
//	lcd.FillRect(0, 0, 800, 480, LCD_BLACK);
//	
//	// LCD画点
//	for(uint16_t x=100; x<300; x++)
//	{
//		for(uint16_t y = 100; y<200; y++)
//		{
//			lcd.DrawPoint(x, y, LCD_RED);
//		}
//	}
//	lcd.DrawPoint(200, 200, LCD_BLUE);
//	lcd.DrawPoint(200, 201, LCD_BLUE);
//	lcd.DrawPoint(200, 202, LCD_WHITE);
//	lcd.DrawPoint(200, 203, LCD_BLUE);
//	
//	while(1);	
}