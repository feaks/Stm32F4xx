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
File        : LCDConf.c
Purpose     : Display configuration file for GUIDRV_Bitplains to be
              used without any display controller. This implementation
              has been written and tested with the RSKR32C111 with
              Visualization Add On.
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>

#include "GUI.h"
#include "LCD_Protected.h"

#include "GUIDRV_BitPlains.h"

#ifndef WIN32

#include <ior32c111.h>
#include <intrinsics.h>

#include "RTOS.h"

#define INIT_HARDWARE() _InitHW()

#else

#define INIT_HARDWARE()

#endif

/*********************************************************************
*
*       Layer configuration
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS 320
#define YSIZE_PHYS 240

//
// Initial color conversion API
//
#define COLOR_CONVERSION GUICC_1

//
// Back- and front porch
//
#define HFP 24
#define HBP 8
#define VFP 18
#define VBP 4

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
//
// Checking display driver configuration
//
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif

//
// Checking back- and front porch definitions
//
#ifndef   HBP
  #error Horizontal back porch needs to be defined!
#else
  #if (HBP == 0)
    #error Horizontal back porch needs to be >0!
  #else
    #if (HBP & 0x7)
      #error Horizontal back porch needs to be a multiple of 8!
    #endif
  #endif
#endif
#ifndef   HFP
  #error Horizontal front porch needs to be defined!
#else
  #if (HFP == 0)
    #error Horizontal front porch needs to be >0!
  #else
    #if (HFP & 0x7)
      #error Horizontal front porch needs to be a multiple of 8!
    #endif
  #endif
#endif
#ifndef   VBP
  #error Vertical back porch needs to be defined!
#endif
#ifndef   VFP
  #error Vertical front porch needs to be defined!
#endif

#define NOP1() asm("nop"); asm("nop"); asm("nop"); asm("nop")
#define NOP2() NOP1(); NOP1(); NOP1(); NOP1()
#define NOP3() NOP2(); NOP2(); NOP2(); NOP2()

//
// Number of bytes per line
//
#define BYTES_PER_LINE ((XSIZE_PHYS + 7) >> 3)

//
// Macros for control signals
//
#define HSYNC_LOW()  P7 &= ~(1 << 5)
#define HSYNC_HIGH() P7 |=  (1 << 5)
#define VSYNC_LOW()  P9 &= ~(1 << 7)
#define VSYNC_HIGH() P9 |=  (1 << 7)
#define LCDEN_LOW()  P8 &= ~(1 << 0)
#define LCDEN_HIGH() P8 |=  (1 << 0)

//
// Defines for state machine
//
#define STATE_FPORCH 0
#define STATE_DATA   1
#define STATE_BPORCH 2

//
// Joystick states
//
#define JOYSTICK_LEFT  (1 << 0)
#define JOYSTICK_RIGHT (1 << 1)
#define JOYSTICK_UP    (1 << 2)
#define JOYSTICK_DOWN  (1 << 3)
#define JOYSTICK_ENTER (1 << 4)

//
// Display modes
//
#define MODE_8C 0
#define MODE_8G 1
#define MODE_4G 2
#define MODE_M  3

//
// Color conversion API for 8 grayscales
//
#define GUICC_3 &_API_ColorConv_3

//
// Fast interrupts
//
#define USE_FAST_INT (1)

//
// Touch support
//
#define SUPPORT_TOUCH (1)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
//
// Structure used for mode select menu
//
typedef struct {
  char * s;                                 // Descripton
  U8 Mode;                                  // Display mode
  const LCD_API_COLOR_CONV * pColorConvAPI; // Color conversion API
  GUI_RECT Rect;                            // Position on screen
} MENU_STRUCT;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
//
// Data arrays to be used by the display driver
//
static U8 * _pPlain_0;
static U8 * _pPlain_1;
static U8 * _pPlain_2;

//
// Mode dependent pointer association
//
static U8 * _p0;
static U8 * _p1;
static U8 * _p2;

//
// Structure to be passed to the driver
//
static struct {
  U8 * apVRAM[8];
} _VRAM_Desc;

//
// Driver device
//
static GUI_DEVICE * _pDriverDevice;

#ifndef WIN32

OS_STACKPTR int _Stack_PID[256];   // Task stack(s)
OS_TASK         _TCB_PID;          // Task-control-block(s)

static volatile int _v0;
static void (* _pStartTransmission)(U32 Off);
static void (* _pWaitUntilReady)   (void);

#endif

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _Color2Index_3
*/
static unsigned _Color2Index_3(LCD_COLOR Color) {
  int r, g, b;
  r = (Color >> ( 0 + 5)) & 0x7;
  g = (Color >> ( 8 + 5)) & 0x7;
  b = (Color >> (16 + 5)) & 0x7;
  return (r + g + b + 1) / 3;
}

/*********************************************************************
*
*       _Index2Color_3
*/
static LCD_COLOR _Index2Color_3(unsigned Index) {
  return ((U32)Index) * 0x555555;
}

/*********************************************************************
*
*       _GetIndexMask_3
*/
static unsigned _GetIndexMask_3(void) {
  return 0x07;
}

/*********************************************************************
*
*       _API_ColorConv_3
*/
static const LCD_API_COLOR_CONV _API_ColorConv_3 = {
  _Color2Index_3,
  _Index2Color_3,
  _GetIndexMask_3
};

/*********************************************************************
*
*       _aMenu
*
* Purpose:
*   Mode select menu. Needs to be defined after the color conversion routines
*/
static MENU_STRUCT _aMenu[] = {
  {
    "8 color mode",
    MODE_8C,
    GUICC_111
  },
  {
    "8 grayscale mode",
    MODE_8G,
    GUICC_3
  },
  {
    "4 grayscale mode",
    MODE_4G,
    GUICC_2
  },
  {
    "B/W mode",
    MODE_M,
    GUICC_1
  },
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
#ifndef WIN32
/*********************************************************************
*
*       _ReadJoystick
*
* Purpose:
*   Reads the current state of the joystick
*/
static U8 _ReadJoystick(void) {
  U8 r, Port;
  
  Port = P3;
  r    = 0;
  if ((Port & (1 << 6)) == 0) {
    r |= JOYSTICK_LEFT;
  }
  if ((Port & (1 << 5)) == 0) {
    r |= JOYSTICK_RIGHT;
  }
  if ((Port & (1 << 7)) == 0) {
    r |= JOYSTICK_UP;
  }
  if ((Port & (1 << 4)) == 0) {
    r |= JOYSTICK_DOWN;
  }
  if ((Port & (1 << 3)) == 0) {
    r |= JOYSTICK_ENTER;
  }
  return r;
}

/*********************************************************************
*
*       _PID_Task
*
* Purpose:
*   Polling the touch periodically using GUI_TOUCH_Exec()
*/
static void _PID_Task(void) {
  int xSize, ySize, TimeAcc;
  U8 State_JOY, State_JOY_Prev;
  GUI_PID_STATE State_PID;
  
  TimeAcc = 0;
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  while (1) {
    //
    // Manage joystick
    //
    State_JOY = _ReadJoystick();
    //
    // Check if something is to do
    //
    if (State_JOY | (State_JOY != State_JOY_Prev)) {
      //
      // Handle dynamic pointer acceleration
      //
      if (State_JOY == State_JOY_Prev) {
        if (TimeAcc < 10) {
          TimeAcc++;
        }
      } else {
        TimeAcc = 1;
      }
      //
      // Get current state
      //
      GUI_PID_GetState(&State_PID);
      if (State_JOY & JOYSTICK_LEFT) {
        State_PID.x -= TimeAcc;
      }
      if (State_JOY & JOYSTICK_RIGHT) {
        State_PID.x += TimeAcc;
      }
      if (State_JOY & JOYSTICK_UP) {
        State_PID.y -= TimeAcc;
      }
      if (State_JOY & JOYSTICK_DOWN) {
        State_PID.y += TimeAcc;
      }
      //
      // Make sure coordinates are still in bounds
      //
      if (State_PID.x < 0) {
        State_PID.x = 0;
      }
      if (State_PID.y < 0) {
        State_PID.y = 0;
      }
      if (State_PID.x >= xSize) {
        State_PID.x = xSize - 1;
      }
      if (State_PID.y > ySize) {
        State_PID.y = ySize - 1;
      }
      //
      // Check if pressed
      //
      State_PID.Pressed = (State_JOY & JOYSTICK_ENTER) ? 1: 0;
      //
      // Set new state
      //
      GUI_PID_StoreState(&State_PID);
      State_JOY_Prev = State_JOY;
    }
    //
    // Wait a while
    //
    OS_Delay(40);
  }
}

/*********************************************************************
*
*       _DMA_0_Init
*/
static void _DMA_0_Init(U32 Source_Address, U32 Destination_Address, U32 Len) {
  __set_DMD_register(0, (0 << 0)   // DMA transfer disabled
                      | (0 << 2)   // 8 bits
                      | (1 << 4)   // SRC updated with each transfer
                      | (0 << 5)); // DST not updated
  DM0SL2 = 0x1A;                   // UART8 transmit interrupt request
  __set_DCT_register(0, Len);
  __set_DCR_register(0, Len);
  __set_DSA_register(0, Source_Address);
  __set_DSR_register(0, Source_Address);
  __set_DDA_register(0, Destination_Address);
  __set_DDR_register(0, Destination_Address);
}

/*********************************************************************
*
*       _DMA_1_Init
*/
static void _DMA_1_Init(U32 Source_Address, U32 Destination_Address, U32 Len) {
  __set_DMD_register(1, (0 << 0)   // DMA transfer disabled
                      | (0 << 2)   // 8 bits
                      | (1 << 4)   // SRC updated with each transfer
                      | (0 << 5)); // DST not updated
  DM1SL = 0x16;                    // UART4 transmit interrupt request
  __set_DCT_register(1, Len);
  __set_DCR_register(1, Len);
  __set_DSA_register(1, Source_Address);
  __set_DSR_register(1, Source_Address);
  __set_DDA_register(1, Destination_Address);
  __set_DDR_register(1, Destination_Address);
}

/*********************************************************************
*
*       _DMA_2_Init
*/
static void _DMA_2_Init(U32 Source_Address, U32 Destination_Address, U32 Len) {
  __set_DMD_register(2, (0 << 0)   // DMA transfer disabled
                      | (0 << 2)   // 8 bits
                      | (1 << 4)   // SRC updated with each transfer
                      | (0 << 5)); // DST not updated
  DM2SL = 0x14;                    // UART5 transmit interrupt request
  __set_DCT_register(2, Len);
  __set_DCR_register(2, Len);
  __set_DSA_register(2, Source_Address);
  __set_DSR_register(2, Source_Address);
  __set_DDA_register(2, Destination_Address);
  __set_DDR_register(2, Destination_Address);
}

/*********************************************************************
*
*       _Transmit_Uart_8
*/
static void _Transmit_Uart_8(U8 Data) {
  while (U8C0_bit.TXEPT == 0);
  U8TB = Data;
}

/*********************************************************************
*
*       _Transmit_Uart_5
*/
static void _Transmit_Uart_5(U8 Data) {
  while (U5C0_bit.TXEPT == 0);
  U5TB = Data;
}

/*********************************************************************
*
*       _Transmit_Uart_4
*/
static void _Transmit_Uart_4(U8 Data) {
  while (U4C0_bit.TXEPT == 0);
  U4TB = Data;
}

/*********************************************************************
*
*       _Transmit_3
*
* Purpose:
*   Trigger DMA transmission of 3 UARTs for 8C and 8G mode
*/
static void _Transmit_3(U32 Off) {
  U8 * p0;
  U8 * p1;
  U8 * p2;
  
  //
  // Calculate pointers for transfer operation
  //
  Off += 1;
  p0 = _p0 + Off;
  p1 = _p1 + Off;
  p2 = _p2 + Off;
  //
  // Initialize DMA transfer
  //
  _DMA_0_Init((U32)p0, (U32)&U8TB, BYTES_PER_LINE - 1);
  _DMA_1_Init((U32)p1, (U32)&U4TB, BYTES_PER_LINE - 1);
  _DMA_2_Init((U32)p2, (U32)&U5TB, BYTES_PER_LINE - 1);
  //
  // Start DMA
  //
  __set_DMD_register(0, 0x11);
  __set_DMD_register(1, 0x11);
  __set_DMD_register(2, 0x11);
  //
  // Release SYNC signals to high
  //
  HSYNC_HIGH();
  VSYNC_HIGH();
  //
  // Start transmission with first byte
  //
  _Transmit_Uart_4(*(--p1));
  _Transmit_Uart_5(*(--p2));
  _Transmit_Uart_8(*(--p0));
}

/*********************************************************************
*
*       _Transmit_2
*
* Purpose:
*   Trigger DMA transmission of 2 UARTs for 4G mode
*/
static void _Transmit_2(U32 Off) {
  U8 * p0;
  U8 * p1;
  
  //
  // Calculate pointers for transfer operation
  //
  Off += 1;
  p0 = _p0 + Off;
  p1 = _p1 + Off;
  //
  // Initialize DMA transfer
  //
  _DMA_0_Init((U32)p0, (U32)&U8TB, BYTES_PER_LINE - 1);
  _DMA_2_Init((U32)p1, (U32)&U5TB, BYTES_PER_LINE - 1);
  //
  // Start DMA
  //
  __set_DMD_register(0, 0x11);
  __set_DMD_register(2, 0x11);
  //
  // Release SYNC signals to high
  //
  HSYNC_HIGH();
  VSYNC_HIGH();
  //
  // Start transmission with first byte
  //
  _Transmit_Uart_5(*(--p1));
  _Transmit_Uart_8(*(--p0));
}

/*********************************************************************
*
*       _Transmit_1
*
* Purpose:
*   Trigger DMA transmission of 1 UARTs for M mode
*/
static void _Transmit_1(U32 Off) {
  U8 * p0;
  
  //
  // Calculate pointers for transfer operation
  //
  Off += 1;
  p0 = _p0 + Off;
  //
  // Initialize DMA transfer
  //
  _DMA_0_Init((U32)p0, (U32)&U8TB, BYTES_PER_LINE - 1);
  //
  // Start DMA
  //
  __set_DMD_register(0, 0x11);
  //
  // Release SYNC signals to high
  //
  HSYNC_HIGH();
  VSYNC_HIGH();
  //
  // Start transmission with first byte
  //
  _Transmit_Uart_8(*(--p0));
}

/*********************************************************************
*
*       _WaitUntilReady_3
*
* Purpose:
*   Waits until UARTs 8, 5 and 4 are ready
*/
void _WaitUntilReady_3(void) {
  while (U8C0_bit.TXEPT == 0);
  while (U5C0_bit.TXEPT == 0);
  while (U4C0_bit.TXEPT == 0);
  while (U8C1_bit.TI == 0);
  while (U5C1_bit.TI == 0);
  while (U4C1_bit.TI == 0);
}

/*********************************************************************
*
*       _WaitUntilReady_2
*
* Purpose:
*   Waits until UARTs 8 and 5 are ready
*/
void _WaitUntilReady_2(void) {
  while (U8C0_bit.TXEPT == 0);
  while (U5C0_bit.TXEPT == 0);
  while (U8C1_bit.TI == 0);
  while (U5C1_bit.TI == 0);
}

/*********************************************************************
*
*       _WaitUntilReady_1
*
* Purpose:
*   Waits until UART 8 is ready
*/
void _WaitUntilReady_1(void) {
  while (U8C0_bit.TXEPT == 0);
  while (U8C1_bit.TI == 0);
  NOP3();
}

/*********************************************************************
*
*       _TransferCompleteInterrupt
*
* Purpose:
*   Refreshes the display. It is called after the DMA transfer of one
*   line is completed. Horizontal front- and back porch is transfered 
*   within the interrupt routine. Data is transfered via DMA.
*
*   USE_FAST_INT can be used to run the routine with or without
*   fast interrupt
*/
#if USE_FAST_INT
static __fast_interrupt void _TransferCompleteInterrupt(void) {
#else
#pragma vector = 0x08
static __interrupt void _TransferCompleteInterrupt(void) {
#endif
  volatile static int DataLine, VState, Porch;
  volatile U32 Off, Len;

  //
  // Make sure transmission is complete...
  //
  _pWaitUntilReady();
  //
  // Disable LCDEN signal
  //
  LCDEN_HIGH();
  //
  // Horizontal back porch
  //
  Len = HBP >> 3;
  while (Len--) {
    _Transmit_Uart_8(0);
  }
  while (U8C0_bit.TXEPT == 0);
  while (U8C1_bit.TI == 0);
  //
  // VSYNC
  //
  if (Porch == 0) {
    VSYNC_LOW();
  }
  //
  // HSYNC
  //
  HSYNC_LOW();
  //
  // Horizontal front porch
  //
  Len = HFP >> 3;
  while (Len--) {
    _Transmit_Uart_8(0);
  }
  while (U8C0_bit.TXEPT == 0);
  while (U8C1_bit.TI == 0);
  //
  // Vertical state check
  //
  switch (VState) {
  case STATE_FPORCH:
    //
    // Vertical back porch
    //
    if (++Porch == VFP) {
      Porch = 1;
      VState = STATE_DATA;
    }
    break;
  case STATE_DATA:
    //
    // Vertical Data
    //
    Off = DataLine * BYTES_PER_LINE;
    LCDEN_LOW();
    if (++DataLine == YSIZE_PHYS) {
      DataLine = 0;
      VState = STATE_BPORCH;
    }
    break;
  case STATE_BPORCH:
    //
    // Vertical front porch
    //
    LCDEN_HIGH();
    if (++Porch == VBP) {
      Porch = 0;
      VState = STATE_FPORCH;
    }
    break;
  }
  _pStartTransmission(Off);
}

/*********************************************************************
*
*       _MeasurePerformance
*/
static int _MeasurePerformance(void) {
  int t0;
  volatile int Cnt = 0;
  
  OS_Delay(1);
  t0 = OS_GetTime() + 10;
  do {
    Cnt++;
  } while ((t0 - OS_GetTime()) > 0);
  return Cnt;
}

/*********************************************************************
*
*       _SetDisplayMode
*
* Purpose:
*   Sets the mode pins in accordance to the desired mode
*/
static void _SetDisplayMode(int Mode) {
  P2   |= ((1 << 5)   // P25 (MODE_8G) high
        |  (1 << 6)   // P26 (MODE_8C) high
        |  (1 << 7)); // P27 (MODE_4G) high
  P3   |=  (1 << 0);  // P30 (MODE_M)  high
  switch (Mode) {
  case MODE_8C:
    _pStartTransmission = _Transmit_3;
    _pWaitUntilReady    = _WaitUntilReady_3;
    //
    // Set display mode pin
    //
    P2 &= ~(1 << 6);  // P26 (MODE_8C) low
    //
    // Memory allocation of bitplains
    //
    if (_pPlain_0 == NULL) {
      _pPlain_0 = GUI_ALLOC_GetFixedBlock(BYTES_PER_LINE * YSIZE_PHYS);
    }
    if (_pPlain_1 == NULL) {
      _pPlain_1 = GUI_ALLOC_GetFixedBlock(BYTES_PER_LINE * YSIZE_PHYS);
    }
    if (_pPlain_2 == NULL) {
      _pPlain_2 = GUI_ALLOC_GetFixedBlock(BYTES_PER_LINE * YSIZE_PHYS);
    }
    //
    // Assignment of bitplains
    //
    _p0 = _pPlain_2;
    _p1 = _pPlain_1;
    _p2 = _pPlain_0;
    break;
  case MODE_8G:
    _pStartTransmission = _Transmit_3;
    _pWaitUntilReady    = _WaitUntilReady_3;
    //
    // Set display mode pin
    //
    P2 &= ~(1 << 5);  // P25 (MODE_8G) low
    //
    // Memory allocation of bitplains
    //
    if (_pPlain_0 == NULL) {
      _pPlain_0 = GUI_ALLOC_GetFixedBlock(BYTES_PER_LINE * YSIZE_PHYS);
    }
    if (_pPlain_1 == NULL) {
      _pPlain_1 = GUI_ALLOC_GetFixedBlock(BYTES_PER_LINE * YSIZE_PHYS);
    }
    if (_pPlain_2 == NULL) {
      _pPlain_2 = GUI_ALLOC_GetFixedBlock(BYTES_PER_LINE * YSIZE_PHYS);
    }
    //
    // Assignment of bitplains
    //
    _p0 = _pPlain_2;
    _p1 = _pPlain_1;
    _p2 = _pPlain_0;
    break;
  case MODE_4G:
    _pStartTransmission = _Transmit_2;
    _pWaitUntilReady    = _WaitUntilReady_2;
    //
    // Set display mode pin
    //
    P2 &= ~(1 << 7);  // P27 (MODE_4G) low
    //
    // Memory allocation of bitplains
    //
    if (_pPlain_0 == NULL) {
      _pPlain_0 = GUI_ALLOC_GetFixedBlock(BYTES_PER_LINE * YSIZE_PHYS);
    }
    if (_pPlain_1 == NULL) {
      _pPlain_1 = GUI_ALLOC_GetFixedBlock(BYTES_PER_LINE * YSIZE_PHYS);
    }
    //
    // Assignment of bitplains
    //
    _p0 = _pPlain_1;
    _p1 = _pPlain_0;
    _p2 = NULL;
    break;
  case MODE_M:
    _pStartTransmission = _Transmit_1;
    _pWaitUntilReady    = _WaitUntilReady_1;
    //
    // Set display mode pin
    //
    P3 &= ~(1 << 0);  // P30 (MODE_M)  low
    //
    // Memory allocation of bitplains
    //
    if (_pPlain_0 == NULL) {
      _pPlain_0 = GUI_ALLOC_GetFixedBlock(BYTES_PER_LINE * YSIZE_PHYS);
    }
    //
    // Assignment of bitplains
    //
    _p0 = _pPlain_0;
    _p1 = NULL;
    _p2 = NULL;
    break;
  }
}

/*********************************************************************
*
*       _InitHW
*
* Purpose:
*   Initializes the peripherals used for driving the display
*
*   Start refreshing of display:
*   DMA     UART     Color    Plain
*   ---------------------------------
*   DMA0 -> UART8 -> R0-R5 -> Plain 0
*   DMA1 -> UART4 -> G0-G5 -> Plain 1
*   DMA2 -> UART5 -> B0-B5 -> Plain 2
*/
static void _InitHW(void) {
  //
  // Setup port direction registers for display
  //
  PD2  |= ((1 << 5)   // P25 (MODE_8G) as output
        |  (1 << 6)   // P26 (MODE_8C) as output
        |  (1 << 7)); // P27 (MODE_4G) as output
  PD3  |=  (1 << 0);  // P30 (MODE_M) as output
  PD7  |= ((1 << 3)   // P73 (TXD8) as output
        |  (1 << 4)   // P74 (CLK8) as output
        |  (1 << 5)   // P75 (HSYNC) as output
        |  (1 << 6)); // P76 (TXD5) as output
  PD8  |=  (1 << 0);  // P80 (LCDEN) as output
  PRCR |=  (1 << 2);  // Enable access to PD9
  PD9  |= ((1 << 6)   // P96 (TXD4) as output
        |  (1 << 7)); // P97 (VSYNC) as output
  //
  // Setup function select registers for display
  //
  P7_3S = 0x07;       // TXD8 output
  P7_4S = 0x07;       // CLK8 output
  P7_6S = 0x03;       // TXD5 output
  PRCR |=  (1 << 2);  // Enable access to P9_6S
  P9_6S = 0x03;       // TXD4 output
  P7_7S = 0x03;       // CLK5 input
  PRCR |=  (1 << 2);  // Enable access to P9_5S
  P9_5S = 0x03;       // CLK4 input
  //
  // Setup function select registers for A/D converter input pins
  //
  P10_1S = (1 << 7);  // AN1 input A/D converter
  P10_2S = (1 << 7);  // AN2 input A/D converter
  //
  // Pullup joystick pins
  //
  PUR0 |= (1 << 6) | (1 << 7);
  //
  // UART8, master, R0-R5
  //
  U8MR  = 0;        // Reset
  U8MR  = (1 << 0)  // Synchronous mode
        | (0 << 3)  // Internal clock
        | (0 << 4)  // 1 stop bit
        | (0 << 5)  // Odd parity
        | (0 << 6)  // Parity disabled
        | (0 << 7); // -
  U8C0  = (0 << 0)  // Count source f1 (16 MHz)
        | (0 << 2)  // -
        | (0 << 3)  // Data held in the transmit shift register
        | (1 << 4)  // CTS disabled
        | (0 << 5)  // Pins TXDi/SDAi and SCLi are push-pull output
        | (0 << 6)  // Output transmit data on the falling edge
        | (0 << 7); // LSB first
  U8C1  = (0 << 0)  // Transmission disabled
        | (0 << 2); // Reception disabled
  U8BRG = 2;        // fx / (m + 1) -> 16 / (2 + 1) = 5.33 MHz
  U8C1  = (1 << 0)  // Transmission enabled
        | (0 << 2); // Reception disabled
  //
  // UART5, slave, B0-B5
  //
  U5MR  = 0;        // Reset
  U5MR  = (1 << 0)  // Synchronous mode
        | (1 << 3)  // External clock
        | (0 << 4)  // 1 stop bit
        | (0 << 5)  // Odd parity
        | (0 << 6)  // Parity disabled
        | (0 << 7); // Non inverted
  U5C0  = (0 << 0)  // Count source f1 (16 MHz)
        | (0 << 2)  // -
        | (0 << 3)  // Data held in the transmit shift register
        | (1 << 4)  // CTS disabled
        | (0 << 5)  // Pins TXDi/SDAi and SCLi are push-pull output
        | (0 << 6)  // Output transmit data on the falling edge
        | (0 << 7); // LSB first
  U5C1  = (0 << 0)  // Transmission disabled
        | (0 << 2); // Reception disabled
  U5BRG = 2;        // fx / (m + 1) -> 16 / (2 + 1) = 5.33 MHz
  U5C1  = (1 << 0)  // Transmission enabled
        | (0 << 2); // Reception disabled
  //
  // UART4, slave, B0-B5
  //
  U4MR  = 0;        // Reset
  U4MR  = (1 << 0)  // Synchronous mode
        | (1 << 3)  // External clock
        | (0 << 4)  // 1 stop bit
        | (0 << 5)  // Odd parity
        | (0 << 6)  // Parity disabled
        | (0 << 7); // Non inverted
  U4C0  = (0 << 0)  // Count source f1 (16 MHz)
        | (0 << 2)  // -
        | (0 << 3)  // Data held in the transmit shift register
        | (1 << 4)  // CTS disabled
        | (0 << 5)  // Pins TXDi/SDAi and SCLi are push-pull output
        | (0 << 6)  // Output transmit data on the falling edge
        | (0 << 7); // LSB first
  U4C1  = (0 << 0)  // Transmission disabled
        | (0 << 2); // Reception disabled
  U4BRG = 2;        // fx / (m + 1) -> 16 / (2 + 1) = 5.33 MHz
  U4C1  = (1 << 0)  // Transmission enabled
        | (0 << 2); // Reception disabled
  //
  // Set initial states of control signals
  //
  HSYNC_HIGH();
  VSYNC_HIGH();
  LCDEN_HIGH();
  //
  // Enable DMA0 transfer complete interrupt
  //
  DM0IC = (0 << 0); // Disable interrupt
  DM0IC = (7 << 0); // Level 7
  //
  // Enable fast interrupt
  //
#if USE_FAST_INT
  __set_VCT_register((U32)_TransferCompleteInterrupt);
  RIPL1 |= (1 << 3);
  RIPL2 |= (1 << 3);
#endif
  //
  // Set initial display mode
  //
  _SetDisplayMode(MODE_M);
  //
  // Start transmission
  //
  _pStartTransmission(0);
}

#endif

/*********************************************************************
*
*       _CreateDriverDevice
*
* Purpose:
*   Creates a display driver device with the given color conversion API
*/
static _CreateDriverDevice(const LCD_API_COLOR_CONV * pColorConvAPI) {
  //
  // Set display driver and color conversion for 1st layer
  //
  _pDriverDevice = GUI_DEVICE_CreateAndLink(GUIDRV_BITPLAINS, pColorConvAPI, 0, 0);
  //
  // Display driver configuration
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
  }
  //
  // Initialize VRAM access off driver
  //
  _VRAM_Desc.apVRAM[0] = _pPlain_0;
  _VRAM_Desc.apVRAM[1] = _pPlain_1;
  _VRAM_Desc.apVRAM[2] = _pPlain_2;
  LCD_SetVRAMAddrEx(0, (void *)&_VRAM_Desc);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*   
*/
void LCD_X_Config(void) {
#ifndef WIN32
  //
  // Get number if iterations without display driver
  //
  _v0 = _MeasurePerformance();
  //
  // Initialize hardware
  //
  INIT_HARDWARE();
#endif
  //
  // Create initial display driver device
  //
  _CreateDriverDevice(COLOR_CONVERSION);
  //
  // Create PID task for polling joystick and touch screen
  //
  OS_CREATETASK(&_TCB_PID, "PID Task", _PID_Task, 200, _Stack_PID);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if 
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * p) {
  return -1;
}

/*********************************************************************
*
*       LCD_X_SelectMode
*/
void LCD_X_SelectMode(void) {
  int i, Select;
  GUI_PID_STATE State;
  GUI_RECT * pRect;
#if !defined(WIN32) & 0
  int v1, CPU_Load;
#endif
  
  //
  // Show cursor
  //
  GUI_Clear();
  GUI_CURSOR_Show();
  //
  // Draw menu
  //
  GUI_DispStringHCenterAt("Please use the joystick for\nselecting the desired color mode...", 160, 20);
  for (i = 0; i < GUI_COUNTOF(_aMenu); i++) {
    pRect = &_aMenu[i].Rect;
    //
    // Calculate position
    //
    pRect->x0 = 10;
    pRect->x1 = 309;
    pRect->y0 = 50 + i * 30;
    pRect->y1 = 50 + i * 30 + 28;
    //
    // Draw
    //
    GUI_DispStringInRect(_aMenu[i].s, pRect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    GUI_DrawRectEx(pRect);
  }
  //
  // Menu loop
  //
  State.x = 160;
  State.y = 180;
  GUI_CURSOR_SetPosition(State.x, State.y);
  GUI_PID_StoreState(&State);
  while (1) {
    GUI_PID_GetState(&State);
    //
    // Check if pressed
    //
    if (State.Pressed) {
      //
      // Get menu item
      //
      for (i = Select = 0; i < GUI_COUNTOF(_aMenu); i++) {
        pRect = &_aMenu[i].Rect;
        if ((State.x >= pRect->x0) && (State.x <= pRect->x1) && (State.y >= pRect->y0) && (State.y <= pRect->y1)) {
          Select = i + 1;
          break;
        }
      }
      if (Select) {
        Select--;
        break;
      }
    }
    GUI_Delay(10);
  }
  GUI_CURSOR_Hide();
  //
  // 'Delete' current cursor
  //
  GUI_CURSOR_Select(NULL);
  //
  // Erase current driver device
  //
  GUI_DEVICE_Unlink(_pDriverDevice);
  GUI_ALLOC_FreeFixedBlock(_pDriverDevice->u.pContext);
  _pDriverDevice->u.pContext = NULL;
  GUI_DEVICE_Delete(_pDriverDevice);
  //
  // Set display mode
  //
#ifndef WIN32
  _SetDisplayMode(_aMenu[Select].Mode);
#endif
  //
  // Create new driver device
  //
  _CreateDriverDevice(_aMenu[Select].pColorConvAPI);
  LCD_UpdateColorIndices();
  //
  // Show a short confirmation message
  //
  GUI_Clear();
  GUI_DispString("Starting ");
  GUI_DispString(_aMenu[i].s);
  GUI_DispString(" ...");
#if 0
  //
  // Calculate and show CPU load of driver
  //
  v1 = _MeasurePerformance();
  CPU_Load = ((_v0 - v1) * 100) / _v0;
  GUI_DispString("\n\nCPU_Load of driver: ");
  GUI_DispDecMin(CPU_Load);
  GUI_DispChar('%');
#endif
  GUI_DispString("\n\nGUI Memory: ");
  GUI_DispDecMin(GUI_ALLOC_GetNumFreeBytes());
  GUI_DispString(" bytes");
  GUI_Delay(2000);
}

/*************************** End of file ****************************/
