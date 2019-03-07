#include "bsp_lcd_NT35510.h"
#include "string.h"

// 检查LCD是否正确
bool NT35510_Check(void)
{
	lcd_param.id = 0x5510;
	lcd_param.wramcmd = Memory_Write;
			
	return (LCD_ReadReg(0xDA00) == 0x00) && (LCD_ReadReg(0xDB00) == 0x80) && (LCD_ReadReg(0xDC00) == 0x00);
}

// LCD初始化
void NT35510_Init(void)
{
	// 重新配置写时序控制寄存器的时序	 							    
	FSMC_Bank1E->BWTR[6] &= ~(0XF<<0); // 地址建立时间（ADDSET）清零
	FSMC_Bank1E->BWTR[6] &= ~(0XF<<8); // 数据保存时间清零
	FSMC_Bank1E->BWTR[6] |= 3<<0;			 // 地址建立时间（ADDSET）为3个HCLK = 18ns  	 
	FSMC_Bank1E->BWTR[6] |= 2<<8; 		 // 数据保存时间（DATAST）为6ns*2个HCLK=12ns
	
	// LV2 Page 1 enable
	LCD_WriteReg(0xF000, 0x55);
	LCD_WriteReg(0xF001, 0xAA);
	LCD_WriteReg(0xF002, 0x52);
	LCD_WriteReg(0xF003, 0x08);
	LCD_WriteReg(0xF004, 0x01);
	
	// AVDD Set AVDD 5.2V
	LCD_WriteReg(0xB000, 0x0D);
	LCD_WriteReg(0xB001, 0x0D);
	LCD_WriteReg(0xB002, 0x0D);
	
	// AVDD ratio
	LCD_WriteReg(0xB600, 0x34);
	LCD_WriteReg(0xB601, 0x34);
	LCD_WriteReg(0xB602, 0x34);
	
	// AVEE -5.2V
	LCD_WriteReg(0xB100, 0x0D);
	LCD_WriteReg(0xB101, 0x0D);
	LCD_WriteReg(0xB102, 0x0D);
	
	// AVEE ratio
	LCD_WriteReg(0xB700, 0x35);
	LCD_WriteReg(0xB701, 0x35);
	LCD_WriteReg(0xB702, 0x35);
	
	// VCL -2.5V
	LCD_WriteReg(0xB200, 0x00);
	LCD_WriteReg(0xB201, 0x00);
	LCD_WriteReg(0xB202, 0x00);
	
	//VCL ratio
	LCD_WriteReg(0xB800,0x24);
	LCD_WriteReg(0xB801,0x24);
	LCD_WriteReg(0xB802,0x24);
	
	// VGH 15V
	LCD_WriteReg(0xBF00, 0x01);
	LCD_WriteReg(0xB300, 0x08);
	LCD_WriteReg(0xB301, 0x08);
	LCD_WriteReg(0xB302, 0x08);
	
	// VGH ratio
	LCD_WriteReg(0xB900, 0x34);
	LCD_WriteReg(0xB901, 0x34);
	LCD_WriteReg(0xB902, 0x34);

	// VGLX ratio
	LCD_WriteReg(0xBA00, 0x24);
	LCD_WriteReg(0xBA01, 0x24);
	LCD_WriteReg(0xBA02, 0x24);
	
	// VGMP/VGSP 4.7V/0V
	LCD_WriteReg(0xBC00, 0x00);
	LCD_WriteReg(0xBC01, 0x88);
	LCD_WriteReg(0xBC02, 0x00);
	
	// VGMN/VGSN -4.7V/0V
	LCD_WriteReg(0xBD00, 0x00);
	LCD_WriteReg(0xBD01, 0x88);
	LCD_WriteReg(0xBD02, 0x00);
	
	// VCOM -2.0375V
	LCD_WriteReg(0xBE00, 0x00);
	LCD_WriteReg(0xBE01, 0xA3);
	
	// Gamma Setting
	LCD_WriteReg(0xD100, 0x00);
	LCD_WriteReg(0xD101, 0x05);
	LCD_WriteReg(0xD102, 0x00);
	LCD_WriteReg(0xD103, 0x40);
	LCD_WriteReg(0xD104, 0x00);
	LCD_WriteReg(0xD105, 0x6D);
	LCD_WriteReg(0xD106, 0x00);
	LCD_WriteReg(0xD107, 0x90);
	LCD_WriteReg(0xD108, 0x00);
	LCD_WriteReg(0xD109, 0x99);
	LCD_WriteReg(0xD10A, 0x00);
	LCD_WriteReg(0xD10B, 0xBB);
	LCD_WriteReg(0xD10C, 0x00);
	LCD_WriteReg(0xD10D, 0xDC);
	LCD_WriteReg(0xD10E, 0x01);
	LCD_WriteReg(0xD10F, 0x04);
	LCD_WriteReg(0xD110, 0x01);
	LCD_WriteReg(0xD111, 0x25);
	LCD_WriteReg(0xD112, 0x01);
	LCD_WriteReg(0xD113, 0x59);
	LCD_WriteReg(0xD114, 0x01);
	LCD_WriteReg(0xD115, 0x82);
	LCD_WriteReg(0xD116, 0x01);
	LCD_WriteReg(0xD117, 0xC6);
	LCD_WriteReg(0xD118, 0x02);
	LCD_WriteReg(0xD119, 0x01);
	LCD_WriteReg(0xD11A, 0x02);
	LCD_WriteReg(0xD11B, 0x02);
	LCD_WriteReg(0xD11C, 0x02);
	LCD_WriteReg(0xD11D, 0x39);
	LCD_WriteReg(0xD11E, 0x02);
	LCD_WriteReg(0xD11F, 0x79);
	LCD_WriteReg(0xD120, 0x02);
	LCD_WriteReg(0xD121, 0xA1);
	LCD_WriteReg(0xD122, 0x02);
	LCD_WriteReg(0xD123, 0xD9);
	LCD_WriteReg(0xD124, 0x03);
	LCD_WriteReg(0xD125, 0x00);
	LCD_WriteReg(0xD126, 0x03);
	LCD_WriteReg(0xD127, 0x38);
	LCD_WriteReg(0xD128, 0x03);
	LCD_WriteReg(0xD129, 0x67);
	LCD_WriteReg(0xD12A, 0x03);
	LCD_WriteReg(0xD12B, 0x8F);
	LCD_WriteReg(0xD12C, 0x03);
	LCD_WriteReg(0xD12D, 0xCD);
	LCD_WriteReg(0xD12E, 0x03);
	LCD_WriteReg(0xD12F, 0xFD);
	LCD_WriteReg(0xD130, 0x03);
	LCD_WriteReg(0xD131, 0xFE);
	LCD_WriteReg(0xD132, 0x03);
	LCD_WriteReg(0xD133, 0xFF);
	
	LCD_WriteReg(0xD200, 0x00);
	LCD_WriteReg(0xD201, 0x05); 
	LCD_WriteReg(0xD202, 0x00);
	LCD_WriteReg(0xD203, 0x40);
	LCD_WriteReg(0xD204, 0x00);
	LCD_WriteReg(0xD205, 0x6D);
	LCD_WriteReg(0xD206, 0x00);
	LCD_WriteReg(0xD207, 0x90);
	LCD_WriteReg(0xD208, 0x00);
	LCD_WriteReg(0xD209, 0x99);
	LCD_WriteReg(0xD20A, 0x00);
	LCD_WriteReg(0xD20B, 0xBB);
	LCD_WriteReg(0xD20C, 0x00);
	LCD_WriteReg(0xD20D, 0xDC);
	LCD_WriteReg(0xD20E, 0x01);
	LCD_WriteReg(0xD20F, 0x04);
	LCD_WriteReg(0xD210, 0x01);
	LCD_WriteReg(0xD211, 0x25);
	LCD_WriteReg(0xD212, 0x01);
	LCD_WriteReg(0xD213, 0x59);
	LCD_WriteReg(0xD214, 0x01);
	LCD_WriteReg(0xD215, 0x82);
	LCD_WriteReg(0xD216, 0x01);
	LCD_WriteReg(0xD217, 0xC6);
	LCD_WriteReg(0xD218, 0x02);
	LCD_WriteReg(0xD219, 0x01);
	LCD_WriteReg(0xD21A, 0x02);
	LCD_WriteReg(0xD21B, 0x02);
	LCD_WriteReg(0xD21C, 0x02);
	LCD_WriteReg(0xD21D, 0x39);
	LCD_WriteReg(0xD21E, 0x02);
	LCD_WriteReg(0xD21F, 0x79);
	LCD_WriteReg(0xD220, 0x02);
	LCD_WriteReg(0xD221, 0xA1);
	LCD_WriteReg(0xD222, 0x02);
	LCD_WriteReg(0xD223, 0xD9);
	LCD_WriteReg(0xD224, 0x03);
	LCD_WriteReg(0xD225, 0x00);
	LCD_WriteReg(0xD226, 0x03);
	LCD_WriteReg(0xD227, 0x38);
	LCD_WriteReg(0xD228, 0x03);
	LCD_WriteReg(0xD229, 0x67);
	LCD_WriteReg(0xD22A, 0x03);
	LCD_WriteReg(0xD22B, 0xCD);
	LCD_WriteReg(0xD22C, 0x03);
	LCD_WriteReg(0xD22D, 0xCD);
	LCD_WriteReg(0xD22E, 0x03);
	LCD_WriteReg(0xD22F, 0xFD);
	LCD_WriteReg(0xD230, 0x03);
	LCD_WriteReg(0xD231, 0xFE);
	LCD_WriteReg(0xD232, 0x03);
	LCD_WriteReg(0xD233, 0xFF);

	LCD_WriteReg(0xD300, 0x00);
	LCD_WriteReg(0xD301, 0x05);
	LCD_WriteReg(0xD302, 0x00);
	LCD_WriteReg(0xD303, 0x40);
	LCD_WriteReg(0xD304, 0x00);
	LCD_WriteReg(0xD305, 0x6D);
	LCD_WriteReg(0xD306, 0x00);
	LCD_WriteReg(0xD307, 0x90);
	LCD_WriteReg(0xD308, 0x00);
	LCD_WriteReg(0xD309, 0x99);
	LCD_WriteReg(0xD30A, 0x00);
	LCD_WriteReg(0xD30B, 0xBB);
	LCD_WriteReg(0xD30C, 0x00);
	LCD_WriteReg(0xD30D, 0xDC);
	LCD_WriteReg(0xD30E, 0x01);
	LCD_WriteReg(0xD30F, 0x04);
	LCD_WriteReg(0xD310, 0x01);
	LCD_WriteReg(0xD311, 0x25);
	LCD_WriteReg(0xD312, 0x01);
	LCD_WriteReg(0xD313, 0x59);
	LCD_WriteReg(0xD314, 0x01);
	LCD_WriteReg(0xD315, 0x82);
	LCD_WriteReg(0xD316, 0x01);
	LCD_WriteReg(0xD317, 0xC6);
	LCD_WriteReg(0xD318, 0x02);
	LCD_WriteReg(0xD319, 0x01);
	LCD_WriteReg(0xD31A, 0x02);
	LCD_WriteReg(0xD31B, 0x02);
	LCD_WriteReg(0xD31C, 0x02);
	LCD_WriteReg(0xD31D, 0x39);
	LCD_WriteReg(0xD31E, 0x02);
	LCD_WriteReg(0xD31F, 0x79);
	LCD_WriteReg(0xD320, 0x02);
	LCD_WriteReg(0xD321, 0xA1);
	LCD_WriteReg(0xD322, 0x02);
	LCD_WriteReg(0xD323, 0xD9);
	LCD_WriteReg(0xD324, 0x03);
	LCD_WriteReg(0xD325, 0x00);
	LCD_WriteReg(0xD326, 0x03);
	LCD_WriteReg(0xD327, 0x38);
	LCD_WriteReg(0xD328, 0x03);
	LCD_WriteReg(0xD329, 0x67);
	LCD_WriteReg(0xD32A, 0x03);
	LCD_WriteReg(0xD32B, 0x8F);
	LCD_WriteReg(0xD32C, 0x03);
	LCD_WriteReg(0xD32D, 0xCD);
	LCD_WriteReg(0xD32E, 0x03);
	LCD_WriteReg(0xD32F, 0xFD);
	LCD_WriteReg(0xD330, 0x03);
	LCD_WriteReg(0xD331, 0xFE);
	LCD_WriteReg(0xD332, 0x03);
	LCD_WriteReg(0xD333, 0xFF);

	LCD_WriteReg(0xD400, 0x00);
	LCD_WriteReg(0xD401, 0x05);
	LCD_WriteReg(0xD402, 0x00);
	LCD_WriteReg(0xD403, 0x40);
	LCD_WriteReg(0xD404, 0x00);
	LCD_WriteReg(0xD405, 0x6D);
	LCD_WriteReg(0xD406, 0x00);
	LCD_WriteReg(0xD407, 0x90);
	LCD_WriteReg(0xD408, 0x00);
	LCD_WriteReg(0xD409, 0x99);
	LCD_WriteReg(0xD40A, 0x00);
	LCD_WriteReg(0xD40B, 0xBB);
	LCD_WriteReg(0xD40C, 0x00);
	LCD_WriteReg(0xD40D, 0xDC);
	LCD_WriteReg(0xD40E, 0x01);
	LCD_WriteReg(0xD40F, 0x04);
	LCD_WriteReg(0xD410, 0x01);
	LCD_WriteReg(0xD411, 0x25);
	LCD_WriteReg(0xD412, 0x01);
	LCD_WriteReg(0xD413, 0x59);
	LCD_WriteReg(0xD414, 0x01);
	LCD_WriteReg(0xD415, 0x82);
	LCD_WriteReg(0xD416, 0x01);
	LCD_WriteReg(0xD417, 0xC6);
	LCD_WriteReg(0xD418, 0x02);
	LCD_WriteReg(0xD419, 0x01);
	LCD_WriteReg(0xD41A, 0x02);
	LCD_WriteReg(0xD41B, 0x02);
	LCD_WriteReg(0xD41C, 0x02);
	LCD_WriteReg(0xD41D, 0x39);
	LCD_WriteReg(0xD41E, 0x02);
	LCD_WriteReg(0xD41F, 0x79);
	LCD_WriteReg(0xD420, 0x02);
	LCD_WriteReg(0xD421, 0xA1);
	LCD_WriteReg(0xD422, 0x02);
	LCD_WriteReg(0xD423, 0xD9);
	LCD_WriteReg(0xD424, 0x03);
	LCD_WriteReg(0xD425, 0x00);
	LCD_WriteReg(0xD426, 0x03);
	LCD_WriteReg(0xD427, 0x38);
	LCD_WriteReg(0xD428, 0x03);
	LCD_WriteReg(0xD429, 0x67);
	LCD_WriteReg(0xD42A, 0x03);
	LCD_WriteReg(0xD42B, 0x8F);
	LCD_WriteReg(0xD42C, 0x03);
	LCD_WriteReg(0xD42D, 0xCD);
	LCD_WriteReg(0xD42E, 0x03);
	LCD_WriteReg(0xD42F, 0xFD);
	LCD_WriteReg(0xD430, 0x03);
	LCD_WriteReg(0xD431, 0xFE);
	LCD_WriteReg(0xD432, 0x03);
	LCD_WriteReg(0xD433, 0xFF);

	LCD_WriteReg(0xD500, 0x00);
	LCD_WriteReg(0xD501, 0x05);
	LCD_WriteReg(0xD502, 0x00);
	LCD_WriteReg(0xD503, 0x40);
	LCD_WriteReg(0xD504, 0x00);
	LCD_WriteReg(0xD505, 0x6D);
	LCD_WriteReg(0xD506, 0x00);
	LCD_WriteReg(0xD507, 0x90);
	LCD_WriteReg(0xD508, 0x00);
	LCD_WriteReg(0xD509, 0x99);
	LCD_WriteReg(0xD50A, 0x00);
	LCD_WriteReg(0xD50B, 0xBB);
	LCD_WriteReg(0xD50C, 0x00);
	LCD_WriteReg(0xD50D, 0xDC);
	LCD_WriteReg(0xD50E, 0x01);
	LCD_WriteReg(0xD50F, 0x04);
	LCD_WriteReg(0xD510, 0x01);
	LCD_WriteReg(0xD511, 0x25);
	LCD_WriteReg(0xD512, 0x01);
	LCD_WriteReg(0xD513, 0x59);
	LCD_WriteReg(0xD514, 0x01);
	LCD_WriteReg(0xD515, 0x82);
	LCD_WriteReg(0xD516, 0x01);
	LCD_WriteReg(0xD517, 0xC6);
	LCD_WriteReg(0xD518, 0x02);
	LCD_WriteReg(0xD519, 0x01);
	LCD_WriteReg(0xD51A, 0x02);
	LCD_WriteReg(0xD51B, 0x02);
	LCD_WriteReg(0xD51C, 0x02);
	LCD_WriteReg(0xD51D, 0x39);
	LCD_WriteReg(0xD51E, 0x02);
	LCD_WriteReg(0xD51F, 0x79);
	LCD_WriteReg(0xD520, 0x02);
	LCD_WriteReg(0xD521, 0xA1);
	LCD_WriteReg(0xD522, 0x02);
	LCD_WriteReg(0xD523, 0xD9);
	LCD_WriteReg(0xD524, 0x03);
	LCD_WriteReg(0xD525, 0x00);
	LCD_WriteReg(0xD526, 0x03);
	LCD_WriteReg(0xD527, 0x38);
	LCD_WriteReg(0xD528, 0x03);
	LCD_WriteReg(0xD529, 0x67);
	LCD_WriteReg(0xD52A, 0x03);
	LCD_WriteReg(0xD52B, 0x8F);
	LCD_WriteReg(0xD52C, 0x03);
	LCD_WriteReg(0xD52D, 0xCD);
	LCD_WriteReg(0xD52E, 0x03);
	LCD_WriteReg(0xD52F, 0xFD);
	LCD_WriteReg(0xD530, 0x03);
	LCD_WriteReg(0xD531, 0xFE);
	LCD_WriteReg(0xD532, 0x03);
	LCD_WriteReg(0xD533, 0xFF);

	LCD_WriteReg(0xD600, 0x00);
	LCD_WriteReg(0xD601, 0x05);
	LCD_WriteReg(0xD602, 0x00);
	LCD_WriteReg(0xD603, 0x40);
	LCD_WriteReg(0xD604, 0x00);
	LCD_WriteReg(0xD605, 0x6D);
	LCD_WriteReg(0xD606, 0x00);
	LCD_WriteReg(0xD607, 0x90);
	LCD_WriteReg(0xD608, 0x00);
	LCD_WriteReg(0xD609, 0x99);
	LCD_WriteReg(0xD60A, 0x00);
	LCD_WriteReg(0xD60B, 0xBB);
	LCD_WriteReg(0xD60C, 0x00);
	LCD_WriteReg(0xD60D, 0xDC);
	LCD_WriteReg(0xD60E, 0x01);
	LCD_WriteReg(0xD60F, 0x04);
	LCD_WriteReg(0xD610, 0x01);
	LCD_WriteReg(0xD611, 0x25);
	LCD_WriteReg(0xD612, 0x01);
	LCD_WriteReg(0xD613, 0x59);
	LCD_WriteReg(0xD614, 0x01);
	LCD_WriteReg(0xD615, 0x82);
	LCD_WriteReg(0xD616, 0x01);
	LCD_WriteReg(0xD617, 0xC6);
	LCD_WriteReg(0xD618, 0x02);
	LCD_WriteReg(0xD619, 0x01);
	LCD_WriteReg(0xD61A, 0x02);
	LCD_WriteReg(0xD61B, 0x02);
	LCD_WriteReg(0xD61C, 0x02);
	LCD_WriteReg(0xD61D, 0x39);
	LCD_WriteReg(0xD61E, 0x02);
	LCD_WriteReg(0xD61F, 0x79);
	LCD_WriteReg(0xD620, 0x02);
	LCD_WriteReg(0xD621, 0xA1);
	LCD_WriteReg(0xD622, 0x02);
	LCD_WriteReg(0xD623, 0xD9);
	LCD_WriteReg(0xD624, 0x03);
	LCD_WriteReg(0xD625, 0x00);
	LCD_WriteReg(0xD626, 0x03);
	LCD_WriteReg(0xD627, 0x38);
	LCD_WriteReg(0xD628, 0x03);
	LCD_WriteReg(0xD629, 0x67);
	LCD_WriteReg(0xD62A, 0x03);
	LCD_WriteReg(0xD62B, 0x8F);
	LCD_WriteReg(0xD62C, 0x03);
	LCD_WriteReg(0xD62D, 0xCD);
	LCD_WriteReg(0xD62E, 0x03);
	LCD_WriteReg(0xD62F, 0xFD);
	LCD_WriteReg(0xD630, 0x03);
	LCD_WriteReg(0xD631, 0xFE);
	LCD_WriteReg(0xD632, 0x03);
	LCD_WriteReg(0xD633, 0xFF);

	// LV2 Page 0 enable
	LCD_WriteReg(0xF000,0x55);
	LCD_WriteReg(0xF001,0xAA);
	LCD_WriteReg(0xF002,0x52);
	LCD_WriteReg(0xF003,0x08);
	LCD_WriteReg(0xF004,0x00);
	
	// Display control
	LCD_WriteReg(0xB100, 0xFC);
	LCD_WriteReg(0xB101, 0x00);
	
	// Source hold time
	LCD_WriteReg(0xB600, 0x05);
	
	// Gate EQ control
	LCD_WriteReg(0xB700, 0x70);
	LCD_WriteReg(0xB701, 0x70);
	
	// Source EQ control (Mode 2)
	LCD_WriteReg(0xB800, 0x01);
	LCD_WriteReg(0xB801, 0x05);
	LCD_WriteReg(0xB802, 0x05);
	LCD_WriteReg(0xB803, 0x05);
	
	// Inversion mode (2-dot)
	LCD_WriteReg(0xBC00, 0x00);
	LCD_WriteReg(0xBC01, 0x00);
	LCD_WriteReg(0xBC02, 0x00);
	
	// Timing control 8phase dual side/4H/4delay/RST_EN
	LCD_WriteReg(0xC900, 0xD0);
	LCD_WriteReg(0xC901, 0x02);
	LCD_WriteReg(0xC902, 0x50);
	LCD_WriteReg(0xC903, 0x50);
	LCD_WriteReg(0xC904, 0x50);
	
	// 16-bit/pixel
	LCD_WriteReg(0x3A00, 0x55);  
	
	LCD_WR_REG(0x1100);
	// delay_us(120);
	LCD_WR_REG(0x2900);
}


// LCD设置屏幕方向。默认颜色采用RGB格式，屏幕方向竖屏，扫描方向，从左到右，从上到下
void NT35510_SetDisplayDirection(DIRECTION direction, uint16_t width, uint16_t height)
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
			value = 1 << 7 | 0 << 6 | 1 << 5;
			break;
		case DOWN:
			lcd_param.direction = DOWN;
			value = 1 << 7 | 1 << 6 | 0 << 5;
			break;
		case RIGHT:
			lcd_param.direction = RIGHT;
			value = 0 << 7 | 1 << 6 | 1 << 5;
			break;
	}
	LCD_WriteReg(Memory_Data_Access_Control, value);
	
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
	
	// 限制CPU访问屏幕内存的大小，再重新设置访问内并超出此前设置的范围后，前面的设置将失效
	LCD_WriteReg(Column_Address_Set_XSH, 0);
	LCD_WriteReg(Column_Address_Set_XSL, 0);
	LCD_WriteReg(Column_Address_Set_XEH, (lcd_param.width-1)>>8);
	LCD_WriteReg(Column_Address_Set_XEL, (lcd_param.width-1)&0XFF);
		
	LCD_WriteReg(Column_Address_Set_YSH, 0);
	LCD_WriteReg(Column_Address_Set_YSL, 0);
	LCD_WriteReg(Column_Address_Set_YEH, (lcd_param.height-1)>>8);
	LCD_WriteReg(Column_Address_Set_YEL, (lcd_param.height-1)&0XFF);
}

// 水平翻转
void NT35510_FlipHorizontal(void)
{
	uint16_t value = LCD_ReadReg(Memory_Data_Access_Control);
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
	LCD_WriteReg(Memory_Data_Access_Control, value);
}

// LCD设置指针位置
inline void NT35510_SetCursor(uint16_t x, uint16_t y)
{
	LCD_WriteReg(Column_Address_Set_XSH, x>>8);
	LCD_WriteReg(Column_Address_Set_XSL, x);
	LCD_WriteReg(Column_Address_Set_YSH, y>>8);
	LCD_WriteReg(Column_Address_Set_YSL, y);
}

// LCD画点
void NT35510_DrawPoint(uint16_t x, uint16_t y, uint16_t PixelIndex)
{	
	NT35510_SetCursor(x, y);
	LCD_WriteReg(lcd_param.wramcmd,   PixelIndex);
}

uint16_t NT35510_ReadPoint(uint16_t x, uint16_t y)
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
	
	NT35510_SetCursor(x, y);
	
	LCD_WR_REG(0X2E00); // 读颜色指令
	r = LCD_RD_DATA();  // 读dummy
	r = LCD_RD_DATA();  // 读R1 G1
	g = r & 0xff;				
	b = LCD_RD_DATA();	// 读B1 R2
	return (r & 0xF800) | (g & 0x00FC) << 3 | (b & 0xF8) >> 11; // 颜色处理，拼接为 RGB565
}

// 矩形填充
void NT35510_FillRect(int x0, int y0, int x1, int y1, uint16_t PixelIndex)
{
	for(uint16_t y=y0; y<=y1; y++)
	{
		NT35510_SetCursor(x0, y);
		LCD_WR_CMD();
		for(uint16_t x=x0; x<=x1; x++)
		{
			LCD_WR_DATA(PixelIndex);
		}
	}
}

// 画水平线
void NT35510_DrawHLine(int x0, int y, int x1, uint16_t PixelIndex)
{
	NT35510_SetCursor(x0, y);
	LCD_WR_CMD();
	for(uint16_t x=x0; x<=x1; x++)
	{
		LCD_WR_DATA(PixelIndex);
	}
}

// 清屏
void NT35510_Clear(uint16_t PixelIndex)
{
	NT35510_SetCursor(0, 0);
	LCD_WR_CMD();
	uint32_t area = lcd_param.width * lcd_param.height;
	
	for(uint32_t i=0; i<area; i++)
	{
		LCD_WR_DATA(PixelIndex);
	}
}

