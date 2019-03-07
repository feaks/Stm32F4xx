#ifndef _TASK_MAVLINK_H_
#define _TASK_MAVLINK_H_

#include "mavlink.h"

#define MAVLINK_ENABLE 0     // Mavlink协议总开关
#define MAVLINK_WAIT 0/portTICK_RATE_MS  // Mavlink发送最长等待时间，1ms内为完成发送此条消息将丢弃
#define MAVLINK_QUEUE_LENGTH 20
#define MAVLINK_SYSID  1      
#define MAVLINK_COMPID  1

#define MAVLINK_MESSAGE_30 30   	// 三轴角度、角速度 30ms			需要绘折线图		有时间	长度：36	9600
#define MAVLINK_MESSAGE_26 30 		// 9DOF 物理量 30ms						需要绘折线图		有时间  长度：30	8000
#define MAVLINK_MESSAGE_34 40  	  // 遥控输入值 40ms						需要绘条形图		有时间	长度：30	6000
#define MAVLINK_MESSAGE_35 40     // 遥控输入原始值 40ms				需要绘条形图		有时间	长度：30	6000
#define MAVLINK_MESSAGE_36 40	  	// 电机输出										需要绘条形图		有时间	长度：29	5800

#define MAVLINK_MESSAGE_187 50    // 磁力计校准数据							需要显示文本		无时间	长度：20	3200
#define MAVLINK_MESSAGE_188 40	  // 电机输出										需要绘折线图		无时间	长度：44	8800

#define MAVLINK_MESSAGE_27 500  	// 9DOF 原始值 500ms					需要显示文本		有时间	长度：34	544
#define MAVLINK_MESSAGE_29 500  	// 气压高度 500ms							需要显示文本		有时间	长度：22	352
#define MAVLINK_MESSAGE_186 500  	// 超声波高度 500ms						需要显示文本		有时间	长度：14	244
#define MAVLINK_MESSAGE_2 1000  	// 电池												需要显示文本		无时间	长度：44	44
#define MAVLINK_MESSAGE_0 1000  	// 心跳包											需要显示文本		无时间	长度：17	17

#define MAVLINK_MESSAGE_24  50  	// GPS												需要显示文本		有时间	长度：30	4800

void Task_MavLink( void *pvParameters );

#endif