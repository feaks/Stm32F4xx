/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.46 - Graphical user interface for embedded applications **
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
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              ARM Ltd, 110 Fulbourn Road, CB1 9NJ Cambridge, UK
Licensed SEGGER software: emWin
License number:           GUI-00181
License model:            LES-SLA-20007, Agreement, effective since October 1st 2011 
Licensed product:         MDK-ARM Professional
Licensed platform:        ARM7/9, Cortex-M/R4
Licensed number of seats: -
----------------------------------------------------------------------
File        : GUI_SPY_X_StartServer.c
Purpose     : Start of emWinSPY server via RTT and/or TCP/IP.
---------------------------END-OF-HEADER------------------------------
*/

#include <stdlib.h>

#include "main.h"
#include "GUI.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Select interface to be used
//
#ifndef   USE_RTT
  #define USE_RTT (0)
#endif
#ifndef   USE_TCP
  #define USE_TCP (1)
#endif

#if (USE_TCP == 1)
  #include "FreeRTOS_TCP_IP.h"         // BSD socket interface
#endif

#if (USE_RTT == 1)
  #include "SEGGER_RTT.h" // RTT interface
#endif

//
// Check selection
//
#if (USE_TCP == 0) && (USE_RTT == 0)
  #error Please select TCP or RTT
#endif

//
// Port definition for TCP connection
//
#if (USE_TCP == 1)
  #define PORT 2468 // emWinSPY-Port
#endif

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
#if (USE_TCP == 1)

//
// embOS Stack area of the server
//
//
// embOS Task-control-block of the server
//
static TaskHandle_t Task_TCPHandler;
#endif

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _Send_RTT
*
*  Function description:
*    Sending data via RTT
*
*  Return value:
*    On success number of successfully transmitted bytes, otherwise -1.
*/
#if (USE_RTT == 1)
static int _Send_RTT(const U8 * buf, int len, void * pConnectInfo) {
  int r, RemBytes, Cnt;

  GUI_USE_PARA(pConnectInfo);
  Cnt = 10000;
  RemBytes = len;
  do {
    r = SEGGER_RTT_Write(_IndexUp, buf, RemBytes);
    buf += r;
    RemBytes -= r;
    if (RemBytes) {
      GUI_X_Delay(1);
    }
  } while ((RemBytes > 0) && (--Cnt));
  return Cnt ? len : -1;
}

/*********************************************************************
*
*       _Recv_RTT
*
*  Function description:
*    Receiving data via RTT
*
*  Return value:
*    On success number of successfully received bytes, otherwise -1.
*/
static int _Recv_RTT(U8 * buf, int len, void * pConnectInfo) {
  int r, RemBytes, Cnt;

  GUI_USE_PARA(pConnectInfo);
  Cnt = 10000;
  RemBytes = len;
  do {
    while (SEGGER_RTT_HasData(_IndexDown) == 0) {
      GUI_X_Delay(1);
    };
    r = SEGGER_RTT_Read(_IndexDown, buf, RemBytes);
    buf += r;
    RemBytes -= r;
  } while ((RemBytes > 0) && (--Cnt));
  return Cnt ? len : -1;
}

/*********************************************************************
*
*       _ServerTask_RTT
*
*  Function description:
*    This task  waits for an incoming connection. If a connection
*    has been established it calls GUI_SPY_Process() which should
*    return if the connection is closed.
*/
static void _ServerTask_RTT(void) {
  while (1) {
    //
    // Execute emWinSPY, returns after connection is closed
    //
    GUI_SPY_Process(_Send_RTT, _Recv_RTT, NULL);
    //
    // Wait a while...
    //
    GUI_X_Delay(10);
  }
}
#endif

/*********************************************************************
*
*       _ListenAtTcpAddr
*
* Starts listening at the given TCP port.
*/
#if (USE_TCP == 1)


/*********************************************************************
*
*       _Send_TCP
*
*  Function description:
*    Sending data via TCP
*
*  Return value:
*    On success number of successfully transmitted bytes, otherwise -1.
*/
static int _Send_TCP(const U8 * buf, int len, void * pConnectInfo) {
  int r;
	Socket_t *sendSocket =(Socket_t *)pConnectInfo;
	
  r = FreeRTOS_send(*sendSocket, (const char *)buf, len, 0);
  return r;
}

/*********************************************************************
*
*       _Recv_TCP
*
*  Function description:
*    Receiving data via TCP
*
*  Return value:
*    On success number of successfully received bytes, otherwise -1.
*/
static int _Recv_TCP(U8 * buf, int len, void * pConnectInfo) {
    int r;
	Socket_t *recvSocket =(Socket_t *)pConnectInfo;
	
  r = FreeRTOS_recv(recvSocket, (char *)buf, len, 0);
  return r;
}

/*********************************************************************
*
*       _ServerTask_TCP
*
*  Function description:
*    This task  waits for an incoming connection. If a connection
*    has been established it calls GUI_SPY_Process() which should
*    return if the connection is closed.
*/
static void _ServerTask_TCP(void * p) {
	int s, Sock, AddrLen;
	
  struct freertos_sockaddr xClient,xBindAddress;
	socklen_t xSize = sizeof( xClient );
	Socket_t xListeningSocket, xConnectedSocket;
	static const TickType_t TimeOut = portMAX_DELAY;
	const BaseType_t xBacklog = 20;
	
	static const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( 1000 );
	static const TickType_t xSendTimeOut = pdMS_TO_TICKS( 1000 );
	
	/* Attempt to open the socket. */
  xListeningSocket = FreeRTOS_socket( FREERTOS_AF_INET,
                                      FREERTOS_SOCK_STREAM,  /* SOCK_STREAM for TCP. */
                                      FREERTOS_IPPROTO_TCP );
	configASSERT( xListeningSocket != FREERTOS_INVALID_SOCKET );
	
	/* Set a time out so accept() will just wait for a connection. */
	FreeRTOS_setsockopt( xListeningSocket, 0, FREERTOS_SO_RCVTIMEO, &TimeOut, sizeof( TimeOut ) );
	
	/* Bind the socket to the port that the client task will send to, then
	listen for incoming connections. */
	xBindAddress.sin_port = PORT;
	xBindAddress.sin_port = FreeRTOS_htons( xBindAddress.sin_port );
	FreeRTOS_bind( xListeningSocket, &xBindAddress, sizeof( xBindAddress ) );
	FreeRTOS_listen( xListeningSocket, xBacklog );
	
	for( ;; )
	{
		/* Wait for a client to connect. */
		xConnectedSocket = FreeRTOS_accept( xListeningSocket, &xClient, &xSize );
		configASSERT( xConnectedSocket != FREERTOS_INVALID_SOCKET );
		
		FreeRTOS_setsockopt( xConnectedSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
		FreeRTOS_setsockopt( xConnectedSocket, 0, FREERTOS_SO_SNDTIMEO, &xSendTimeOut, sizeof( xReceiveTimeOut ) );
		//
    // Execute emWinSPY
    //
    GUI_SPY_Process(_Send_TCP, _Recv_TCP, (void *)&xConnectedSocket);
		//
    // Close the connection
    //
		xConnectedSocket = 0;
		FreeRTOS_closesocket( xConnectedSocket );
	}

}
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SPY_X_StartServer
*/
int GUI_SPY_X_StartServer(void) {
#if (USE_RTT == 1)
  //
  // Allocate buffers for RTT
  //
  _IndexUp   = SEGGER_RTT_AllocUpBuffer  ("emWinSPY", _acBufferUp,   sizeof(_acBufferUp),   SEGGER_RTT_MODE_NO_BLOCK_TRIM);
  if (_IndexUp < 0) {
    return 1;
  }
  _IndexDown = SEGGER_RTT_AllocDownBuffer("emWinSPY", _acBufferDown, sizeof(_acBufferDown), SEGGER_RTT_MODE_NO_BLOCK_TRIM);
  if (_IndexDown < 0) {
    return 1;
  }
  //
  // Create task for RTT Server
  //
  OS_CREATETASK(&_SPYServer_TCB_RTT, "SPY-Server(RTT)", _ServerTask_RTT, 100, _StackSPYServer_RTT);
#endif
#if (USE_TCP == 1)
  //
  // Create task for TCP/IP server
  //
	xTaskCreate(_ServerTask_TCP,"SPY-Server(TCP)",0x200, NULL, 2,   &Task_TCPHandler );    // 100	
#endif
  //
  // O.k., server(s) started
  //
  return 0;
}

/*************************** End of file ****************************/
