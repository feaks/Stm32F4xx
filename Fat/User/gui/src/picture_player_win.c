/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "filelist.h"
#include "bsp_file.h"
/* External variables --------------------------------------------------------*/
extern GUI_CONST_STORAGE GUI_BITMAP bmwarning;
extern GUI_CONST_STORAGE GUI_BITMAP BMPpicturelist;
extern GUI_CONST_STORAGE GUI_BITMAP BMPpicturePlay;
extern GUI_CONST_STORAGE GUI_BITMAP bmplay;
extern GUI_CONST_STORAGE GUI_BITMAP bmplay_fore;
extern GUI_CONST_STORAGE GUI_BITMAP bmplay_back;
extern GUI_CONST_STORAGE GUI_BITMAP bmreturn;

extern int _PNG_BMPstream_GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off);
extern int _BMP_GIF_JPEG_GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off);

extern void _ShowBMPEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ;
extern void _ShowJPEGEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY); 
extern void _DrawPNG_file(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY);
extern void _ShowGIFEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime);
/* Private function prototypes -----------------------------------------------*/
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);
static void FILEMGR_GetFileOnly (char *file, char *path);
static void _StopPlay(FF_FILE * file) ;
static uint8_t _StartPlay(char * fn,int xPos,int yPos) ;
static void _cbMediaConnection(WM_MESSAGE * pMsg);
static void _AddEntireFolder(char *Foldername);
static void AddFileToViewList(WM_HWIN hItem,char *path);

static uint8_t toggle_hw = 0;
/* Private typedef -----------------------------------------------------------*/

typedef struct {
  int xSize;         /* X-size of images                                      */
  int ySize;         /* Y-size of images                                      */
  U32 Size;     		 /* Number of frames                                      */
} GUI_PICTURE_INFO;


/* Private defines -----------------------------------------------------------*/
K_ModuleItem_Typedef 			picturePlayer = 
{ 
	0,
	"Picture Player",
	0,
	0,
	Startup,
	0
}
;
/* Private macros ------------------------------------------------------------*/
/* MAIN WINDOWS*/
#define PICTURE_FRAME_ID             	(GUI_ID_USER + 0x00)
/* TWO iconview of main windows*/
#define ID_PICTURE_ICON_PLAY        	(GUI_ID_USER + 0x01)
#define ID_PICTURE_ICON_PLAYLIST      (GUI_ID_USER + 0x02)
#define ID_BUTTON_MENU            		(GUI_ID_USER + 0x03)


/* file list dialoag*/		
#define ID_FRAMEWIN_1             		(GUI_ID_USER + 0x10)		
#define ID_BUTTON_OKPL            		(GUI_ID_USER + 0x11)
#define ID_BUTTON_ADDPL           		(GUI_ID_USER + 0x12)
#define ID_PICTURE_LIST             	(GUI_ID_USER + 0x13)

/* ERR FILE INFO */
#define ID_FRAMEWIN_0             		(GUI_ID_USER + 0x17)
#define ID_TEXT_FILENAME          		(GUI_ID_USER + 0x18)
#define ID_BUTTON_OK_MESSAGE      		(GUI_ID_USER + 0x19)
		
/* add file dialoag */	
#define ID_FRAMEWIN_2             		(GUI_ID_USER + 0x21)		
#define ID_LISTVIEW_FILE            	(GUI_ID_USER + 0x22)
#define ID_DROPDOWN_DISK           		(GUI_ID_USER + 0x23)
#define ID_BUTTON_OK           				(GUI_ID_USER + 0x24)
#define ID_BUTTON_DEL           			(GUI_ID_USER + 0x25)
#define ID_BUTTON_UP           				(GUI_ID_USER + 0x26)

/* picture play windows */
#define ID_WARNING_MSG           			(GUI_ID_USER + 0x30)
#define ID_WARNING_ERROR           		(GUI_ID_USER + 0x31)

#define ID_BUTTON_BACK								(GUI_ID_USER + 0x40)
#define ID_BUTTON_STOP            		(GUI_ID_USER + 0x41)
#define ID_BUTTON_PLAY            		(GUI_ID_USER + 0x42)
#define ID_BUTTON_FORE            		(GUI_ID_USER + 0x43)
#define ID_BUTTON_MENU1								(GUI_ID_USER + 0x44)
#define ID_SLIDER_DURATION            (GUI_ID_USER + 0x45)
#define ID_BUTTON_EXIT_FROM_PICTURE   (GUI_ID_USER + 0x46)
#define ID_TIMER_PROCESS							(GUI_ID_USER + 0x47)
#define ID_IAMGE											(GUI_ID_USER + 0x48)
#define ID_IAMGE_WIN											(GUI_ID_USER + 0x48)

#define PICTURE_IDLE           			  0
#define PICTURE_PLAY           			  1
#define PICTURE_STOP           			  2
#define PICTURE_PAUSE          			  3

/* Private variables ---------------------------------------------------------*/

static const GUI_WIDGET_CREATE_INFO _aDialog[] = {
  { WINDOW_CreateIndirect, "Window", PICTURE_FRAME_ID, 0, 0, 800, 480, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _imageWin[] = {
  { WINDOW_CreateIndirect, "_imageWin", ID_IAMGE_WIN, 0, 0, 800, 480, 0, 0x0, 0 },
};
/* Info Dialog Create */
static const GUI_WIDGET_CREATE_INFO _aFileInfoDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Picture player", ID_FRAMEWIN_0, 0, 0, 600, 350, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Please Add Picture file to playList", ID_TEXT_FILENAME, 90, 140, 430, 30, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "OK", ID_BUTTON_OK_MESSAGE, 230, 240, 120, 120, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aPlaylistDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Playlist", ID_FRAMEWIN_1, 0, 0, 600, 350, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "OK", ID_BUTTON_OKPL, 520, 240, 120, 120, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Add", ID_BUTTON_ADDPL, (-60), 240, 120, 120, 0, 0x0, 0 },  
};

static const GUI_WIDGET_CREATE_INFO _aChooseFileDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Picture files", ID_FRAMEWIN_2, 0, 0, 600, 350, 0, 0x0, 0 },
	{ LISTVIEW_CreateIndirect, "file LISTWHEEL", ID_LISTVIEW_FILE, 0, 0, 590, 270, 0, 0x0, 0 },
	{ DROPDOWN_CreateIndirect,  "DISK LISTWHEEL", ID_DROPDOWN_DISK, 5, 275, 400, 30, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "UP", ID_BUTTON_UP, 410, 270, 50, 30, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "OK", ID_BUTTON_OK, 470, 270, 50, 30, 0, 0x0, 0 }, 
  { BUTTON_CreateIndirect, "Del", ID_BUTTON_DEL, 530, 270, 50, 30, 0, 0x0, 0 },
};

static WM_HWIN  playbackwin = 0;
static WM_HWIN  PictureWin = 0, hFrame = 0, hPlaylistWin = 0;
static WM_HWIN  hImage = 0;
uint8_t         PicturePlayer_State = PICTURE_IDLE;
FF_FILE                          Picture_File;

uint8_t                          SelLayer = 0;
uint8_t                          VisLayer = 1;

static  CHOOSEFILE_INFO           FileInfo;
static uint32_t                   Picture_FilePos = 0;
static uint32_t                   elapsed_time = 0;
GUI_PICTURE_INFO   Video_Info;

static FILELIST_FileTypeDef       PictureList;
static uint8_t               playlist_select = 0;

const char * pText[4];
static char absdir[64] = "/";

/**
  * @brief  Paints exit button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_exit(BUTTON_Handle hObj) {
  
  GUI_SetBkColor(GUI_WHITE);
  GUI_Clear();
  if((WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED))
  {
    GUI_SetColor(GUI_DARKGRAY);
    GUI_AA_FillCircle(100, 0, 100);
    GUI_SetBkColor(GUI_DARKGRAY);
    GUI_SetColor(GUI_WHITE);   
  }
  else
  {
    GUI_SetColor(GUI_LIGHTBLUE);
    GUI_AA_FillCircle(100, 0, 100);
    GUI_SetBkColor(GUI_LIGHTBLUE);
    GUI_SetColor(GUI_WHITE);    
  }
	GUI_SetFont(GUI_FONT_24_ASCII);
//  GUI_SetFont(&GUI_FontLubalGraph32);
  GUI_DispStringAt("Menu", 20, 20);
}

/**
  * @brief  Paints exit1 button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_exit1(BUTTON_Handle hObj) {

	GUI_DrawBitmap( &bmreturn ,0,0);
}

/**
  * @brief  Paints menu button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_menu(BUTTON_Handle hObj) {

  GUI_SetBkColor(GUI_WHITE);
  GUI_Clear();

  if((WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED))
  {
    GUI_SetColor(GUI_DARKGRAY);
    GUI_AA_FillCircle(100, 0, 100);
    GUI_SetBkColor(GUI_DARKGRAY);
    GUI_SetColor(GUI_WHITE);   
  }
  else
  {
    GUI_SetColor(GUI_LIGHTBLUE);
    GUI_AA_FillCircle(100, 0, 100);
    GUI_SetBkColor(GUI_LIGHTBLUE);
    GUI_SetColor(GUI_WHITE);    
  }
  GUI_SetFont(GUI_FONT_32_ASCII);
  GUI_DispStringAt("Menu", 20, 20);
}

/**
  * @brief  Paints menu button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_menu1(BUTTON_Handle hObj) {

  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_Clear();

  if((WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED))
  {
    GUI_SetColor(GUI_DARKGRAY);
    GUI_AA_FillCircle(100, 0, 100);
    GUI_SetBkColor(GUI_DARKGRAY);
    GUI_SetColor(GUI_WHITE);   
  }
  else
  {
    GUI_SetColor(GUI_LIGHTBLUE);
    GUI_AA_FillCircle(100, 0, 100);
    GUI_SetBkColor(GUI_LIGHTBLUE);
    GUI_SetColor(GUI_WHITE);    
  }
  
  GUI_SetFont(GUI_FONT_32_ASCII);
  GUI_DispStringAt("Menu", 20, 20);
}


/**
  * @brief  Paints add to playlist button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_add2PL(BUTTON_Handle hObj) {

  GUI_SetBkColor(GUI_WHITE);
  GUI_Clear();

  
  if((WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED))
  {
    GUI_SetColor(GUI_DARKGRAY);
    GUI_AA_FillCircle(56, 60, 60);
    GUI_SetBkColor(GUI_DARKGRAY);
    GUI_SetColor(GUI_WHITE);   
  }
  else
  {
    GUI_SetColor(GUI_LIGHTBLUE);
    GUI_AA_FillCircle(56, 60, 60);
    GUI_SetBkColor(GUI_LIGHTBLUE);
    GUI_SetColor(GUI_WHITE);    
  }  

  GUI_SetFont(&GUI_Font24B_1);
  GUI_DispStringAt("Add", 65, 30);
}

/**
  * @brief  Paints ok playlist button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_okPL(BUTTON_Handle hObj) {

  GUI_SetBkColor(GUI_WHITE);
  GUI_Clear();

  
  if((WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED))
  {
    GUI_SetColor(GUI_DARKGRAY);
    GUI_AA_FillCircle(68, 60, 60);
    GUI_SetBkColor(GUI_DARKGRAY);
    GUI_SetColor(GUI_WHITE);   
  }
  else
  {
    GUI_SetColor(GUI_LIGHTBLUE);
    GUI_AA_FillCircle(68, 60, 60);
    GUI_SetBkColor(GUI_LIGHTBLUE);
    GUI_SetColor(GUI_WHITE);    
  } 

  GUI_SetFont(&GUI_Font24B_1);
  GUI_DispStringAt("OK", 24, 30);
}
/**
  * @brief  callback for add to playlist button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_add2PL(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_add2PL(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
/**
  * @brief  callback for menu button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_menu1(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_menu1(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
/**
  * @brief  callback for Menu button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_menu(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_menu(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}

/**
  * @brief  callback for Exit button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_exit1(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_exit1(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
/**
  * @brief  callback for ok playlist button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_okPL(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_okPL(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
/**
  * @brief  Paints ok message button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_okMessage(BUTTON_Handle hObj) {

  GUI_SetBkColor(GUI_WHITE);
  GUI_Clear();

  if((WIDGET_GetState(hObj) & BUTTON_STATE_PRESSED))
  {
    GUI_SetColor(GUI_DARKGRAY);
    GUI_AA_FillCircle(60, 60, 50);
    GUI_SetBkColor(GUI_DARKGRAY);
    GUI_SetColor(GUI_WHITE);   
  }
  else
  {
    GUI_SetColor(GUI_LIGHTBLUE);
    GUI_AA_FillCircle(60, 60, 50);
    GUI_SetBkColor(GUI_LIGHTBLUE);
    GUI_SetColor(GUI_WHITE);    
  } 
  
  GUI_SetFont(GUI_FONT_24_ASCII);
  GUI_DispStringAt("OK", 43, 35);
}

/**
  * @brief  callback for ok message button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_okMessage(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_okMessage(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
/**
  * @brief  Paints play button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_play(BUTTON_Handle hObj) {

    GUI_DrawBitmap(&bmplay, 0, 0);
}


/**
  * @brief  Paints back button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_back(BUTTON_Handle hObj) {
  GUI_DrawBitmap(&bmplay_fore, 0, 0);
}
/**
  * @brief  Paints foreward button
  * @param  hObj: button handle
  * @retval None
  */
static void _OnPaint_fore(BUTTON_Handle hObj) {
  
  GUI_DrawBitmap(&bmplay_back, 0, 0);
}

/**
  * @brief  callback for start button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_play(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_play(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
/**
  * @brief  callback for back button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_back(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_back(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
/**
  * @brief  callback for foreward button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton_fore(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _OnPaint_fore(pMsg->hWin);
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}

/**/
void refreshPictureListWin(void )
{
	WM_HWIN hItem;
	static char tmp[64];
	
	hItem = WM_GetDialogItem(hPlaylistWin,ID_PICTURE_LIST);
	
	if(PictureList.ptr > 0)
	{
		for(int i=0; i<PictureList.ptr; i++)
		{
			strcpy(FileInfo.pRoot, (char *)PictureList.file[i].name);
			FILEMGR_GetFileOnly ((char *)tmp, (char *)FileInfo.pRoot);       
			LISTVIEW_AddRow(hItem, NULL);         
			LISTVIEW_SetItemText(hItem, 0, i, (char *)tmp);
		}
		sprintf(tmp, "PlayList [%d Item(s)]", PictureList.ptr);
		hItem = WM_GetDialogItem(hPlaylistWin,ID_FRAMEWIN_1);
		FRAMEWIN_SetText(hItem, tmp);
	}
}
/**
  * @brief  Callback routine of Info dialog
  * @param  pMsg: pointer to data structure of type WM_MESSAGE 
  * @retval None
  */
static void _cbFileInfoDialog(WM_MESSAGE * pMsg) {
  int     NCode;
  int     Id;
  WM_HWIN hItem;
  
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OK_MESSAGE);
    WM_SetCallback(hItem, _cbButton_okMessage);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_FILENAME);
    TEXT_SetFont(hItem,GUI_FONT_32_ASCII);
    
    hItem = WM_GetDialogItem(pMsg->hWin, (ID_TEXT_FILENAME+2));
    TEXT_SetFont(hItem, GUI_FONT_32_ASCII);    
    break;
        
  case WM_PAINT:    
    GUI_DrawBitmap(&bmwarning, 260, 60);
    break;
    
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_OK_MESSAGE: /* Notifications sent by 'OK' */
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
        GUI_EndDialog(pMsg->hWin, 0); 

        break;
      }
      break;
    }
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/**
  * @brief  Callback routine of choose file dialog
  * @param  pMsg: pointer to data structure of type WM_MESSAGE 
  * @retval None
  */
static void _cbChooseFileDialog(WM_MESSAGE * pMsg) {
  int     NCode;
  int     Id;
  WM_HWIN hItem;
  
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    
//    hItem = WM_GetDialogItem(pMsg->hWin, ID_FRAMEWIN_2);
    FRAMEWIN_SetText(pMsg->hWin," Picture file");
	  FRAMEWIN_SetFont(pMsg->hWin,GUI_FONT_32_ASCII);
   
		hItem = WM_GetDialogItem(pMsg->hWin, (ID_LISTVIEW_FILE));
		/* set listview header font */
		HEADER_Handle hHandle = LISTVIEW_GetHeader(hItem);
		HEADER_SetFont(hHandle,GUI_FONT_24_ASCII);
	
    LISTVIEW_SetFont(hItem, GUI_FONT_24_ASCII);
    LISTVIEW_SetTextColor(hItem, LISTVIEW_CI_UNSEL, GUI_LIGHTBLUE);
    LISTVIEW_SetTextColor(hItem, LISTVIEW_CI_SEL, GUI_WHITE);
//    LISTVIEW_SetHeaderHeight(hItem, 28);
		LISTVIEW_SetAutoScrollV(hItem,1);
	
    LISTVIEW_AddColumn(hItem, 300, "Name", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 100, "Size", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 100, "Type", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 100, "Attrib", GUI_TA_VCENTER | GUI_TA_LEFT);
		
//		LISTVIEW_AddRow(hItem,NULL);

		/* clear adsdir */
		memset(absdir,0,64);
		memcpy(absdir,"/",2);
		AddFileToViewList(hItem,absdir);
		
		hItem = WM_GetDialogItem(pMsg->hWin, (ID_DROPDOWN_DISK));
		DROPDOWN_AddString(hItem,"0:/");
	  DROPDOWN_SetFont(hItem,GUI_FONT_24_ASCII);
		
		
		hItem = WM_GetDialogItem(pMsg->hWin, (ID_BUTTON_OK));
		BUTTON_SetText(hItem,"ok");
		hItem = WM_GetDialogItem(pMsg->hWin, (ID_BUTTON_DEL));
		BUTTON_SetText(hItem,"del");
		hItem = WM_GetDialogItem(pMsg->hWin, (ID_BUTTON_UP));
		BUTTON_SetText(hItem,"up");
    break;
        
  case WM_PAINT:    
    GUI_DrawBitmap(&bmwarning, 260, 60);
    break;
    
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
		case ID_BUTTON_OK: /* Notifications sent by 'OK' */
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
				 hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_FILE);
				/* add picture to list */
				/*  1-> get point text */
				int rows = 0;
				char name[64] = {0};
				char attrib[20] = {0};
				rows = LISTVIEW_GetSel(hItem);
				LISTVIEW_GetItemText(hItem,0,rows,name,64);
				LISTVIEW_GetItemText(hItem,3,rows,attrib,20);
				
				/* 2-> is pic ?*/
				if(strstr(attrib, "d"))
				{
					if(name[0] == '.')
					{
						if(name[1] == '.')
						{
							/* cd .. */
							char *temp = strrchr(absdir,'/');
							while(*temp)
								*temp++ = 0;
							
							AddFileToViewList(hItem,absdir);
						}
						else
						{
							/* cd .*/
							
						}
					}
					else
					{
						if( (strcmp(absdir,"/")) != 0)
							strcat(absdir,"/");
						strcat(absdir,name);				
					}
					AddFileToViewList(hItem,absdir);				
				}
				else
				{
					if((strstr(name, ".jpg")) || (strstr(name, ".bmp")) || (strstr(name, ".gif")) || (strstr(name, ".png")) || (strstr(name, ".BMP")) )
          {
						char temp[64] = {0};
						memcpy(temp,absdir,64);
						strcat(temp,"/");
						strcat(temp,name);
						memcpy((char *)PictureList.file[PictureList.ptr++].name,temp,64 );
					}
				}		
        break;
      }
      break;
		case ID_BUTTON_DEL: /* Notifications sent by 'OK' */
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
				/* refresh picture list*/
				refreshPictureListWin();
        WM_DeleteWindow(pMsg->hWin); 
        break;
      }
      break;
		case ID_BUTTON_UP: /* Notifications sent by 'OK' */
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
				refreshPictureListWin();
        WM_DeleteWindow(pMsg->hWin); 
        break;
      }
     
    }
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}


/**
  * @brief  Start play
  * @param  filename: pointer to the video file name
  * @retval None
  */

static uint8_t _StartPlay(char * fn,int xPos,int yPos)
{ 

	if( strstr(fn, ".png") )
	{
		IMAGE_SetPNGEx(hImage,_PNG_BMPstream_GetData,fn);
	}	
	else if ( strstr(fn, ".gif"))
	{
		IMAGE_SetGIFEx(hImage,_BMP_GIF_JPEG_GetData,fn);
	}
	else if( strstr(fn, ".bmp") )
	{
		IMAGE_SetBMPEx(hImage,_BMP_GIF_JPEG_GetData,fn);
	}
	else if( strstr(fn, ".jpg") ||  strstr(fn, ".jpeg"))
	{
		IMAGE_SetJPEGEx(hImage,_BMP_GIF_JPEG_GetData,fn);
	}
}
/**
  * @brief  Callback routine of playlist Info dialog
  * @param  pMsg: pointer to data structure of type WM_MESSAGE 
  * @retval None
  */
static void _cbPlaylistDialog(WM_MESSAGE * pMsg) {
  int     NCode;
  int     Id;
  int     r;
  WM_HWIN hItem;

  static char tmp[64];
  uint32_t i = 0;
  
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_OKPL);
    WM_SetCallback(hItem, _cbButton_okPL);
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ADDPL);
    WM_SetCallback(hItem, _cbButton_add2PL);   
    
    
    hItem = LISTVIEW_CreateEx(85, 50, 440, 260, pMsg->hWin, WM_CF_SHOW, 0, ID_PICTURE_LIST);

    LISTVIEW_SetFont(hItem, GUI_FONT_24_ASCII);
    LISTVIEW_SetTextColor(hItem, LISTVIEW_CI_UNSEL, GUI_LIGHTBLUE);
    LISTVIEW_SetTextColor(hItem, LISTVIEW_CI_SEL, GUI_WHITE);
    LISTVIEW_SetHeaderHeight(hItem, 28);
	
		HEADER_Handle hHandle = LISTVIEW_GetHeader(hItem);
		HEADER_SetFont(hHandle,GUI_FONT_24_ASCII);

    LISTVIEW_AddColumn(hItem, 300, "File Name", GUI_TA_VCENTER | GUI_TA_LEFT);
		LISTVIEW_AddColumn(hItem, 120, "File Type", GUI_TA_VCENTER | GUI_TA_LEFT);
		

    LISTVIEW_SetGridVis(hItem, 0);
    LISTVIEW_SetAutoScrollV(hItem, 1);  
    WIDGET_SetEffect(hItem, &WIDGET_Effect_None);
    LISTVIEW_SetTextAlign(hItem, 0, GUI_TA_HCENTER); 
    
    if(PictureList.ptr > 0)
    {
      for(i=0; i<PictureList.ptr; i++)
      {
        strcpy(FileInfo.pRoot, (char *)PictureList.file[i].name);
        FILEMGR_GetFileOnly ((char *)tmp, (char *)FileInfo.pRoot);       
        LISTVIEW_AddRow(hItem, NULL);         
        LISTVIEW_SetItemText(hItem, 0, i, (char *)tmp);
      }
      sprintf(tmp, "PlayList [%d Item(s)]", PictureList.ptr);
      FRAMEWIN_SetText(pMsg->hWin, tmp);
    }
    else
    {
      FRAMEWIN_SetText(pMsg->hWin, "PlayList [no Item]");
    }
    break;
    
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;   
    switch(Id) {
    case ID_BUTTON_OKPL: /* Notifications sent by 'OK' */
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
        GUI_EndDialog(pMsg->hWin, 0); 
        hPlaylistWin = 0;
        break;
      }
      break;
      
    case ID_BUTTON_ADDPL: 
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
        FileInfo.pfGetData = k_GetData;
        FileInfo.pMask = 0;     

				hItem = GUI_CreateDialogBox(_aChooseFileDialogCreate, 
																		GUI_COUNTOF(_aChooseFileDialogCreate), 
																		_cbChooseFileDialog, 
																		pMsg->hWin, 
																		0, 0);

				WM_MakeModal(hItem);

        break;
      }
      break;        
    }
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}
/**
  * @brief  callback for video window
  * @param  pMsg: pointer to a data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbimagewin(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int Id, NCode;
	
	switch (pMsg->MsgId) 
  {
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
			NCode = pMsg->Data.v;               /* Notification code */
			switch(Id) {
				case ID_BUTTON_EXIT_FROM_PICTURE: 
					switch(NCode) {
					case WM_NOTIFICATION_RELEASED:								 
						GUI_EndDialog(playbackwin, 0);       
						WM_DeleteWindow(hFrame);
						WM_ShowWindow(PictureWin);
						WM_InvalidateWindow(PictureWin);
						hFrame = 0;
						break;
					}
					break;

				 
				case ID_BUTTON_FORE: 
					switch(NCode) {
					case WM_NOTIFICATION_RELEASED:
						Picture_FilePos++;
						if(Picture_FilePos>= PictureList.ptr)
							Picture_FilePos = 0;
						_StartPlay((char *)PictureList.file[Picture_FilePos].name,0,0);
//						_StartPlay("/pic/BMP_800x480/image01.bmp",0,0);
						break;
					}
					break;
	 
				
				case ID_BUTTON_BACK: 
					switch(NCode) {
					case WM_NOTIFICATION_RELEASED:
						
						if(Picture_FilePos == 0)
							Picture_FilePos = PictureList.ptr -1;
						else
							Picture_FilePos --;
						
						_StartPlay((char *)PictureList.file[Picture_FilePos].name,0,0);
//						_StartPlay("/pic/BMP_800x480/image03.bmp",0,0);
						break;
					}
					break;
				
				case ID_BUTTON_PLAY: 
					switch(NCode) {
					case WM_NOTIFICATION_RELEASED:
					{
						//_StartPlay((char *)PictureList.file[Picture_FilePos].name,0,0);
					}
				}
					break;
			}
			break;
			
		default:
			IMAGE_Callback(pMsg);
			break;
				
	}
}

/**
  * @brief  callback for video window
  * @param  pMsg: pointer to a data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbplaybackwin(WM_MESSAGE * pMsg) 
{
  WM_HWIN hItem;
  int Id, NCode;
  char tmp[64];
  int r = 0;		
	switch (pMsg->MsgId) 
  {
		case WM_INIT_DIALOG:  	 

			
    break;
    
		case WM_DELETE:
			playbackwin = 0;
			break;    
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
			NCode = pMsg->Data.v;               /* Notification code */ 
		case WM_PAINT:
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
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
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN  hItem;
  int Id, NCode;

  static uint8_t sel = 0;
  
  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
	/* menu button */	
//	hItem = BUTTON_CreateEx(700, 0, 100, 100, pMsg->hWin, WM_CF_SHOW, 0, ID_BUTTON_MENU);
//	WM_SetCallback(hItem, _cbButton_menu); 	
	
	/* play iconview */
	hItem = ICONVIEW_CreateEx(262, 136, 128, 200, pMsg->hWin, WM_CF_SHOW, 0,ID_PICTURE_ICON_PLAY,128,128);
	ICONVIEW_AddBitmapItem(hItem,&BMPpicturePlay,"picture play");
	/* playlist  iconview*/
	hItem = ICONVIEW_CreateEx(410, 136, 128, 200, pMsg->hWin, WM_CF_SHOW, 0,ID_PICTURE_ICON_PLAYLIST,128,128);
	ICONVIEW_AddBitmapItem(hItem,&BMPpicturePlay,"open playlist");

    break;     

  case WM_PAINT: 
    GUI_SetColor(GUI_BLACK);
    GUI_DrawLine(799, 0, 799, 480);   

    break;
    
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
    NCode = pMsg->Data.v;               /* Notification code */   
    
    switch(Id) {
    case ID_BUTTON_MENU: 
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
				/* nothing to do */
        break;
        
      case WM_NOTIFICATION_MOVED_OUT: 
        break;
        
      case WM_NOTIFICATION_RELEASED:
        break;
      }
      break; 
     case ID_PICTURE_ICON_PLAYLIST: 
			switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
          /* create play list  dialog */
					hPlaylistWin = GUI_CreateDialogBox(_aPlaylistDialogCreate, 
                                GUI_COUNTOF(_aPlaylistDialogCreate), 
                                _cbPlaylistDialog, 
                                pMsg->hWin, 
                                100, 50);
					WM_MakeModal(hPlaylistWin);		  
        break;
      }
      break;  
      
    case ID_PICTURE_ICON_PLAY: 
			switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
          /* create picture play  dialog */
					if( PictureList.ptr>0)
					{
						/* create picture windows*/ 
					
						WM_HideWindow(PictureWin);
						WM_Exec();

						playbackwin = WINDOW_CreateEx(0,0,800,480,WM_HBKWIN,WM_CF_SHOW ,0,ID_IAMGE_WIN,0);
					
						hImage = IMAGE_CreateEx(0,0,800,480,playbackwin,WM_CF_SHOW  ,0 ,ID_IAMGE);	
						_StartPlay((char *)PictureList.file[Picture_FilePos].name,0,0);


						hItem = BUTTON_CreateEx(320, 430, 43, 32, hImage, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON_BACK);
						WM_SetCallback(hItem, _cbButton_back);
						
						hItem = BUTTON_CreateEx(385, 430, 32, 32, hImage, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON_PLAY);
						WM_SetCallback(hItem, _cbButton_play);
								
						hItem = BUTTON_CreateEx(450, 430, 43, 32, hImage, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON_FORE);
						WM_SetCallback(hItem, _cbButton_fore);
								
						hItem = BUTTON_CreateEx(0, 440, 32, 32, hImage, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON_EXIT_FROM_PICTURE);
						WM_SetCallback(hItem, _cbButton_exit1);  
					
						WM_SetCallback(hImage, _cbimagewin);
						
						PicturePlayer_State = PICTURE_PLAY;  
					}
					else
					{
						hItem = GUI_CreateDialogBox(_aFileInfoDialogCreate, 
																				GUI_COUNTOF(_aFileInfoDialogCreate), 
																				_cbFileInfoDialog, 
																				pMsg->hWin, 
																				100, 50);
						WM_MakeModal(hItem); 
					}
			  
        break;
        
      case WM_NOTIFICATION_MOVED_OUT: 
        break;
        
      case WM_NOTIFICATION_RELEASED:
        break;
      }
      break;
    }
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }    
}


static void AddFileToViewList(WM_HWIN hItem,char *path)
{
	FF_FindData_t *pxFindStruct;
	 char *fn;
const char  *pcAttrib,
            *pcWritableFile = 	"rw",
            *pcReadOnlyFile = 	"r",
            *pcDirectory 		= 	"drw";

    /* FF_FindData_t can be large, so it is best to allocate the structure
    dynamically, rather than declare it as a stack variable. */
    pxFindStruct = ( FF_FindData_t * ) pvPortMalloc( sizeof( FF_FindData_t ) );

    /* FF_FindData_t must be cleared to 0. */
    memset( pxFindStruct, 0x00, sizeof( FF_FindData_t ) );
		
		 
		while(LISTVIEW_GetNumRows(hItem) !=0)
		{
			LISTVIEW_DeleteRow(hItem,0);
		}
		
    /* The first parameter to ff_findfist() is the directory being searched.  Do
    not add wildcards to the end of the directory name. */
    if( ff_findfirst( path, pxFindStruct ) == 0 )
    {
        do
        {
            /* Point pcAttrib to a string that describes the file. */
            if( ( pxFindStruct->ucAttributes & FF_FAT_ATTR_DIR ) != 0 )
            {
                pcAttrib = pcDirectory;
            }
            else if( pxFindStruct->ucAttributes & FF_FAT_ATTR_READONLY )
            {
                pcAttrib = pcReadOnlyFile;
            }                        
            else
            {
                pcAttrib = pcWritableFile;
            }
            /* Print the files name, size, and attribute string. */

						char   acText[4][64] = {{ 0 }};
						sprintf(acText[0],"%s",pxFindStruct->pcFileName);
						
						sprintf(acText[1],"%d",pxFindStruct->ulFileSize);
						sprintf(acText[3],"%s",pcAttrib);
						for (int i = 0; i < 4; i++) {
							pText[i] = acText[i];
						}

						LISTVIEW_AddRow(hItem,pText);

        } while( ff_findnext( pxFindStruct ) == 0 );
    }

    /* Free the allocated FF_FindData_t structure. */
    vPortFree( pxFindStruct );
}

/**
  * @brief  Retrieve the file name from a full file path
  * @param  file: pointer to base path
  * @param  path: pointer to full path
  * @retval None
*/
static void FILEMGR_GetFileOnly (char *file, char *path)
{
  char *baseName1, *baseName2;
  baseName1 = strrchr(path,'/');
  baseName2 = strrchr(path,':');
  
  if(baseName1++) 
  { 
    strcpy(file, baseName1);
  }
  else 
  {
    if (baseName2++) 
    {
      strcpy(file, baseName2);
    }
    else
    {
      strcpy(file, path);
    }
  }
}
/**
  * @brief  Add entire folder to play list.
  * @param  Foldername: pointer to folder name.
  * @retval None
  */
static void _AddEntireFolder(char *Foldername)
{
	FF_Error_t res = FF_ERR_NONE;
	FF_FindData_t fno = {0};
  char *fn;
  static char tmp[FILEMGR_FILE_NAME_SIZE];
	
	res = ff_chdir(Foldername);
 
	if (res == FF_ERR_NONE)
  {
		while(1)
		{		
			if (PictureList.ptr < FILEMGR_LIST_DEPDTH)
      {
        if ((fno.ucAttributes & FF_FAT_ATTR_DIR) == 0)
        {
          if((strstr(fn, ".jpg")) || (strstr(fn, ".bmp")) || (strstr(fn, ".gif")) || (strstr(fn, ".png")) || (strstr(fn, ".BMP")) )
          {
            strcpy(tmp, Foldername);
            strcat(tmp, "/");
            strcat(tmp, fn);
            
            strncpy((char *)PictureList.file[PictureList.ptr].name, (char *)tmp, FILEMGR_FILE_NAME_SIZE);
            PictureList.ptr++;
          }
        }
      }
		}
	}
}
/**
  * @brief  Picture window Starup
  * @param  hWin: pointer to the parent handle.
  * @param  xpos: X position 
  * @param  ypos: Y position
  * @retval None
  */
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos)
{
  PicturePlayer_State = PICTURE_IDLE;  
 
	PictureWin = GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), _cbDialog, hWin, xpos, ypos);

}



