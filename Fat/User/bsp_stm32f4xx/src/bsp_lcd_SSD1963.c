#include "bsp_lcd_SSD1963.h"
#include "string.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bsp_uart.h"

// 检查LCD是否正确
uint8_t a[5];
bool SSD1963_Check(void)
{
	lcd_param.id = 0x1963;
	lcd_param.wramcmd = SSD1963_CMD_WMS;
	
	LCD_WR_REG(0xA1); // 读取第一字节时而等于1，时而等于0
	a[0] = LCD_RD_DATA();
	a[1] = LCD_RD_DATA();
	a[2] = LCD_RD_DATA();
	a[3] = LCD_RD_DATA();
	a[4] = LCD_RD_DATA();
	return ((a[0] | 0x01) == 0x01) && (a[1]  == 0x57) && (a[2] == 0x61) && (a[3] == 0x01) && (a[4] == 0xFF);
//	return ((LCD_RD_DATA() | 0x01) == 0x01) && (LCD_RD_DATA() == 0x57) && (LCD_RD_DATA() == 0x61) && (LCD_RD_DATA() == 0x01) && (LCD_RD_DATA() == 0xFF);
}

// LCD初始化
void SSD1963_Init(void)
{
	// 重新配置写时序控制寄存器的时序	 							    
	FSMC_Bank1E->BWTR[6] &= ~(0XF<<0);	// 地址建立时间（ADDSET）清零
	FSMC_Bank1E->BWTR[6] &= ~(0XF<<8);	// 数据保存时间清零
	FSMC_Bank1E->BWTR[6] |= 3<<0;				// 地址建立时间（ADDSET）为3个HCLK = 18ns  	 
	FSMC_Bank1E->BWTR[6] |= 2<<8;				// 数据保存时间（DATAST）为6ns*2个HCLK=12ns
	
	LCD_WR_REG(SSD1963_CMD_SPll);				//Set PLL with OSC = 10MHz (hardware),	Multiplier M = 29, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 300MHz
	LCD_WR_DATA(0x1D);									//参数1 Multiplier M = 29。 VCO = Reference input clock  x  (M + 1) = 300MHz
	LCD_WR_DATA(0x02);									//参数2 Divider N = 2。     PLL = VCO / (N+1) = 100MHz
	LCD_WR_DATA(0x04);									//参数3 Validate M and N values   

	LCD_WR_REG(SSD1963_CMD_StPll);			// Start PLL command
	LCD_WR_DATA(0x01);									// enable PLL
	vTaskDelay(1);											// 使能PLL后至少100us才能锁定时钟，此处延时1ms
	LCD_WR_REG(SSD1963_CMD_StPll);			// Start PLL command again
	LCD_WR_DATA(0x03);									// now, use PLL output as system clock	
	LCD_WR_REG(SSD1963_CMD_Reset);			// 软复位
	
	LCD_WR_REG(SSD1963_CMD_SLF);				// 设置像素频率，100Mhz 。  PLL * ( LCDC_FPR+ 1) / 2^20。     100Mhz = 100Mhz * (LCDC_FPR+ 1) / 2^20 
	LCD_WR_DATA(0x07);									// LCDC_FPR = 2^20-1
	LCD_WR_DATA(0xFF);
	LCD_WR_DATA(0xFF);
	
	LCD_WR_REG(SSD1963_CMD_SLM);				// 设置LCD模式
	LCD_WR_DATA(0x20);									// 24位模式
	LCD_WR_DATA(0x00);									// TFT 模式 

	LCD_WR_DATA((SSD_HOR_RESOLUTION-1)>>8);	// 设置LCD水平像素
	LCD_WR_DATA(SSD_HOR_RESOLUTION-1);		 
	LCD_WR_DATA((SSD_VER_RESOLUTION-1)>>8);	// 设置LCD垂直像素
	LCD_WR_DATA(SSD_VER_RESOLUTION-1);		 
	LCD_WR_DATA(0x00);									// RGB序列 
	
	LCD_WR_REG(SSD1963_CMD_SHP);				// Set horizontal period
	LCD_WR_DATA((SSD_HT-1)>>8);					// horizontal total period (display + non-display) in pixel clock
	LCD_WR_DATA(SSD_HT-1);
	LCD_WR_DATA(SSD_HPS>>8);						// non-display period between the start of the horizontal sync (LLINE) signal and the first display data
	LCD_WR_DATA(SSD_HPS);
	LCD_WR_DATA(SSD_HOR_PULSE_WIDTH-1);	// Set the horizontal sync pulse width (LLINE) in pixel clock.
	LCD_WR_DATA(0x00);									// Set the horizontal sync pulse (LLINE) start location in pixel clock.
	LCD_WR_DATA(0x00);									// Set the horizontal sync pulse width (LLINE) in start
	LCD_WR_DATA(0x00);									// Set the horizontal sync pulse subpixel start position for serial TFT interface
	
	LCD_WR_REG(SSD1963_CMD_SVP);				// Set vertical period
	LCD_WR_DATA((SSD_VT-1)>>8);					// vertical total (display + non-display) period in lines
	LCD_WR_DATA(SSD_VT-1);
	LCD_WR_DATA(SSD_VPS>>8);						// non-display period in lines between the start of the frame and the first display data in line
	LCD_WR_DATA(SSD_VPS);
	LCD_WR_DATA(SSD_VER_FRONT_PORCH-1); // Set the vertical sync pulse width (LFRAME) in lines.
	LCD_WR_DATA(0x00>>8);								// High byte of the vertical sync pulse (LFRAME) start location in lines
	LCD_WR_DATA(0x00);									// Low byte of the vertical sync pulse (LFRAME) start location in lines
	
	LCD_WR_REG(SSD1963_CMD_SPDI);				// 设置SSD1963与MCU接口为16bit  
	LCD_WR_DATA(0x03);									// 16-bit(565 format) data for 16bpp 

	LCD_WR_REG(SSD1963_CMD_SDOn);				// 开启显示
	
	LCD_WR_REG(SSD1963_CMD_SPC);				// 配置PWM输出
	LCD_WR_DATA(0x05);									// 设置PWM频率。	100Mhz / (256 * PWMF) / 256 = 305.17578125Hz
	LCD_WR_DATA(0xFF);									// 设置PWM占空比。最大占空比 99.609375%
	LCD_WR_DATA(0x01);									// 0x01 PWM控制		0x09 DBC控制
	LCD_WR_DATA(0xFF);									// DBC manual brightness
	LCD_WR_DATA(0x00);									// DBC minimum brightness
	LCD_WR_DATA(0x00);									// Brightness prescaler
	
	LCD_WR_REG(SSD1963_CMD_SDC);				// 设置自动白平衡DBC
	LCD_WR_DATA(0x00);									// 0xD0 disable    0x0D enable
	
	LCD_WR_REG(SSD1963_CMD_SGC);				// 设置GPIO配置
	LCD_WR_DATA(0x03);									// GPIO0 & GPIO1 is output 
	LCD_WR_DATA(0x01);									// GPIO使用正常的IO功能
	
//	LCD_WR_REG(SSD1963_CMD_SGV);				// GPIO输出值
//	LCD_WR_DATA(0X01);									// GPIO[1:0] = 01，控制LCD方向
}

// LCD设置屏幕方向。默认颜色采用RGB格式，屏幕方向竖屏，扫描方向，从左到右，从上到下
void SSD1963_SetDisplayDirection(DIRECTION direction, uint16_t width, uint16_t height)
{
	uint16_t value;
	switch(direction)
	{
		case UP:
			lcd_param.direction = UP;
			value = 0 << 7 | 0 << 6 | 0 << 5;
			break;
		case LEFT:
			lcd_param.direction = LEFT;
			value = 0 << 7 | 0 << 6 | 0 << 5;
			break;
		case DOWN:
			lcd_param.direction = DOWN;
			value = 0 << 7 | 0 << 6 | 0 << 5;
			break;
		case RIGHT:
			lcd_param.direction = RIGHT;
			value = 0 << 7 | 0 << 6 | 0 << 5;
			break;
	}
	LCD_WriteReg(SSD1963_CMD_SAM, value);
	
	if(direction == UP || direction == DOWN)
	{
		lcd_param.width  = width < height ? width : height;
		lcd_param.height = height > width ? height : width;
	}
	else
	{
		lcd_param.width  = width > height ? width : height;
		lcd_param.height = height < width ? height : width;
	}
	
	if(direction == LEFT)
	{
		LCD_WR_REG(SSD1963_CMD_SGV);				// GPIO输出值
		LCD_WR_DATA(0X02);									// GPIO[1:0] = 01，控制LCD方向
	} 
	else if(direction == RIGHT)
	{
		LCD_WR_REG(SSD1963_CMD_SGV);				// GPIO输出值
		LCD_WR_DATA(0X01);									// GPIO[1:0] = 10，控制LCD方向
	}
	
	// 限制CPU访问屏幕内存的大小，再重新设置访问内并超出此前设置的范围后，前面的设置将失效
	LCD_WR_REG(SSD1963_CMD_SCAdd);	
	LCD_WR_DATA(0x00>>8);						
	LCD_WR_DATA(0x00);
	LCD_WR_DATA((lcd_param.width-1)>>8);						
	LCD_WR_DATA((lcd_param.width-1)&0xff);
	
	LCD_WR_REG(SSD1963_CMD_SPAdd);	
	LCD_WR_DATA(0x00>>8);						
	LCD_WR_DATA(0x00);
	LCD_WR_DATA((lcd_param.height-1)>>8);						
	LCD_WR_DATA((lcd_param.height-1)&0xff);	
}

// 水平翻转
void SSD1963_FlipHorizontal(void)
{
	uint16_t value = LCD_ReadReg(SSD1963_CMD_SAM);
	if(lcd_param.direction == UP || lcd_param.direction == DOWN)
	{
		if(value & 1<<1)
		{
			value &= ~(1<<1);
		}
		else
		{
			value |= 1<<1;
		}
	}
	else
	{
		if(value & 1<<0)
		{
			value &= ~(1<<0);
		}
		else
		{
			value |= 1<<0;
		}
	}
	LCD_WriteReg(SSD1963_CMD_SAM, value);
}

// LCD设置指针位置
void SSD1963_SetCursor(uint16_t x, uint16_t y)
{
	if(lcd_param.direction == UP || lcd_param.direction == DOWN)
	{
		LCD_WR_REG(SSD1963_CMD_SCAdd);
		LCD_WR_DATA(0x00>>8);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA((lcd_param.width-1 - x)>>8);
		LCD_WR_DATA((lcd_param.width-1 - x)&0xff);
	}
	else
	{
		LCD_WR_REG(SSD1963_CMD_SCAdd);
		LCD_WR_DATA(x>>8);
		LCD_WR_DATA(x&0xff);
		LCD_WR_DATA((lcd_param.width-1)>>8);
		LCD_WR_DATA((lcd_param.width-1)&0xff);
	}
	LCD_WR_REG(SSD1963_CMD_SPAdd);
	LCD_WR_DATA(y>>8);
	LCD_WR_DATA(y&0xff);
	LCD_WR_DATA((lcd_param.height-1)>>8);
	LCD_WR_DATA((lcd_param.height-1)&0xff);
}

// LCD画点
void SSD1963_DrawPoint(uint16_t x, uint16_t y, uint16_t PixelIndex)
{	
	// SSD1963_SetCursor(x, y);

	LCD_WR_REG(SSD1963_CMD_SCAdd); 
	LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF); 		
	LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF); 		
	LCD_WR_REG(SSD1963_CMD_SPAdd); 
	LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 		
	LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 			

	LCD_WriteReg(lcd_param.wramcmd,   PixelIndex);
}

uint16_t SSD1963_ReadPoint(uint16_t x, uint16_t y)
{
	/*-------------------------------------------------------------------------------------------------*
	* 顺序	 |	 控制       |                           各位描述									           |  HEX    *
  *        | RS | RD | WR | D15~D11 | D10 | D9 D8 | D7 | D6  | D5 | D4 | D3 | D2 | D1 | D0 |         *
	* 指令   | 0  | 1  | ↑  |          2EH          | 0  | 0   | 0  | 0  | 0  | 0  | 0  | 0  | 2E00H   *
	* 参数1  | 1  | ↑  | 1  |                            XX                                  | dummy   *
  * 参数2  | 1  | ↑  | 1  | R1[4:0] |     XX      |            G1[5:0]           |    XX   | R1 G1   *
  * 参数3  | 1  | ↑  | 1  | B1[4:0] |     XX      |          R2[4:0]        |       XX     | B1 R2   *
  * 参数4  | 1  | ↑  | 1  |    G2[5:0]    |   XX  |          B2[4:0]        |       XX     | G2 B2   *
  * 参数5  | 1  | ↑  | 1  | R3[4:0] |     XX      |            G3[5:0]           |    XX   | R3 G3   *
  * 参数N  | 1  | ↑  | 1  |             按以上规律输出                                               *
	*--------------------------------------------------------------------------------------------------*/
	uint16_t r, g, b;
	if(x>=lcd_param.width || y>=lcd_param.height) // 超出屏幕范围
		return 0;
	
	SSD1963_SetCursor(x, y);
	
	LCD_WR_REG(0X2E00); // 读颜色指令
	r = LCD_RD_DATA();  // 读dummy
	r = LCD_RD_DATA();  // 读R1 G1
	g = r & 0xff;				
	b = LCD_RD_DATA();	// 读B1 R2
	return (r & 0xF800) | (g & 0x00FC) << 3 | (b & 0xF8) >> 11; // 颜色处理，拼接为 RGB565
}

// 矩形填充
void SSD1963_FillRect(int x0, int y0, int x1, int y1, uint16_t PixelIndex)
{
	for(uint16_t y=y0; y<=y1; y++)
	{
		SSD1963_SetCursor(x0, y);
		LCD_WR_CMD();
		for(uint16_t x=x0; x<=x1; x++)
		{
			LCD_WR_DATA(PixelIndex);
		}
	}
}

// 画水平线
void SSD1963_DrawHLine(int x0, int y, int x1, uint16_t PixelIndex)
{
	SSD1963_SetCursor(x0, y);
	LCD_WR_CMD();
	for(uint16_t x=x0; x<=x1; x++)
	{
		LCD_WR_DATA(PixelIndex);
	}
}

// 清屏
void SSD1963_Clear(uint16_t PixelIndex)
{
	SSD1963_SetCursor(0, 0);
	LCD_WR_CMD();
	uint32_t area = lcd_param.width * lcd_param.height;
	
	for(uint32_t i=0; i<area; i++)
	{
		LCD_WR_DATA(PixelIndex);
	}
}

//SSD1963 背光设置。	pwm：背光等级，0~0xFF。越大越亮。
void SSD1963_SetBackLight(uint8_t pwm)
{	
	LCD_WR_REG(SSD1963_CMD_SPC);				// 配置PWM输出
	LCD_WR_DATA(0x05);									// 设置PWM频率。	100Mhz / (256 * PWMF) / 256 = 305.17578125Hz
	LCD_WR_DATA(pwm);										// 设置PWM占空比。最大占空比 99.609375%
	LCD_WR_DATA(0x01);									// 0x01 PWM控制		0x09 DBC控制
	LCD_WR_DATA(0xFF);									// DBC manual brightness
	LCD_WR_DATA(0x00);									// DBC minimum brightness
	LCD_WR_DATA(0x00);									// Brightness prescaler
}
