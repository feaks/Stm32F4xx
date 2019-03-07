/**
  ******************************************************************************
  * @file    k_bsp.c
  * @author  MCD Application Team
  * @brief   This file provides the kernel bsp functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "k_module.h"
/* External variables --------------------------------------------------------*/
extern GUI_CONST_STORAGE GUI_BITMAP bmbkpicture;
extern int _BMP_GIF_JPEG_GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off); 
/* Private typedef -----------------------------------------------------------*/
#define BK_PICTUREN			"/natur.jpg"
/* Private defines -----------------------------------------------------------*/
#define ID_ICONVIEW_MENU               (GUI_ID_USER + 0x00)
#define ID_FEATURES_FPU                (GUI_ID_USER + 0x44)
#define ID_FEATURES_IC                 (GUI_ID_USER + 0x45)
#define ID_FEATURES_DC                 (GUI_ID_USER + 0x46)
#define ID_FEATURES_CPU                (GUI_ID_USER + 0x47)
#define ID_FEATURES_FREQ               (GUI_ID_USER + 0x48)
#define ID_STM32_F7                    (GUI_ID_USER + 0x49)
#define ID_TARA_NAME                   (GUI_ID_USER + 0x50)

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ICONVIEW_Handle hIcon ;
WM_HWIN         hCPULoad;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Callback routine of desktop window.
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbBk(WM_MESSAGE * pMsg) {
  uint32_t NCode, Id;
	static uint8_t sel;
	switch(pMsg->MsgId)
	{
		case WM_PAINT:
					GUI_SetBkColor(GUI_WHITE);
					GUI_Clear();
					
					/*H/W Information */
					GUI_EnableAlpha(1);
					GUI_SetColor(GUI_BLUE );
					GUI_FillRoundedRect(650,50,900,430,20);
					GUI_SetTextMode(GUI_TEXTMODE_TRANS);
					GUI_SetFont(&GUI_Font20B_ASCII);
					GUI_SetColor( (GUI_YELLOW & 0x00FFFFFF) | (0x40 <<24));
					GUI_DispStringAt(" STM32F407 ",680,80);
		      GUI_DispStringAt(" GUI DEMO ",685,100);
		
					GUI_EnableAlpha(0);

			break;
		case WM_NOTIFY_PARENT:
			Id = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			
			switch(Id)
			{
				case ID_ICONVIEW_MENU:
					switch(NCode)
					{
						case WM_NOTIFICATION_CLICKED:
							sel = ICONVIEW_GetSel(pMsg->hWinSrc);
						
							if(sel < k_ModuleGetNumber())
							{
								ICONVIEW_SetSel(pMsg->hWinSrc,-1);
								if(module_prop[sel].in_use == 0)
								{
									module_prop[sel].in_use = 1;
									module_prop[sel].module->startup(pMsg->hWin, 0, 0);
								}
								else if(module_prop[sel].win_state == 1)
								{
									module_prop[sel].module->startup(pMsg->hWin, 0, 0);
								}
							}
							else
							{
								WM_InvalidateWindow(pMsg->hWinSrc);
							}
							break;
							
						case WM_NOTIFICATION_CHILD_DELETED:
							module_prop[sel].in_use = 0;
							module_prop[sel].win_state = 0;
							break;
						
						default:
							break;
					}
					break;
					
				default:
					break;
			}

			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void 	k_SetGuiProfile(void)
{

}

void 	k_InitMenu(void)
{
	GUI_SelectLayer(0);
	WM_SetDesktopColorEx(GUI_WHITE,0);
	
	WM_SetCallback(WM_GetDesktopWindowEx(0),_cbBk);
	
	hIcon = ICONVIEW_CreateEx(0,0,LCD_GetXSize() - 155,LCD_GetYSize() - 50,WM_GetDesktopWindowEx(0),WM_CF_SHOW | WM_CF_HASTRANS |WM_CF_LATE_CLIP ,0,ID_ICONVIEW_MENU,150,180);
	ICONVIEW_SetFont(hIcon,&GUI_Font20B_ASCII);
	
	ICONVIEW_SetSpace(hIcon,GUI_COORD_Y,5);
	ICONVIEW_SetSpace(hIcon,GUI_COORD_X,5);
	ICONVIEW_SetFrame(hIcon, GUI_COORD_Y, 30);
  ICONVIEW_SetFrame(hIcon, GUI_COORD_X, 10);
	
	ICONVIEW_SetTextColor(hIcon, ICONVIEW_CI_UNSEL, 0x00DCA939);
	ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_UNSEL, GUI_WHITE);
	ICONVIEW_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_WHITE);
	
	ICONVIEW_SetSel(hIcon,-1);
	for(int i=0;i<k_ModuleGetNumber();i++)
	{
		ICONVIEW_AddBitmapItem(hIcon,module_prop[i].module->open_icon[0],module_prop[i].module->name);
	}
	
	WM_Exec();
}
void 	k_periodicProcesses(void)
{

}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
