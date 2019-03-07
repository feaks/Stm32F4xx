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

#include "bsp_gps.h"
#include "config.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdbool.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "stdlib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "bsp_uart.h"
#include "math_extend.h"
#include "ahrs.h"
#include "PID_Position.h"

/*
GPS接口
	USART6_TX:	DMA2_Stream6	DMA_Channel_5
	USART6_RX:	DMA2_Stream1	DMA_Channel_5
*/
#define GPS_LEN 650   // gps缓存区大小
#define DISTANCE_BETWEEN_TWO_LONGITUDE_POINTS_AT_EQUATOR    111319.5f 

GPS gps;
Point local; 					// 起点经纬度
char gps_route = 0;							// 是否设置了路径 0：没有路径信息	1：设置过路径信息
int gps_route_num = 0;					// 路径点数

static uint8_t gps_tx_buf[GPS_LEN];
static uint8_t gps_rx_buf[GPS_LEN];
static uint16_t gps_rx_index;

void nmea_parse_header(uint8_t* buffer, uint8_t len);
float geoCalculateMagDeclination(const float lon, float lat);
bool checkSum(uint8_t* buffer, uint8_t len);
	
GPS_POINT *route_head;   // 主函数头
GPS_POINT *route_tail;      

 
//	GPS参数初始化
void GPS_Init(void)
{
	// 关闭GPGGA
	const uint8_t CLOSE_GGA[11] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x00, 0x00, 0xFA, 0x0F};		
	// 关闭GPGSV
	const uint8_t CLOSE_GSV[11] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x03, 0x00, 0xFD, 0x15};
	// 关闭GPGSA
	const uint8_t CLOSE_GSA[11] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x02, 0x00, 0xFC, 0x13};
	// 关闭GNVTG
	const uint8_t CLOSE_VTG[11] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x05, 0x00, 0xFF, 0x19};
	// 关闭GNGLL
	const uint8_t CLOSE_GLL[11] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x01, 0x00, 0xFB, 0x11};
	//	target:1-UART1		Protocol in:0+1+2		Protocol out:0+1		Baudrate:9600
	const uint8_t CFG_PRT[37] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDA, 0x3D, 0xB5, 0x62 , 0x06, 0x00, 0x01, 0x00, 0x01, 0x08, 0x22};
	//	以10Hz输出（100ms  	10Hz	1cyc	10Hz）	
	const uint8_t CFG_RATE_10Hz[22] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x03, 0x00, 0x7C, 0x16, 0xB5, 0x62, 0x06, 0x08, 0x00, 0x00, 0x0E, 0x30};

	// UART2_Write(CLOSE_GGA, 11);
	UART2_Write(CLOSE_GSV, 11);
	UART2_Write(CLOSE_GSA, 11);
	UART2_Write(CLOSE_VTG, 11);
	UART2_Write(CLOSE_GLL, 11);
	UART2_Write(CFG_PRT, 37);
	//UART2_Write(CFG_RATE_10Hz, 22);
		
		
	//	以5Hz输出（200ms  	5Hz	1cyc	5Hz）	
	//const uint8_t CFG_RATE_5Hz[22] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A, 0xB5, 0x62, 0x06, 0x08, 0x00, 0x00, 0x0E, 0x30};
	//	target:1-UART1		Protocol in:0+1+2		Protocol out:0+1		Baudrate:115200
	// const uint8_t CFG_PRT[37] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x7E, 0xB5, 0x62 , 0x06, 0x00, 0x01, 0x00, 0x01, 0x08, 0x22};
}

void GPS_DMA_Init(void)
{
	//local.lon = 114.4474801;
	//local.lat =  30.4191875;
	/* Enable DMA1 clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	/* DMA1 Stream6 Configuration */
	DMA_DeInit(DMA1_Stream6);
	DMA_InitTypeDef DMA_InitStruct_TX;
	DMA_InitStruct_TX.DMA_Channel = DMA_Channel_4;                // 通道4 
	DMA_InitStruct_TX.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;  
	DMA_InitStruct_TX.DMA_Memory0BaseAddr = (uint32_t)&gps_tx_buf[0];
	DMA_InitStruct_TX.DMA_DIR = DMA_DIR_MemoryToPeripheral;       // 内存到外设模式
	DMA_InitStruct_TX.DMA_BufferSize = GPS_LEN;
  DMA_InitStruct_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 
	DMA_InitStruct_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
	DMA_InitStruct_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct_TX.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct_TX.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct_TX.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct_TX.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStruct_TX.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct_TX.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream6, &DMA_InitStruct_TX);
	
	/* Enable the DMA global Interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE); // transfer complete interrupt
	USART_DMACmd(USART2, USART_DMAReq_Tx , ENABLE);
	//DMA_Cmd(DMA1_Stream6, ENABLE);
	
	/* DMA1 Stream5 Configuration */
	DMA_DeInit(DMA1_Stream5);
	DMA_InitTypeDef DMA_InitStruct_RX;
	DMA_InitStruct_RX.DMA_Channel = DMA_Channel_4;                // 通道4 
	DMA_InitStruct_RX.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;  
	DMA_InitStruct_RX.DMA_Memory0BaseAddr = (uint32_t)&gps_rx_buf[0];
	DMA_InitStruct_RX.DMA_DIR = DMA_DIR_PeripheralToMemory;       // 外设到内存模式
	DMA_InitStruct_RX.DMA_BufferSize = GPS_LEN;
  DMA_InitStruct_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
	DMA_InitStruct_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct_RX.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct_RX.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct_RX.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct_RX.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStruct_RX.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct_RX.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5, &DMA_InitStruct_RX);
	USART_DMACmd(USART2, USART_DMAReq_Rx , ENABLE);
	DMA_Cmd(DMA1_Stream5, ENABLE);
}

// GPGGA
// GPRMC
void nmea_parse(void)
{

	static int16_t current;			// 当前解析帧的起始位置（$符号所在位置）
	static int16_t next;				// 下一帧的起始位置（$符号所在位置）
	
	while(gps_rx_index!=GPS_LEN - DMA1_Stream5->NDTR)
	{
		// 处理消息帧
		if(gps_rx_buf[current] == '$' && gps_rx_buf[next] == '$' && current != next)
		{
			if(next>current && checkSum(&gps_rx_buf[current], next-current))
			{
				// 处理current~next之间的数据
				nmea_parse_header(&gps_rx_buf[current], next-current);
			}
			current = next;
		}
		
		// 寻找$符
		if(gps_rx_buf[gps_rx_index] == '$')
		{
			if(gps_rx_buf[current] != '$')				current = gps_rx_index;
			else																	next    = gps_rx_index;
		}
		
		// 缓冲区循环
		if(gps_rx_index++ == GPS_LEN)
		{
			gps_rx_index = 0;
		}
	}
}

void nmea_parse_header(uint8_t* buffer, uint8_t len)
{
	char *endp; // str 转 number 结束时的指针
	
	int uiCalcComma = 0;	// 逗号数量

	char *bufptr = (char *)(buffer + 6);
	
	for (uint8_t i = 0 ; i < len; i++) {
		if (buffer[i] == ',') { uiCalcComma++; }
	}
	
	if ((memcmp(buffer + 3, "GGA,", 3) == 0) && (uiCalcComma == 14))
	{
		// $GPGGA,172814.0,3723.46587704,N,12202.26957864,W,2,6 ,1.2,18.893,M,-25.669,M,2.0,0031*4F
		// $GPGGA,014227.000,3025.1563,N,11426.8634,      E,1,05,2.1,126.2,M,-13.4,M,,0000*78
		double ashtech_time __attribute__((unused)) = 0.0, lat = 0.0, lon = 0.0, alt = 0.0;
		int num_of_sv __attribute__((unused)) = 0, fix_quality = 0;
		double hdop __attribute__((unused)) = 99.9;
		char ns = '?', ew = '?';
		
		if (bufptr && *(++bufptr) != ',') { ashtech_time = strtod(bufptr, &endp); bufptr = endp; }

		if (bufptr && *(++bufptr) != ',') { lat = strtod(bufptr, &endp); bufptr = endp; }

		if (bufptr && *(++bufptr) != ',') { ns = *(bufptr++); }

		if (bufptr && *(++bufptr) != ',') { lon = strtod(bufptr, &endp); bufptr = endp; }

		if (bufptr && *(++bufptr) != ',') { ew = *(bufptr++); }

		if (bufptr && *(++bufptr) != ',') { fix_quality = strtol(bufptr, &endp, 10); bufptr = endp; }

		if (bufptr && *(++bufptr) != ',') { num_of_sv = strtol(bufptr, &endp, 10); bufptr = endp; }

		if (bufptr && *(++bufptr) != ',') { hdop = strtod(bufptr, &endp); bufptr = endp; }

		if (bufptr && *(++bufptr) != ',') { alt = strtod(bufptr, &endp); bufptr = endp; }

		if (ns == 'S') {
			lat = -lat;
		}

		if (ew == 'W') {
			lon = -lon;
		}
		
		/* convert from degrees, minutes and seconds to degrees * 1e7 */
		gps.lat = (int)(lat * 0.01) + (lat * 0.01 - (int)(lat * 0.01)) * 100.0 / 60.0;
		gps.lon = (int)(lon * 0.01) + (lon * 0.01 - (int)(lon * 0.01)) * 100.0 / 60.0;
		gps.alt = (int)(alt * 1000);
		gps.fix_quality = fix_quality; // 				// 0: Fix not valid
																							// 1: GPS fix
																							// 2: Differential GPS fix, OmniSTAR VBS
																							// 4: Real-Time Kinematic, fixed integers
																							// 5: Real-Time Kinematic, float integers, OmniSTAR XP/HP or Location RTK
		gps.num_of_sv = num_of_sv; // 大于4颗有效  0~24
		gps.hdop = hdop; // 0.5~2.1 ,越小越好
		
		if(fix_quality > 0)
		{
			if(local.lat == 0 || local.lon == 0)
			{
				local.lat = gps.lat;
				local.lon = gps.lon;
			}
			
			// GPS大地位置误差
			gps.x = (gps.lon - local.lon) * DISTANCE_BETWEEN_TWO_LONGITUDE_POINTS_AT_EQUATOR * __cos(gps.lat * DEG2RAD);
			gps.y = (gps.lat - local.lat) * DISTANCE_BETWEEN_TWO_LONGITUDE_POINTS_AT_EQUATOR;
			gps.mag_declination = geoCalculateMagDeclination(gps.lon, gps.lat);
			gps.isReady = true;
		}
	} 
	else if ((memcmp(buffer + 3, "RMC,", 3) == 0) && (uiCalcComma == 12))
	{
		// $GPRMC,014226.000,A,3025.1564,N,11426.8635,E,0.08,184.13,191217,,,A*6D
		double ashtech_time __attribute__((unused)) = 0.0, lat = 0.0, lon = 0.0;
		double speed = 0;
		char ns = '?', ew = '?', state = '?', degree = 0;
		
		if (bufptr && *(++bufptr) != ',') { ashtech_time = strtod(bufptr, &endp); bufptr = endp; }

		if (bufptr && *(++bufptr) != ',') { state = *(bufptr++); }
		
		if (bufptr && *(++bufptr) != ',') { lat = strtod(bufptr, &endp); bufptr = endp; }

		if (bufptr && *(++bufptr) != ',') { ns = *(bufptr++); }

		if (bufptr && *(++bufptr) != ',') { lon = strtod(bufptr, &endp); bufptr = endp; }

		if (bufptr && *(++bufptr) != ',') { ew = *(bufptr++); }

		if (bufptr && *(++bufptr) != ',') { speed = strtod(bufptr, &endp); bufptr = endp; }

		if (bufptr && *(++bufptr) != ',') { degree = strtod(bufptr, &endp); bufptr = endp; }

		if (ns == 'S') {
			lat = -lat;
		}

		if (ew == 'W') {
			lon = -lon;
		}
		
		gps.speed = speed * 5144L / 1000L;    // speed in cm/s
		gps.degree = degree; // ground course deg * 10
		gps.state = state == 'A'; // V or A
	}
}


float gps_kp = 0.15f; // Px4  1 / (2PI) = 0.15915494309189533576888376337251
float gps_ki = 0.3f;	// Px4	2 / (2PI) = 0.31830988618379067153776752674503
float gs_x, gs_y;
// GPS水平位置融合
void GPS_Fusion_Position(float dt) // 暂未调用
{
	static float last_px, last_py;
	static float last_sx, last_sy;
	static float last_gps_x, last_gps_y;
	static float gps_dt = 0;
	
	gps_dt += dt;
	
	if(gps.isReady)
	{
		gs_x = (gps.x -last_gps_x) / gps_dt;
		gs_y = (gps.y -last_gps_y) / gps_dt;
		
		last_gps_x = gps.x;
		last_gps_y = gps.y;
		
		gps_dt = 0;
		gps.isReady = false;
	}

	float cosYaw = __cos(status.attitude.yaw + gps.mag_declination);
	float sinYaw = __sin(status.attitude.yaw + gps.mag_declination);

	// 惯性速度积分位置
	status.position.x += (status.speed.x + last_sx) * 0.5f * dt;
	status.position.y += (status.speed.y + last_sy) * 0.5f * dt;
	
							  
															   
															   
   
	// 记录速度
	last_sx = status.speed.x;
	last_sy = status.speed.y;
   
	// GPS位置与惯导位置融合
	status.position.x += TWO_PI * dt * (gps.x - status.position.x) * gps_kp;
	status.position.y += TWO_PI * dt * (gps.y - status.position.y) * gps_kp;
	
	// 大地位置微分出大地速度
	//	float speed_x = (status.position.x - last_px) / dt;
	//	float speed_y = (status.position.y - last_py) / dt;
	//	
	//	last_px = status.position.x;
	//	last_py = status.position.y;
	
	// 大地速度融合
	status.speed.x += TWO_PI * dt * (gs_x /* speed_x */ - status.speed.x) * gps_ki;
	status.speed.y += TWO_PI * dt * (gs_y /* speed_y */ - status.speed.y) * gps_ki;

	// 大地位置转机体位置
	ctrl_position.body.x =  status.position.x * cosYaw + status.position.y * sinYaw;
	ctrl_position.body.y = -status.position.x * sinYaw + status.position.y * cosYaw;
	
	ctrl_speed.body.x =  status.speed.x * cosYaw + status.speed.y * sinYaw;
	ctrl_speed.body.y = -status.speed.x * sinYaw + status.speed.y * cosYaw;
	
	horizon.isReady = true;
	horizon.source = 2;
}

#define HEXDIGIT_CHAR(d) ((char)((d) + (((d) < 0xA) ? '0' : 'A'-0xA)))
bool checkSum(uint8_t* buffer, uint8_t len)
{
	uint8_t checksum = 0;
	uint8_t * start = buffer + 1;
	uint8_t * end = buffer + len - 5;
	for (; start < end; start++) { checksum ^= *start;	}
	
	return ((HEXDIGIT_CHAR(checksum >> 4) == *(buffer + len - 4)) && (HEXDIGIT_CHAR(checksum & 0x0F) == *(buffer + len - 3)));
}

/* Declination calculation code from PX4 project */
/* set this always to the sampling in degrees for the table below */
#define SAMPLING_RES        10.0f
#define SAMPLING_MIN_LAT    -60.0f
#define SAMPLING_MAX_LAT    60.0f
#define SAMPLING_MIN_LON    -180.0f
#define SAMPLING_MAX_LON    180.0f

static const int8_t declination_table[13][37] = \
{
    { 46, 45, 44, 42, 41, 40, 38, 36, 33, 28, 23, 16, 10, 4, -1, -5, -9, -14, -19, -26, -33, -40, -48, -55, -61, -66, -71, -74, -75, -72, -61, -25, 22, 40, 45, 47, 46 },
    { 30, 30, 30, 30, 29, 29, 29, 29, 27, 24, 18, 11, 3, -3, -9, -12, -15, -17, -21, -26, -32, -39, -45, -51, -55, -57, -56, -53, -44, -31, -14, 0, 13, 21, 26, 29, 30 },
    { 21, 22, 22, 22, 22, 22, 22, 22, 21, 18, 13, 5, -3, -11, -17, -20, -21, -22, -23, -25, -29, -35, -40, -44, -45, -44, -40, -32, -22, -12, -3, 3, 9, 14, 18, 20, 21 },
    { 16, 17, 17, 17, 17, 17, 16, 16, 16, 13, 8, 0, -9, -16, -21, -24, -25, -25, -23, -20, -21, -24, -28, -31, -31, -29, -24, -17, -9, -3, 0, 4, 7, 10, 13, 15, 16 },
    { 12, 13, 13, 13, 13, 13, 12, 12, 11, 9, 3, -4, -12, -19, -23, -24, -24, -22, -17, -12, -9, -10, -13, -17, -18, -16, -13, -8, -3, 0, 1, 3, 6, 8, 10, 12, 12 },
    { 10, 10, 10, 10, 10, 10, 10, 9, 9, 6, 0, -6, -14, -20, -22, -22, -19, -15, -10, -6, -2, -2, -4, -7, -8, -8, -7, -4, 0, 1, 1, 2, 4, 6, 8, 10, 10 },
    { 9, 9, 9, 9, 9, 9, 8, 8, 7, 4, -1, -8, -15, -19, -20, -18, -14, -9, -5, -2, 0, 1, 0, -2, -3, -4, -3, -2, 0, 0, 0, 1, 3, 5, 7, 8, 9 },
    { 8, 8, 8, 9, 9, 9, 8, 8, 6, 2, -3, -9, -15, -18, -17, -14, -10, -6, -2, 0, 1, 2, 2, 0, -1, -1, -2, -1, 0, 0, 0, 0, 1, 3, 5, 7, 8 },
    { 8, 9, 9, 10, 10, 10, 10, 8, 5, 0, -5, -11, -15, -16, -15, -12, -8, -4, -1, 0, 2, 3, 2, 1, 0, 0, 0, 0, 0, -1, -2, -2, -1, 0, 3, 6, 8 },
    { 6, 9, 10, 11, 12, 12, 11, 9, 5, 0, -7, -12, -15, -15, -13, -10, -7, -3, 0, 1, 2, 3, 3, 3, 2, 1, 0, 0, -1, -3, -4, -5, -5, -2, 0, 3, 6 },
    { 5, 8, 11, 13, 15, 15, 14, 11, 5, -1, -9, -14, -17, -16, -14, -11, -7, -3, 0, 1, 3, 4, 5, 5, 5, 4, 3, 1, -1, -4, -7, -8, -8, -6, -2, 1, 5 },
    { 4, 8, 12, 15, 17, 18, 16, 12, 5, -3, -12, -18, -20, -19, -16, -13, -8, -4, -1, 1, 4, 6, 8, 9, 9, 9, 7, 3, -1, -6, -10, -12, -11, -9, -5, 0, 4 },
    { 3, 9, 14, 17, 20, 21, 19, 14, 4, -8, -19, -25, -26, -25, -21, -17, -12, -7, -2, 1, 5, 9, 13, 15, 16, 16, 13, 7, 0, -7, -12, -15, -14, -11, -6, -1, 3 },
};

static float get_lookup_table_val(unsigned lat_index, unsigned lon_index)
{
    return declination_table[lat_index][lon_index];
}

float geoCalculateMagDeclination(const float lon, float lat)
{
    /*
     * If the values exceed valid ranges, return zero as default
     * as we have no way of knowing what the closest real value
     * would be.
     */
    if (lat < -90.0f || lat > 90.0f ||
        lon < -180.0f || lon > 180.0f) {
        return 0.0f;
    }

    /* round down to nearest sampling resolution */
    int min_lat = (int)(lat / SAMPLING_RES) * SAMPLING_RES;
    int min_lon = (int)(lon / SAMPLING_RES) * SAMPLING_RES;

    /* for the rare case of hitting the bounds exactly
     * the rounding logic wouldn't fit, so enforce it.
     */

    /* limit to table bounds - required for maxima even when table spans full globe range */
    if (lat <= SAMPLING_MIN_LAT) {
        min_lat = SAMPLING_MIN_LAT;
    }

    if (lat >= SAMPLING_MAX_LAT) {
        min_lat = (int)(lat / SAMPLING_RES) * SAMPLING_RES - SAMPLING_RES;
    }

    if (lon <= SAMPLING_MIN_LON) {
        min_lon = SAMPLING_MIN_LON;
    }

    if (lon >= SAMPLING_MAX_LON) {
        min_lon = (int)(lon / SAMPLING_RES) * SAMPLING_RES - SAMPLING_RES;
    }

    /* find index of nearest low sampling point */
    const unsigned min_lat_index = (-(SAMPLING_MIN_LAT) + min_lat)  / SAMPLING_RES;
    const unsigned min_lon_index = (-(SAMPLING_MIN_LON) + min_lon) / SAMPLING_RES;

    const float declination_sw = get_lookup_table_val(min_lat_index, min_lon_index);
    const float declination_se = get_lookup_table_val(min_lat_index, min_lon_index + 1);
    const float declination_ne = get_lookup_table_val(min_lat_index + 1, min_lon_index + 1);
    const float declination_nw = get_lookup_table_val(min_lat_index + 1, min_lon_index);

    /* perform bilinear interpolation on the four grid corners */

    const float declination_min = ((lon - min_lon) / SAMPLING_RES) * (declination_se - declination_sw) + declination_sw;
    const float declination_max = ((lon - min_lon) / SAMPLING_RES) * (declination_ne - declination_nw) + declination_nw;

    return (((lat - min_lat) / SAMPLING_RES) * (declination_max - declination_min) + declination_min) * DEG2RAD;
}
	



// 添加航点
void GPS_AddPoint(GPS_POINT *point)
{
	GPS_POINT *node = (GPS_POINT *)pvPortMalloc(sizeof(GPS_POINT));			
	node->index = point->index;						// 航点序号
	node->holdTime = point->holdTime;     // 悬停时间
	node->lat = point->lat;               // 纬度
	node->lon = point->lon;               // 经度
	node->alt = point->alt;               // 海波高度
	node->degree = point->degree;         // 航向角度
	node->desiredYaw = point->desiredYaw;
	node->radius = point->radius;
	node->through = point->through;
	node->next = NULL;
	
	if(route_head == NULL)
	{
		route_head = node;
		route_tail = node;
	}
	else
	{
		route_tail->next = node;
		route_tail = node;
	}
}


// 删除航点信息
// GPS_DeleteRoute(route_head)
void GPS_DeleteRoute(GPS_POINT *head)
{
	GPS_POINT *p1,*p2;
	p1 = head;
	p2 = head;
	while(p1 != NULL)
	{
		p2 = p1->next;
		vPortFree(p1);  
		p1 = NULL;
		p1 = p2;
	}
	head = NULL;
	route_head = NULL;
	route_tail = NULL;
}

// 经纬度计算 参考 ： https://www.cnblogs.com/softfair/p/distance_of_two_latitude_and_longitude_points.html

/*
GPS上电后，每隔一定的时间就会返回一定格式的数据，数据格式为：

$信息类型，x，x，x，x，x，x，x，x，x，x，x，x，x

每行开头的字符都是‘$’，接着是信息类型，后面是数据，以逗号分隔开。一行完整的数据如下：

    $GPRMC,080655.00,A,4546.40891,N,12639.65641,E,1.045,328.42,170809,,,A*60


信息类型为：

GPGSV：可见卫星信息

GPGLL：地理定位信息

GPRMC：推荐最小定位信息

GPVTG：地面速度信息

GPGGA：GPS定位信息

GPGSA：当前卫星信息



1、 GPS DOP and Active Satellites（GSA）当前卫星信息

$GPGSA,<1>,<2>,<3>,<3>,,,,,<3>,<3>,<3>,<4>,<5>,<6>,<7>

<1>模式 ：M = 手动， A = 自动。
<2>定位型式 1 = 未定位， 2 = 二维定位， 3 = 三维定位。
<3>PRN 数字：01 至 32 表天空使用中的卫星编号，最多可接收12颗卫星信息。
<4> PDOP位置精度因子（0.5~99.9）
<5> HDOP水平精度因子（0.5~99.9）
<6> VDOP垂直精度因子（0.5~99.9）
<7> Checksum.(检查位).

2、 GPS Satellites in View（GSV）可见卫星信息

$GPGSV, <1>,<2>,<3>,<4>,<5>,<6>,<7>,?<4>,<5>,<6>,<7>,<8>

<1> GSV语句的总数
<2> 本句GSV的编号
<3> 可见卫星的总数，00 至 12。
<4> 卫星编号， 01 至 32。
<5>卫星仰角， 00 至 90 度。
<6>卫星方位角， 000 至 359 度。实际值。
<7>讯号噪声比（C/No）， 00 至 99 dB；无表未接收到讯号。
<8>Checksum.(检查位).
第<4>,<5>,<6>,<7>项个别卫星会重复出现，每行最多有四颗卫星。其余卫星信息会于次一行出现，若未使用，这些字段会空白。


3、Global Positioning System Fix Data（GGA）GPS定位信息
$GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<10>,M,<11>,<12>*hh

<1> UTC时间，hhmmss（时分秒）格式
<2> 纬度ddmm.mmmm（度分）格式（前面的0也将被传输）
<3> 纬度半球N（北半球）或S（南半球）
<4> 经度dddmm.mmmm（度分）格式（前面的0也将被传输）
<5> 经度半球E（东经）或W（西经）
<6> GPS状态：0=未定位，1=非差分定位，2=差分定位，6=正在估算
<7> 正在使用解算位置的卫星数量（00~12）（前面的0也将被传输）
<8> HDOP水平精度因子（0.5~99.9）
<9> 海拔高度（-9999.9~99999.9）
<10> 地球椭球面相对大地水准面的高度
<11> 差分时间（从最近一次接收到差分信号开始的秒数，如果不是差分定位将为空）
<12> 差分站ID号0000~1023（前面的0也将被传输，如果不是差分定位将为空）


4、Recommended Minimum Specific GPS/TRANSIT Data（RMC）推荐定位信息
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh

<1> UTC时间，hhmmss（时分秒）格式
<2> 定位状态，A=有效定位，V=无效定位
<3> 纬度ddmm.mmmm（度分）格式（前面的0也将被传输）
<4> 纬度半球N（北半球）或S（南半球）
<5> 经度dddmm.mmmm（度分）格式（前面的0也将被传输）
<6> 经度半球E（东经）或W（西经）
<7> 地面速率（000.0~999.9节，前面的0也将被传输）
<8> 地面航向（000.0~359.9度，以真北为参考基准，前面的0也将被传输）
<9> UTC日期，ddmmyy（日月年）格式
<10> 磁偏角（000.0~180.0度，前面的0也将被传输）
<11> 磁偏角方向，E（东）或W（西）
<12> 模式指示（仅NMEA0183 3.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效）


5、 Track Made Good and Ground Speed（VTG）地面速度信息
$GPVTG,<1>,T,<2>,M,<3>,N,<4>,K,<5>*hh

<1> 以真北为参考基准的地面航向（000~359度，前面的0也将被传输）
<2> 以磁北为参考基准的地面航向（000~359度，前面的0也将被传输）
<3> 地面速率（000.0~999.9节，前面的0也将被传输）
<4> 地面速率（0000.0~1851.8公里/小时，前面的0也将被传输）
<5> 模式指示（仅NMEA0183 3.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效）




// 实测武汉地区 GPS数据
$GPGGA,064518.046,,,,,0,00,,,M,0.0,M,,0000*5A
$GPGLL,,,,,064518.046,V,N*76
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,18,56,351,,22,51,026,,14,51,206,21,19,48,285,*78
$GPGSV,3,2,12,26,38,041,,24,37,323,,03,37,281,,09,31,097,*78
$GPGSV,3,3,12,21,17,122,,25,13,176,,31,13,054,,20,00,266,*7A
$GPRMC,064518.046,V,,,,,,,250213,,,N*46
$GPVTG,,T,,M,,N,,K,N*2C

//蔡甸区
$GPGGA,161037.000,3030.6548,N,11402.4568,E,1,04,5.2,51.1,M,-15.5,M,,0000*42
$GPGSA,A,3,05,12,02,25,,,,,,,,,6.0,5.2,2.9*3B
$GPGSV,3,1,10,02,49,314,31,05,37,225,41,12,33,291,32,25,09,318,33*7C
$GPGSV,3,2,10,10,85,027,18,04,57,019,18,17,45,123,20,13,26,075,*7F
$GPGSV,3,3,10,23,14,050,23,40,18,253,33*71
$GPRMC,161037.000,A,3030.6548,N,11402.4568,E,0.00,,010613,,,A*71
$GPVTG,,T,,M,0.00,N,0.0,K,A*13

//第2次
$GPGGA,165538.000,3030.6519,N,11402.4480,E,2,05,1.9,39.5,M,-15.5,M,6.8,0000*68
$GPGSA,A,3,26,05,25,12,02,,,,,,,,2.7,1.9,2.0*3A
$GPGSV,3,1,11,10,63,029,18,02,58,344,23,05,55,247,46,04,50,053,26*75
$GPGSV,3,2,11,12,31,265,39,17,27,139,22,13,22,053,23,25,17,301,37*78
$GPGSV,3,3,11,26,11,180,43,23,04,036,,40,18,253,33*4A
$GPRMC,165538.000,A,3030.6519,N,11402.4480,E,0.00,71.87,010613,,,D*5E
$GPVTG,71.87,T,,M,0.00,N,0.0,K,D*31
$GPGGA,165539.000,3030.6519,N,11402.4480,E,2,05,1.9,39.5,M,-15.5,M,7.8,0000*68
$GPRMC,

度分秒 换算: 30度 30分 65

3030.6519 = 30度 + 30.6519分， 60进制， 
30.6519 分  --> 30.6519 / 60 = 0.510865度。  30.510865度


*/
