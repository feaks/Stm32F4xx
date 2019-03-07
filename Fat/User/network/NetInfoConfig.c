/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "phyHandling.h" 
  
#include <stdio.h>  
#include <string.h>  
#include <stdarg.h>  
  
#include "bsp_random.h"  
#include "../network/netInfoConfig.h"  
  


#include "GUI.h"  
  
/* Define names that will be used for DNS, LLMNR and NBNS searches.  This allows
mainHOST_NAME to be used when the IP address is not known.  For example
"ping RTOSDemo" to resolve RTOSDemo to an IP address then send a ping request. */
#define mainHOST_NAME							"server"
#define mainDEVICE_NICK_NAME			"stm32" 

  /* Simple echo client task parameters ---------------------------------------
See http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Clients.html */
#define mainECHO_CLIENT_TASK_STACK_SIZE ( configMINIMAL_STACK_SIZE * 4 )
#define mainECHO_CLIENT_TASK_PRIORITY	( tskIDLE_PRIORITY + 1 )


#define mainECHO_SERVER_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )
#define	mainECHO_SERVER_STACK_SIZE		( configMINIMAL_STACK_SIZE * 4 )

	
#define  getRandomNum() Random_GetNumber()

static BaseType_t UDP_DEBUG_INIT();  
  
/* 
    return a rand num 
*/  
UBaseType_t uxRand(){  
    return (UBaseType_t) getRandomNum();  
}  
  
  
/* The default IP and MAC address used by the demo.  The address configuration 
defined here will be used if ipconfigUSE_DHCP is 0, or if ipconfigUSE_DHCP is 
1 but a DHCP server could not be contacted.  See the online documentation for 
more information. */  
const uint8_t ucIPAddress[ 4 ] = { configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3 };  
const uint8_t ucNetMask[ 4 ] = { configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3 };  
const uint8_t ucGatewayAddress[ 4 ] = { configGATEWAY_ADDR0, configGATEWAY_ADDR1, configGATEWAY_ADDR2, configGATEWAY_ADDR3 };  
const uint8_t ucDNSServerAddress[ 4 ] = { configDNS_SERVER_ADDR0, configDNS_SERVER_ADDR1, configDNS_SERVER_ADDR2, configDNS_SERVER_ADDR3 };  
  
/* Default MAC address configuration.  The demo creates a virtual network 
connection that uses this MAC address by accessing the raw Ethernet data 
to and from a real network connection on the host PC.  See the 
configNETWORK_INTERFACE_TO_USE definition for information on how to configure 
the real network connection to use. */  
const uint8_t ucMACAddress[ 6 ] = { configMAC_ADDR0, configMAC_ADDR1, configMAC_ADDR2, configMAC_ADDR3, configMAC_ADDR4, configMAC_ADDR5 };  
  
  
/* 
*Brief: 
    vApplicationIPNetworkEventHook() is an application  
    defined hook (or callback) function that is called by the  
    TCP/IP stack when the network either connects or disconnects.  
    As the function is called by the TCP/IP stack the TCP/IP sets  
    sets the value of the function's parameter. 
*param: 
    eNetworkEvent = 0 , the net connected. 
    eNetworkEvent = 1 , the net disconnected. 
*/    
/* Called by FreeRTOS+TCP when the network connects or disconnects.  Disconnect
events are only received if implemented in the MAC driver. */
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
char cBuffer[ 16 ];
static BaseType_t xTasksAlreadyCreated = pdFALSE;

	FreeRTOS_printf( ( "vApplicationIPNetworkEventHook: event %ld\n", eNetworkEvent ) );

	/* If the network has just come up...*/
	if( eNetworkEvent == eNetworkUp )
	{
		/* Create the tasks that use the IP stack if they have not already been
		created. */
		if( xTasksAlreadyCreated == pdFALSE )
		{
			/* Tasks that use the TCP/IP stack can be created here. */

			/* Start a new task to fetch logging lines and send them out. */
			#if( mainCREATE_UDP_LOGGING_TASK == 1 )
			{
				vUDPLoggingTaskCreate();
			}
			#endif

			#if( ( mainCREATE_FTP_SERVER == 1 ) || ( mainCREATE_HTTP_SERVER == 1 ) )
			{
				/* Let the server work task now it can now create the servers. */
				xTaskNotifyGive( xServerWorkTaskHandle );
			}
			#endif

			#if( TCP_CLIENT == 1 )
			{
				vStartTCPEchoClientTasks_SingleTasks( mainECHO_CLIENT_TASK_STACK_SIZE, mainECHO_CLIENT_TASK_PRIORITY );
			}
			#endif

			#if( TCP_SERVER == 1 )
			{
				vStartSimpleTCPServerTasks( mainECHO_SERVER_STACK_SIZE, mainECHO_SERVER_TASK_PRIORITY );
			}
			#endif

			/* Register example commands with the FreeRTOS+CLI command
			interpreter via the UDP port specified by the
			mainUDP_CLI_PORT_NUMBER constant.  If the HTTP or FTP servers are
			being created then file system related commands will also be
			registered from the TCP server task. */
			#if( mainCREATE_UDP_CLI_TASKS == 1 )
			{
				vRegisterSampleCLICommands();
				vRegisterTCPCLICommands();
				vStartUDPCommandInterpreterTask( mainUDP_CLI_TASK_STACK_SIZE, mainUDP_CLI_PORT_NUMBER, mainUDP_CLI_TASK_PRIORITY );
			}
			#endif

			xTasksAlreadyCreated = pdTRUE;
		}

		/* Print out the network configuration, which may have come from a DHCP
		server. */
		FreeRTOS_GetAddressConfiguration( &ulIPAddress, &ulNetMask, &ulGatewayAddress, &ulDNSServerAddress );
		FreeRTOS_inet_ntoa( ulIPAddress, cBuffer );
		FreeRTOS_printf( ( "IP Address: %s\n", cBuffer ) );
		printf(  "IP Address: %s\n", cBuffer  );

		FreeRTOS_inet_ntoa( ulNetMask, cBuffer );
		FreeRTOS_printf( ( "Subnet Mask: %s\n", cBuffer ) );
		printf(  "Subnet Mask: %s\n", cBuffer  );

		FreeRTOS_inet_ntoa( ulGatewayAddress, cBuffer );
		FreeRTOS_printf( ( "Gateway Address: %s\n", cBuffer ) );
		printf(  "Gateway Address: %s\n", cBuffer  );
		
		FreeRTOS_inet_ntoa( ulDNSServerAddress, cBuffer );
		FreeRTOS_printf( ( "DNS Server Address: %s\n", cBuffer ) );
		printf( "DNS Server Address: %s\n", cBuffer  );
	}
}
/*--------------------------------------------------------*/  
 
/* 
    there are series of func should be defined when ipconfigSUPPORT_OUTGOING_PINGS is set to 1. 
*/  
QueueHandle_t xPingReplyQueue;//this queue should be create for vApplicationPingReplyHook()  
 /* this func should be used before FreeRTOS_SendPingRequest() to init the xPingReplyQueue*/  
void xPingReplyQueueCreate(){  
    xPingReplyQueue = xQueueCreate( 20, sizeof( uint16_t ) );  
} 
BaseType_t vSendPing( const int8_t *pcIPAddress )
{
uint16_t usRequestSequenceNumber, usReplySequenceNumber;
uint32_t ulIPAddress;

    /* The pcIPAddress parameter holds the destination IP address as a string in
    decimal dot notation (for example, "192.168.0.200").  Convert the string into
    the required 32-bit format. */
    ulIPAddress = FreeRTOS_inet_addr( pcIPAddress );
		
	if(xPingReplyQueue == NULL)
		xPingReplyQueueCreate();
    /* Send a ping containing 8 data bytes.  Wait (in the Blocked state) a
    maximum of 100ms for a network buffer into which the generated ping request
    can be written and sent. */
    usRequestSequenceNumber = FreeRTOS_SendPingRequest( ulIPAddress, 8, 100 / portTICK_PERIOD_MS );

    if( usRequestSequenceNumber == pdFAIL )
    {
        /* The ping could not be sent because a network buffer could not be
        obtained within 100ms of FreeRTOS_SendPingRequest() being called. */
    }
    else
    {
        /* The ping was sent.  Wait 200ms for a reply.  The sequence number from
        each reply is sent from the vApplicationPingReplyHook() on the
        xPingReplyQueue queue (this is not standard behaviour, but implemented in
        the example function above).  It is assumed the queue was created before
        this function was called! */
        if( xQueueReceive( xPingReplyQueue,
                           &usReplySequenceNumber,
                           200 / portTICK_PERIOD_MS ) == pdPASS )
        {
            /* A ping reply was received.  Was it a reply to the ping just sent? */
            if( usRequestSequenceNumber == usReplySequenceNumber )
            {
                /* This was a reply to the request just sent. */
            }
        }
    }
}

  
void vApplicationPingReplyHook( ePingReplyStatus_t eStatus, uint16_t usIdentifier )  
{  
    switch( eStatus )  
    {  
        case eSuccess    :  
            /* A valid ping reply has been received.  Post the sequence number 
            on the queue that is read by the vSendPing() function below.  Do 
            not wait more than 10ms trying to send the message if it cannot be 
            sent immediately because this function is called from the TCP/IP 
            RTOS task - blocking in this function will block the TCP/IP RTOS task. */  
            xQueueSend( xPingReplyQueue, &usIdentifier, 10 / portTICK_PERIOD_MS );  
            break;  
  
        case eInvalidChecksum :  
        case eInvalidData :  
            /* A reply was received but it was not valid. */  
            break;  
    }  
}  
  
  
/* 
 * The following function should be provided by the user and return true if it 
 * matches the domain name. 
 *this func will be used to judge whether *pcName matche DNS request or LLMNR request. 
    return pdTRUE if matching DNS request, 
    return pdFALSE if matching LLMNR request. 
 */  
BaseType_t xApplicationDNSQueryHook( const char *pcName ){  
        BaseType_t xReturn;  
  
        /* Determine if a name lookup is for this node.  Two names are given 
        to this node: that returned by pcApplicationHostnameHook() and that set 
        by mainDEVICE_NICK_NAME. */  
        if( strcmp( pcName, pcApplicationHostnameHook() ) == 0 )  
        {  
            xReturn = pdPASS;  
        }  
        else if( strcmp( pcName, mainDEVICE_NICK_NAME ) == 0 )  
        {  
            xReturn = pdPASS;  
        }  
        else  
        {  
            xReturn = pdFAIL;  
        }  
  
        return xReturn;  
}  
/*----------------------------------------------*/  
  
const char *pcApplicationHostnameHook( void )
{
	/* Assign the name "rtosdemo" to this network node.  This function will be
	called during the DHCP: the machine will be registered with an IP address
	plus this name. */
	return mainHOST_NAME;
}
  
  
/*-----------------------------------------------------------*/  
      
void IP_init( void ){  
    /***NOTE*** Tasks that use the network are created in the network event hook 
    when the network is connected and ready for use (see the definition of 
    vApplicationIPNetworkEventHook() below).  The address values passed in here 
    are used if ipconfigUSE_DHCP is set to 0, or if ipconfigUSE_DHCP is set to 1 
    but a DHCP server cannot be contacted. */  
    FreeRTOS_IPInit( ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress );  
}  
  
  void lcd_printf_err(BaseType_t err)
{
	char string[10];
	GUI_GotoXY(300,400);
	GUI_DispString(" \n");
	GUI_GotoXY(300,400);
	switch(err)
	{
		case -pdFREERTOS_ERRNO_EINTR:
			GUI_DispString("Interrupted system call");
			break;
		case -pdFREERTOS_ERRNO_ENXIO:
			GUI_DispString("No such device or address");
			break;
		case -pdFREERTOS_ERRNO_EAGAIN:
			GUI_DispString("No more processes");
			break;
		case -pdFREERTOS_ERRNO_ENOMEM:
			GUI_DispString(" Not enough memory");
			break;
		case -pdFREERTOS_ERRNO_EUNATCH:
			GUI_DispString("Protocol driver not attached");
			break;
		case -pdFREERTOS_ERRNO_ENOBUFS:
			GUI_DispString("No buffer space available");
			break;
		case -pdFREERTOS_ERRNO_EADDRINUSE:
			GUI_DispString("Address already in use");
			break;
		case -pdFREERTOS_ERRNO_ENOTCONN:
			GUI_DispString("Socket is not connected");
			break;
		default: sprintf(string,"%d",-err);GUI_DispString(string);;break;
	}
}
/*------------DEBUG ONLY----------------*/  
Socket_t UDP_DEBUG_SOCKET;  
  
/* 
Brief : func used for init the udp 
Return : if sucess,return pdTRUE 
*/  
static BaseType_t UDP_DEBUG_INIT(){  
    struct freertos_sockaddr xBindAddress;  
  
    /* Create a UDP socket. */  
    UDP_DEBUG_SOCKET = FreeRTOS_socket( FREERTOS_AF_INET,  
                               FREERTOS_SOCK_DGRAM,  
                               FREERTOS_IPPROTO_UDP );  
  
    /* Check the socket was created successfully. */  
    if( UDP_DEBUG_SOCKET != FREERTOS_INVALID_SOCKET )  
    {  
        /* The socket was created successfully and can now be used to send data 
        using the FreeRTOS_sendto() API function.  Sending to a socket that has 
        not first been bound will result in the socket being automatically bound 
        to a port number.  Use FreeRTOS_bind() to bind the socket to a 
        specific port number.  This example binds the socket to port 9999.  The 
        port number is specified in network byte order, so FreeRTOS_htons() is 
        used. */  
        xBindAddress.sin_port = FreeRTOS_htons( 9999 );  
        if( FreeRTOS_bind( UDP_DEBUG_SOCKET, &xBindAddress, sizeof( &xBindAddress ) ) == 0 )  
        {  
                    /* bind success */  
                    return pdTRUE;  
                }  
                else{  
                    return pdFALSE;  
                }  
        }else{  
            return pdFALSE;  
        }  
}  
  
/* 
Brief: 
this func only for debug printf 
Param: 
msg: the message. 
len: the message length 
Return: 
the length 
*/  
static size_t UDP_SEND_DATA(const char* msg , size_t len){  
    static struct freertos_sockaddr xDestinationAddress;  
    int32_t iReturned;  
      
    /* dest address and port */  
    xDestinationAddress.sin_addr = FreeRTOS_inet_addr_quick( 192, 168, 31, 123 );  
  xDestinationAddress.sin_port = FreeRTOS_htons( 60000 );  
      
    /* Send the buffer with ulFlags set to 0, so the FREERTOS_ZERO_COPY bit 
  is clear. */  
  iReturned = FreeRTOS_sendto(  
                                  /* The socket being send to. */  
                                  UDP_DEBUG_SOCKET,  
                                  /* The data being sent. */  
                                  msg,  
                                  /* The length of the data being sent. */  
                                  len,  
                                  /* ulFlags with the FREERTOS_ZERO_COPY bit clear. */  
                                  0,  
                                  /* Where the data is being sent. */  
                                  &xDestinationAddress,  
                                  /* Not used but should be set as shown. */  
                                  sizeof( xDestinationAddress )  
                             );  
  
    if( iReturned == len )  
    {  
       return len;  
    }     
        else{  
            return 0;  
        }  
}  
  
/* 
    only for debug 
*/  
int lUDPLoggingPrintf( const char *fmt, ... ){  
    va_list ap;  
    char msg[128];  
    int len;  
  
    va_start(ap, fmt);  
    len = vsnprintf(msg, sizeof(msg), fmt, ap);  
    UDP_SEND_DATA(msg, len);  
    va_end(ap);  
    return len;  
}  
/*------------DEBUG ONLY----------------*/ 