#ifndef _BSP_TOUCH_H_
#define _BSP_TOUCH_H_

#include "stdint.h"
#include "stdbool.h"
#include "bsp_lcd.h"

#define LCD_BL_RST 	GPIOB->BSRRL = GPIO_Pin_15
#define LCD_BL_OFF 	GPIOB->BSRRH = GPIO_Pin_15

#define MTOUCH_FLAG_DOWN 			(1 << 0)   // New touch point has touched the surface.
#define MTOUCH_FLAG_MOVE 			(1 << 1)   // Touch point has been moved.
#define MTOUCH_FLAG_UP   			(1 << 2)   // Touch point has released the surface.

// 触摸屏设备参数
typedef struct  
{										    
	uint16_t			width;				// TS 宽度
	uint16_t			height;				// TS 高度
	uint16_t			id;						// TS ID
	DIRECTION 		direction;		// 屏幕方向
} TS_PARAM;

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint8_t	 flags;	// MTOUCH_FLAG_DOWN、MTOUCH_FLAG_MOVE、MTOUCH_FLAG_UP
	uint8_t  id;		// Touch Id
} Point;

typedef struct
{
	bool (*Check)(void);
	void (*Init)(void);
	void (*SetTouchDirection)(DIRECTION direction, uint16_t width, uint16_t height);
	void (*Scan)(void);
	int (*MeasureX)(void);
	int (*MeasureY)(void);
	Point point[5];					// 触摸坐标点
	uint8_t number;					// 有效触摸点个数
} TS;

extern TS_PARAM ts_param; // 触摸屏设备参数
extern TS 			ts; 			// 触摸屏控制函数

void TS_Init(DIRECTION direction, uint16_t width, uint16_t height);

#endif
