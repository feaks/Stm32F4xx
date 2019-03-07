#ifndef _BSP_LCD_H_
#define _BSP_LCD_H_

#include "stm32f4xx.h"
#include "stdint.h"
#include "stdbool.h"

// 显示方向定义
typedef enum
{
	UP,		// 向上
	LEFT,	// 向左
	DOWN,	// 向下
	RIGHT	// 向右
} DIRECTION;

// LCD默认方向
#define DFT_LCD_DIRECTION 			RIGHT

// 颜色定义
#define LCD_WHITE							0xFFFF
#define LCD_BLACK							0x0000
#define LCD_BLUE							0x001F  
#define LCD_BRED							0XF81F
#define LCD_GRED							0XFFE0
#define LCD_GBLUE							0X07FF
#define LCD_RED								0xF800
#define LCD_MAGENTA						0xF81F
#define LCD_GREEN							0x07E0
#define LCD_CYAN							0x7FFF
#define LCD_YELLOW						0xFFE0
#define LCD_BROWN							0XBC40
#define LCD_BRRED							0XFC07
#define LCD_GRAY							0X8430

// LCD地址结构体
typedef struct
{
	volatile uint16_t LCD_REG; // 0x6C00007E   	FSMC_A6 = 0
	volatile uint16_t LCD_RAM; // 0x6C000080		FSMC_A6 = 1
} LCD_AddrTypeDef;

// LCD设备参数
typedef struct  
{										    
	uint16_t			width;				// LCD 宽度
	uint16_t			height;				// LCD 高度
	uint16_t			id;						// LCD ID
	uint16_t			wramcmd;			// 写GRAM指令
	DIRECTION 		direction;		// 屏幕方向
} LCD_PARAM;

typedef struct
{
	void (*SetCursor)(uint16_t Xpos, uint16_t Ypos);
	bool (*Check)();
	void (*Init)();
	void (*SetDisplayDirection)(DIRECTION direction, uint16_t width, uint16_t height);
	void (*FlipHorizontal)(void);
	void (*DrawPoint)(uint16_t x, uint16_t y, uint16_t PixelIndex);
	uint16_t (*ReadPoint)(uint16_t x, uint16_t y);
	void (*FillRect)(int x0, int y0, int x1, int y1, uint16_t PixelIndex);
	void (*DrawHLine)(int x0, int y, int x1, uint16_t PixelIndex);
	void (*Clear)(uint16_t PixelIndex);
} LCD;

extern LCD_PARAM lcd_param;	// LCD设备参数
extern LCD 			 lcd;				// LCD控制函数

// 使用NOR/SRAM的Bank1.sector4，地址位HADDR[27，26] =11 , FSMC_A6 作为数据命令区分线
// 注意设置时STM32内部会右移一位对齐！111 1110 = 0x7E
#define LCD_BASE        				((uint32_t)(0x6C000000 | 0x0000007E))
#define LCD_ADDR         				((LCD_AddrTypeDef *) LCD_BASE)

#define LCD_BL_ON  	GPIOB->BSRRL = GPIO_Pin_15
#define LCD_BL_OFF 	GPIOB->BSRRH = GPIO_Pin_15

// 写指令
static inline void LCD_WR_REG(volatile uint16_t reg)
{
	reg = reg;
	LCD_ADDR->LCD_REG = reg;
}

// 写数据
void LCD_WR_DATA(volatile uint16_t data);
//static inline void LCD_WR_DATA(volatile uint16_t data)
//{
//	data = data;
//	LCD_ADDR->LCD_RAM = data;
//}

// 发送写GRAM指令
void LCD_WR_CMD();			

// 读数据
static inline uint16_t LCD_RD_DATA()
{
	volatile uint16_t data;
	data = LCD_ADDR->LCD_RAM;
	return data;
}	

// 写寄存器
static inline void LCD_WriteReg(volatile uint16_t reg, volatile uint16_t value)
{
	LCD_ADDR->LCD_REG = reg;
	LCD_ADDR->LCD_RAM = value;
}

// 读寄存器
static inline uint16_t LCD_ReadReg(volatile uint16_t reg)
{
	LCD_WR_REG(reg);
	return LCD_RD_DATA();
}

// LCD初始化
void LCD_Init(DIRECTION direction, uint16_t width, uint16_t height);

#endif
