/****************************************************************************************
COPYRIGHT NOTICE																																				*
Copyright (c) 2016 武汉飞航科技																													*
技术讨论QQ群：  363450228																																*
淘宝链接：  https://shop149667832.taobao.com/																						*
																																												*
以下所有内容版权均属武汉飞航科技所有，未经允许不得用于商业用途。												*
欢迎各位使用并推广本程序，修改内容时必须保留飞航科技的版权声明。												*
																																												*
@file     config.h																																			*
@brief    工程配置文件																																*
@author   武汉飞航科技																																	*
@version  V1.0																																				  *
*****************************************************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

#define ULTRASONIC_WAIT 25/portTICK_RATE_MS  // 超声波最长等待时间   340m/1000ms = 6m/?    18.647ms

#define ENABLED 1    // 1  使能
#define DISABLED 0   // 0	 失能


/*********串口配置使能************************/
#define UART_1  ENABLED
#define UART_2  ENABLED
#define UART_3  ENABLED
#define UART_4  ENABLED
#define UART_5  ENABLED
#define UART_6  ENABLED

/*******串口波特率配置*****************/
#define UART1_BAUD 115200     // Debug
#define UART2_BAUD 9600
#define UART3_BAUD 57600      // MavLink协议
#define UART4_BAUD 9600       
#define UART5_BAUD 115200
#define UART6_BAUD 9600				// 超声波

/**********串口接收使能****************/
#define EN_USART1_RX 			0		//使能（1）/禁止（0）串口1接收
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口2接收
#define EN_USART3_RX 			0		//使能（1）/禁止（0）串口3接收
#define EN_UART4_RX 			0		//使能（1）/禁止（0）串口4接收
#define EN_UART5_RX 			0		//使能（1）/禁止（0）串口5接收
#define EN_USART6_RX 			0		//使能（1）/禁止（0）串口6接收

#define EN_USART3_DMA_TX  1
	  	
/**串口引脚映射配置******************************/
#define UART1_Mapping 1         // (1:TX = PA9   RX = PA10   0: TX = PB6   RX = PB7  )
#define UART2_Mapping 1         // (1:TX = PA2   RX = PA3    0: TX = PD5   RX = PD6  ) 
#define UART3_Mapping 0         // (1:TX = PB10  RX = PB11   0: TX = PD8   RX = PD9  2:TX = PC10  RX = PC11)
#define UART4_Mapping 0         // (1:TX = PA0   RX = PA1    0: TX = PC10  RX = PC11 )
#define UART5_Mapping 1         // (0:TX = PC12  RX = PD2                            )
#define UART6_Mapping 0         // (1:TX = PG14  RX = PG9    0: TX = PC6   RX = PC7  )
#define UART7_Mapping 1 				// (1:TX = PE8   RX = PE7    0: TX = PF7   RX = PF6  )
#define UART8_Mapping 1 				// (0:TX = PE1   RX = PE0

/*********定时器设置************************/
#define TIM1_ENABLE ENABLED   // PWM1~PWM4
#define TIM5_ENABLE DISABLED  // PWM5~PWM8

#define TIM_CAP_IN_PRESC 84-1 // PWM捕获预分频，使用TIM3、TIM4捕获，捕获时钟频率 = 42M * 2 / ((84-1) + 1) = 1M

#define CHANNEL_MAX 5  // 最大输入通道
#define MOTORS_MAX 4   // 最大PWM输出通道

#define TIM_PWM_MAX_FREQ 1000 // Hz PWM输出最大频率（1~84M / ACCURACY）
#define TIM_PWM_OUT_FREQ 400 // PWM输出频率

// 输出频率400Hz时，计数器时钟为4M，计数频率0.25us
#define ACCURACY 10000 	// 周期（2500us / 0.25），-
#define INIT_DUTY 1000	// 初始占空比（1000us * PWM_RADIO / 0.25）
#define PWM_RADIO 4     // PWM输出放大比例，默认输出范围0~1000

#define MAX_PWM							1000		//	最大PWM输出为100%油门
#define MAX_ADJUST          500     //  最大控制量50%
#define MAX_THR       			800 		//	油门通道最大占比80%
#define MIN_THR       			400 		//	油门通道最小占比40%
#define READY_SPEED   			200   	//	解锁后电机转速20%油门

#define LOG_MAX_LENGTH  100
#define LOG_WAIT 100/portTICK_RATE_MS   // Log发送最长等待时间，100ms内为完成发送此条消息将丢弃

#define EEPROM_SIZE 16   // 16  64

#define F450  //QV250 // F550 // F450 

#define DEFAULT_MPU6050_CALIBRATE  false        // MPU6050上电校准
	
#define DEFAULT_RC_CALIBRATE  false            // RC上电校准
	
#define DEFAULT_HMC5883L_CALIBRATE  false      // HMC5883L上电校准
#define DEFAULT_HMC5883L_SELF_TEST  false       // HMC5883L上电自检，计算比例系数

#endif

/**
 *	LED1	 亮：解锁			灭：锁定
 * 	LED2	 250hz闪烁：遥控器初始化异常
 */	

/***************************** 武汉飞航科技有限公司 (END OF FILE) *********************************/
