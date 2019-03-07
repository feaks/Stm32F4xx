
1.	头文件
#include "GUI.h"			// GUI基本头文件
#include "WM.h"		

2.	设置当前文本位置
	GUI_GotoXY                		设置当前 (X,Y) 位置。
	GUI_GotoX                 		设置当前 X 位置。
	GUI_GotoY                 		设置当前 (X,Y) 位置。
	
3.	返回当前文本位置
	GUI_GetDispPosX           		返回当前 X 位置。
	GUI_GetDispPosY           		返回当前 Y 位置。

3.	文本显示
	GUI_DispChar() 								在当前位置显示单个字符。
	GUI_DispCharAt() 							在指定位置显示单个字符。
	GUI_DispChars() 							按指定次数显示字符。
	GUI_DispNextLine							将光标移至下一行的开始。
	GUI_DispString() 							在当前位置显示字符串。
	GUI_DispStringAt() 							在指定位置显示字符串。
	GUI_DispStringAtCEOL() 						在指定位置显示字符串，并清除至行末。
	GUI_DispStringHCenterAt() 					在指定位置（水平中心点）水平居中显示字符串。
	GUI_DispStringInRect()						在指定的矩形区域中显示字符串，可设置对其方式。
		TextAlign : 可以通过 “OR”操作进行组合。垂直对齐和水平对齐标记应该组合使用。
					垂直对齐： GUI_TA_TOP、 GUI_TA_BOTTOM、 GUI_TA_VCENTER。
					水平对齐： GUI_TA_LEFT、 GUI_TA_RIGHT、 GUI_TA_HCENTER。
	GUI_DispStringInRectMax() 		在指定的矩形区域中显示指定字符数的字符串，可设置对其方式。
	GUI_DispStringInRectEx() 		在指定的矩形区域中显示指定字符数的字符串，可设置对其方式，并可旋转 （可选）。
		GUI_ROTATION : GUI_ROTATE_0 不旋转文本。从左到右显示。		GUI_ROTATE_180 180 度旋转文本。
					   GUI_ROTATE_CCW 逆时针旋转文本。				GUI_ROTATE_CW 顺时针旋转文本。
	GUI_DispStringInRectWrap() 		在指定的矩形区域中显示字符串，并可自动换行 。
		typedef enum { GUI_WRAPMODE_NONE, GUI_WRAPMODE_WORD, GUI_WRAPMODE_CHAR } GUI_WRAPMODE;
	GUI_DispStringLen() 			在当前位置显示指定字符数的字符串。
	GUI_WrapGetNumLines() 			为指定自动换行模式获取文本行数。
	
4. 选择文本绘制模式
	GUI_SetTextMode() 						设置文本绘制模式。
		Mode : 	GUI_TEXTMODE_NORMAL    			正常模式：底色+文字颜色
						GUI_TEXTMODE_REV		反转模式：底色与文字颜色互换
						GUI_TEXTMODE_TRANS		透明模式：底色透明+文字颜色
						GUI_TEXTMODE_XOR		异或模式：底色透明+文字新像素颜色（= 颜色的值 – 实际像素颜色 –1）
						参数 TextMode 的允许值 （可以通过 “OR”操作进行组合），均可缩写GUI_TM_XXX
	GUI_GetTextMode() 						返回当前文本模式。
	GUI_SetTextStyle() 						设置要使用的文本样式。
		Style : GUI_TS_NORMAL 					显示正常文本 （默认）。
				GUI_TS_UNDERLINE 				显示带下划线的文本。
				GUI_TS_STRIKETHRU 				显示带删除线的文本。
				GUI_TS_OVERLINE 				显示带顶线的文本。
		
5. 选择文本对齐模式
	GUI_SetTextAlign() 						设置文本对齐模式。
	GUI_GetTextAlign() 						返回当前文本对齐模式。
	GUI_SetLBorder() 						设置换行后的左边界。
	
	
6. 清除窗口或部分窗口的例程
	GUI_Clear() 							清除活动窗口 （如果背景是活动窗口，则清除整个显示）
	GUI_DispCEOL()							清除从当前文本位置到行末的显示内容。



void  GUI_GetTextExtend        (GUI_RECT* pRect, const char * s, int Len);
int   GUI_GetYAdjust           (void);
const GUI_FONT * GUI_GetFont(void);
int   GUI_GetCharDistX         (U16 c);
int   GUI_GetCharDistXEx       (U16 c, int * pSizeX);
int   GUI_GetStringDistX       (const char * s);
GUI_DRAWMODE GUI_GetDrawMode   (void);
int   GUI_GetFontDistY         (void);
int   GUI_GetFontSizeY         (void);
void  GUI_GetFontInfo          (const GUI_FONT * pFont, GUI_FONTINFO * pfi);
void  GUI_GetOrg               (int * px, int * py);
int   GUI_GetYSizeOfFont       (const GUI_FONT * pFont);
int   GUI_GetYDistOfFont       (const GUI_FONT * pFont);
int   GUI_GetTextAlign         (void);
int   GUI_GetTextMode          (void);
char  GUI_IsInFont             (const GUI_FONT * pFont, U16 c);
int   GUI_SetTextAlign         (int Align);
int   GUI_SetTextMode          (int Mode);
char  GUI_SetTextStyle         (char Style);
int   GUI_SetLBorder           (int x);
const GUI_FONT * GUI_SetFont(const GUI_FONT * pNewFont);
char  GUI_GotoXY               (int x, int y);
char  GUI_GotoX                (int x);
char  GUI_GotoY                (int y);
int   GUI_WrapGetNumLines      (const char * pText, int xSize, GUI_WRAPMODE WrapMode);
int   GUI_WrapGetPositions     (const char * pText, int xSize, GUI_WRAPMODE WrapMode, int * aPos, int NumItems);
void  GUI_WrapSetSeparators    (const U16 * pSep, int NumSeps);

int   GUI_GetLeadingBlankCols (U16 c);
int   GUI_GetTrailingBlankCols(U16 c);


---------------------------------------------GUI显示屏显示部分------------------------------------------------

1：显示设置
	
	void GUI_SetColor(GUI_COLOR Color);									设定前景色,Color参数在 GUI_USE_ARGB 中查询 [注意： 清屏之后才能够正常显示]
	GUI_COLOR GUI_SetBkColor(GUI_COLOR Color);							设定桌布背景颜色,Color参数在 GUI_USE_ARGB 中查询
	int GUI_SetBkColorIndex(int Index);									设定该行的背景颜色,Index参数变量在 GUI_USE_ARGB中查询
	void GUI_Clear(void);												清屏

	void GUI_FillRectEx(const GUI_RECT * pRect);						绘制矩形,填写矩形对角线坐标					

2: 显示文本



	int GUI_GetTextMode(void);											返回当前设置的绘图模式									
	int GUI_SetTextMode(int TextMode);									设置绘图模式
		TextMode：
			GUI_TEXTMODE_NORMAL	：	文本正常显示
			GUI_TEXTMODE_REV	：	文本反向显示
			GUI_TEXTMODE_TRANS	：	文本显示为透明
			GUI_TEXTMODE_XOR	：	使文本反转背景
	

	
3：显示数值	

	void GUI_DispDec(I32 v, U8 Len);									显示数值[参数1： 需要显示的数据 , 参数2： 需要显示数据的长度]		
	void GUI_DispDecAt(I32 v, I16P x, I16P y, U8 Len);					在指定坐标(x,y)上显示数值v ,显示的数值的位数为x
	void GUI_DispDecMin(I32 v);											用于显示十进制数值 [不需要指定数值的长度]
	void GUI_DispSDecShift(I32 v, U8 Len, U8 Shift);					显示十进制数据,支持带有小数点显示,小数点位置可以调整 [具有校验的功能, 若想显示12345.67的话 填充的变量为1234567,9,2;若书写错误则不会显示]
	void GUI_DispSDec(I32 v, U8 Len);									显示十进制数据,才有±符号
	void GUI_DispFloat(float v, char Len);								显示浮点型数据 [显示浮点型中,其小数符号占据一位]
	void GUI_DispFloatFix(float v, char Len, char Decs);				显示浮点型数据 [v的数据长度为len , 其中小数点位于第Decs位]
	void GUI_DispBin(U32 v, U8 Len);									显示二进制数据 [需要显示的数据v , 二进制数据的长度]
	void GUI_DispBinAt(U32 v, I16P x, I16P y, U8 Len);					显示二进制数据 [需要显示的数据v , 二进制数据的长度LEN , 显示数据的坐标为 (x,y)]				
	void GUI_DispHex(U32 v, U8 Len);									显示十六进制数据 [数据不带有0x作为开头]
	void GUI_DispBinAt(U32 v, I16P x, I16P y, U8 Len);					显示十六进制数据 [数据不带有0x作为开头,显示数据的坐标为 (x,y)]
	const char * GUI_GetVersionString(void);							获取emWin的版本号


			
			