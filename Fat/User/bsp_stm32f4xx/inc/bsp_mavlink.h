/*
*********************************************************************************************************
*	                                  
*	模块名称 : MAVLINK数据发送
*	文件名称 : mavlink.c
*	版    本 : V1.0
*	说    明 : MAVLINK数据发送
* 日		期 ：2016-2-20   
*		
*	Copyright(c)2016 武汉飞航科技有限公司 
*	技术讨论QQ群：363450228 淘宝官网：https://shop149667832.taobao.com
* 以下所有内容版权均属武汉飞航科技所有，未经允许不得用于商业用途。
* 欢迎各位使用并推广本程序，修改内容时必须保留飞航科技的版权声明。
*********************************************************************************************************
*/
#ifndef _BSP_MAVLINK_H_
#define	_BSP_MAVLINK_H_

#include "mavlink.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "bsp_gps.h"

typedef enum ACK
{
	COMMAND_ACK=0,
	SET_PID=1,
	GET_PID=2,
	ACK_ENUM_END=3
} ACK;

typedef struct
{
	bool 									ack[ACK_ENUM_END];
	mavlink_command_ack_t mavlink_command_ack;
	mavlink_set_pid_t 		mavlink_set_pid;
	mavlink_get_pid_t     mavlink_get_pid;
} MAVLINK;

typedef struct
{
	uint32_t 				last_receive_time;		// 最后一次收到消息的时间
	uint8_t					last_seq;							// 最后一次收到消息的序列号
	uint8_t 				lost;									// 1秒之内接收失败的数量
	uint8_t					receive;							// 1秒之内接收成功的数量
	uint32_t 				last_signal_time;			// 最后一次更新信号强度时间
	uint16_t 				signal_percent;				// 信号百分比  0~10000对应0%~100%
} MAVLINK_STATUS;

extern xSemaphoreHandle xMavlinkSendBinarySemaphore;
extern xQueueHandle xMavlinkReceiveQueue;
extern MAVLINK mavlink;
extern MAVLINK_STATUS mavlink_status;
extern GPS_POINT piont;

void MAVLINK_DMA_Init(void);
void mavlink_send_with_dma(mavlink_message_t * msg);
void mavlink_parse_message(void);
void mavlink_handler_message(uint8_t msgId, mavlink_message_t* rx_message);

#endif
