#ifndef _BSP_TOUCH_GT9147_H_
#define _BSP_TOUCH_GT9147_H_

#include "bsp_gpio.h"
#include "stdbool.h"
#include "bsp_touch.h"

#define GT9147_ADDR 0x28

// GT9147部分寄存器定义
#define GT9147_CTRL_REG 	0X8040   	//GT9147控制寄存器
/**************************GT9147_CTRL_REG***************************************
 *  0：读坐标状态             1、2：差值原始值                                  *
 *  3：基准更新（内部测试）   4：基准校准（内部测试）    5：关屏                *
 *  6：进入充电模式           7：退出充电模式            8：切换手势唤醒固件    *
 *  0x20: 进入从机接近检测模式 0x21：进入主机接近检测模式                       *
 *  0x22：进入数据传输模式     0x23: 进入主机传输模式                           *
 *  0x28：退出从机检测模式     0x29：退出主机接近检测模式                       *
 *  0x2A：退出数据传输模式                                                      *
 *  0xAA:ESD 保护机制使用，由驱动定时写入 0xAA 并定时读取检查                   *
 *  其余值无效                                                                  *
 *******************************************************************************/
 
#define GT9147_PROXI_EN   			0X8042    //GT9147接近感应开关
#define GT9147_COMMAND_CHECK   	0X8042    //GT9147命令检查            下发 0x20~0x2F 间的命令，需将命令先写此处，再写 0x8040，命令才会生效

#define GT9147_CFGS_REG 	      0X8047   	//GT9147配置文件寄存器首地址（共184个），详细配置见GT9147编程指南
#define GT9147_CHECK_REG 	      0X80FF   	//GT9147校验和寄存器
#define GT9147_PID_REG 		      0X8140   	//GT9147产品ID寄存器首地址（共4个）
                                
#define GT9147_GSTID_REG 	      0X814E   	//GT9147当前检测到的触摸情况  
/********************************GT9147_GSTID_REG**********************************************************************
 *         7               6             5         4        3               2~0                                       *
 *  | buffer status | large detect | Proximity | Valid | HaveKey | number of touch points |                           *
 * Bit7:   Buffer status， 1 表示坐标（或按键）已经准备好，主控可以读取；                                             *
 *                         0 表示未就绪，数据无效。当主控读取完坐标后，必须通过 I2C 将此标志（或整个字节）写为 0。    *
 * Bit5：  Proximity Valid，接近感应有效置 1                                                                          *
 * Bit4:   HaveKey, 1 表示有按键， 0 表示无按键（已经松键）                                                           *
 * Bit3~0: Number of touch points, 屏上的坐标点个数                                                                   *
 *********************************************************************************************************************/
 
#define GT9147_TP1_XL_REG 		     0X8150  		//第一个触摸点数据寄存器 - x轴 - 低字节
#define GT9147_TP1_XH_REG 		     0X8151  		//第一个触摸点数据寄存器 - x轴 - 高字节
#define GT9147_TP1_YL_REG 		     0X8152  		//第一个触摸点数据寄存器 - y轴 - 低字节
#define GT9147_TP1_YH_REG 		     0X8153  		//第一个触摸点数据寄存器 - y轴 - 高字节
#define GT9147_TP1_SL_REG 		     0X8154  		//第一个触摸点数据寄存器 - 点大小 - 低字节
#define GT9147_TP1_SH_REG 		     0X8155  		//第一个触摸点数据寄存器 - 点大小 - 高字节

#define GT9147_TP2_XL_REG 		     0X8158 		//第二个触摸点数据寄存器 - x轴 - 低字节
#define GT9147_TP2_XH_REG 		     0X8159 		//第二个触摸点数据寄存器 - x轴 - 高字节
#define GT9147_TP2_YL_REG 		     0X815A 		//第二个触摸点数据寄存器 - y轴 - 低字节
#define GT9147_TP2_YH_REG 		     0X815B 		//第二个触摸点数据寄存器 - y轴 - 高字节
#define GT9147_TP2_SL_REG 		     0X815C 		//第二个触摸点数据寄存器 - 点大小 - 低字节
#define GT9147_TP2_SH_REG 		     0X815D 		//第二个触摸点数据寄存器 - 点大小 - 高字节
                                   
#define GT9147_TP3_XL_REG 		     0X8160 		//第三个触摸点数据寄存器 - x轴 - 低字节
#define GT9147_TP3_XH_REG 		     0X8161 		//第三个触摸点数据寄存器 - x轴 - 高字节
#define GT9147_TP3_YL_REG 		     0X8162 		//第三个触摸点数据寄存器 - y轴 - 低字节
#define GT9147_TP3_YH_REG 		     0X8163 		//第三个触摸点数据寄存器 - y轴 - 高字节
#define GT9147_TP3_SL_REG 		     0X8164 		//第三个触摸点数据寄存器 - 点大小 - 低字节
#define GT9147_TP3_SH_REG 		     0X8165 		//第三个触摸点数据寄存器 - 点大小 - 高字节
                                   
#define GT9147_TP4_XL_REG 		     0X8168 		//第四个触摸点数据寄存器 - x轴 - 低字节
#define GT9147_TP4_XH_REG 		     0X8169 		//第四个触摸点数据寄存器 - x轴 - 高字节
#define GT9147_TP4_YL_REG 		     0X816A 		//第四个触摸点数据寄存器 - y轴 - 低字节
#define GT9147_TP4_YH_REG 		     0X816B 		//第四个触摸点数据寄存器 - y轴 - 高字节
#define GT9147_TP4_SL_REG 		     0X816C 		//第四个触摸点数据寄存器 - 点大小 - 低字节
#define GT9147_TP4_SH_REG 		     0X816D 		//第四个触摸点数据寄存器 - 点大小 - 高字节
                                   
#define GT9147_TP5_XL_REG 		     0X8170     //第五个触摸点数据寄存器 - x轴 - 低字节
#define GT9147_TP5_XH_REG 		     0X8171     //第五个触摸点数据寄存器 - x轴 - 高字节
#define GT9147_TP5_YL_REG 		     0X8172     //第五个触摸点数据寄存器 - y轴 - 低字节
#define GT9147_TP5_YH_REG 		     0X8173     //第五个触摸点数据寄存器 - y轴 - 高字节
#define GT9147_TP5_SL_REG 		     0X8174     //第五个触摸点数据寄存器 - 点大小 - 低字节
#define GT9147_TP5_SH_REG 		     0X8175     //第五个触摸点数据寄存器 - 点大小 - 高字节


#define GT9147_RST(x)   GPIO_Set(GPIOC, GPIO_Pin_13,  x) // 复位引脚
#define GT9147_INT(x)   GPIO_Set(GPIOB, GPIO_Pin_1,   x) // 中断引脚

// 检查触摸屏是否正确
bool GT9147_Check(void);

// 触摸屏初始化
void GT9147_Init(void);

// 设置触摸屏方向及宽高
void GT9147_SetTouchDirection(DIRECTION direction, uint16_t width, uint16_t height);

// 触摸屏扫描按下状态
void GT9147_Scan(void);

// 根据X，Y物理触摸点计算当前屏幕方向的触摸点
void GT9147_TransX(Point* point, uint16_t x, uint16_t y);
	
// 获取触摸点X轴坐标
int GT9147_MeasureX();

// 获取触摸点Y轴坐标
int GT9147_MeasureY();

#endif
