#include "bsp_touch.h"
#include "bsp_touch_GT9147.h"
#include "bsp_touch_FT5206.h"

TS_PARAM ts_param; // 触摸屏设备参数
TS 			 ts; 			 // 触摸屏控制函数

void TS_Init(DIRECTION direction, uint16_t width, uint16_t height)
{
	ts.Init 							= GT9147_Init;
	ts.Check 							= GT9147_Check;
	ts.Scan 							= GT9147_Scan;
	ts.SetTouchDirection 	= GT9147_SetTouchDirection;
	ts.MeasureX 					= GT9147_MeasureX;
	ts.MeasureY				 		= GT9147_MeasureY;

	// 触摸屏初始化
	ts.Init();
		
	// 检查触摸屏是否正确
	if(!ts.Check())	
		while(1);
	
	ts.SetTouchDirection(direction, width, height);
}