#ifndef __BSP_LAN8720_H_
#define __BSP_LAN8720_H_





#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include "stm32f4x7_eth.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "semphr.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define LAN8720_PHY_ADDRESS       0x00
	 


/* Ethernet Flags for EthStatus variable */   
#define ETH_INIT_FLAG           0x01 /* Ethernet Init Flag */
#define ETH_LINK_FLAG           0x10 /* Ethernet Link Flag */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


void LAN8720_Init(void);
void ETH_MAC_DMA_Config(void);

void Eth_Link_IT_task( void * pvParameters );

void ETH_BSP_Config(void);

#ifdef __cplusplus
}
#endif

#endif


