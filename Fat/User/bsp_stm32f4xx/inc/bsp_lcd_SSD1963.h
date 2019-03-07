#ifndef _BSP_LCD_SSD1963_H_
#define _BSP_LCD_SSD1963_H_

#include "stdint.h"
#include "stdbool.h"
#include "bsp_lcd.h"

#define SSD1963_CMD_Nop                     0x00
#define SSD1963_CMD_Reset                   0x01
#define SSD1963_CMD_GPM                     0x0A
#define SSD1963_CMD_GAM                     0x0B
#define SSD1963_CMD_GDM                     0x0D
#define SSD1963_CMD_EnSM                    0x10
#define SSD1963_CMD_ExSM                    0x11
#define SSD1963_CMD_EnPM                    0x12
#define SSD1963_CMD_EnNM                    0x13
#define SSD1963_CMD_ExIM                    0x20
#define SSD1963_CMD_EnIM                    0x21
#define SSD1963_CMD_SGaC                    0x26
#define SSD1963_CMD_SDOff                   0x28
#define SSD1963_CMD_SDOn                    0x29
#define SSD1963_CMD_SCAdd                   0x2A
#define SSD1963_CMD_SPAdd                   0x2B
#define SSD1963_CMD_WMS                     0x2C
#define SSD1963_CMD_RMS                     0x2E
#define SSD1963_CMD_SPArea                  0x30
#define SSD1963_CMD_SSArea                  0x33
#define SSD1963_CMD_STOff                   0x34
#define SSD1963_CMD_STOn                    0x35
#define SSD1963_CMD_SAM                     0x36
#define SSD1963_CMD_SSS                     0x37
#define SSD1963_CMD_ExIdM                   0x38
#define SSD1963_CMD_EnIdM                   0x39
#define SSD1963_CMD_WMC                     0x3C
#define SSD1963_CMD_RMC                     0x3E
#define SSD1963_CMD_STS                     0x44
#define SSD1963_CMD_GS                      0x45
#define SSD1963_CMD_RDDB                    0xA1
#define SSD1963_CMD_SLM                     0xB0
#define SSD1963_CMD_GLM                     0xB1
#define SSD1963_CMD_SHP                     0xB4
#define SSD1963_CMD_GHP                     0xB5
#define SSD1963_CMD_SVP                     0xB6
#define SSD1963_CMD_GVP                     0xB7
#define SSD1963_CMD_SGC                     0xB8
#define SSD1963_CMD_GGC                     0xB9
#define SSD1963_CMD_SGV                     0xBA
#define SSD1963_CMD_GGS                     0xBB
#define SSD1963_CMD_SPP                     0xBC
#define SSD1963_CMD_GPP                     0xBD
#define SSD1963_CMD_SPC                     0xBE
#define SSD1963_CMD_GPC                     0xBF
#define SSD1963_CMD_SLG0                    0xC0
#define SSD1963_CMD_GLG0                    0xC1
#define SSD1963_CMD_SLG1                    0xC2
#define SSD1963_CMD_GLG1                    0xC3
#define SSD1963_CMD_SLG2                    0xC4
#define SSD1963_CMD_GLG2                    0xC5
#define SSD1963_CMD_SLG3                    0xC6
#define SSD1963_CMD_GLG3                    0xC7
#define SSD1963_CMD_SG0R                    0xC8
#define SSD1963_CMD_GG0R                    0xC9
#define SSD1963_CMD_SG1R                    0xCA
#define SSD1963_CMD_GG1R                    0xCB
#define SSD1963_CMD_SG2R                    0xCC
#define SSD1963_CMD_GG2R                    0xCD
#define SSD1963_CMD_SG3R                    0xCE
#define SSD1963_CMD_GG3R                    0xCF
#define SSD1963_CMD_SDC                     0xD0
#define SSD1963_CMD_GDC                     0xD1
#define SSD1963_CMD_SDT                     0xD4
#define SSD1963_CMD_GDT                     0xD5
#define SSD1963_CMD_StPll                   0xE0
#define SSD1963_CMD_SPll                    0xE2
#define SSD1963_CMD_GPll                    0xE3
#define SSD1963_CMD_GPllS                   0xE4
#define SSD1963_CMD_SDS                     0xE5
#define SSD1963_CMD_SLF                     0xE6
#define SSD1963_CMD_GLF                     0xE7
#define SSD1963_CMD_SPDI                    0xF0
#define SSD1963_CMD_GPDI                    0xF1

//LCD分辨率设置
#define SSD_HOR_RESOLUTION		800		//LCD水平分辨率
#define SSD_VER_RESOLUTION		480		//LCD垂直分辨率
//LCD驱动参数设置
#define SSD_HOR_PULSE_WIDTH		30		//水平脉宽
#define SSD_HOR_BACK_PORCH		16		//水平前廊
#define SSD_HOR_FRONT_PORCH		210		//水平后廊

#define SSD_VER_PULSE_WIDTH		13		//垂直脉宽
#define SSD_VER_BACK_PORCH		10		//垂直前廊
#define SSD_VER_FRONT_PORCH		22		//垂直前廊

//如下几个参数，自动计算
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_PULSE_WIDTH+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_PULSE_WIDTH+SSD_HOR_BACK_PORCH)
#define SSD_VT 	(SSD_VER_PULSE_WIDTH+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH+SSD_VER_RESOLUTION)
#define SSD_VPS (SSD_VER_PULSE_WIDTH+SSD_VER_BACK_PORCH)

// 检查LCD是否正确
bool SSD1963_Check(void);

// LCD初始化
void SSD1963_Init(void);

// LCD设置屏幕方向
void SSD1963_SetDisplayDirection(DIRECTION direction, uint16_t width, uint16_t height);

// 水平翻转
void SSD1963_FlipHorizontal(void);
	
// LCD设置指针位置
void SSD1963_SetCursor(uint16_t Xpos, uint16_t Ypos);

// LCD画点
void SSD1963_DrawPoint(uint16_t x, uint16_t y, uint16_t PixelIndex);

// LCD读点
uint16_t SSD1963_ReadPoint(uint16_t x, uint16_t y);

// 矩形填充
void SSD1963_FillRect(int x0, int y0, int x1, int y1, uint16_t PixelIndex);

// 画水平线
void SSD1963_DrawHLine(int x0, int y, int x1, uint16_t PixelIndex);

// 清屏
void SSD1963_Clear(uint16_t PixelIndex);

// SSD1963 背光设置。	pwm：背光等级，0~0xFF。越大越亮。
void SSD1963_SetBackLight(uint8_t pwm);

// 开启显示
void SSD1963_DisplayOn(void);

// 关闭显示
void SSD1963_DisplayOff(void);

#endif
