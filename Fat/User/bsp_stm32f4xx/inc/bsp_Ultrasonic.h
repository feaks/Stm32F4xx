/*
*********************************************************************************************************                             
*	模块名称 : 超声波数据处理
*	文件名称 : bsp_Ultrasonic.h
*	版    本 : V1.0
*	说    明 : 超声波数据处理
* 日		期 ：2016-2-20   
*		
*	Copyright(c)2016 武汉飞航科技有限公司 
*	技术讨论QQ群：363450228 淘宝官网：https://shop149667832.taobao.com
* 以下所有内容版权均属武汉飞航科技所有，未经允许不得用于商业用途。
* 欢迎各位使用并推广本程序，修改内容时必须保留飞航科技的版权声明。
*********************************************************************************************************
*/
#ifndef _BSP_ULTRASONIC_H_
#define _BSP_ULTRASONIC_H_

#include "stdint.h"
#include "stdbool.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define ULTRA_CMD      0x55
#define Ultra_Send_CMD(ch) UART4_Putc(ch);

#define MAX_HEIGHT 							2 // m

enum Data_State 
{
	START, MBS, LBS, END
};

typedef struct 
{
	float distance_raw;							// 姿态补偿后的垂直高度   			m   
	float distance_compensate;			// 姿态补偿后的垂直高度   			m
	float last_distance_compensate;	// 上一次姿态补偿后的垂直高度		m
	float distance_offset;					// 超声波相对于当前位置的偏移量 m		
	float distance;									// 低通滤波后的垂直高度   			m				实际高度等于 distance - distance_offset
	float speed;										// 高度变化后，估计变化时的速度 m/s
	bool isReady;										// 超声波数据已经计算完成
	uint8_t state;									// 数据接收状态
	int8_t lost;										// 超声波数据丢失次数
	
	bool calibrating;       // 是否在校准中
	bool present;						// 是否存在
	bool enabled;						// 是否使能
	bool health;						// 是否可用
} Ultrasonic;

extern Ultrasonic ultra;
extern xSemaphoreHandle xUltrasonicReceiveBinarySemaphore;

void Ultra_Read();
void Ultra_Receive(uint8_t data);
void Ultra_Distance_Offset();
void Ultra_Calculate_Height(float dt);
void Ultra_Fusion_Height(float dt);

#endif


