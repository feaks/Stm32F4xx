#include "bsp_hard_i2c.h"
#include "stm32f4xx_gpio.h"

#define I2C_SLAVE_ADDRESS7      0xA0

uint8_t aTxBuffer[] = "I2C DMA Example: Communication between two I2C using DMA";
uint8_t aRxBuffer [256];
#define I2C1_DR_Address	((uint32_t)0x40005410)
#define I2C2_DR_Address	((uint32_t)0x40005810)
#define I2C3_DR_Address	((uint32_t)0x40005C10)

DMA_InitTypeDef  DMA_InitStructure;

void HARD_I2C_Init(I2C_TypeDef * I2Cx)
{
	// 时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); /* 打开 GPIO 时钟 */
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); /* 打开 DMA 时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE); /* 打开 I2C 时钟 */
	
	// 引脚复用映射
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2); /* 将 PB8 映射为 I2C_SCL */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2); /* 将 PB9 映射为 I2C_SDA */
	
	/* GPIO Configuration */
  /*Configure I2C SCL pin */
	GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; /* 复用模式 */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /* 速度50M */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; /* 输出类型为开漏输出 */
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; /* 浮空模式 */
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
//	/* =============================== DMA Configuration ===============================*/
// 	/* Init DMA for Transmission */
//  DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6 | DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_HTIF6);  /* Clear any pending flag on Tx Stream  */
//  DMA_Cmd(DMA1_Stream6, DISABLE); /* Disable the I2C Tx DMA stream */
//  DMA_DeInit(DMA1_Stream6);  /* Configure the DMA stream for the I2C peripheral TX direction */
//  DMA_InitStructure.DMA_Channel = DMA_Channel_1; /* Initialize the DMA_Channel member */
//  DMA_InitStructure.DMA_PeripheralBaseAddr = I2C1_DR_Address; /* Initialize the DMA_PeripheralBaseAddr member */
//	DMA_InitStructure.DMA_Memory0BaseAddr = 0; /* Initialize the DMA_Memory0BaseAddr member */
//	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; /* Initialize the DMA_DIR member */
//	DMA_InitStructure.DMA_BufferSize = 0xFFFF; /* Initialize the DMA_BufferSize member */
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; /* Initialize the DMA_PeripheralInc member */
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; /* Initialize the DMA_MemoryInc member */
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  /* Initialize the DMA_PeripheralDataSize member */
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; /* Initialize the DMA_MemoryDataSize member */
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; /* Initialize the DMA_Mode member */
//  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; /* Initialize the DMA_Priority member */
//  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable; /* Initialize the DMA_FIFOMode member */
//  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; /* Initialize the DMA_FIFOThreshold member */
//  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single; /* Initialize the DMA_MemoryBurst member */
//  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; /* Initialize the DMA_PeripheralBurst member */
//  DMA_Init(DMA1_Stream6, &DMA_InitStructure);
//	
//	/* Init DMA for Reception */
//  DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0 | DMA_FLAG_FEIF0 | DMA_FLAG_DMEIF0 | DMA_FLAG_TEIF0 | DMA_FLAG_HTIF0); /* Clear any pending flag on Rx Stream  */
//	DMA_Cmd(DMA1_Stream0, DISABLE); /* Disable the I2C Rx DMA stream */
//  DMA_DeInit(DMA1_Stream0);
//	DMA_InitStructure.DMA_Channel = DMA_Channel_1; /* Initialize the DMA_Channel member */
//  DMA_InitStructure.DMA_PeripheralBaseAddr = I2C1_DR_Address; /* Initialize the DMA_PeripheralBaseAddr member */
//	DMA_InitStructure.DMA_Memory0BaseAddr = 0; /* Initialize the DMA_Memory0BaseAddr member */
//	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; /* Initialize the DMA_DIR member */
//	DMA_InitStructure.DMA_BufferSize = 0xFFFF; /* Initialize the DMA_BufferSize member */
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; /* Initialize the DMA_PeripheralInc member */
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; /* Initialize the DMA_MemoryInc member */
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  /* Initialize the DMA_PeripheralDataSize member */
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; /* Initialize the DMA_MemoryDataSize member */
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; /* Initialize the DMA_Mode member */
//  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; /* Initialize the DMA_Priority member */
//  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable; /* Initialize the DMA_FIFOMode member */
//  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; /* Initialize the DMA_FIFOThreshold member */
//  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single; /* Initialize the DMA_MemoryBurst member */
//  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; /* Initialize the DMA_PeripheralBurst member */
//  DMA_Init(DMA1_Stream0, &DMA_InitStructure);

//	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);
//	DMA_ITConfig(DMA1_Stream0, DMA_IT_TC, ENABLE);

	/* I2C Struct Initialize */
	I2C_DeInit(I2C2);
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; // I2C_DutyCycle_16_9
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	
	// I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE); // 使能I2C错误中断
	
  I2C_Cmd(I2C2, ENABLE); /* 使能 I2C1 */

	I2C_Init(I2C2, &I2C_InitStructure); /* I2C1 初始化 */

	// I2C_DMACmd(I2C1, ENABLE); /* I2C1 DMA Enable */
	
	/* Configure I2C Filters */
  I2C_AcknowledgeConfig(I2C2, ENABLE); /*允许1字节1应答模式*/
}

void HARD_I2C_Write_REG(I2C_TypeDef * I2Cx, uint8_t Slave_Address, uint8_t REG_Address, uint8_t REG_data)
{
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)); // 设备忙等待

	I2C_GenerateSTART(I2Cx, ENABLE); // 发送开始信号

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)); /* BUSY, MSL and SB flag */

	I2C_Send7bitAddress(I2Cx, Slave_Address, I2C_Direction_Transmitter); // 发送从设备地址 + 写信号

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); /* BUSY, MSL, ADDR, TXE and TRA flags */

	I2C_SendData(I2Cx, REG_Address); // 发送寄存器地址

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)); /* TRA, BUSY, MSL, TXE flags */
	
	I2C_SendData(I2Cx, REG_data); // 发送数据
	
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)); /* TRA, BUSY, MSL, TXE flags */
	
	I2C_GenerateSTOP(I2Cx, ENABLE); // 发送停止信号
	
	I2C_AcknowledgeConfig(I2Cx, DISABLE); // 无应答
	
//	DMA_InitStructure.DMA_Channel = DMA_Channel_1;
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)WriteBuf;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//	DMA_InitStructure.DMA_BufferSize = (uint32_t)(*NumByte);
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	DMA_Init(DMA1_Stream6, &DMA_InitStructure);

//	DMA_Cmd(DMA1_Stream6, ENABLE);
}


uint8_t HARD_I2C_Read_REG(I2C_TypeDef * I2Cx, uint8_t Slave_Address, uint8_t REG_Address)
{
	uint8_t REG_data; // 获取的数据
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); // 检查总线是否忙

	I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 允许1字节1应答模式
	
	I2C_GenerateSTART(I2Cx, ENABLE); // 发送开始信号

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)); /* BUSY, MSL and SB flag */

	I2C_Send7bitAddress(I2Cx, Slave_Address, I2C_Direction_Transmitter); // 发送从设备地址 + 写信号

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); /* BUSY, MSL, ADDR, TXE and TRA flags */

	I2C_SendData(I2Cx, REG_Address); // 发送寄存器地址

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); /* TRA, BUSY, MSL, TXE and BTF flags */

	I2C_GenerateSTART(I2Cx, ENABLE); // 发送开始信号

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)); /* BUSY, MSL and SB flag */

	I2C_Send7bitAddress(I2Cx, Slave_Address, I2C_Direction_Receiver);  // 发送从设备地址 + 读信号

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){}; /* BUSY, MSL and ADDR flags */

	I2C_AcknowledgeConfig(I2Cx, DISABLE);  // 禁止1字节1应答模式

	I2C_GenerateSTOP(I2Cx, ENABLE); // 发送停止信号

	while(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))){}; /* BUSY, MSL and RXNE flags */
	
	REG_data = I2C_ReceiveData(I2Cx);
	
	I2C_AcknowledgeConfig(I2Cx, ENABLE); // 允许1字节1应答模式
		
	return REG_data;
}

void HARD_I2C_Write_NByte(I2C_TypeDef * I2Cx, uint8_t Slave_Address, uint8_t REG_Address, uint8_t * buffer, uint8_t len)
{
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)); // 设备忙等待

	I2C_GenerateSTART(I2Cx, ENABLE); // 发送开始信号

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)); /* BUSY, MSL and SB flag */

	I2C_Send7bitAddress(I2Cx, Slave_Address, I2C_Direction_Transmitter); // 发送从设备地址 + 写信号

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); /* BUSY, MSL, ADDR, TXE and TRA flags */

	I2C_SendData(I2Cx, REG_Address); // 发送寄存器地址

	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)); /* TRA, BUSY, MSL, TXE flags */
	
	for(uint8_t i=0; i<len; i++)
	{
		I2C_SendData(I2Cx, buffer[i]); // 发送数据
	
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)); /* TRA, BUSY, MSL, TXE flags */
	}
	
	I2C_GenerateSTOP(I2Cx, ENABLE); // 发送停止信号
	
	I2C_AcknowledgeConfig(I2Cx, DISABLE); // 无应答
}

#if 0
void I2C1_ER_IRQHandler()  // I2C1错误中断处理
{
	uint8_t error_num = 0;
	if(I2C_GetITStatus(I2C1, I2C_IT_TIMEOUT) != RESET) // Timeout or Tlow error flag
	{
		error_num = 1;
		I2C_ClearITPendingBit(I2C1, I2C_IT_TIMEOUT);
		
	} 
	else if(I2C_GetITStatus(I2C1, I2C_IT_PECERR) != RESET) // PEC error in reception flag
	{
		error_num = 2;
		I2C_ClearITPendingBit(I2C1, I2C_IT_PECERR);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_OVR) != RESET) // Overrun/Underrun flag (Slave mode)
	{
		error_num = 3;	
		I2C_ClearITPendingBit(I2C1, I2C_IT_OVR);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_AF) != RESET) // Acknowledge failure flag
	{
		error_num = 4;
		I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
	} 
	else if(I2C_GetITStatus(I2C1, I2C_IT_ARLO) != RESET) // Arbitration lost flag (Master mode)
	{
		error_num = 5;
		I2C_ClearITPendingBit(I2C1, I2C_IT_ARLO);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_BERR) != RESET) // Bus error flag
	{
		error_num = 6;
		I2C_ClearITPendingBit(I2C1, I2C_IT_BERR);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_TXE) != RESET) // Data register empty flag (Transmitter)
	{
		error_num = 7;
		I2C_ClearITPendingBit(I2C1, I2C_IT_TXE);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_RXNE) != RESET) // Data register not empty (Receiver) flag
	{
		error_num = 8;
		I2C_ClearITPendingBit(I2C1, I2C_IT_RXNE);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_STOPF) != RESET) // Stop detection flag (Slave mode)
	{
		error_num = 9;
		I2C_ClearITPendingBit(I2C1, I2C_IT_STOPF);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_ADD10) != RESET) // 10-bit header sent flag (Master mode)
	{
		error_num = 10;
		I2C_ClearITPendingBit(I2C1, I2C_IT_ADD10);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_BTF) != RESET) // Byte transfer finished flag
	{
		error_num = 11;
		I2C_ClearITPendingBit(I2C1, I2C_IT_BTF);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_ADDR) != RESET) // Address sent flag (Master mode) "ADSL"    Address matched flag (Slave mode)"ENDAD"
	{
		error_num = 12;
		I2C_ClearITPendingBit(I2C1, I2C_IT_ADDR);
	}
	else if(I2C_GetITStatus(I2C1, I2C_IT_SB) != RESET) // Start bit flag (Master mode)
	{
		error_num = 13;
		I2C_ClearITPendingBit(I2C1, I2C_IT_SB);
	}	
	I2C_SoftwareResetCmd(I2C1, ENABLE);
	I2C_GenerateSTOP(I2C1, ENABLE);
}

uint32_t I2C_DMA_Read(uint8_t* ReadBuf, uint8_t SlaveAddr, uint8_t ReadAddr, uint8_t* NumByte )
{
	I2C_ReadPtr = NumByte;

	I2C_TimeCnt = I2C_TIME;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) if((I2C_TimeCnt--) == 0)	return ERROR; // 检查总线是否忙

	I2C_GenerateSTART(I2C1, ENABLE); // 发送开始信号

	I2C_TimeCnt = I2C_TIME;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) if((I2C_TimeCnt--) == 0)	return ERROR; // 等待应答

	I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Transmitter); // 发送从设备地址

	I2C_TimeCnt = I2C_TIME;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) if((I2C_TimeCnt--) == 0)	return ERROR; // 检查主发送模式切换

	I2C_SendData(I2C1, ReadAddr); // 发送寄存器地址

	I2C_TimeCnt = I2C_TIME;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET) if((I2C_TimeCnt--) == 0) return ERROR; // 等待数据发送完成

	I2C_GenerateSTART(I2C1, ENABLE); // 发送开始信号

	I2C_TimeCnt = I2C_TIME;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) if((I2C_TimeCnt--) == 0)	return ERROR; // 等待应答

	I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Receiver);  // 发送从设备地址 + 读信号

	if((uint16_t)(*NumByte) < 2) 
	{
		I2C_TimeCnt = I2C_TIME;
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET) if((I2C_TimeCnt--) == 0)	return ERROR;

		I2C_AcknowledgeConfig(I2C1, DISABLE);  // 发送无应答信号
		(void)I2C1->SR2;

		I2C_GenerateSTOP(I2C1, ENABLE); // 发送停止信号

		I2C_TimeCnt = I2C_TIME;
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET) if((I2C_TimeCnt--) == 0)	return ERROR; // 等待数据寄存器非空

		*ReadBuf = I2C_ReceiveData(I2C1);  // 获取接收到的数据
		(u16)(*NumByte)--;

		I2C_TimeCnt = I2C_TIME;	while(I2C1->CR1 & I2C_CR1_STOP) if((I2C_TimeCnt--) == 0)	return ERROR; // 发送停止信号

		I2C_AcknowledgeConfig(I2C1, ENABLE); // 发送应答信号
	}
	else 
	{
		I2C_TimeCnt = I2C_TIME;
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) if((I2C_TimeCnt--) == 0)	return ERROR; // 检查是否处于接收状态

		DMA_InitStructure.DMA_Channel = DMA_Channel_1;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ReadBuf;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
		DMA_InitStructure.DMA_BufferSize = (uint32_t)(*NumByte);
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA1_Stream0, &DMA_InitStructure);

		I2C_DMALastTransferCmd(I2C1, ENABLE);

		DMA_Cmd(DMA1_Stream0, ENABLE);
	}

	I2C_TimeCnt = I2C_TIME;
	while(*NumByte > 0) if((I2C_TimeCnt--) == 0)	return ERROR; // 等待数据接收完成

	return SUCCESS;
}


uint32_t I2C_DMA_ReadReg(uint8_t* ReadBuf, uint8_t SlaveAddr, uint8_t ReadAddr, uint8_t NumByte )
{
	I2C_DMA_Read(ReadBuf, SlaveAddr, ReadAddr, (uint8_t*)(&NumByte));

	I2C_TimeCnt = I2C_TIME;
	while(NumByte > 0) if((I2C_TimeCnt--) == 0)	return ENABLE; // 等待数据接收完成

	return SUCCESS;
}

void DMA1_Stream7_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_Stream7, DMA_FLAG_TCIF7) != RESET) {
		DMA_Cmd(DMA1_Stream7, DISABLE);
		DMA_ClearFlag(DMA1_Stream7, DMA_FLAG_TCIF7);

		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); /* TRA, BUSY, MSL, TXE and BTF flags */
		
		I2C_GenerateSTOP(I2C2, ENABLE); // 发送停止信号
	
		I2C_AcknowledgeConfig(I2C2, DISABLE); // 无应答
	}
}

uint32_t I2C_DMA_WriteReg(uint8_t* WriteBuf, uint8_t SlaveAddr, uint8_t WriteAddr, uint8_t NumByte )
{
	I2C_DMA_Write(WriteBuf, SlaveAddr, WriteAddr, (uint8_t*)(&NumByte));

	I2C_TimeCnt = I2C_TIME;
	while(NumByte > 0) if((I2C_TimeCnt--) == 0)	return ERROR;

	return SUCCESS;
}

void I2C1_Send_DMA_IRQ(void)
{
	if(DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) != RESET) {
		DMA_Cmd(DMA1_Stream6, DISABLE);
		DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);

		I2C_TimeCnt = I2C_TIME;
		while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)) if((I2C_TimeCnt--) == 0) while(1){}; // 等待数据发送完成

		I2C_GenerateSTOP(I2C1, ENABLE); // 发送停止信号
		*I2C_WritePtr = 0;
	}
}

void I2C1_Recv_DMA_IRQ(void)
{
	if(DMA_GetFlagStatus(DMA1_Stream0, DMA_FLAG_TCIF0) != RESET) {
		I2C_GenerateSTOP(I2C1, ENABLE);
		DMA_Cmd(DMA1_Stream0, DISABLE);
		DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0);
		*I2C_ReadPtr = 0;
	}
}



void I2C2_Recv_DMA_IRQ(void)
{
	if(DMA_GetFlagStatus(DMA1_Stream0, DMA_FLAG_TCIF0) != RESET) {
		I2C_GenerateSTOP(I2C1, ENABLE);
		DMA_Cmd(DMA1_Stream0, DISABLE);
		DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0);
		*I2C_ReadPtr = 0;
	}
}

#endif
//void Master_Init(void) 
//{
//	
//	/*************************************Master Code****************************/
//#if defined (I2C_MASTER)
//  /* I2C Struct Initialize */
//  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//  I2C_InitStructure.I2C_DutyCycle = I2C_DUTYCYCLE;
//  I2C_InitStructure.I2C_OwnAddress1 = 0xA0;
//  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
//  
//	#ifndef I2C_10BITS_ADDRESS
//		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//	#else
//		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
//	#endif /* I2C_10BITS_ADDRESS */
//  
//  /* I2C Initialize */
//  I2C_Init(I2Cx, &I2C_InitStructure);
//  
//  /* Master Transmitter ------------------------------------------------------*/   
//  
//  /* Generate the Start condition */
//  I2C_GenerateSTART(I2Cx, ENABLE);
//  
//	#ifdef I2C_10BITS_ADDRESS  
//		/* Test on I2C1 EV5 and clear it */
//		TimeOut = USER_TIMEOUT;
//		while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
//		{} 
//		if(TimeOut == 0)
//		{
//			TimeOut_UserCallback();
//		}
//		/* Send Header to I2Cx for write or time out */
//		I2C_SendData(I2Cx, HEADER_ADDRESS_Write);
//		/* Test on I2Cx EV9 and clear it */
//		TimeOut = USER_TIMEOUT;
//		while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_ADDRESS10))&&(TimeOut != 0x00))
//		{} 
//		if(TimeOut == 0)
//		{
//			TimeOut_UserCallback();
//		}
//		/* Send I2Cx slave Address for write */
//		I2C_Send7bitAddress(I2Cx, (uint8_t)SLAVE_ADDRESS, I2C_Direction_Transmitter);
//		
//	 #else /* I2C_7BITS_ADDRESS */
//				
//		/* Test on I2Cx EV5 and clear it or time out */
//		TimeOut = USER_TIMEOUT;
//		while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
//		{} 
//		if(TimeOut == 0)
//		{
//			TimeOut_UserCallback();
//		}
//		/* Send I2Cx slave Address for write */
//		I2C_Send7bitAddress(I2Cx, SLAVE_ADDRESS, I2C_Direction_Transmitter);
//				
//	#endif /* I2C_10BITS_ADDRESS */
//  
//  /* Test on I2Cx EV6 and clear it or time out */
//  TimeOut = USER_TIMEOUT;
//  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&(TimeOut != 0x00))
//  {} 
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  /* I2Cx DMA Enable */
//  I2C_DMACmd(I2Cx, ENABLE);
//  
//  /* Enable DMA TX Channel */
//  DMA_Cmd(I2Cx_DMA_STREAM_TX, ENABLE);
//  
//  /* Wait until I2Cx_DMA_STREAM_TX enabled or time out */
//  TimeOut = USER_TIMEOUT;
//  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_TX)!= ENABLE)&&(TimeOut != 0x00))
//  {}  
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  
//  /* Wait until DMA Transfer Complete or time out */
//  TimeOut = USER_TIMEOUT;
//  while ((DMA_GetFlagStatus(I2Cx_DMA_STREAM_TX,I2Cx_TX_DMA_TCFLAG) == RESET)&&(TimeOut != 0x00))
//  {}
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  
//  /* I2Cx DMA Disable */
//  I2C_DMACmd(I2Cx, DISABLE);
//  
//  /* Wait until BTF Flag is set before generating STOP or time out */
//  TimeOut = USER_TIMEOUT;
//  while ((!I2C_GetFlagStatus(I2Cx,I2C_FLAG_BTF))&&(TimeOut != 0x00))  
//  {}
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  
//  /* Send I2Cx STOP Condition */
//  I2C_GenerateSTOP(I2Cx, ENABLE);

//  /* Disable DMA TX Channel */
//  DMA_Cmd(I2Cx_DMA_STREAM_TX, DISABLE);
//  
//  /* Wait until I2Cx_DMA_STREAM_TX disabled or time out */
//  TimeOut = USER_TIMEOUT;
//  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_TX)!= DISABLE)&&(TimeOut != 0x00))
//  {}  
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  
//  /* Clear any pending flag on Tx Stream  */
//  DMA_ClearFlag(I2Cx_DMA_STREAM_TX, I2Cx_TX_DMA_TCFLAG | I2Cx_TX_DMA_FEIFLAG | I2Cx_TX_DMA_DMEIFLAG | \
//                                       I2Cx_TX_DMA_TEIFLAG | I2Cx_TX_DMA_HTIFLAG);
//  
//  /* Master Receiver ---------------------------------------------------------*/ 
//  
//  /* Enable DMA NACK automatic generation */
//  I2C_DMALastTransferCmd(I2Cx, ENABLE);
//  
//  /* Send I2Cx START condition */
//  I2C_GenerateSTART(I2Cx, ENABLE);
//  
//#ifdef I2C_10BITS_ADDRESS  
//  /* Test on EV5 and clear it or time out */
//  TimeOut = USER_TIMEOUT;
//  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
//  {}
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  /* Send Header to Slave for write */
//  I2C_SendData(I2Cx, HEADER_ADDRESS_Write);

//  /* Test on EV9 and clear it or time out */
//  TimeOut = USER_TIMEOUT;
//  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_ADDRESS10))&&(TimeOut != 0x00))
//  {}
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  /* Send slave Address */
//  I2C_Send7bitAddress(I2Cx, (uint8_t)SLAVE_ADDRESS, I2C_Direction_Transmitter);

//  /* Test on I2Cx EV6 and clear it or time out*/
//  TimeOut = USER_TIMEOUT;
//  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&(TimeOut != 0x00))
//  {} 
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  /* Repeated Start */
//  I2C_GenerateSTART(I2Cx, ENABLE);

//  /* Test on EV5 and clear it or time out*/
//  TimeOut = USER_TIMEOUT;
//  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
//  {} 
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  /* Send Header to Slave for Read */
//  I2C_SendData(I2Cx, HEADER_ADDRESS_Read);

//#else /* I2C_7BITS_ADDRESS */
//      
//  /* Test on I2Cx EV5 and clear it or time out*/
//  TimeOut = USER_TIMEOUT;
//  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))&&(TimeOut != 0x00))
//  {}
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  /* Send I2Cx slave Address for write */
//  I2C_Send7bitAddress(I2Cx, SLAVE_ADDRESS, I2C_Direction_Receiver);
//      
//#endif /* I2C_10BITS_ADDRESS */
//  
//  /* Test on I2Cx EV6 and clear it or time out */
//  TimeOut = USER_TIMEOUT;
//  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))&&(TimeOut != 0x00))
//  {}
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  /* I2Cx DMA Enable */
//  I2C_DMACmd(I2Cx, ENABLE);
//  
//  /* Enable DMA RX Channel */
//  DMA_Cmd(I2Cx_DMA_STREAM_RX, ENABLE);
//  
//  /* Wait until I2Cx_DMA_STREAM_RX enabled or time out */
//  TimeOut = USER_TIMEOUT;
//  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_RX)!= ENABLE)&&(TimeOut != 0x00))
//  {}  
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  
//  /* Transfer complete or time out */
//  TimeOut = USER_TIMEOUT;
//  while ((DMA_GetFlagStatus(I2Cx_DMA_STREAM_RX,I2Cx_RX_DMA_TCFLAG)==RESET)&&(TimeOut != 0x00))
//  {}
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  /* Send I2Cx STOP Condition */
//  I2C_GenerateSTOP(I2Cx, ENABLE);

//  /* Disable DMA RX Channel */
//  DMA_Cmd(I2Cx_DMA_STREAM_RX, DISABLE);
//  
//  /* Wait until I2Cx_DMA_STREAM_RX disabled or time out */
//  TimeOut = USER_TIMEOUT;
//  while ((DMA_GetCmdStatus(I2Cx_DMA_STREAM_RX)!= DISABLE)&&(TimeOut != 0x00))
//  {}  
//  if(TimeOut == 0)
//  {
//    TimeOut_UserCallback();
//  }
//  
//  /* Disable I2C DMA request */  
//  I2C_DMACmd(I2Cx,DISABLE);
//  
//  /* Clear any pending flag on Rx Stream  */
//  DMA_ClearFlag(I2Cx_DMA_STREAM_RX, I2Cx_RX_DMA_TCFLAG | I2Cx_RX_DMA_FEIFLAG | I2Cx_RX_DMA_DMEIFLAG | \
//                                       I2Cx_RX_DMA_TEIFLAG | I2Cx_RX_DMA_HTIFLAG);
//  
//  if (Buffercmp(aTxBuffer, aRxBuffer, RXBUFFERSIZE) == PASSED)
//  {
//    /* LED2, LED3 and LED4 On */
//    STM_EVAL_LEDOn(LED2);
//    STM_EVAL_LEDOn(LED3);
//    STM_EVAL_LEDOn(LED4);
//  }
//  else 
//  {   /* LED2, LED3 and LED4 Off */
//    STM_EVAL_LEDOff(LED2);
//    STM_EVAL_LEDOff(LED3);
//    STM_EVAL_LEDOff(LED4);
//  }
//  
//#endif /* I2C_MASTER */
//}