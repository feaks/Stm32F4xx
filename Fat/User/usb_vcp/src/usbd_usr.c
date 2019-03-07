/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file includes the user application layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_usr.h"
#include "bsp_usb_vcp.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
* @{
*/

/** @defgroup USBD_USR 
* @brief    This file includes the user application layer
* @{
*/ 

/** @defgroup USBD_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_USR_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_USR_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_USR_Private_Variables
* @{
*/ 

USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,    
};

/**
* @}
*/

/** @defgroup USBD_USR_Private_Constants
* @{
*/ 

/**
* @}
*/



/** @defgroup USBD_USR_Private_FunctionPrototypes
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_USR_Private_Functions
* @{
*/ 

/**
* @brief  USBD_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{  
	usb_vcp.usbd_status = USBD_Status_Init;
//  /* Initialize LEDs */
//  STM_EVAL_LEDInit(LED1);
//  STM_EVAL_LEDInit(LED2);
//  STM_EVAL_LEDInit(LED3);
//  STM_EVAL_LEDInit(LED4);   
//  
//  /* Initialize the LCD */
//#if defined (USE_STM322xG_EVAL)
//  STM322xG_LCD_Init();
//#elif defined(USE_STM324xG_EVAL)
//  STM324xG_LCD_Init();
//  
//#elif defined(USE_STM324x9I_EVAL)
//  
//  LCD_Init();
//  LCD_LayerInit();
//  
//  /* Enable The Display */
//  LCD_DisplayOn(); 
//  /* Connect the Output Buffer to LCD Background Layer  */
//  LCD_SetLayer(LCD_FOREGROUND_LAYER);
//  
//  /* Clear the Background Layer */ 
//  LCD_Clear(LCD_COLOR_WHITE);
//  
//#elif defined (USE_STM3210C_EVAL)
//  STM3210C_LCD_Init();
//#else
// #error "Missing define: Evaluation board (ie. USE_STM322xG_EVAL)"
//#endif

//  LCD_LOG_Init();
  
//#ifdef USE_USB_OTG_HS 
//  LCD_LOG_SetHeader((uint8_t*)" USB OTG HS VCP Device");
//#else
//  LCD_LOG_SetHeader((uint8_t*)" USB OTG FS VCP Device");
//#endif
//  LCD_UsrLog("> USB device library started.\n"); 
//  LCD_LOG_SetFooter ((uint8_t*)"     USB Device Library v1.2.0" );
}

/**
* @brief  USBD_USR_DeviceReset 
*         Displays the message on LCD on device Reset Event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed )
{
	usb_vcp.usbd_status = USBD_Status_Reset;
  switch (speed)
  {
    case USB_OTG_SPEED_HIGH: 
//     LCD_LOG_SetFooter ((uint8_t*)"     USB Device Library v1.2.0 [HS]" );
     break;

   case USB_OTG_SPEED_FULL: 
//     LCD_LOG_SetFooter ((uint8_t*)"     USB Device Library v1.2.0 [FS]" );
     break;
 //default:
 //    LCD_LOG_SetFooter ((uint8_t*)"     USB Device Library v1.2.0 [??]" );
 }
}


/**
* @brief  USBD_USR_DeviceConfigured
*         Displays the message on LCD on device configuration Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConfigured (void)
{
	usb_vcp.usbd_status = USBD_Status_Configured;
//  LCD_UsrLog("> VCP Interface configured.\n");
}

/**
* @brief  USBD_USR_DeviceSuspended 
*         Displays the message on LCD on device suspend Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
	usb_vcp.usbd_status = USBD_Status_Suspended;
//  LCD_UsrLog("> USB Device in Suspend Mode.\n");
  /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceResumed 
*         Displays the message on LCD on device resume Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
	usb_vcp.usbd_status = USBD_Status_Resumed;
//  LCD_UsrLog("> USB Device in Idle Mode.\n");
  /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConnected (void)
{
	usb_vcp.usbd_status = USBD_Status_Connected;
//  LCD_UsrLog("> USB Device Connected.\n");
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceDisconnected (void)
{
	usb_vcp.usbd_status = USBD_Status_Disconnected;
//  LCD_UsrLog("> USB Device Disconnected.\n");
}




/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
