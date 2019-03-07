#ifndef _MATH_EXTEND_
#define _MATH_EXTEND_

#include "stm32f4xx.h"
#include <math.h>
#include "arm_math.h"


#define HALF_PI   					1.5707963267948966192313216916398f
#define TWO_PI   						6.283185307179586476925286766559f

#define DEG2RAD 						0.017453292519943295769236907684886f	//角度转弧度
#define RAD2DEG 						57.295779513082320876798154814105f		//弧度转角度

#define MM2M								0.001f		// 毫米转米
#define M2MM								1000			// 米转毫米

#define MS2S								0.001f		// 毫秒转秒
#define S2MS								1000  		// 秒转毫秒


#define RAD2MILLIRAD				1000      // 弧度转毫弧度
#define GS2MGS							1000      // 高斯转毫高斯

#define G2MG								1000      // G转mg,重力加速度放大1000倍
#define MG2G								0.001f      // mg转G,重力加速度缩小1000倍

#define radians(deg)				((deg)*DEG2RAD)
#define degrees(rad)				((rad)*RAD2DEG)

#define TAN_MAP_RES     0.003921569f     /* (smallest non-zero value in table) */
#define TAN_MAP_SIZE    256

#define FLT_EPSILON 1.1920929e-07F  /* 1E-5 */
#define LIMIT(x, min, max) 	( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )

#define ABS(x) 							fabsf(x)
#define MIN(a, b) 					((a) < (b) ? (a) : (b))
#define MAX(a, b) 					((a) > (b) ? (a) : (b))

#define sq(a) ((a)*(a))
#define sqrtf4(a,b,c,d)			__sqrtf(sq(a)+sq(b)+sq(c)+sq(d))	// 四元数长度
#define sqrtf3(a,b,c)     	__sqrtf(sq(a)+sq(b)+sq(c))      	// 三维向量长度
#define sqrtf2(a,b)       	__sqrtf(sq(a)+sq(b))            	// 二维向量长度

#define __cos(x)					 	arm_cos_f32(x)
#define __sin(x)					 	arm_sin_f32(x)

// 2维向量(矢量)结构
typedef struct {
	float x;
	float y;
} Vector2f;

// 3维向量(矢量)结构
typedef struct {
	float x;
	float y;
	float z;
} Vector3f;

typedef struct {
  int16_t x;
	int16_t y;
	int16_t z;
} Vector3i;

typedef struct {
  uint16_t x;
	uint16_t y;
	uint16_t z;
} Vector3ui;

typedef struct {
	long int x;
	long int y;
	long int z;
} Vector3ll;

typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t z;
} Vector3ul;

typedef struct {
	int32_t x;
	int32_t y;
	int32_t z;
} Vector3l;

// 矩阵结构
typedef struct {
	Vector3f a;
	Vector3f b;
	Vector3f c;
} Matrix3;

typedef struct
{
  float x;
	float y;
	float z;
} Acc;

typedef struct
{
  float x;
	float y;
	float z;
} Speed;

typedef struct
{
  float x;
	float y;
	float z;
} Position;

typedef struct
{
  float roll;
	float pitch;
	float yaw;
} Attitude;

typedef struct
{
	float kp;
	float kd;
	float ki;
} PID;

float fast_atan2(float y, float x);
float dead_zone(float x,float zoom);
float deadzone_range(float x,float zoom);
float wrap_around_PI(float x);
float wrap_around_2PI(float x);
float invSqrt(float x);

#endif

