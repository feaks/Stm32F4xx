/*
 * Some constants, hardware definitions and comments taken from ST's HAL driver
 * library, COPYRIGHT(c) 2015 STMicroelectronics.
 */

/*
 * FreeRTOS+TCP V2.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"
#include "phyHandling.h"

/* ST includes. */
#include "stm32f4xx.h"
#include "stm32f4x7_eth.h"
#include "bsp_eth.h"


#ifndef	BMSR_LINK_STATUS
	#define BMSR_LINK_STATUS            0x0004UL
#endif

#ifndef	PHY_LS_HIGH_CHECK_TIME_MS
	/* Check if the LinkSStatus in the PHY is still high after 15 seconds of not
	receiving packets. */
	#define PHY_LS_HIGH_CHECK_TIME_MS	15000
#endif

#ifndef	PHY_LS_LOW_CHECK_TIME_MS
	/* Check if the LinkSStatus in the PHY is still low every second. */
	#define PHY_LS_LOW_CHECK_TIME_MS	1000
#endif

/* Interrupt events to process.  Currently only the Rx event is processed
although code for other events is included to allow for possible future
expansion. */
#define EMAC_IF_RX_EVENT        1UL
#define EMAC_IF_TX_EVENT        2UL
#define EMAC_IF_ERR_EVENT       4UL
#define EMAC_IF_ALL_EVENT       ( EMAC_IF_RX_EVENT | EMAC_IF_TX_EVENT | EMAC_IF_ERR_EVENT )

#define ETH_DMA_ALL_INTS \
	( ETH_DMA_IT_NIS |ETH_DMA_IT_AIS | ETH_DMA_IT_ER  |  ETH_DMA_IT_FBE | ETH_DMA_IT_ET  | \
		ETH_DMA_IT_RWT |ETH_DMA_IT_RPS | ETH_DMA_IT_RBU |  ETH_DMA_IT_R   | ETH_DMA_IT_TU  | \
		ETH_DMA_IT_RO  |ETH_DMA_IT_TJT | ETH_DMA_IT_TBU | ETH_DMA_IT_TPS |  ETH_DMA_IT_T )



#define ipFRAGMENT_OFFSET_BIT_MASK		( ( uint16_t ) 0x0fff ) /* The bits in the two byte IP header field that make up the fragment offset value. */

/*
 * Most users will want a PHY that negotiates about
 * the connection properties: speed, dmix and duplex.
 * On some rare cases, you want to select what is being
 * advertised, properties like MDIX and duplex.
 */

#if !defined( ipconfigETHERNET_AN_ENABLE )
	/* Enable auto-negotiation */
	#define ipconfigETHERNET_AN_ENABLE				1
#endif

#if !defined( ipconfigETHERNET_AUTO_CROSS_ENABLE )
	#define ipconfigETHERNET_AUTO_CROSS_ENABLE		1
#endif

#if( ipconfigETHERNET_AN_ENABLE == 0 )
	/*
	 * The following three defines are only used in case there
	 * is no auto-negotiation.
	 */
	#if !defined( ipconfigETHERNET_CROSSED_LINK )
		#define	ipconfigETHERNET_CROSSED_LINK			1
	#endif

	#if !defined( ipconfigETHERNET_USE_100MB )
		#define ipconfigETHERNET_USE_100MB				1
	#endif

	#if !defined( ipconfigETHERNET_USE_FULL_DUPLEX )
		#define ipconfigETHERNET_USE_FULL_DUPLEX		1
	#endif
#endif /* ipconfigETHERNET_AN_ENABLE == 0 */

/* Default the size of the stack used by the EMAC deferred handler task to twice
the size of the stack used by the idle task - but allow this to be overridden in
FreeRTOSConfig.h as configMINIMAL_STACK_SIZE is a user definable constant. */
#ifndef configEMAC_TASK_STACK_SIZE
	#define configEMAC_TASK_STACK_SIZE ( 2 * configMINIMAL_STACK_SIZE )
#endif

/* Two choices must be made: RMII versus MII,
and the index of the PHY in use ( between 0 and 31 ). */
#ifndef ipconfigUSE_RMII
		#define ipconfigUSE_RMII	1
#endif /* ipconfigUSE_RMII */



/*-----------------------------------------------------------*/

/*
 * A deferred interrupt handler task that processes
 */
static void prvEMACHandlerTask( void *pvParameters ); 


/*
 * Force a negotiation with the Switch or Router and wait for LS.
 */
static void prvEthernetUpdateConfig( BaseType_t xForce );

/*
 * See if there is a new packet and forward it to the IP-task.
 */
static BaseType_t prvNetworkInterfaceInput( void );

#if( ipconfigUSE_LLMNR != 0 )
	/*
	 * For LLMNR, an extra MAC-address must be configured to
	 * be able to receive the multicast messages.
	 */
	static void prvMACAddressConfig(uint32_t ulIndex, uint8_t *Addr);
#endif

/*
 * Check if a given packet should be accepted.
 */
static BaseType_t xMayAcceptPacket( uint8_t *pcBuffer );

/*
 * Initialise the TX descriptors.
 */
static void prvDMATxDescListInit( void );

/*
 * Initialise the RX descriptors.
 */
static void prvDMARxDescListInit( void );


/* After packets have been sent, the network
buffers will be released. */
static void vClearTXBuffers( void );

/*-----------------------------------------------------------*/
//typedef struct _PhyProperties_t
//{
//	uint8_t speed;
//	uint8_t mdix;
//	uint8_t duplex;
//	uint8_t spare;
//} PhyProperties_t;

/* Bit map of outstanding ETH interrupt events for processing.  Currently only
the Rx interrupt is handled, although code is included for other events to
enable future expansion. */
static volatile uint32_t ulISREvents;

#if( ipconfigUSE_LLMNR == 1 )
	static const uint8_t xLLMNR_MACAddress[] = { 0x01, 0x00, 0x5E, 0x00, 0x00, 0xFC };
#endif
	
/* Value to be written into the 'Basic mode Control Register'. */
static uint32_t ulBCRvalue;

/* Value to be written into the 'Advertisement Control Register'. */
static uint32_t ulACRValue;

	
/* A copy of PHY register 1: 'PHY_REG_01_BMSR' */
static uint32_t ulPHYLinkStatus = 0;
	
static EthernetPhy_t xPhyObject;

/* Ethernet handle. */
//static ETH_HandleTypeDef xETH;

/* xTXDescriptorSemaphore is a counting semaphore with
a maximum count of ETH_TXBUFNB, which is the number of
DMA TX descriptors. */
static SemaphoreHandle_t xTXDescriptorSemaphore = NULL;

/*
 * Note: it is adviced to define both
 *
 *     #define  ipconfigZERO_COPY_RX_DRIVER   1
 *     #define  ipconfigZERO_COPY_TX_DRIVER   1
 *
 * The method using memcpy is slower and probaly uses more RAM memory.
 * The possibility is left in the code just for comparison.
 *
 * It is adviced to define ETH_TXBUFNB at least 4. Note that no
 * TX buffers are allocated in a zero-copy driver.
 */
/* MAC buffers: ---------------------------------------------------------*/

/* Put the DMA descriptors in '.first_data'.
This is important for STM32F7, which has an L1 data cache.
The first 64KB of the SRAM is not cached. */


#define ETH_DMATXDESC_OWN  ETH_DMATxDesc_OWN
#define ETH_DMADescTypeDef ETH_DMADESCTypeDef

/* Ethernet Rx MA Descriptor */
__attribute__ ((aligned (32)))
__attribute__ ((section(".first_data")))
	ETH_DMADescTypeDef  DMARxDscrTab[ ETH_RXBUFNB ];

#if( ipconfigZERO_COPY_RX_DRIVER == 0 )
	/* Ethernet Receive Buffer */
	__align(4)  uint8_t Rx_Buff[ ETH_RXBUFNB ][ ETH_RX_BUF_SIZE ];
#endif

/* Ethernet Tx DMA Descriptor */
__attribute__ ((aligned (32)))
__attribute__ ((section(".first_data")))
	ETH_DMADescTypeDef  DMATxDscrTab[ ETH_TXBUFNB ];

#if( ipconfigZERO_COPY_TX_DRIVER == 0 )
	/* Ethernet Transmit Buffer */
	__align(4)  uint8_t Tx_Buff[ ETH_TXBUFNB ][ ETH_TX_BUF_SIZE ];
#endif





//#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
	/* DMATxDescToClear points to the next TX DMA descriptor
	that must be cleared by vClearTXBuffers(). */
	static __IO ETH_DMADescTypeDef  *DMATxDescToClear;
//#endif




/* Global pointers on Tx and Rx descriptor used to track transmit and receive descriptors */
extern __IO ETH_DMADESCTypeDef  *DMATxDescToSet;
extern __IO ETH_DMADESCTypeDef  *DMARxDescToGet;


/* Structure used to hold the last received packet descriptors info */
extern ETH_DMA_Rx_Frame_infos RX_Frame_Descriptor;
extern __IO ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;
extern __IO uint32_t Frame_Rx_index;


/* ucMACAddress as it appears in main.c */
extern const uint8_t ucMACAddress[ 6 ];

/* Holds the handle of the task used as a deferred interrupt processor.  The
handle is used so direct notifications can be sent to the task for all EMAC/DMA
related interrupts. */
static TaskHandle_t xEMACTaskHandle = NULL;

/* For local use only: describe the PHY's properties: */
const PhyProperties_t xPHYProperties =
{
	#if( ipconfigETHERNET_AN_ENABLE != 0 )
		.ucSpeed = PHY_SPEED_AUTO,
		.ucDuplex = PHY_DUPLEX_AUTO,
	#else
		#if( ipconfigETHERNET_USE_100MB != 0 )
			.ucSpeed = PHY_SPEED_100,
		#else
			.ucSpeed = PHY_SPEED_10,
		#endif

		#if( ipconfigETHERNET_USE_FULL_DUPLEX != 0 )
			.duplex = PHY_DUPLEX_FULL,
		#else
			.duplex = PHY_DUPLEX_HALF,
		#endif
	#endif

	#if( ipconfigETHERNET_AN_ENABLE != 0 ) && ( ipconfigETHERNET_AUTO_CROSS_ENABLE != 0 )
		.ucMDI_X = PHY_MDIX_AUTO,
	#elif( ipconfigETHERNET_CROSSED_LINK != 0 )
		.ucMDI_X = PHY_MDIX_CROSSED,
	#else
		.ucMDI_X = PHY_MDIX_DIRECT,
	#endif
};
BaseType_t xSTM32_PhyRead( BaseType_t xAddress, BaseType_t xRegister, uint32_t *pulValue );
void phy_test();
/*-----------------------------------------------------------*/

void ETH_RxCpltCallback( void)
{
BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* Ethernet RX-Complete callback function, elsewhere declared as weak. */
    ulISREvents |= EMAC_IF_RX_EVENT;
	/* Wakeup the prvEMACHandlerTask. */
	if( xEMACTaskHandle != NULL )
	{
		vTaskNotifyGiveFromISR( xEMACTaskHandle, &xHigherPriorityTaskWoken );
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}
/*-----------------------------------------------------------*/

void ETH_TxCpltCallback( void )
{
BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* This call-back is only useful in case packets are being sent
	zero-copy.  Once they're sent, the buffers will be released
	by the function vClearTXBuffers(). */
	ulISREvents |= EMAC_IF_TX_EVENT;
	/* Wakeup the prvEMACHandlerTask. */
	if( xEMACTaskHandle != NULL )
	{
		vTaskNotifyGiveFromISR( xEMACTaskHandle, &xHigherPriorityTaskWoken );
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}
/*-----------------------------------------------------------*/

static void vClearTXBuffers()
{
__IO ETH_DMADescTypeDef  *txLastDescriptor = DMATxDescToSet;
size_t uxCount = ( ( UBaseType_t ) ETH_TXBUFNB ) - uxSemaphoreGetCount( xTXDescriptorSemaphore );
#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
	NetworkBufferDescriptor_t *pxNetworkBuffer;
	uint8_t *ucPayLoad;
#endif

	/* This function is called after a TX-completion interrupt.
	It will release each Network Buffer used in xNetworkInterfaceOutput().
	'uxCount' represents the number of descriptors given to DMA for transmission.
	After sending a packet, the DMA will clear the 'ETH_DMATXDESC_OWN' bit. */
	while( ( uxCount > 0 ) && ( ( DMATxDescToClear->Status & ETH_DMATXDESC_OWN ) == 0 ) )
	{
		if( ( DMATxDescToClear == txLastDescriptor ) && ( uxCount != ETH_TXBUFNB ) )
		{
			break;
		}
		#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
		{
			ucPayLoad = ( uint8_t * )DMATxDescToClear->Buffer1Addr;

			if( ucPayLoad != NULL )
			{
				pxNetworkBuffer = pxPacketBuffer_to_NetworkBuffer( ucPayLoad );
				if( pxNetworkBuffer != NULL )
				{
					vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer ) ;
				}
				DMATxDescToClear->Buffer1Addr = ( uint32_t )0u;
			}
		}
		#endif /* ipconfigZERO_COPY_TX_DRIVER */

		DMATxDescToClear = ( ETH_DMADescTypeDef * )( DMATxDescToClear->Buffer2NextDescAddr );

		uxCount--;
		/* Tell the counting semaphore that one more TX descriptor is available. */
		xSemaphoreGive( xTXDescriptorSemaphore );
	}
}
/*-----------------------------------------------------------*/
typedef enum
{
  HAL_OK       = 0x00,
  HAL_ERROR    = 0x01,
  HAL_BUSY     = 0x02,
  HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;

BaseType_t xNetworkInterfaceInitialise( void )
{
HAL_StatusTypeDef hal_eth_init_status;
	BaseType_t xResult;

	if( xEMACTaskHandle == NULL )
	{
		if( xTXDescriptorSemaphore == NULL )
		{
			xTXDescriptorSemaphore = xSemaphoreCreateCounting( ( UBaseType_t ) ETH_TXBUFNB, ( UBaseType_t ) ETH_TXBUFNB );
			configASSERT( xTXDescriptorSemaphore );
		}

		/* Initialise ETH */
		LAN8720_Init();

		/* Only for inspection by debugger. */
		( void ) hal_eth_init_status;
		
	
		/* Set the TxDesc and RxDesc pointers. */
		DMATxDescToSet = DMATxDscrTab;
		DMARxDescToGet = DMARxDscrTab;

		/* Make sure that all unused fields are cleared. */
		memset( &DMATxDscrTab, '\0', sizeof( DMATxDscrTab ) );
		memset( &DMARxDscrTab, '\0', sizeof( DMARxDscrTab ) );

		/* Initialize Tx Descriptors list: Chain Mode */
		DMATxDescToClear = DMATxDscrTab;

		/* Initialise TX-descriptors. */
		prvDMATxDescListInit();

		/* Initialise RX-descriptors. */
		prvDMARxDescListInit();

		#if( ipconfigUSE_LLMNR != 0 )
		{
			/* Program the LLMNR address at index 1. */
			prvMACAddressConfig( ETH_MAC_Address1, ( uint8_t *) xLLMNR_MACAddress );
		}
		#endif

		/* Force a negotiation with the Switch or Router and wait for LS. */
		prvEthernetUpdateConfig( pdTRUE );

		/* The deferred interrupt handler task is created at the highest
		possible priority to ensure the interrupt handler can return directly
		to it.  The task's handle is stored in xEMACTaskHandle so interrupts can
		notify the task when there is something to process. */
		xTaskCreate( prvEMACHandlerTask, "EMAC", configEMAC_TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &xEMACTaskHandle );
	} /* if( xEMACTaskHandle == NULL ) */

	
//	xGetPhyLinkStatus();
	//检查PHY连接状态
	xSTM32_PhyRead( 0x00,0x01, &ulPHYLinkStatus);
	xPhyObject.ulLinkStatusMask = ulPHYLinkStatus;
	return ( ulPHYLinkStatus & BMSR_LINK_STATUS ) != 0;
	
//	if( xPhyObject.ulLinkStatusMask != 0 )
//	{
//		ETH->DMAIER |= ETH_DMA_ALL_INTS;
//		xResult = pdPASS;
//		FreeRTOS_printf( ( "Link Status is high\n" ) ) ;
//	}
//	else
//	{
//		/* For now pdFAIL will be returned. But prvEMACHandlerTask() is running
//		and it will keep on checking the PHY and set 'ulLinkStatusMask' when necessary. */
//		xResult = pdFAIL;
//		FreeRTOS_printf( ( "Link Status still low\n" ) ) ;
//	}
//	/* When returning non-zero, the stack will become active and
//    start DHCP (in configured) */
//	return xResult;
}
/*-----------------------------------------------------------*/

static void prvDMATxDescListInit()
{
ETH_DMADescTypeDef *pxDMADescriptor;
BaseType_t xIndex;

	/* Get the pointer on the first member of the descriptor list */
	pxDMADescriptor = DMATxDscrTab;

	/* Fill each DMA descriptor with the right values */
	for( xIndex = 0; xIndex < ETH_TXBUFNB; xIndex++, pxDMADescriptor++ )
	{
		/* Set Second Address Chained bit */
		pxDMADescriptor->Status = ETH_DMATxDesc_TCH;

		#if( ipconfigZERO_COPY_TX_DRIVER == 0 )
		{
			/* Set Buffer1 address pointer */
			pxDMADescriptor->Buffer1Addr = ( uint32_t )( Tx_Buff[ xIndex ] );
		}
		#endif

		#ifdef CHECKSUM_BY_HARDWARE 
		
			/* Set the DMA Tx descriptors checksum insertion for TCP, UDP, and ICMP */
			pxDMADescriptor->Status |= ETH_DMATxDesc_ChecksumTCPUDPICMPFull;
		
		#endif
		/* Initialize the next descriptor with the Next Descriptor Polling Enable */
		if( xIndex < ETH_TXBUFNB - 1 )
		{
			/* Set next descriptor address register with next descriptor base address */
			pxDMADescriptor->Buffer2NextDescAddr = ( uint32_t ) ( pxDMADescriptor + 1 );
		}
		else
		{
			/* For last descriptor, set next descriptor address register equal to the first descriptor base address */
			pxDMADescriptor->Buffer2NextDescAddr = ( uint32_t ) DMATxDscrTab;
		}
	}

	/* Set Transmit Descriptor List Address Register */
	ETH->DMATDLAR = ( uint32_t ) DMATxDscrTab;
	
}
/*-----------------------------------------------------------*/

static void prvDMARxDescListInit()
{
ETH_DMADescTypeDef *pxDMADescriptor;
BaseType_t xIndex;
	/*
	 * RX-descriptors.
	 */

	/* Get the pointer on the first member of the descriptor list */
	pxDMADescriptor = DMARxDscrTab;

	/* Fill each DMA descriptor with the right values */
	for( xIndex = 0; xIndex < ETH_RXBUFNB; xIndex++, pxDMADescriptor++ )
	{

		/* Set Buffer1 size and Second Address Chained bit */
		pxDMADescriptor->ControlBufferSize = ETH_DMARxDesc_RCH | (uint32_t)ETH_RX_BUF_SIZE;  

		#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
		{
		/* Set Buffer1 address pointer */
		NetworkBufferDescriptor_t *pxBuffer;

			pxBuffer = pxGetNetworkBufferWithDescriptor( ETH_RX_BUF_SIZE, 100ul );
			/* If the assert below fails, make sure that there are at least 'ETH_RXBUFNB'
			Network Buffers available during start-up ( ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ) */
			configASSERT( pxBuffer != NULL );
			if( pxBuffer != NULL )
			{
				pxDMADescriptor->Buffer1Addr = (uint32_t)pxBuffer->pucEthernetBuffer;
				pxDMADescriptor->Status = ETH_DMARxDesc_OWN;
			}
		}
		#else
		{
			/* Set Buffer1 address pointer */
			pxDMADescriptor->Buffer1Addr = ( uint32_t )( Rx_Buff[ xIndex ] );
			/* Set Own bit of the Rx descriptor Status */
			pxDMADescriptor->Status = ETH_DMARxDesc_OWN;
		}
		#endif

		/* Initialize the next descriptor with the Next Descriptor Polling Enable */
		if( xIndex < ETH_RXBUFNB - 1 )
		{
			/* Set next descriptor address register with next descriptor base address */
			pxDMADescriptor->Buffer2NextDescAddr = ( uint32_t )( pxDMADescriptor + 1 );
		}
		else
		{
			/* For last descriptor, set next descriptor address register equal to the first descriptor base address */
			pxDMADescriptor->Buffer2NextDescAddr = ( uint32_t ) DMARxDscrTab;
		}

	}
	/* Set Receive Descriptor List Address Register */
	ETH->DMARDLAR = ( uint32_t ) DMARxDscrTab;
	DMA_RX_FRAME_infos = &RX_Frame_Descriptor;
}
/*-----------------------------------------------------------*/

static void prvMACAddressConfig( uint32_t ulIndex, uint8_t *Addr)
{
uint32_t ulTempReg;

	/* Calculate the selected MAC address high register. */
	ulTempReg = 0x80000000ul | ( ( uint32_t ) Addr[ 5 ] << 8 ) | ( uint32_t ) Addr[ 4 ];

	/* Load the selected MAC address high register. */
	( *(__IO uint32_t *)( ( uint32_t ) ( ETH_MAC_ADDR_HBASE + ulIndex ) ) ) = ulTempReg;

	/* Calculate the selected MAC address low register. */
	ulTempReg = ( ( uint32_t ) Addr[ 3 ] << 24 ) | ( ( uint32_t ) Addr[ 2 ] << 16 ) | ( ( uint32_t ) Addr[ 1 ] << 8 ) | Addr[ 0 ];

	/* Load the selected MAC address low register */
	( *(__IO uint32_t *) ( ( uint32_t ) ( ETH_MAC_ADDR_LBASE + ulIndex ) ) ) = ulTempReg;
	
}
/*-----------------------------------------------------------*/

BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxDescriptor, BaseType_t bReleaseAfterSend )
{
	BaseType_t xReturn = pdFAIL;
uint32_t ulTransmitSize = 0;
__IO ETH_DMADescTypeDef *pxDmaTxDesc;
/* Do not wait too long for a free TX DMA buffer. */
const TickType_t xBlockTimeTicks = pdMS_TO_TICKS( 50u );

	#if( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM != 0 )
	{
	ProtocolPacket_t *pxPacket;

		#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
		{
			configASSERT( bReleaseAfterSend != 0 );
		}
		#endif /* ipconfigZERO_COPY_RX_DRIVER */

		/* If the peripheral must calculate the checksum, it wants
		the protocol checksum to have a value of zero. */
		pxPacket = ( ProtocolPacket_t * ) ( pxDescriptor->pucEthernetBuffer );

		if( pxPacket->xICMPPacket.xIPHeader.ucProtocol == ipPROTOCOL_ICMP )
		{
			pxPacket->xICMPPacket.xICMPHeader.usChecksum = ( uint16_t )0u;
		}
	}
	#endif

	/* Open a do {} while ( 0 ) loop to be able to call break. */
	do
	{
		if( xPhyObject.ulLinkStatusMask != 0 )
		{
			if( xSemaphoreTake( xTXDescriptorSemaphore, xBlockTimeTicks ) != pdPASS )
			{
				/* Time-out waiting for a free TX descriptor. */
				break;
			}

			/* This function does the actual transmission of the packet. The packet is
			contained in 'pxDescriptor' that is passed to the function. */
			pxDmaTxDesc = DMATxDescToSet;

			/* Is this buffer available? */
			configASSERT ( ( pxDmaTxDesc->Status & ETH_DMATXDESC_OWN ) == 0 );

			{
				/* Is this buffer available? */
				/* Get bytes in current buffer. */
				ulTransmitSize = pxDescriptor->xDataLength;

				if( ulTransmitSize > ETH_TX_BUF_SIZE )
				{
					ulTransmitSize = ETH_TX_BUF_SIZE;
				}

				#if( ipconfigZERO_COPY_TX_DRIVER == 0 )
				{
					/* Copy the bytes. */
					memcpy( ( void * ) pxDmaTxDesc->Buffer1Addr, pxDescriptor->pucEthernetBuffer, ulTransmitSize );
				}
				#else
				{
					/* Move the buffer. */
					pxDmaTxDesc->Buffer1Addr = ( uint32_t )pxDescriptor->pucEthernetBuffer;
					/* The Network Buffer has been passed to DMA, no need to release it. */
					bReleaseAfterSend = pdFALSE_UNSIGNED;
				}
				#endif /* ipconfigZERO_COPY_TX_DRIVER */

				/* Ask to set the IPv4 checksum.
				Also need an Interrupt on Completion so that 'vClearTXBuffers()' will be called.. */
				pxDmaTxDesc->Status |= ETH_DMATxDesc_CIC_TCPUDPICMP_Full | ETH_DMATxDesc_IC;

				/* Prepare transmit descriptors to give to DMA. */

				/* Set LAST and FIRST segment */
				pxDmaTxDesc->Status |= ETH_DMATxDesc_FS | ETH_DMATxDesc_LS;
				/* Set frame size */
				pxDmaTxDesc->ControlBufferSize = ( ulTransmitSize & ETH_DMATxDesc_TBS1 );
				/* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
				pxDmaTxDesc->Status |= ETH_DMATXDESC_OWN;

				/* Point to next descriptor */
				DMATxDescToSet = ( ETH_DMADescTypeDef * ) ( DMATxDescToSet->Buffer2NextDescAddr );
				/* Ensure completion of memory access */
				__DSB();
				/* Resume DMA transmission*/
				ETH->DMATPDR = 0;
				iptraceNETWORK_INTERFACE_TRANSMIT();
				xReturn = pdPASS;
			}
		}
		else
		{
			/* The PHY has no Link Status, packet shall be dropped. */
		}
	} while( 0 );
	/* The buffer has been sent so can be released. */
	if( bReleaseAfterSend != pdFALSE )
	{
		vReleaseNetworkBufferAndDescriptor( pxDescriptor );
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static BaseType_t xMayAcceptPacket( uint8_t *pcBuffer )
{
const ProtocolPacket_t *pxProtPacket = ( const ProtocolPacket_t * )pcBuffer;

	switch( pxProtPacket->xTCPPacket.xEthernetHeader.usFrameType )
	{
	case ipARP_FRAME_TYPE:
		/* Check it later. */
		return pdTRUE;
	case ipIPv4_FRAME_TYPE:
		/* Check it here. */
		break;
	default:
		/* Refuse the packet. */
		return pdFALSE;
	}

	#if( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 1 )
	{
		const IPHeader_t *pxIPHeader = &(pxProtPacket->xTCPPacket.xIPHeader);
		uint32_t ulDestinationIPAddress;

		/* Ensure that the incoming packet is not fragmented (only outgoing packets
		 * can be fragmented) as these are the only handled IP frames currently. */
		if( ( pxIPHeader->usFragmentOffset & FreeRTOS_ntohs( ipFRAGMENT_OFFSET_BIT_MASK ) ) != 0U )
		{
			return pdFALSE;
		}
		/* HT: Might want to make the following configurable because
		 * most IP messages have a standard length of 20 bytes */

		/* 0x45 means: IPv4 with an IP header of 5 x 4 = 20 bytes
		 * 0x47 means: IPv4 with an IP header of 7 x 4 = 28 bytes */
		if( pxIPHeader->ucVersionHeaderLength < 0x45 || pxIPHeader->ucVersionHeaderLength > 0x4F )
		{
			return pdFALSE;
		}

		ulDestinationIPAddress = pxIPHeader->ulDestinationIPAddress;
		/* Is the packet for this node? */
		if( ( ulDestinationIPAddress != *ipLOCAL_IP_ADDRESS_POINTER ) &&
			/* Is it a broadcast address x.x.x.255 ? */
			( ( FreeRTOS_ntohl( ulDestinationIPAddress ) & 0xff ) != 0xff ) &&
		#if( ipconfigUSE_LLMNR == 1 )
			( ulDestinationIPAddress != ipLLMNR_IP_ADDR ) &&
		#endif
			( *ipLOCAL_IP_ADDRESS_POINTER != 0 ) ) {
			FreeRTOS_printf( ( "Drop IP %lxip\n", FreeRTOS_ntohl( ulDestinationIPAddress ) ) );
			return pdFALSE;
		}

		if( pxIPHeader->ucProtocol == ipPROTOCOL_UDP )
		{
			uint16_t port = pxProtPacket->xUDPPacket.xUDPHeader.usDestinationPort;

			if( ( xPortHasUDPSocket( port ) == pdFALSE )
			#if ipconfigUSE_LLMNR == 1
				&& ( port != FreeRTOS_ntohs( ipLLMNR_PORT ) )
			#endif
			#if ipconfigUSE_NBNS == 1
				&& ( port != FreeRTOS_ntohs( ipNBNS_PORT ) )
			#endif
			#if ipconfigUSE_DNS == 1
				&& ( pxProtPacket->xUDPPacket.xUDPHeader.usSourcePort != FreeRTOS_ntohs( ipDNS_PORT ) )
			#endif
				) {
				/* Drop this packet, not for this device. */
				return pdFALSE;
			}
		}
	}
	#endif	/* ipconfigETHERNET_DRIVER_FILTERS_PACKETS */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvNetworkInterfaceInput( void )
{
NetworkBufferDescriptor_t *pxCurDescriptor;
NetworkBufferDescriptor_t *pxNewDescriptor = NULL;
BaseType_t xReceivedLength, xAccepted;
__IO ETH_DMADescTypeDef *pxDMARxDescriptor;
xIPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };
const TickType_t xDescriptorWaitTime = pdMS_TO_TICKS( 250 );
uint8_t *pucBuffer;

	pxDMARxDescriptor = DMARxDescToGet;

	if( ( pxDMARxDescriptor->Status & ETH_DMATxDesc_OWN) == 0 )
	{
		/* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
		xReceivedLength = ( ( pxDMARxDescriptor->Status & ETH_DMARxDesc_FL ) >> ETH_DMARxDesc_FrameLengthShift ) - 4;

		pucBuffer = (uint8_t *) pxDMARxDescriptor->Buffer1Addr;

		/* Update the ETHERNET DMA global Rx descriptor with next Rx descriptor */
		/* Chained Mode */    
		/* Selects the next DMA Rx descriptor list for next buffer to read */ 
		DMARxDescToGet = ( ETH_DMADescTypeDef* )pxDMARxDescriptor->Buffer2NextDescAddr;
	}
	else
	{
		xReceivedLength = 0;
	}
	/* Obtain the size of the packet and put it into the "usReceivedLength" variable. */

	/* get received frame */
	if( xReceivedLength > 0ul )
	{
		/* In order to make the code easier and faster, only packets in a single buffer
		will be accepted.  This can be done by making the buffers large enough to
		hold a complete Ethernet packet (1536 bytes).
		Therefore, two sanity checks: */
		configASSERT( xReceivedLength <= ETH_RX_BUF_SIZE );

		if( ( pxDMARxDescriptor->Status & ( ETH_DMARxDesc_CE | ETH_DMARxDesc_IPV4HCE | ETH_DMARxDesc_FT ) ) != ETH_DMARxDesc_FT )
		{
			/* Not an Ethernet frame-type or a checmsum error. */
			xAccepted = pdFALSE;
		}
		else
		{
			/* See if this packet must be handled. */
			xAccepted = xMayAcceptPacket( pucBuffer );
		}

		if( xAccepted != pdFALSE )
		{
			/* The packet wil be accepted, but check first if a new Network Buffer can
			be obtained. If not, the packet will still be dropped. */
			pxNewDescriptor = pxGetNetworkBufferWithDescriptor( ETH_RX_BUF_SIZE, xDescriptorWaitTime );

			if( pxNewDescriptor == NULL )
			{
				/* A new descriptor can not be allocated now. This packet will be dropped. */
				xAccepted = pdFALSE;
			}
		}
		#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
		{
			/* Find out which Network Buffer was originally passed to the descriptor. */
			pxCurDescriptor = pxPacketBuffer_to_NetworkBuffer( pucBuffer );
			configASSERT( pxCurDescriptor != NULL );
		}
		#else
		{
			/* In this mode, the two descriptors are the same. */
			pxCurDescriptor = pxNewDescriptor;
			if( pxNewDescriptor != NULL )
			{
				/* The packet is acepted and a new Network Buffer was created,
				copy data to the Network Bufffer. */
				memcpy( pxNewDescriptor->pucEthernetBuffer, pucBuffer, xReceivedLength );
//				memcpy( pxNewDescriptor->pucEthernetBuffer, ( uint8_t * )(DMA_RX_FRAME_infos->FS_Rx_Desc)->Buffer1Addr, xReceivedLength);
			}
		}
		#endif

		if( xAccepted != pdFALSE )
		{
			pxCurDescriptor->xDataLength = xReceivedLength;
			xRxEvent.pvData = ( void * ) pxCurDescriptor;

			/* Pass the data to the TCP/IP task for processing. */
			if( xSendEventStructToIPTask( &xRxEvent, xDescriptorWaitTime ) == pdFALSE )
			{
				/* Could not send the descriptor into the TCP/IP stack, it
				must be released. */
				vReleaseNetworkBufferAndDescriptor( pxCurDescriptor );
				iptraceETHERNET_RX_EVENT_LOST();
			}
			else
			{
				iptraceNETWORK_INTERFACE_RECEIVE();
			}
		}

		/* Release descriptors to DMA */
		#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
		{
			/* Set Buffer1 address pointer */
			if( pxNewDescriptor != NULL )
			{
				pxDMARxDescriptor->Buffer1Addr = (uint32_t)pxNewDescriptor->pucEthernetBuffer;
			}
			else
			{
				/* The packet was dropped and the same Network
				Buffer will be used to receive a new packet. */
			}
		}
		#endif /* ipconfigZERO_COPY_RX_DRIVER */

		/* Set Buffer1 size and Second Address Chained bit */
		pxDMARxDescriptor->ControlBufferSize = ETH_DMARxDesc_RCH | (uint32_t)ETH_RX_BUF_SIZE;  
		pxDMARxDescriptor->Status = ETH_DMARxDesc_OWN;

		/* Ensure completion of memory access */
		__DSB();
		/* When Rx Buffer unavailable flag is set clear it and resume
		reception. */
		if( ( ETH->DMASR & ETH_DMASR_RBUS ) != 0 )
		{
			/* Clear RBUS ETHERNET DMA flag. */
			ETH->DMASR = ETH_DMASR_RBUS;

			/* Resume DMA reception. */
			ETH->DMARPDR = 0;
		}
	}

	return ( xReceivedLength > 0 );
}
/*-----------------------------------------------------------*/

BaseType_t ETH_PhyAddress;
BaseType_t xSTM32_PhyRead( BaseType_t xAddress, BaseType_t xRegister, uint32_t *pulValue )
{
uint16_t usPrevAddress = ETH_PhyAddress;
BaseType_t xResult;
HAL_StatusTypeDef xHALResult;

	ETH_PhyAddress = xAddress;
	*pulValue = ETH_ReadPHYRegister(xAddress,( uint16_t )xRegister );
	ETH_PhyAddress = usPrevAddress;

	if( *pulValue == ETH_ERROR )
	{
		xResult = 0;
	}
	else
	{
		xResult = -1;
	}
	return xResult;
}
/*-----------------------------------------------------------*/

BaseType_t xSTM32_PhyWrite( BaseType_t xAddress, BaseType_t xRegister, uint32_t ulValue )
{
uint16_t usPrevAddress = ETH_PhyAddress;
BaseType_t xResult;
HAL_StatusTypeDef xHALResult;

	ETH_PhyAddress = xAddress;
	xHALResult = ETH_WritePHYRegister(xAddress,( uint16_t )xRegister, ulValue );
	ETH_PhyAddress = usPrevAddress;

	if( xHALResult == ETH_SUCCESS )
	{
		xResult = 0;
	}
	else
	{
		xResult = -1;
	}
	return xResult;
}
/*-----------------------------------------------------------*/

void phy_test()
{
BaseType_t xPhyCount;
BaseType_t xPhyIndex;

	vPhyInitialise( &xPhyObject, xSTM32_PhyRead, xSTM32_PhyWrite );
	xPhyCount = xPhyDiscover( &xPhyObject );
	FreeRTOS_printf(  ("PHY count %ld\n", xPhyCount)  );
	for( xPhyIndex = 0; xPhyIndex < xPhyCount; xPhyIndex++ )
	{
		FreeRTOS_printf(  ("PHY[%d] at address %d ( 0x%08X )\n",
			xPhyIndex,
			xPhyObject.ucPhyIndexes[ xPhyIndex ],
			xPhyObject.ulPhyIDs[ xPhyIndex ] ) );

	}
	
}

void vMACBProbePhy( void )
{
	vPhyInitialise( &xPhyObject, xSTM32_PhyRead, xSTM32_PhyWrite );
	xPhyDiscover( &xPhyObject );
	xPhyConfigure( &xPhyObject, &xPHYProperties );
}
/*-----------------------------------------------------------*/
uint32_t             AutoNegotiation = ETH_AutoNegotiation_Enable;
uint32_t             DuplexMode = ETH_Mode_FullDuplex;
uint32_t             Speed = ETH_Speed_100M;

static void prvEthernetUpdateConfig( BaseType_t xForce )
{
	FreeRTOS_printf( ( "prvEthernetUpdateConfig: LS mask %02lX Force %d\n",\
		xPhyObject.ulLinkStatusMask,
		( int )xForce ) );

	if( ( xForce != pdFALSE ) || ( xPhyObject.ulLinkStatusMask != 0 ) )
	{
		/* Restart the auto-negotiation. */
		if( AutoNegotiation != ETH_AutoNegotiation_Disable )
		{
			xPhyStartAutoNegotiation( &xPhyObject, xPhyGetMask( &xPhyObject ) );

			/* Configure the MAC with the Duplex Mode fixed by the
			auto-negotiation process. */
			if( xPhyObject.xPhyProperties.ucDuplex == PHY_DUPLEX_FULL )
			{
				DuplexMode = ETH_Mode_FullDuplex;
			}
			else
			{
				DuplexMode = ETH_Mode_HalfDuplex;
			}

			/* Configure the MAC with the speed fixed by the
			auto-negotiation process. */
			if( xPhyObject.xPhyProperties.ucSpeed == PHY_SPEED_10 )
			{
				Speed = ETH_Speed_10M;
			}
			else
			{
				Speed = ETH_Speed_100M;
			}
		}
		else /* AutoNegotiation Disable */
		{
			/* Check parameters */
			assert_param( IS_ETH_SPEED( Speed ) );
			assert_param( IS_ETH_DUPLEX_MODE( DuplexMode ) );

			if( DuplexMode == ETH_Mode_FullDuplex )
			{
				xPhyObject.xPhyPreferences.ucDuplex = PHY_DUPLEX_HALF;
			}
			else
			{
				xPhyObject.xPhyPreferences.ucDuplex = PHY_DUPLEX_FULL;
			}

			if( Speed == ETH_Speed_10M )
			{
				xPhyObject.xPhyPreferences.ucSpeed = PHY_SPEED_10;
			}
			else
			{
				xPhyObject.xPhyPreferences.ucSpeed = PHY_SPEED_100;
			}

			xPhyObject.xPhyPreferences.ucMDI_X = PHY_MDIX_AUTO;

			/* Use predefined (fixed) configuration. */
			xPhyFixedValue( &xPhyObject, xPhyGetMask( &xPhyObject ) );
		}

		/* ETHERNET MAC Re-Configuration */
		ETH_ConfigMAC(Speed,DuplexMode);
		
		/* Restart MAC interface */
		ETH_Start( );
	}
	else
	{
		/* Stop MAC interface */
		ETH_Stop( );
	}
}
/*-----------------------------------------------------------*/

BaseType_t xGetPhyLinkStatus( void )
{
BaseType_t xReturn;

	if( xPhyObject.ulLinkStatusMask != 0 )
	{
		xReturn = pdPASS;
	}
	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

/* Uncomment this in case BufferAllocation_1.c is used. */

/*
#define niBUFFER_1_PACKET_SIZE		1536

static __attribute__ ((section(".first_data"))) uint8_t ucNetworkPackets[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS * niBUFFER_1_PACKET_SIZE ] __attribute__ ( ( aligned( 32 ) ) );

void vNetworkInterfaceAllocateRAMToBuffers( NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] )
{

uint8_t *ucRAMBuffer = ucNetworkPackets;
uint32_t ul;

	for( ul = 0; ul < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; ul++ )
	{
		pxNetworkBuffers[ ul ].pucEthernetBuffer = ucRAMBuffer + ipBUFFER_PADDING;
		*( ( unsigned * ) ucRAMBuffer ) = ( unsigned ) ( &( pxNetworkBuffers[ ul ] ) );
		ucRAMBuffer += niBUFFER_1_PACKET_SIZE;
	}
}
*/
/*-----------------------------------------------------------*/

static void prvEMACHandlerTask( void *pvParameters )
{
TimeOut_t xPhyTime;
UBaseType_t uxLastMinBufferCount = 0;
#if( ipconfigCHECK_IP_QUEUE_SPACE != 0 )
UBaseType_t uxLastMinQueueSpace = 0;
#endif
	TickType_t xPhyRemTime;
UBaseType_t uxCurrentCount;
BaseType_t xResult;
	uint32_t xStatus;
const TickType_t ulMaxBlockTime = pdMS_TO_TICKS( 100UL );

	/* Remove compiler warnings about unused parameters. */
	( void ) pvParameters;
	vTaskSetTimeOutState( &xPhyTime );
	xPhyRemTime = pdMS_TO_TICKS( PHY_LS_LOW_CHECK_TIME_MS );
	for( ;; )
	{
		xResult = 0;
		uxCurrentCount = uxGetMinimumFreeNetworkBuffers();
		if( uxLastMinBufferCount != uxCurrentCount )
		{
			/* The logging produced below may be helpful
			while tuning +TCP: see how many buffers are in use. */
			uxLastMinBufferCount = uxCurrentCount;
			FreeRTOS_printf(  ("Network buffers: %lu lowest %lu\n",
			uxGetNumberOfFreeNetworkBuffers(), uxCurrentCount ) );
		}

		if( xTXDescriptorSemaphore != NULL )
		{
		static UBaseType_t uxLowestSemCount = ( UBaseType_t ) ETH_TXBUFNB - 1;

			uxCurrentCount = uxSemaphoreGetCount( xTXDescriptorSemaphore );
			if( uxLowestSemCount > uxCurrentCount )
			{
				uxLowestSemCount = uxCurrentCount;
				FreeRTOS_printf(  ("TX DMA buffers: lowest %lu\n", uxLowestSemCount)  );
			}

		}

		#if( ipconfigCHECK_IP_QUEUE_SPACE != 0 )
		{
			uxCurrentCount = uxGetMinimumIPQueueSpace();
			if( uxLastMinQueueSpace != uxCurrentCount )
			{
				/* The logging produced below may be helpful
				while tuning +TCP: see how many buffers are in use. */
				uxLastMinQueueSpace = uxCurrentCount;
				FreeRTOS_printf( ( "Queue space: lowest %lu\n", uxCurrentCount ) );
			}
		}
		#endif /* ipconfigCHECK_IP_QUEUE_SPACE */

		if( ( ulISREvents & EMAC_IF_ALL_EVENT ) == 0 )
		{
			/* No events to process now, wait for the next. */
			ulTaskNotifyTake( pdFALSE, ulMaxBlockTime );
		}

		if( ( ulISREvents & EMAC_IF_RX_EVENT ) != 0 )
		{
			ulISREvents &= ~EMAC_IF_RX_EVENT;

			xResult = prvNetworkInterfaceInput();
			if( xResult > 0 )
			{
			  	while( prvNetworkInterfaceInput() > 0 )
				{
				}
			}
		}

		if( ( ulISREvents & EMAC_IF_TX_EVENT ) != 0 )
		{
			/* Code to release TX buffers if zero-copy is used. */
			ulISREvents &= ~EMAC_IF_TX_EVENT;
			/* Check if DMA packets have been delivered. */
			vClearTXBuffers();
		}

		if( ( ulISREvents & EMAC_IF_ERR_EVENT ) != 0 )
		{
			/* Future extension: logging about errors that occurred. */
			ulISREvents &= ~EMAC_IF_ERR_EVENT;
		}
		if( xResult > 0 )
		{
			/* A packet was received. No need to check for the PHY status now,
			but set a timer to check it later on. */
			vTaskSetTimeOutState( &xPhyTime );
			xPhyRemTime = pdMS_TO_TICKS( PHY_LS_HIGH_CHECK_TIME_MS );
			xResult = 0;
		}
		else if( xTaskCheckForTimeOut( &xPhyTime, &xPhyRemTime ) != pdFALSE )
		{
			xStatus =ETH_ReadPHYRegister( 0x00, 0x01 );
			xPhyObject.ulLinkStatusMask = xStatus;
			if( ( ulPHYLinkStatus & BMSR_LINK_STATUS ) != ( xStatus & BMSR_LINK_STATUS ) )
			{
				ulPHYLinkStatus = xStatus;
				FreeRTOS_printf(  ("prvEMACHandlerTask: PHY LS now %d\n", ( ulPHYLinkStatus & BMSR_LINK_STATUS ) != 0)  );
				prvEthernetUpdateConfig( pdFALSE );
			}

			vTaskSetTimeOutState( &xPhyTime );
			if( ( ulPHYLinkStatus & BMSR_LINK_STATUS ) != 0 )
			{
				xPhyRemTime = pdMS_TO_TICKS( PHY_LS_HIGH_CHECK_TIME_MS );
			}
			else
			{
				xPhyRemTime = pdMS_TO_TICKS( PHY_LS_LOW_CHECK_TIME_MS );
			}
		}
	}
}
/*-----------------------------------------------------------*/

void ETH_IRQHandler( void )
{
	uint32_t dmasr;
	dmasr = ETH->DMASR & ETH_DMA_ALL_INTS;
	ETH->DMASR = dmasr;
	
	/* Frame received */
	if( ( dmasr & ( ETH_DMA_FLAG_R | ETH_DMA_IT_RBU ) ) != 0 )
	{
		/* Receive complete callback */
		ETH_RxCpltCallback();
	}
	/* Frame transmitted */
	if( ( dmasr & ( ETH_DMA_FLAG_T ) ) != 0 )
	{
		/* Transfer complete callback */
		ETH_TxCpltCallback();
	}

	/* ETH DMA Error */
	if( ( dmasr & ( ETH_DMA_FLAG_AIS ) ) != 0 )
	{
		/* Ethernet Error callback */
	}
}


