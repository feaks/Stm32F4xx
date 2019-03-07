#include "bsp_Ultrasonic.h"
#include "bsp_uart.h"
#include "task.h"
#include "math_extend.h"
#include "ahrs.h"
#include "PID_Position.h"

#define ULTRA_FILTER_NUM 11

uint8_t MAX_LOST=	 			 15; // 超声波最大丢失次数

Ultrasonic ultra= {.present = true, .health = true, .enabled = true};

xSemaphoreHandle xUltrasonicReceiveBinarySemaphore;

void Ultra_Read(void)
{
	Ultra_Send_CMD(ULTRA_CMD);
	ultra.state = START;
	if(++ultra.lost>MAX_LOST)
		ultra.lost = MAX_LOST;
}

// 超声波距离获取
void Ultra_Receive(uint8_t data)
{
	static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	
	static uint8_t tmp;
	ultra.state++;
	if( ultra.state == MBS )
	{
		tmp = data;
	}
	else if( ultra.state == LBS )
	{
		uint16_t distance = (tmp<<8) + data;
		if(distance<3000)
		{
			ultra.distance_raw = distance * MM2M;
			// 高度倾角补偿
			ultra.distance_compensate = ultra.distance_raw * __cos(status.attitude.roll) * __cos(status.attitude.pitch);
			if(ABS(status.position.z - (ultra.distance_compensate + ultra.distance_offset)) < 0.3f) // 超声波变化小于0.3m时有效
			{
				ultra.lost -= 2;
				if(ultra.lost<=0)
				{
					ultra.health = true;
					ultra.present = true;
					ultra.lost = 0;
				}
			}
			else
			{
				ultra.distance_compensate = ultra.last_distance_compensate;
			}
			xSemaphoreGiveFromISR(xUltrasonicReceiveBinarySemaphore, &xHigherPriorityTaskWoken);
		}
	}
}

// 修正超声波与当前位置的误差
void Ultra_Calculate_Height(float dt)
{	
	static float ultra_distance_fifo[ULTRA_FILTER_NUM];         	// 中值滤波数组
	static float *p_ultra_distance_fifo[ULTRA_FILTER_NUM] = { &ultra_distance_fifo[0], &ultra_distance_fifo[1], &ultra_distance_fifo[2], &ultra_distance_fifo[3], &ultra_distance_fifo[4], &ultra_distance_fifo[5], &ultra_distance_fifo[6], &ultra_distance_fifo[7], &ultra_distance_fifo[8], &ultra_distance_fifo[9], &ultra_distance_fifo[10]};					// 指向中值滤波数组

	static int8_t index = -1;                                     // 中值滤波索引
		
	static uint8_t tmp;																						// 用于存储超声波高度，高位数据

	ultra.speed = (ultra.distance_compensate - ultra.last_distance_compensate) / dt;
	ultra.last_distance_compensate = ultra.distance_compensate;
		
	if(index == -1)  // 第一次进行初始化
	{
		for(uint8_t i=0;i<ULTRA_FILTER_NUM;i++)
		{
			ultra_distance_fifo[i] = ultra.distance_compensate;
		}
	}
	else
	{
		ultra_distance_fifo[index] = ultra.distance_compensate;
			
		// 滑动中值滤波
		uint32_t tmp_p;
		for(uint8_t i=0; i<ULTRA_FILTER_NUM-1; i++)
		{
			for(uint8_t j=i+1;j<ULTRA_FILTER_NUM;j++)
			{
				if(*p_ultra_distance_fifo[i] > *p_ultra_distance_fifo[j])
				{
					tmp_p = (uint32_t)p_ultra_distance_fifo[i];
					p_ultra_distance_fifo[i] = p_ultra_distance_fifo[j];
					p_ultra_distance_fifo[j] = (float* )tmp_p;
				}
			}
		}
	}

	if(++index == ULTRA_FILTER_NUM)			index = 0;
	
	ultra.distance = *p_ultra_distance_fifo[ULTRA_FILTER_NUM/2+1];
		
	if(height.source != 1)  // 当前定高数据不是超声波提供的数据，计算当前高度与超声波的偏移量
		ultra.distance_offset = status.position.z - ultra.distance;
}

// 超声波高度融合
void Ultra_Fusion_Height(float dt)
{
	static float last_sz = 0;
	
	// 超声波速度与加速计估计速度融合
	if(ABS(ultra.speed) < 1) // 超声波速度小于1m/s和有效
		status.speed.z += TWO_PI * dt * ( ultra.speed - status.speed.z ) * 0.2f;
	
	// 高度融合低通滤波
	status.position.z += (status.speed.z + last_sz) * 0.5f * dt;
	if(ABS(ultra.speed) < 1 && ultra.distance < MAX_HEIGHT)
		status.position.z += TWO_PI * dt * ( ultra.distance + ultra.distance_offset - status.position.z ) * 0.2f;
	last_sz = status.speed.z;
	
	height.isReady = true;
	height.source = 1;			// 数据来源超声波
	
	// 检查超声波状态
	if(ultra.lost >= MAX_LOST) // 超声波连续丢失超过15次
	{
		ultra.health = false;
		ultra.present = false;
	}
}