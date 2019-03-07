/*
*********************************************************************************************************
*	                                  
*	模块名称 : GPS驱动
*	文件名称 : bsp_gps.c
*	版    本 : V1.0
* 日		期 ：2015-12-15   
*		
*	2015.12.15 武汉飞航科技有限公司 
*
*********************************************************************************************************
*/
#ifndef _BSP_GPS_H_
#define _BSP_GPS_H_

#include "stdint.h"
#include "stdbool.h"

typedef struct
{
	float lat; 							// 纬度
	float lon; 							// 经度
} Point;

typedef struct 
{
	long utc;
	float lat; 							// 纬度
	float lon; 							// 经度
	uint8_t fix_quality;		// 定位质量
	uint8_t num_of_sv;			// 卫星数量
	float hdop; 						// 水平定位精度
	float alt;							// 海波高度
	float speed;						// 速度
	float degree;						// 航向角度
	char state;							// 定位状态
	float x;								// 相对于起始点，x轴的偏移量
	float y;								// 相对于起始点，y轴的偏移量
	float mag_declination;	// 磁偏量
	bool isReady;
} GPS;

typedef struct GPS_POINT{ 
	float index;						// 航点序号
	float holdTime;					// 悬停时间
	float radius;				  	// 触发半径		单位m
	float through;					// 航路点通过方式：正值顺时针轨迹；负值逆时针。用于航路控制
	float desiredYaw;				// 期望航点偏航
	float lat; 							// 纬度
	float lon; 							// 经度
	float alt;							// 海波高度
	float degree;						// 航向角度
	struct GPS_POINT *next;    		//非循环语句
} GPS_POINT;



extern GPS_POINT *route_head;   // 主函数头
extern GPS_POINT *route_tail;     
extern char gps_route;					// 是否设置了路径 0：没有路径信息	1：设置过路径信息
extern int gps_route_num;				// 路径点数
extern Point local; 						// 起点经纬度
extern GPS gps;

void GPS_Init(void);
void GPS_DMA_Init(void);
void nmea_parse(void);
void GPS_Fusion_Position(float dt);
void GPS_AddPoint(GPS_POINT *piont);
void GPS_DeleteRoute(GPS_POINT *head);
#endif



