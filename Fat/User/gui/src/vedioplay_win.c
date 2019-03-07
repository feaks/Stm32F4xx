/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* External variables --------------------------------------------------------*/
extern const GUI_BITMAP * open_picture[]; 

/* Private function prototypes -----------------------------------------------*/
static void _cbWindow(WM_MESSAGE * pMsg);
static void Startup(WM_HWIN hWin,uint16_t xPos,uint16_t yPos);

/* Private typedef -----------------------------------------------------------*/
K_ModuleItem_Typedef  vedio_player_board =
{
  1,
  "Vedio Player",
  open_picture,
  0,
  Startup,
  NULL,
};
/* Private defines -----------------------------------------------------------*/
#define ID_PICTURE_WIN              	 (GUI_ID_USER + 0x00)

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Callback routine of desktop window.
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbWindow(WM_MESSAGE * pMsg) {
  uint32_t NCode, Id;
	
	switch(pMsg->MsgId)
	{	
			
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


/*
 *
 *
*/
static void Startup(WM_HWIN hWin,uint16_t xPos,uint16_t yPos)
{
	GUI_SelectLayer(0);
	

	
}