/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private defines -----------------------------------------------------------*/
/* MAIN WINDOWS*/
#define PICTURE_FRAME_ID             	(GUI_ID_USER + 0x00)
/* TWO botton of main windows*/
#define ID_BUTTON_PLAY               			(GUI_ID_USER + 0x01)
#define ID_BUTTON_ADD_FILE               	(GUI_ID_USER + 0x02)


/* External variables --------------------------------------------------------*/
extern const GUI_BITMAP * open_picture[]; 

/* Private function prototypes -----------------------------------------------*/
static void Startup(WM_HWIN hWin,uint16_t xPos,uint16_t yPos);

/* Private typedef -----------------------------------------------------------*/
K_ModuleItem_Typedef  picture_player_board =
{
  1,
  "Picture Player",
  open_picture,
  0,
  Startup,
  NULL,
};


/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static const GUI_WIDGET_CREATE_INFO _aDialog[] = {
  { WINDOW_CreateIndirect, "Window", PICTURE_FRAME_ID, 0, 0, 800, 480, 0, 0x0, 0 },
};


static WM_HWIN pictureWin;


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  callback for Menu button
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbButton(WM_MESSAGE * pMsg) {
	uint32_t Id;
  switch (pMsg->MsgId) {
  case WM_PAINT:
		Id = WM_GetId(pMsg->hWin);
		switch(Id)
		{
			case ID_BUTTON_PLAY:
				/* draw bitmap */
			  GUI_SetBkColor(GUI_RED);
				GUI_Clear();
			
				GUI_DispStringAt(" open",30,30);
				break;
			case ID_BUTTON_ADD_FILE:
				/* draw bitmap */
				GUI_SetBkColor(GUI_RED);
				GUI_Clear();
			
				GUI_DispStringAt(" add file",30,30);
				break;
		}
    break;
  default:
    /* The original callback */
    BUTTON_Callback(pMsg);
    break;
  }
}
/**
  * @brief  Callback routine of dialog.
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbDialog(WM_MESSAGE * pMsg) {
  uint32_t NCode, Id;
	WM_HWIN  hItem;
	
	switch(pMsg->MsgId)
	{	
		case WM_INIT_DIALOG:
			hItem = BUTTON_CreateEx(200,180,80,80,pMsg->hWin,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_BUTTON_PLAY);
			WM_SetCallback(hItem,_cbButton);
			
			hItem = BUTTON_CreateEx(600,180,80,80,pMsg->hWin,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_BUTTON_ADD_FILE);
			WM_SetCallback(hItem,_cbButton);
		
			break;			
		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			
			switch(Id)
			{
				default:
					break;
			}

			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

static char buf[1024] = {0};
static int conter =0 ;
static void _WriteByte2File(U8 Data, void * p) {
	static int len =0;
	conter++;
	
	
	if(conter > 716800)
	{
		ff_fwrite(&Data, 1, 1, (FF_FILE *)p);
		len = 0;
	}
	else
	{
		buf[len++] = Data;
		if(len >= 1024)
		{
			ff_fwrite(buf, 1, 1024, (FF_FILE *)p);
			len = 0;
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
static void Startup(WM_HWIN hWin,uint16_t xPos,uint16_t yPos)
{
	FF_FILE  *hFile;

	ff_chdir("/");
	char name[64] = {0};
	GUI_COLOR forecolor,backcolor;
	forecolor = GUI_GetColor();
	backcolor = GUI_GetBkColor();
	sprintf(name,"%dos.bmp",xTaskGetTickCount());

	hFile = ff_fopen(name,"a");
	if(hFile != NULL)
	{
		conter = 0;
		printf("start make bmp %d",xTaskGetTickCount());
		GUI_BMP_SerializeEx(_WriteByte2File,0,0,800,480,hFile);
		printf("end make bmp %d",xTaskGetTickCount());
		ff_fclose(hFile);
		GUI_SetColor(forecolor); 
		GUI_SetBkColor(backcolor); 

	}
	
	
	//pictureWin = GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), _cbDialog, hWin, xPos, yPos);
}