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
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#define MAX_MODULES_NUM				10
/* Private functions ---------------------------------------------------------*/
K_ModulePropertyTypedef    module_prop [MAX_MODULES_NUM];
K_ModuleOpenTypedef        open_link   [MAX_MODULES_NUM];
uint16_t module_num = 0;
uint16_t openlink_num = 0;
/**
  * @brief  Module Initialization.
  * @param  None.
  * @retval None.
  */
void k_ModuleInit(void)
{
	module_num = 0;
	openlink_num = 0;
	memset(module_prop,0,sizeof(module_prop));
	memset(open_link,0,sizeof(open_link));
}
/**
  * @brief  Get the module number.
  * @param  None.
  * @retval None.
  */
uint8_t k_ModuleGetNumber(void)
{
	return module_num;
}
/**
  * @brief  Add module.
  * @param  module: pointer to data structure of type K_ModuleItem_Typedef
  * @retval module add status
  */
uint8_t k_ModuleAdd(K_ModuleItem_Typedef *module)
{
	module_prop[module_num].in_use = 0;
	module_prop[module_num].module = module;
	module_prop[module_num].win_state = 0;
	module_num ++;
	return 0;
}
/**
  * @brief  return module index.
  * @param  module: pointer to data structure of type K_ModuleItem_Typedef
  * @retval module add status
  */
uint8_t k_ModuleGetIndex(K_ModuleItem_Typedef *module)
{
	uint8_t ModuleIdx = 0;
	for(;ModuleIdx < module_num ; ModuleIdx++)
	{
		if(module_prop[ModuleIdx].module->id == module->id)
			return ModuleIdx;
	}
	return 0;
}

/**
  * @brief  Remove module
  * @param  module: pointer to data structure of type K_ModuleItem_Typedef
  * @retval None
  */
uint8_t	k_ModuleRemove(K_ModuleItem_Typedef *module)
{
	uint8_t idx = 0;
	for(; idx < module_num ; idx++)
	{
		if(module_prop[idx].module->id == module->id)
		{
			while(idx != module_num)
			{
				module_prop[idx].module = module_prop[idx+1].module;
				idx ++;
			}
			module_prop[module_num].module = NULL;
			module_num --;
			return 1;
		}
	}
	return 0;
}



void 	k_UpdateLog(char *Msg)
{

}


uint8_t k_ModuleOpenLink(K_ModuleItem_Typedef *module, char *ext)
{

}

/**
  * @brief  Show Message Box
  * @param  pTitle: pointer to the title
  * @param  pText: pointer to the text
  * @retval int 
  */
int k_ExecCreatedDialog (WM_HWIN hDialog) 
{
  WM_DIALOG_STATUS DialogStatus = {0};
  /* Let window know how to send feedback (close info & return value) */
  GUI_SetDialogStatusPtr(hDialog, &DialogStatus);
  while (!DialogStatus.Done) {
    if (!GUI_Exec()) {
      GUI_Delay(10); /* Wait for event (keyboard, mouse or whatever) */
    }
  }
  return DialogStatus.ReturnValue;
}


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
