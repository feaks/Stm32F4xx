#ifndef _BSP_LCD_NT35510_H_
#define _BSP_LCD_NT35510_H_

#include "stdint.h"
#include "stdbool.h"
#include "bsp_lcd.h"

#define Column_Address_Set_XSH			0x2A00		// X轴起始坐标高位设置
#define Column_Address_Set_XSL			0x2A01		// X轴起始坐标低位设置
#define Column_Address_Set_XEH			0x2A02		// X轴结束坐标高位设置    最大值应 <= （分辨率-1）
#define Column_Address_Set_XEL			0x2A03		// X轴结束坐标低位设置
#define Column_Address_Set_YSH			0x2B00		// Y轴起始坐标高位设置
#define Column_Address_Set_YSL			0x2B01		// Y轴起始坐标低位设置
#define Column_Address_Set_YEH			0x2B02		// Y轴结束坐标高位设置    最大值应 <= （分辨率-1）
#define Column_Address_Set_YEL			0x2B03		// Y轴结束坐标低位设置
#define Memory_Write								0x2C00		// 写内存指令
#define Memory_Read									0x2E00		// 读内存指令
#define Memory_Data_Access_Control 	0x3600		// 扫描顺序，颜色顺序，镜面翻转控制
#define Read_ID1_Value							0xDA00		// ID1
#define Read_ID2_Value							0xDB00		// ID2
#define Read_ID3_Value							0xDC00		// ID3


// 检查LCD是否正确
bool NT35510_Check(void);

// LCD初始化
void NT35510_Init(void);

// LCD设置屏幕方向
void NT35510_SetDisplayDirection(DIRECTION direction, uint16_t width, uint16_t height);

// 水平翻转
void NT35510_FlipHorizontal(void);
	
// LCD设置指针位置
void NT35510_SetCursor(uint16_t Xpos, uint16_t Ypos);

// LCD画点
void NT35510_DrawPoint(uint16_t x, uint16_t y, uint16_t PixelIndex);

// LCD读点
uint16_t NT35510_ReadPoint(uint16_t x, uint16_t y);

// 矩形填充
void NT35510_FillRect(int x0, int y0, int x1, int y1, uint16_t PixelIndex);

// 画水平线
void NT35510_DrawHLine(int x0, int y, int x1, uint16_t PixelIndex);

// 清屏
void NT35510_Clear(uint16_t PixelIndex);

// 开启显示
void NT35510_DisplayOn(void);

// 关闭显示
void NT35510_DisplayOff(void);

#endif
