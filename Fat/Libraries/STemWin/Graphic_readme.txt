

---------------------------------------------常用头文件------------------------------------------------------

#include "GUI.h"			// GUI基本头文件
#include "WM.h"				



---------------------------------------------GUI显示屏显示部分------------------------------------------------

1：画点画线函数/画笔大小设定

	U8 GUI_GetPenSize(void);										返回当前画笔大小 [单位：像素(pixel)]
	U8 GUI_SetPenSize(U8 PenSize);									设置当前画笔大小 [单位：像素(pixel)]
	void GUI_DrawPoint(int x, int y);								设定当前画笔坐标						
	void GUI_DrawLine(int x0, int y0, int x1, int y1);				划线 其实坐标(x0 , y0) 终点坐标(x1 , y1)
	void GUI_DrawLineRel(int dx, int dy);							从起点坐标到重点坐标(dx,dy)						
	void GUI_DrawLineTo(int x, int y);								从当前位置到指定的坐标(x,y)															

2：2D图形库

	void GUI_DrawGradientRoundedH(int x0, int y0, 
								  int x1, int y1, int rd		
								  GUI_COLOR Color0, GUI_COLOR Color1);		画圆角矩形
		矩形的起始坐标(x0,y0)到终点坐标(x1,y1),设定圆角的半径rd ,梯度颜色效果,左边颜色起始为Color0,右边延时终止为Color1
	
	void GUI_DrawRectEx(const GUI_RECT * pRect);					绘制矩形边框, 边框的起始坐标与重点坐标编写在结构体GUI_RECT中
	void GUI_DrawRoundedFrame(int x0, int y0, int x1, int y1, 
							  int r, int w);						绘制圆角矩形边框,矩形边框的起始坐标为(x0,y0)终点坐标为(x1,y1),圆角半径为r.边框的宽度为w
	void GUI_FillRoundedRect(int x0, int y0, int x1, int y1, int r);绘制圆角矩形,举行的起始坐标为(x0,y0),重点坐标为(x1,y1),圆角半径为r

3：Alpha混合 [主要是用于颜色方面的混合]
	
	unsigned GUI_EnableAlpha(unsigned OnOff);						启动Alpha混合, 1启动 0禁止
	unsigned GUI_SetAlpha(U8 Value);								Alpha值的设定,若Value参数为0表示不透明,255表示完全透明				
	void GUI_DrawBitmap(const GUI_BITMAP * pBM, int x, int y);		显示图片的内存地址,现在坐标为(x,y)
	U32 GUI_SetUserAlpha(GUI_ALPHA_STATE * pAlphaState, U32 UserAlpha);		设定Alpha值
	U32 GUI_RestoreUserAlpha(GUI_ALPHA_STATE * pAlphaState);		恢复Alpha值


4：2D图形库----绘制位图

	void GUI_DrawBitmap(const GUI_BITMAP * pBM, int x, int y);		在指定坐标(x,y)中显示图片pBM
	
5：2D图形库----绘制流位图[streamed bitmaps]

	1：流位图比起位图的好处是能够将图片数据存储在外部介质上,比如：SD、NAND Flash、NOR Flash 、 SPI Flash ; 系统有没有文件系统均可
	2：流位图是原始图片数据,不需要进行解码就可以直接显示在显示屏上,显示速度较快

6：游标[CURSOR]

	void GUI_CURSOR_Show(void);										显示游标[取默认配置]
	void GUI_CURSOR_Select(const GUI_CURSOR * pCursor);				设定游标的样式,参数参数类型参考数据手册
	void GUI_CURSOR_SetPosition(int x, int y);						设定游标的坐标为(x,y)
	
7：看锯齿[Antialiasing]

	抗锯齿是通过使背景色与前景色相混合的方法来平滑曲线。背景色与前景色之间使用的阴影越多,抗锯齿效果约好,计算时间越长
	抗锯齿因子的质量分为6等 分别为1~6 其中因子数越大,抗锯齿效果更好
	
	控制函数：
	void GUI_AA_EnableHiRes(void);									启用高分辨率坐标
	void GUI_AA_DisableHiRes(void);									不启用高分辨率坐标
	int GUI_AA_GetFactor(void);										获取当前抗锯齿因子
	void GUI_AA_SetFactor(int Factor);								设定抗锯齿因子

	绘图函数：
	void GUI_AA_DrawArc(int x0, int y0, int rx, int ry, int a0, int a1);		绘制抗锯齿弧形
	void GUI_AA_DrawLine(int x0, int y0, int x1, int y1);						绘制抗锯齿线段

	void GUI_AA_DrawPolyOutline(const GUI_POINT * pPoint,
									int NumPoints,
									int Thickness,
									int x,
									nt y);							绘制最多十个点的抗锯齿点组成的多边形
	void GUI_AA_DrawPolyOutlineEx(const GUI_POINT * pPoint,
									int NumPoints,
									int Thickness,
									int x,
									int y,
									GUI_POINT * pBuffer);			绘制抗锯齿多边形轮廓														
	void GUI_AA_DrawRoundedRectEx(GUI_RECT * pRect, int r);			绘制抗锯齿圆角举行轮廓
	void GUI_AA_FillCircle(int x0, int y0, int r);					绘制抗锯齿实心圆
	void GUI_AA_FillRoundedRect(int x0, int y0, int x1, int y1, int r);		绘制实心抗锯齿圆角矩形

8：多任务[Single Task / Multitask]

	使用RTOS时，emWin用户界面操作任务设定为最低优先级即可,仅比IDLE任务优先级高即可。若使用窗口管理的回调机制的话,需要在界面操作任务中定时调用更新函数(WM_Exec(),GUI_Exec(),GUI_Delay())


9：指针输入设备

	void GUI_PID_StoreState(const GUI_PID_STATE * pState);			
	



















