#ifndef _BSP_ETH_H_
#define _BSP_ETH_H_

#include "bsp_gpio.h"
#include "stm32f4x7_eth.h"

#define LAN8720_PHY_ADDRESS  	0x00															// LAN8720 PHY芯片地址

#define LAN8720_RESET(x)  		GPIO_Set(GPIOD, GPIO_Pin_3, x)		// LAN8720复位引脚


#endif
