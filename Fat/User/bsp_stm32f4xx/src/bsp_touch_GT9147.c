#include "bsp_touch_gt9147.h"
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

static I2C * I2C_GT9147;

// GT9147配置参数表
// 第一个字节为版本号（0x60）,必须保证新的版本号大于等于GT9147内部flash原有版本号，才会更新配置
static uint8_t GT9147_CFG_TBL[]=
{ 																																				//{
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,                      //0x62,0xe0,0x01,0x20,0x03,0x05,0x34,0xc0,0x01,0x08, 
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,                      //0x28,0x0f,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00, 
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,                      //0x00,0x00,0x06,0x16,0x16,0x1f,0x14,0x89,0x28,0x0a, 
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,                      //0x17,0x15,0x31,0x0d,0x00,0x00,0x08,0x22,0x04,0x11, 
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,                      //0x00,0x00,0x00,0x00,0x00,0x03,0x82,0x08,0x08,0x00, 
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,                      //0x00,0x0f,0x2c,0x94,0xc5,0x02,0x07,0x00,0x00,0x04, 
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,                      //0x9d,0x10,0x00,0x84,0x14,0x00,0x70,0x19,0x00,0x5f, 
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,                      //0x20,0x00,0x55,0x27,0x00,0x54,0x00,0x00,0x00,0x00, 
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,                      //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,                      //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,                      //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01, 
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,                      //0x00,0x00,0x1a,0x18,0x16,0x14,0x12,0x10,0x0e,0x0c, 
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,                      //0x0a,0x08,0x00,0x00,0x00,0x00,0x1f,0xff,0xff,0xff, 
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,                      //0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,                      //0xff,0x00,0x00,0x02,0x04,0x05,0x06,0x08,0x0a,0x0c, 
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,                      //0x0e,0x1d,0x1e,0x1f,0x20,0x22,0x24,0x28,0x29,0xff, 
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,                      //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff, 
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,                      //0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf6, 
	0XFF,0XFF,0XFF,0XFF,0x00,0x01                                           //0xff,0xff,0xff,0xff,
};                                                                        //}

const uint16_t GT9147_TPX_TBL[5] = { GT9147_TP1_XL_REG, GT9147_TP2_XL_REG, GT9147_TP3_XL_REG, GT9147_TP4_XL_REG, GT9147_TP5_XL_REG };

/** GT9147单字节写入
 *   REG_Address：要写入的寄存器地址
 *   REG_data：要写入的数据
 */
static bool GT9147_Write_Byte(uint16_t REG_Address, uint8_t REG_data)
{
	taskENTER_CRITICAL();
	if(!I2C_Start(I2C_GT9147)) { taskEXIT_CRITICAL(); return false; }
	I2C_SendByte(I2C_GT9147, GT9147_ADDR);   //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_GT9147)){I2C_Stop(I2C_GT9147); taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(I2C_GT9147, REG_Address >> 8);   // 发送高8位地址
	I2C_WaitAck(I2C_GT9147);
	I2C_SendByte(I2C_GT9147, REG_Address & 0xff);   // 发送低8位地址
	I2C_WaitAck(I2C_GT9147);
	I2C_SendByte(I2C_GT9147, REG_data);
	if(!I2C_WaitAck(I2C_GT9147)){I2C_Stop(I2C_GT9147); taskEXIT_CRITICAL(); return false;}
	I2C_Stop(I2C_GT9147); 
	vTaskDelay(2);
	taskEXIT_CRITICAL();
	return true;
}

/**
 * GT9147单字节读取
 *   REG_Address：要读取的寄存器地址
 */
static uint8_t GT9147_Read_Byte(uint16_t REG_Address)
{
	taskENTER_CRITICAL();
	uint8_t REG_data;     	
	if(!I2C_Start(I2C_GT9147)) {taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(I2C_GT9147, GT9147_ADDR);
	if(!I2C_WaitAck(I2C_GT9147)){I2C_Stop(I2C_GT9147); taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(I2C_GT9147, REG_Address >> 8);   // 发送高8位地址
	if(!I2C_WaitAck(I2C_GT9147)){I2C_Stop(I2C_GT9147); taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(I2C_GT9147, REG_Address & 0xff); // 发送低8位地址
	if(!I2C_WaitAck(I2C_GT9147)){I2C_Stop(I2C_GT9147); taskEXIT_CRITICAL(); return false;}
	I2C_Start(I2C_GT9147);
	I2C_SendByte(I2C_GT9147, GT9147_ADDR+1);
	if(!I2C_WaitAck(I2C_GT9147)){I2C_Stop(I2C_GT9147); taskEXIT_CRITICAL(); return false;}

	REG_data = I2C_RadeByte(I2C_GT9147);
	I2C_NoAck(I2C_GT9147);
	I2C_Stop(I2C_GT9147);
	taskEXIT_CRITICAL();
	return REG_data;
}

/**
 * GT9147读取n个字节
 *   REG_Address：要读取的寄存器地址
 *   buf：读取后存储数据的buf
 *   len：要读取数据长度
 */
static bool GT9147_Read_NByte(uint16_t REG_Address, uint8_t* buf, uint8_t len)
{
  if(!I2C_Start(I2C_GT9147))return false;
	I2C_SendByte(I2C_GT9147, GT9147_ADDR);  //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_GT9147)){I2C_Stop(I2C_GT9147); return false;}
	I2C_SendByte(I2C_GT9147, REG_Address >> 8);   // 发送高8位地址
	I2C_WaitAck(I2C_GT9147);
	I2C_SendByte(I2C_GT9147, REG_Address &0xff);   // 发送低8位地址
	I2C_WaitAck(I2C_GT9147);
	I2C_Stop(I2C_GT9147);
	I2C_Start(I2C_GT9147);
	I2C_SendByte(I2C_GT9147, GT9147_ADDR | 1); // 读操作
	I2C_WaitAck(I2C_GT9147);
	for(uint16_t i=0; i<len; i++)
	{
	  buf[i] = I2C_RadeByte(I2C_GT9147);
		if(i<len-1)
		  I2C_Ack(I2C_GT9147);
	}	
	I2C_NoAck(I2C_GT9147);
	I2C_Stop(I2C_GT9147);
	return true;
}

/**
 * GT9147写入n个字节
 *   REG_Address：要写入的寄存器地址
 *   buf：要写入数据的buf
 *   len：要写入数据长度
 */
static bool GT9147_Write_NByte(uint16_t REG_Address, uint8_t* buf, uint8_t len)
{
	taskENTER_CRITICAL();
	if(!I2C_Start(I2C_GT9147)) { taskEXIT_CRITICAL(); return false; }
	I2C_SendByte(I2C_GT9147, GT9147_ADDR);   //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_GT9147)){I2C_Stop(I2C_GT9147); taskEXIT_CRITICAL(); return false;}
	I2C_SendByte(I2C_GT9147, REG_Address >> 8);   // 发送高8位地址
	I2C_WaitAck(I2C_GT9147);	
	I2C_SendByte(I2C_GT9147, REG_Address & 0xff);   // 发送低8位地址
	I2C_WaitAck(I2C_GT9147);
	for(uint8_t i=0; i<len; i++)
	{
		I2C_SendByte(I2C_GT9147, buf[i]);
		if(!I2C_WaitAck(I2C_GT9147)){I2C_Stop(I2C_GT9147); taskEXIT_CRITICAL(); return false;}
	}
	I2C_Stop(I2C_GT9147); 
	vTaskDelay(2);
	taskEXIT_CRITICAL();
	return true;
}

// 检查触摸屏是否正确
bool GT9147_Check(void)
{
	uint8_t uuid[4]; 
	GT9147_Read_NByte(GT9147_PID_REG, uuid, 4); // 读产品Id
	
	GT9147_Write_Byte(GT9147_CTRL_REG, 0x02); // 软复位GT9147
	
	uint8_t config_version = GT9147_Read_Byte(GT9147_CFGS_REG); // 读配置版本
	
	if(config_version<0x60) // 如果版本过低，需要更新flash配置
	{
		for(uint8_t i=0; i<sizeof(GT9147_CFG_TBL)-2; i++)
			GT9147_CFG_TBL[sizeof(GT9147_CFG_TBL)-2] += GT9147_CFG_TBL[i]; // 计算校验和
	
    GT9147_CFG_TBL[sizeof(GT9147_CFG_TBL)-2] = (~GT9147_CFG_TBL[sizeof(GT9147_CFG_TBL)-2]) + 1;
	
		GT9147_Write_NByte(GT9147_CFGS_REG, (uint8_t *)GT9147_CFG_TBL, sizeof(GT9147_CFG_TBL)); // 发送寄存器配置
	}
	
	ts_param.id = 0x9147;
	return (uuid[0] == '9') && (uuid[1] == '1') && (uuid[2] == '4') && (uuid[3] == '7');
}

// 触摸屏初始化
void GT9147_Init(void)
{
	I2C_GT9147 = I2C_4;
	
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE); // 使能GPIOB、GPIOC时钟
		
	// GT9147复位引脚初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GT9147_RST(0); // 触摸屏复位
	vTaskDelay(10); 
	
	// GT9147中断引脚初始化为输出高电平
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GT9147_INT(1); // 中断引脚输出高
	vTaskDelay(1);
	
  GT9147_RST(1); // 释放触摸屏复位  
	vTaskDelay(10);
 	
	// GT9147中断引脚初始化为浮空输入
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
void GT9147_SetTouchDirection(DIRECTION direction, uint16_t width, uint16_t height)
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
void GT9147_Scan(void)
{
	uint8_t gstid = GT9147_Read_Byte(GT9147_GSTID_REG); // 读取触摸状态
	if(gstid & 0x80)
	{
		ts.number = gstid & 0x0f;
		
		for(uint8_t i=0; i<5; i++)
		{
			if( i < (gstid & 0x0f) ) // 该触摸点有触摸，读取坐标信息
			{
				GT9147_Read_NByte(GT9147_TPX_TBL[i], (uint8_t *)&ts.point[i], 4);
				
				GT9147_TransX(&ts.point[i], ts.point[i].x, ts.point[i].y);
				
							// 设置Flags
				if(ts.point[i].flags == MTOUCH_FLAG_UP)
				{
					ts.point[i].flags = MTOUCH_FLAG_DOWN;
				}
				else if(ts.point[i].flags == MTOUCH_FLAG_DOWN)
				{
					ts.point[i].flags = MTOUCH_FLAG_MOVE;
				}
				ts.point[i].id = i;
			}
			else // 没有触摸点，清除坐标信息
			{
				ts.point[i].x = 0xFFFF;
				ts.point[i].y = 0xFFFF;
				ts.point[i].flags = MTOUCH_FLAG_UP;
				ts.point[i].id = -1;
			}
		}
		GT9147_Write_Byte(GT9147_GSTID_REG, 0); // 写0，清除状态寄存器
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
void GT9147_TransX(Point* point, uint16_t x, uint16_t y)
{
	switch(ts_param.direction)
	{
		case UP:
			point->x = x;
			point->y = y;
			break;
		case LEFT:
			point->x = ts_param.width - y;
			point->y = x;
			break;
		case DOWN:
			point->x = ts_param.width - x;
			point->y = ts_param.height - y;
			break;
		case RIGHT:
			point->x = y;
			point->y = ts_param.height - x;
			break;
	}
}

// 获取触摸点X轴坐标
int GT9147_MeasureX()
{
	return ts.point[0].x;
}

// 获取触摸点Y轴坐标
int GT9147_MeasureY()
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
* GT9147 中断信号输出时序为（以输出上升沿为例，下降沿与此时序类同）：                                  *
* 1、待机时 INT 脚输出低。                                                                             *
* 2、 有坐标更新时，输出上升沿。                                                                       *
* 3、 2 中输出上升沿后， INT 脚会保持高直到下一个周期（该周期可由配置 Refresh_Rate 决定）。 请在一个   *
* 周期内将坐标读走并将 buffer status(0x814E)写为 0。                                                   *
* 4、 2 中输出上升沿后， 若主控未在一个周期内读走坐标， 下次 GT9147 即使检测到坐标更新会再输出一个     *
* INT 脉冲但不更新坐标。                                                                               *
* 5、若主控一直未读走坐标，则 GT9147 会一直打 INT 脉冲                                                 *
*******************************************************************************************************/