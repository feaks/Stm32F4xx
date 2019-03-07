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
File        : 320_Overview_Whirlpool_B.c
Content     : Bitmap 24 * 15
---------------------------END-OF-HEADER------------------------------
*/

#include <stdlib.h>

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif


static GUI_CONST_STORAGE unsigned short ac320_Overview_Whirlpool_B[] = {
  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x318D, 0x09F3, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0CA6, 0x6B5A, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF,
  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x56B5, 0x0CE9, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0A57, 0x1CE7, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF,
  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x14A6, 0x0ABC, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0D6E, 0x4E73, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF,
  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x39CE, 0x0D90, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0AFD, 0x1064, 0x739C, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF,
  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x6319, 0x0CC7, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0A15, 0x2D4B, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF,
  0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x1CE7, 0x0A78, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0D2B, 0x56B5, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF,
  0x1CE7, 0x1064, 0x1064, 0x1064, 0x1064, 0x1064, 0x1064, 0x1064, 0x0A15, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0CE9, 0x1064, 0x1064, 0x1064, 0x1064, 0x1064, 0x1064, 0x1064, 0x0C63,
  0x1064, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0DD1, 0x0842,
  0x1064, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0DD1, 0x0842,
  0x1064, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0DD1, 0x0842,
  0x1064, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0DD1, 0x0842,
  0x1064, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0DD1, 0x0842,
  0x1064, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x071F, 0x0DD1, 0x0842,
  0x1064, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0DD1, 0x0D2B, 0x0842,
  0x14A5, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842
};

extern GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Whirlpool_B;

GUI_CONST_STORAGE GUI_BITMAP bm320_Overview_Whirlpool_B = {
  24, /* XSize */
  15, /* YSize */
  48, /* BytesPerLine */
  16, /* BitsPerPixel */
  (unsigned char *)ac320_Overview_Whirlpool_B,  /* Pointer to picture data */
  NULL  /* Pointer to palette */
 ,GUI_DRAW_BMP555
};

/*************************** End of file ****************************/
