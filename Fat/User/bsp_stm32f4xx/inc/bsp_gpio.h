/****************************************************************************************
COPYRIGHT NOTICE                                                                        *
Copyright (c) 2016 武汉飞航科技                                                         *
技术讨论QQ群：  363450228                                                               *
淘宝链接：  https://shop149667832.taobao.com/                                           *
                                                                                        *
以下所有内容版权均属武汉飞航科技所有，未经允许不得用于商业用途。                        *
欢迎各位使用并推广本程序，修改内容时必须保留飞航科技的版权声明。                        *
                                                                                        *
@file     bsp_gpio.h                                                                    *
@brief    GPIO初始化头文件                                                              *
@author   武汉飞航科技                                                                  *
@version  V1.0                                                                          *
****************************************************************************************/

#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_

#include  "stm32f4xx_gpio.h"

/*****LED控制*************************************/
#define LED1_ON  GPIOE->BSRRH = GPIO_Pin_0
#define LED1_OFF GPIOE->BSRRL = GPIO_Pin_0
                 
#define LED2_ON  GPIOE->BSRRH = GPIO_Pin_1
#define LED2_OFF GPIOE->BSRRL = GPIO_Pin_1
                 
#define LED3_ON  GPIOE->BSRRH = GPIO_Pin_2
#define LED3_OFF GPIOE->BSRRL = GPIO_Pin_2
                 
#define LED4_ON  GPIOE->BSRRH = GPIO_Pin_3
#define LED4_OFF GPIOE->BSRRL = GPIO_Pin_3

#define LED1_TOGGLE GPIO_ToggleBits(GPIOE, GPIO_Pin_0)
#define LED2_TOGGLE GPIO_ToggleBits(GPIOE, GPIO_Pin_1)
#define LED3_TOGGLE GPIO_ToggleBits(GPIOE, GPIO_Pin_2)
#define LED4_TOGGLE GPIO_ToggleBits(GPIOE, GPIO_Pin_3)


//初始化模块、引脚、输入输出、是否推挽输出、上下拉状态
void GPIO_Initialize(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode, GPIOOType_TypeDef GPIO_OType, GPIOPuPd_TypeDef GPIO_PuPd);

//设置引脚输出电平GPIOx,GPIO_Pin对应引脚   out：0/1 电平
void GPIO_Set(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,unsigned char out);

//获取引脚状态 返回0/1 
uint8_t GPIO_Get(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

void LED_Init();

#endif

/***************************** 武汉飞航科技有限公司 (END OF FILE) *********************************/
