#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f4xx.h"

/* GUI includes components */
#include "GUI.h"
#include "DIALOG.h"

/* FreeRTOS+FAT  includes components */
#include "ff_headers.h"
#include "ff_sys.h"
#include "ff_stdio.h"
#include "ff_sddisk.h"

/* FreeRTOS+IP includes components */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"

 /* Kernel includes components */
#include "k_module.h"
#include "k_storage.h"
#include "k_rtc.h"    
#include "k_bsp.h" 

/* utilities and standard includes components */   
//#include "cpu_utils.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT zzz *****END OF FILE****/

