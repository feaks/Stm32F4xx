/*
*********************************************************************************************************
*
*	模块名称 : 利用STM32F4内部TIM输出PWM信号及输入捕获，顺便实现
*	文件名称 : bsp_tim_pwm.h
*	版    本 : V1.0
*	说    明 : 头文件
* 日		期 ：2016-2-20   
*		
*	Copyright(c)2016 武汉飞航科技有限公司 
*	技术讨论QQ群：363450228 淘宝官网：https://shop149667832.taobao.com
* 以下所有内容版权均属武汉飞航科技所有，未经允许不得用于商业用途。
* 欢迎各位使用并推广本程序，修改内容时必须保留飞航科技的版权声明。

*********************************************************************************************************
*/

#ifndef _BSP_TIM_PWM_H_
#define _BSP_TIM_PWM_H_

#include <stdint.h>
#include "config.h"
#include <stdbool.h>

typedef struct
{
	uint16_t motor_1;
	uint16_t motor_2;
	uint16_t motor_3;
	uint16_t motor_4;
} MOTOR_OUT;

typedef struct
{
	MOTOR_OUT motor_raw;
	MOTOR_OUT motor_pwm;
	bool enable;
} MOTOR;

extern uint16_t Rc_PWMCount[CHANNEL_MAX];
extern uint8_t CAPTURE_STA;  								// 8位PWM捕获状态。 1：有新数据，0：没有数据
extern MOTOR motor;

void TIM_Cap_Init(void); // 输入捕获初始化

bool PWM_Out_Init(); // TIM 模块设置为PWM输出初始化

void MOTOR_Ctrl(float throttle);

/*
 *      定时器种类            位数          计数模式           产生DMA请求     捕获/比较通道     互补输出
 * 高级定时器（TIM1、TIM8）     16      向上、向下、向上/下        可以               4              有
 * 高级定时器（TIM2、TIM5）     32      向上、向下、向上/下        可以               4              无
 * 高级定时器（TIM3、TIM4）     16      向上、向下、向上/下        可以               4              无
 * 高级定时器（TIM9~TIM14）     16             向上                没有               2              无
 * 高级定时器（TIM6、TIM7）     16      向上、向下、向上/下        可以               0              无                  
 */
#endif

/*********************************** 飞航科技 (END OF FILE) ***************************************/



