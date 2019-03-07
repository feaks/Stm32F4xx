#include "bsp_touch_FT5206.h"
#include "bsp_touch.h"
#include "bsp_i2c.h"

#include "task_touch.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

static I2C * I2C_FT5206;

const uint16_t FT5206_TPX_TBL[5] = { FT5206_TP1_XL_REG, FT5206_TP2_XL_REG, FT5206_TP3_XL_REG, FT5206_TP4_XL_REG, FT5206_TP5_XL_REG };

/** 
 * FT5206单字节写入
 *   REG_Address：要写入的寄存器地址
 *   REG_data：要写入的数据
 */
static bool FT5206_Write_Byte(uint8_t REG_Address, uint8_t REG_data)
{
	taskENTER_CRITICAL();
	if(!I2C_Start(I2C_FT5206)) { taskEXIT_CRITICAL(); return false; }
	I2C_SendByte(I2C_FT5206, FT5206_ADDR);   //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_FT5206)){I2C_Stop(I2C_FT5206); taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(I2C_FT5206, REG_Address); // 发送地址
	I2C_WaitAck(I2C_FT5206);
	I2C_SendByte(I2C_FT5206, REG_data);
	if(!I2C_WaitAck(I2C_FT5206)){I2C_Stop(I2C_FT5206); taskEXIT_CRITICAL(); return false;}
	I2C_Stop(I2C_FT5206); 
	vTaskDelay(2);
	taskEXIT_CRITICAL();
	return true;
}

/**
 * FT5206单字节读取
 *   REG_Address：要读取的寄存器地址
 */
static uint8_t FT5206_Read_Byte(uint8_t REG_Address)
{
	taskENTER_CRITICAL();
	uint8_t REG_data;     	
	if(!I2C_Start(I2C_FT5206)) {taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(I2C_FT5206, FT5206_ADDR);
	if(!I2C_WaitAck(I2C_FT5206)){I2C_Stop(I2C_FT5206); taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(I2C_FT5206, REG_Address);
	if(!I2C_WaitAck(I2C_FT5206)){I2C_Stop(I2C_FT5206); taskEXIT_CRITICAL(); return false;}
	I2C_Start(I2C_FT5206);
	I2C_SendByte(I2C_FT5206, FT5206_ADDR+1);
	if(!I2C_WaitAck(I2C_FT5206)){I2C_Stop(I2C_FT5206); taskEXIT_CRITICAL(); return false;}

	REG_data = I2C_RadeByte(I2C_FT5206);
	I2C_NoAck(I2C_FT5206);
	I2C_Stop(I2C_FT5206);
	taskEXIT_CRITICAL();
	return REG_data;
}

/**
 * FT5206读取n个字节
 *   REG_Address：要读取的寄存器地址
 *   buf：读取后存储数据的buf
 *   len：要读取数据长度
 */
static bool FT5206_Read_NByte(uint8_t REG_Address, uint8_t* buf, uint8_t len)
{
  if(!I2C_Start(I2C_FT5206))return false;
	I2C_SendByte(I2C_FT5206, FT5206_ADDR);  //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_FT5206)){I2C_Stop(I2C_FT5206); return false;}
	I2C_SendByte(I2C_FT5206, REG_Address);   // 发送地址
	I2C_WaitAck(I2C_FT5206);
	I2C_Stop(I2C_FT5206);
	I2C_Start(I2C_FT5206);
	I2C_SendByte(I2C_FT5206, FT5206_ADDR | 1); // 读操作
	I2C_WaitAck(I2C_FT5206);
	for(uint16_t i=0; i<len; i++)
	{
	  buf[i] = I2C_RadeByte(I2C_FT5206);
		if(i<len-1)
		  I2C_Ack(I2C_FT5206);
	}	
	I2C_NoAck(I2C_FT5206);
	I2C_Stop(I2C_FT5206);
	return true;
}

/**
 * FT5206写入n个字节
 *   REG_Address：要写入的寄存器地址
 *   buf：要写入数据的buf
 *   len：要写入数据长度
 */
static bool FT5206_Write_NByte(uint8_t REG_Address, uint8_t* buf, uint8_t len)
{
	taskENTER_CRITICAL();
	if(!I2C_Start(I2C_FT5206)) { taskEXIT_CRITICAL(); return false; }
	I2C_SendByte(I2C_FT5206, FT5206_ADDR);   //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_FT5206)){I2C_Stop(I2C_FT5206); taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(I2C_FT5206, REG_Address >> 8);   // 发送地址
	I2C_WaitAck(I2C_FT5206);
	for(uint8_t i=0; i<len; i++)
	{
		I2C_SendByte(I2C_FT5206, buf[i]);
		if(!I2C_WaitAck(I2C_FT5206)){I2C_Stop(I2C_FT5206); taskEXIT_CRITICAL(); return false;}
	}
	I2C_Stop(I2C_FT5206); 
	vTaskDelay(2);
	taskEXIT_CRITICAL();
	return true;
}

// 检查触摸屏是否正确
bool FT5206_Check(void)
{
	uint8_t uuid[4]; 
	FT5206_Read_NByte(FT_ID_G_LIB_VERSION, uuid, 2); // 读产品版本号 0x3003 0x0001 0x0002

	ts_param.id = 0x5206;
	
	return ((uuid[0] == 0x30) && (uuid[1] == 0x03)) || ((uuid[0] == 0x00) && (uuid[1] == 0x01)) || ((uuid[0] == 0x00) && (uuid[1] == 0x02));
}

// 触摸屏初始化
void FT5206_Init(void)
{
	I2C_FT5206 = I2C_4;
	
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE); // 使能GPIOB、GPIOC时钟
		
	// FT5206复位引脚初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	FT5206_RST(0); // 触摸屏复位
	vTaskDelay(20); 
	FT5206_RST(1); // 释放触摸屏复位  
	vTaskDelay(50);
	
	FT5206_Write_Byte(FT_DEVIDE_MODE, 0); 				// 进入正常操作模式
	FT5206_Write_Byte(FT_DEVIDE_MODE, 0);					// 查询模式
	FT5206_Write_Byte(FT_ID_G_THGROUP, 22);	 			// 设置触摸有效值，越小越灵敏
	FT5206_Write_Byte(FT_ID_G_PERIODACTIVE, 12);  // 激活周期，[12~14]
	
	// FT5206中断引脚初始化为浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
//	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);//PB1 连接到中断线1
//	
//  /* 配置EXTI_Line2 */
//  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
	 
	vTaskDelay(100);
}

// 设置触摸屏方向及宽高
void FT5206_SetTouchDirection(DIRECTION direction, uint16_t width, uint16_t height)
{
	ts_param.direction = direction;
	
	if(direction == UP || direction == DOWN)
	{
		ts_param.width  = width < height ? width : height;
		ts_param.height = height > width ? height : width;
	}
	else
	{
		ts_param.width  = width > height ? width : height;
		ts_param.height = height < width ? height : width;
	}
}

// 以扫描的方式查看触摸状态
void FT5206_Scan(void)
{
	static uint8_t buf[4] = {0};
	uint8_t number = FT5206_Read_Byte(FT_REG_NUM_FINGER); // 读取触摸状态
	
	ts.number = number < 6 ? number : 0;

	for(uint8_t i=0; i<5; i++)
	{
		if( i < ts.number ) // 该触摸点有触摸，读取坐标信息
		{
			FT5206_Read_NByte(FT5206_TPX_TBL[i], buf, 4);
			
			Point point;
			FT5206_TransX(&point, ((uint16_t)(buf[0]&0X0F)<<8) + buf[1], ((uint16_t)(buf[2]&0X0F)<<8) + buf[3]); // 单点触摸时，最高并不是数据位，最高位表示方向，X轴=1，Y轴=0
			
			// 设置Flags
			if(ts.point[i].flags == MTOUCH_FLAG_UP)
			{
				ts.point[i].flags = MTOUCH_FLAG_DOWN;
			}
			else if(ts.point[i].flags == MTOUCH_FLAG_DOWN)
			{
				ts.point[i].flags = MTOUCH_FLAG_MOVE;
			}
			
			// 设置Id
			ts.point[i].id = getLastId(&point);
			if(ts.point[i].id == 0xff)
			{
				ts.point[i].id = getIdleId();
			}
			
			// 设置触摸点
			ts.point[i].x = point.x;
			ts.point[i].y = point.y;
		}
		else // 没有触摸点，清除坐标信息
		{
			ts.point[i].x = 0xFFFF;
			ts.point[i].y = 0xFFFF;
			ts.point[i].flags = MTOUCH_FLAG_UP;
			ts.point[i].id = -1;
		}
	}
}

uint8_t getLastId(Point* point)
{
	for(uint8_t i=0; i<5; i++)
	{
		if((__fabs(point->x - ts.point[i].x) < 10) && (__fabs(point->y - ts.point[i].y) < 10))
		{
			return ts.point[i].id;
		}
	}
	return 0xff;
}

uint8_t getIdleId()
{
	for(uint8_t id=1; id<=5;id++)
	{
		uint8_t idle = 1;
		for(uint8_t i=0; i<5; i++)
		{
			if(id == ts.point[i].id)
			{
				idle = 0;
			}
		}
		if(idle)
		{
			return id;
		}
	}
}

//// 以外部中断的方式查看触摸状态
//void EXTI1_IRQHandler(void)
//{
//	static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
//	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
//	{
//		xSemaphoreGiveFromISR( xTouchBinarySemaphore, &xHigherPriorityTaskWoken );
//		EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE1上的中断标志位 
//	}	
//}

// 根据X，Y物理触摸点计算当前屏幕方向的触摸点
void FT5206_TransX(Point* point, uint16_t x, uint16_t y)
{
	switch(ts_param.direction)
	{
		case UP:
			point->x = 0xFFFF;
			point->y = 0xFFFF;
			break;
		case LEFT:
			point->x = ts_param.width - y;
			point->y = ts_param.height - x;
			break;
		case DOWN:
			point->x = 0xFFFF;
			point->y = 0xFFFF;
			break;
		case RIGHT:
			point->x = y;
			point->y = x;
			break;
	}
}

// 获取触摸点X轴坐标
int FT5206_MeasureX()
{
	return ts.point[0].x;
}

// 获取触摸点Y轴坐标
int FT5206_MeasureY()
{
	return ts.point[0].y;
}

/*******************************************************************************************************
* 五、 坐标读取                                                                                        *
* 主控可以采取轮询或 INT 中断触发方式来读取坐标，采用轮询方式时可采取如下步骤读取：                    *
* 1、 按第二节时序，先读取寄存器 0x814E，若当前 buffer（buffer status 为 1）数据准备好，则依据手指个   *
* 数读、按键状态取相应个数的坐标、按键信息。                                                           *
* 2、 若在 1 中发现 buffer 数据（buffer status 为 0）未准备好，则等待 1ms 再进行读取。                 *
* 采用中断读取方式，触发中断后按上述轮询过程读取坐标。                                                 *
* FT5206 中断信号输出时序为（以输出上升沿为例，下降沿与此时序类同）：                                  *
* 1、待机时 INT 脚输出低。                                                                             *
* 2、 有坐标更新时，输出上升沿。                                                                       *
* 3、 2 中输出上升沿后， INT 脚会保持高直到下一个周期（该周期可由配置 Refresh_Rate 决定）。 请在一个   *
* 周期内将坐标读走并将 buffer status(0x814E)写为 0。                                                   *
* 4、 2 中输出上升沿后， 若主控未在一个周期内读走坐标， 下次 FT5206 即使检测到坐标更新会再输出一个     *
* INT 脉冲但不更新坐标。                                                                               *
* 5、若主控一直未读走坐标，则 FT5206 会一直打 INT 脉冲                                                 *
*******************************************************************************************************/
