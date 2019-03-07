#ifndef _BSP_TOUCH_FT5206_H_
#define _BSP_TOUCH_FT5206_H_

#include "bsp_gpio.h"
#include "stdbool.h"
#include "bsp_touch.h"

#define FT5206_ADDR 0X70

// FT5206部分寄存器定义
#define FT_DEVIDE_MODE 				0x00   	// FT5206模式控制寄存器
#define FT_REG_NUM_FINGER     0x02		// 触摸状态寄存器

#define FT5206_TP1_XL_REG			0X03	  // 第一个触摸点数据地址
#define FT5206_TP2_XL_REG			0X09		// 第二个触摸点数据地址
#define FT5206_TP3_XL_REG			0X0F		// 第三个触摸点数据地址
#define FT5206_TP4_XL_REG			0X15		// 第四个触摸点数据地址
#define FT5206_TP5_XL_REG			0X1B		// 第五个触摸点数据地址
 

#define	FT_ID_G_LIB_VERSION		0xA1		// 版本
#define FT_ID_G_MODE 					0xA4   	// FT5206中断模式控制器
#define FT_ID_G_THGROUP			 	0x80   	// 触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE	0x88   	// 激活状态周期设置寄存器

#define FT5206_RST(x)   GPIO_Set(GPIOC, GPIO_Pin_13,  x) // 复位引脚
#define FT5206_INT(x)   GPIO_Set(GPIOB, GPIO_Pin_1,   x) // 中断引脚

// 检查触摸屏是否正确
bool FT5206_Check(void);

// 触摸屏初始化
void FT5206_Init(void);

// 设置触摸屏方向及宽高
void FT5206_SetTouchDirection(DIRECTION direction, uint16_t width, uint16_t height);

// 触摸屏扫描按下状态
void FT5206_Scan(void);

// 根据X，Y物理触摸点计算当前屏幕方向的触摸点
void FT5206_TransX(Point* point, uint16_t x, uint16_t y);

// 获取触摸点X轴坐标
int FT5206_MeasureX();

// 获取触摸点Y轴坐标
int FT5206_MeasureY();

#endif
