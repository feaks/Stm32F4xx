#ifndef _BSP_HARD_I2C_H_
#define _BSP_HARD_I2C_H_

#include "stdint.h"
#include "stm32f4xx_i2c.h"

void HARD_I2C_Init(I2C_TypeDef * I2Cx);
void HARD_I2C_Write_REG(I2C_TypeDef * I2Cx, uint8_t Slave_Address, uint8_t REG_Address, uint8_t REG_data);
uint8_t HARD_I2C_Read_REG(I2C_TypeDef * I2Cx, uint8_t Slave_Address, uint8_t REG_Address);

void HARD_I2C_Write_NByte(I2C_TypeDef * I2Cx, uint8_t Slave_Address, uint8_t REG_Address, uint8_t * buffer, uint8_t len);


/*
	I2C_GenerateSTART  		开始信号
	
	I2C_GenerateSTOP   		停止信号
	
	I2C_Send7bitAddress   发送从设备地址
					I2C_Direction_Transmitter 发送从设备地址+写信号   0
					I2C_Direction_Receiver    发送从设备地址+读信号   1
					
	I2C_AcknowledgeConfig      在接收一个字节之后是否返回应答
					DISABLE   无应答信号
					ENABLE    应答信号
					
	I2C_SoftwareResetCmd    软件复位
					DISABLE   I2C外设未处于复位状态
					ENABLE    I2C外设处于复位状态
	
	I2C_StretchClockCmd   禁止时钟延长（从模式）   ？？？
	
	I2C_FastModeDutyCycleConfig  快速模式下时钟占空比
	
	I2C_NACKPositionConfig    主机接收模式下配置无应答位置   (I2C-I2C模式下使用)    I2C_PECPositionConfig (SMBUS模式下使用)
					I2C_NACKPosition_Current
					I2C_NACKPosition_Next         

	=================  I2C_Group2 Data transfers functions  ================

  I2C_SendData              发送数据
		
	I2C_ReceiveData						接收数据
	
	=================  I2C_Group3 PEC management functions  ================
	
	I2C_TransmitPEC           是否传输数据包错误检验
	
	I2C_CalculatePEC          是否计算数据包错误
	
	I2C_GetPEC                获取数据包错误校验寄存器
	
	=================  I2C_Group4 DMA transfers management functions  ================
	
	I2C_DMACmd                当TxE=1或RxNE=1时，使能DMA请求
	
	I2C_DMALastTransferCmd    下一个DMA EOT是不是最后一次传输，（用于主机接收模式，可对最后接收的数据生成NACK）
	
	=================  I2C_Group5 Interrupts events and flags management functions  =================
	
	错误管理
		 -@@- For error management, it is advised to use the following functions:
        (+@@) I2C_ITConfig() to configure and enable the error interrupts (I2C_IT_ERR).
        (+@@) I2Cx_ER_IRQHandler() which is called when the error interrupt occurs.
              Where x is the peripheral instance (I2C1, I2C2 ...)
        (+@@) I2C_GetFlagStatus() or I2C_GetITStatus()  to be called into the 
              I2Cx_ER_IRQHandler() function in order to determine which error occurred.
        (+@@) I2C_ClearFlag() or I2C_ClearITPendingBit() and/or I2C_SoftwareResetCmd() 
              and/or I2C_GenerateStop() in order to clear the error flag and source 
              and return to correct  communication status.
							
				##### I2C State Monitoring Functions ##### 
	
	I2C_ReadRegister         获取寄存器值
	
	I2C_ITConfig						 中断配置   
				I2C_IT_BUF 	BUFFER中断    
				I2C_IT_EVT  EVENT中断
				I2C_IT_ERR  ERROR中断
	
	I2C_CheckEvent            I2C事件检测   I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED
	I2C_GetLastEvent					获取最后一次产生的事件
	I2C_GetFlagStatus					获取状态标志
	
	I2C_ClearFlag             清除状态标志   ==  I2C_ClearITPendingBit     清楚中断挂起
				 I2C_FLAG_SMBALERT、I2C_FLAG_TIMEOUT、I2C_FLAG_PECERR、I2C_FLAG_OVR、I2C_FLAG_AF、I2C_FLAG_ARLO、I2C_FLAG_BERR

  I2C_GetITStatus           获取中断状态

*/
#endif
