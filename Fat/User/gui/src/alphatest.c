/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* External variables --------------------------------------------------------*/

extern GUI_CONST_STORAGE GUI_BITMAP bmplay;
extern void _ShowBMPEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ;
extern void _ShowJPEGEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY); 
extern void _DrawPNG_file(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY);
extern int _BMP_GIF_JPEG_GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off);
/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static const GUI_WIDGET_CREATE_INFO _aaDialog[] = {
  { WINDOW_CreateIndirect, "Window", 898, 0, 0, 800, 480, 0, 0x0, 0 },
};

WM_HWIN alphaPictureWin;
WM_HWIN hButton;
WM_HWIN hMovie;
static GUI_MOVIE_INFO   MovieInfo;
static const unsigned char _aclay1[311UL + 1];

static void _cbplaybackwin(WM_MESSAGE * pMsg) 
{
  WM_HWIN hItem;
  int Id, NCode;
		
	switch (pMsg->MsgId) 
  {
		case WM_CREATE:   
		 //		_ShowBMPEx("/pic/BMP_800x480/image01.jpg",0,0);
    break;
    
		case WM_DELETE:
			break;    
    case WM_PAINT:
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
			NCode = pMsg->Data.v;               /* Notification code */
			break;
    
		default:
			WM_DefaultProc(pMsg);
    break;
  }
}

/**
  * @brief  Paints play button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_Button(BUTTON_Handle hObj) {
	GUI_DrawBitmap(&bmplay, 0, 0);
}

static void _cbButton(WM_MESSAGE * pMsg) {
	
	
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_Button(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
static void _cbButton1(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
		GUI_DrawBitmap(&bmplay, 0, 0);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
	static int ibool = 0;	
static void _cbButton2(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
		GUI_DrawBitmap(&bmplay, 0, 0);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
/**
  * @brief  Callback routine of the video main dialog
  * @param  pMsg: pointer to a data structure of type WM_MESSAGE
  * @retval None
  */
static void _ccbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN  hItem;
  int Id, NCode;

  static uint8_t sel = 0;
  
  switch (pMsg->MsgId) {
  case WM_CREATE:

    break;     

 
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
    NCode = pMsg->Data.v;               /* Notification code */   
		switch (Id) {
			case 899:
				switch(NCode) {
					case WM_NOTIFICATION_RELEASED:
						WM_Paint(alphaPictureWin);
						if(	ibool)
						{
							ibool = 0;											
						}
						else
						{						
							ibool = 1;
						}	
					}
				break;
		}
		break;
	case WM_PAINT:
		if(ibool)
			_ShowBMPEx("/pic/BMP_800x480/image01.bmp",0,0);
		else
			_ShowBMPEx("/pic/BMP_800x480/image03.bmp",0,0);	
		break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }    
}
/**
  * @brief  Callback routine of the video main dialog
  * @param  pMsg: pointer to a data structure of type WM_MESSAGE
  * @retval None
  */
static void _ccbDialog2(WM_MESSAGE * pMsg) {
  WM_HWIN  hItem;
  int Id, NCode;

  
  switch (pMsg->MsgId) {
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
    NCode = pMsg->Data.v;               /* Notification code */   
    
    switch(Id) {
			case 899: 
				switch(NCode) {
					case WM_NOTIFICATION_RELEASED:
					{
						static int ibool = 0;				
						WM_SelectWindow(alphaPictureWin);
						if(	ibool)
						{
							WINDOW_SetBkColor(alphaPictureWin,GUI_LIGHTBLUE);	
							_ShowBMPEx("/pic/BMP_800x480/image01.bmp",0,0);
							ibool = 0;
//											
						}
						else
						{
							WINDOW_SetBkColor(alphaPictureWin,GUI_DARKRED);
							_ShowBMPEx("/pic/BMP_800x480/image03.bmp",0,0);							
							ibool = 1;
						}
					}
				break;
			}
    break;
		}
  default:
	  WINDOW_Callback(pMsg);
    break;
  }    
}

void _cbNotify(GUI_HMEM hMem, int Notification, U32 CurrentFrame) 
{
	static uint32_t t0, t1;
	
	(void)t1;
	
	switch (Notification) 
	{
		case GUI_MOVIE_NOTIFICATION_PREDRAW:
			t0 = GUI_GetTime();
			break;
		
		case GUI_MOVIE_NOTIFICATION_POSTDRAW:
			t1 = GUI_GetTime() - t0;
			printf("t = %d\r\n", t1);
			break;
		
		case GUI_MOVIE_NOTIFICATION_STOP:
			break;
		
		case GUI_MOVIE_NOTIFICATION_START:
			break;
		
		case GUI_MOVIE_NOTIFICATION_DELETE:
			break;
	}
}
/**
  * @brief  Picture window Starup
  * @retval None
  */

void alphatest(void)
{
	WM_HWIN hWin;
	WM_HWIN playbackwin;
	char *fn = "/Output.emf";
	WM_SetDesktopColor(GUI_WHITE);

	if (GUI_MOVIE_GetInfoEx(_BMP_GIF_JPEG_GetData, fn, &MovieInfo) == 0) 
	{
		printf("msPerFrame :%d NumFrames :%ld xSize: %d,ySize: %d",MovieInfo.msPerFrame,MovieInfo.NumFrames,MovieInfo.xSize,MovieInfo.ySize);
		hMovie = GUI_MOVIE_CreateEx(_BMP_GIF_JPEG_GetData,fn,_cbNotify);
		GUI_MOVIE_SetPeriod(hMovie, 100);
		GUI_MOVIE_Show(hMovie, 0, 0, 1);
		GUI_MOVIE_Play(hMovie);
	}
	
	
	
	
}


