/*
*********************************************************************************************************
*	                                  
*	模块名称 : ADC驱动模块
*	文件名称 : bsp_adc.c
*	版    本 : V1.0
*	说    明 : FreeRTOS系统
* 日		期 ：2016-2-20   
*		
*	Copyright(c)2016 武汉飞航科技有限公司 
*	技术讨论QQ群：363450228 淘宝官网：https://shop149667832.taobao.com
* 以下所有内容版权均属武汉飞航科技所有，未经允许不得用于商业用途。
* 欢迎各位使用并推广本程序，修改内容时必须保留飞航科技的版权声明。
*********************************************************************************************************
*/

#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

#include "stdint.h"

typedef struct 
{
	uint16_t current;
	uint16_t voltage;
	uint16_t temperature;
} ADC_DATA;

typedef struct
{
	float  current;			// A	
	float  voltage;			// V
	float  temperature;	// ℃
} Battery;

extern Battery battery;			// 电池信息

void Battery_ADC_Init(void);

void Get_Battery(void);

#endif

/*********************************** 飞航科技 (END OF FILE) ***************************************/
