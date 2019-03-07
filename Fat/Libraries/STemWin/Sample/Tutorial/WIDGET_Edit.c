/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2014  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.26 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : WIDGET_Edit.c
Purpose     : Example demonstrating the use of a EDIT widget
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/
#include "GUI.h"
#include "EDIT.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define WM_APP_SHOW_TEXT (WM_USER + 0)
#define TEXT_MAXLEN      40

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbBk
*/
static void _cbBk(WM_MESSAGE * pMsg) {
  static WM_HWIN hEdit;
  static U8      ShowText;
  char           aBuffer[TEXT_MAXLEN];

  hEdit    = 0;
  ShowText = 0;
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(GUI_DARKGRAY);
    GUI_Clear();
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_DispStringHCenterAt("WIDGET_Edit - Sample", 160, 5);
    GUI_SetFont(&GUI_Font8x16);
    if (ShowText) {
      GUI_DispStringHCenterAt("The string you have modified is:", 160, 90);
      EDIT_GetText(hEdit, aBuffer, TEXT_MAXLEN);
      GUI_DispStringHCenterAt(aBuffer, 160, 110);
    } else {
      GUI_DispStringHCenterAt("Use keyboard to modify string...", 160, 90);
    }
    break;
  case WM_APP_SHOW_TEXT:
    if (hEdit == 0) {
      hEdit = pMsg->hWinSrc;
    }
    WM_HideWindow(hEdit);
    ShowText = 1;
    WM_InvalidateWindow(WM_HBKWIN);
    WM_CreateTimer(WM_HBKWIN, 0, 3000, 0);
    break;
  case WM_TIMER:
    ShowText = 0;
    WM_InvalidateWindow(WM_HBKWIN);
    WM_ShowWindow(hEdit);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _cbEdit
*/
static void _cbEdit(WM_MESSAGE * pMsg) {
  const WM_KEY_INFO * pInfo;
  WM_MESSAGE          Msg;

  switch (pMsg->MsgId) {
  case WM_KEY:
    pInfo = (WM_KEY_INFO *)pMsg->Data.p;
    if (pInfo->Key == GUI_KEY_ENTER) {
      if (pInfo->PressedCnt == 0) {
        Msg.MsgId   = WM_APP_SHOW_TEXT;
        Msg.hWinSrc = pMsg->hWin;
        WM_SendMessage(WM_HBKWIN, &Msg);
        return;
      }
    }
  }
  EDIT_Callback(pMsg);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  EDIT_Handle hEdit;

  GUI_Init();
  WM_SetCallback(WM_HBKWIN, _cbBk);
  hEdit = EDIT_CreateEx(50, 110, 220, 25, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_EDIT0, TEXT_MAXLEN);
  WM_SetCallback(hEdit, _cbEdit);
  EDIT_SetText(hEdit, "Press <ENTER> when done...");
  EDIT_SetFont(hEdit, &GUI_Font8x16);
  EDIT_SetTextColor(hEdit, 0, GUI_RED);
  EDIT_EnableBlink(hEdit, 300, 1);
  while (1) {
    GUI_Delay(10);
  }
}

/*************************** End of file ****************************/
