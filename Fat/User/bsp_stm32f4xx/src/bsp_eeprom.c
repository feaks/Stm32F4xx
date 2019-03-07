#include "FreeRTOS.h"
#include "task.h"
#include "bsp_eeprom.h"
#include "bsp_i2c.h"
#include "bsp_gpio.h"
#include "config.h"

static I2C * I2C_EERPOM;

/**
 * AT24C16初始化A0,A1,A2
 */
void EEPROM_Init(void)
{
	I2C_EERPOM = I2C_2;
	
	GPIO_Initialize(GPIOE, GPIO_Pin_10, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_Initialize(GPIOE, GPIO_Pin_12, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);
	GPIO_Initialize(GPIOE, GPIO_Pin_15, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL);
}

/**
 * EERPOM单字节写入数据
 *   REG_Address：要写入的寄存器地址
 *   REG_data：要写入的数据
 */
bool EEPROM_Write_Byte(uint16_t REG_Address,uint8_t REG_data)
{
	uint8_t dev, reg1, reg2; 
	#if (EEPROM_SIZE == 64)	
	dev = EEPROM_Address;
	reg1 = REG_Address >> 8;
	reg2 = REG_Address & 0xFF;
	#elif (EEPROM_SIZE == 16)	
	dev = EEPROM_Address | (REG_Address >> 7 & 0x0E);
	reg1 = REG_Address & 0xFF;
	#endif

	if(!I2C_Start(I2C_EERPOM))return false;
	I2C_SendByte(I2C_EERPOM, dev);   //发送设备地址+写信号
	
	if(!I2C_WaitAck(I2C_EERPOM)){I2C_Stop(I2C_EERPOM); return false;}
	I2C_SendByte(I2C_EERPOM, reg1);   //设置起始地址2
	I2C_WaitAck(I2C_EERPOM);
	
	#if (EEPROM_SIZE == 64)
	I2C_SendByte(I2C_EERPOM, reg2);   //设置起始地址2
	I2C_WaitAck(I2C_EERPOM);
	#endif
	
	I2C_SendByte(I2C_EERPOM, REG_data);
	I2C_WaitAck(I2C_EERPOM);   
	I2C_Stop(I2C_EERPOM); 
	vTaskDelay(3 / portTICK_RATE_MS);
	return true;
}

/**
 * EERPOM单字节读寄存器
 *   REG_Address：读取的寄存器地址，16K范围0~2047，64K范围0~8191
 */
uint8_t EEPROM_Read_Byte(uint16_t REG_Address)
{ 
	uint8_t dev, reg1, reg2; 
	#if (EEPROM_SIZE == 64)	
	dev = EEPROM_Address;
	reg1 = REG_Address >> 8;
	reg2 = REG_Address & 0xFF;
	#elif (EEPROM_SIZE == 16)	
	dev = EEPROM_Address | (REG_Address >> 7 & 0x0E);
	reg1 = REG_Address & 0xFF;
	#endif

	uint8_t REG_data;     	
	if(!I2C_Start(I2C_EERPOM))return false;
	I2C_SendByte(I2C_EERPOM, dev);
	if(!I2C_WaitAck(I2C_EERPOM)){I2C_Stop(I2C_EERPOM); return false;}
	
	I2C_SendByte(I2C_EERPOM, reg1);   
	I2C_WaitAck(I2C_EERPOM);
	
	#if (EEPROM_SIZE == 64)
	I2C_SendByte(I2C_EERPOM, reg2);   
	I2C_WaitAck(I2C_EERPOM);
	#endif
	
	I2C_Start(I2C_EERPOM);
	I2C_SendByte(I2C_EERPOM, dev+1);
	I2C_WaitAck(I2C_EERPOM);

	REG_data = I2C_RadeByte(I2C_EERPOM);
	I2C_NoAck(I2C_EERPOM);
	I2C_Stop(I2C_EERPOM);
	return REG_data;
}

/**
 * EERPOM单页写入数据
 *  16K：
 *     page：写入的页码，范围0~127
 *     offset：写入该页的偏移量 范围0~15
 *     data：需要存储的数据
 *     len：数据长度，范围1~16
 *  64K：
 *     page：写入的页码，范围0~255
 *     offset：写入该页的偏移量 范围0~31
 *     data：需要存储的数据
 *     len：数据长度，范围1~32
 */
bool EEPROM_Write_Page(uint8_t page, uint8_t offset, uint8_t * data, uint8_t len)
{
	if(!I2C_Start(I2C_EERPOM))return false;
	
	#if (EEPROM_SIZE == 64)
	I2C_SendByte(I2C_EERPOM, EEPROM_Address);   //发送设备地址+写信号
	#elif (EEPROM_SIZE == 16)
	I2C_SendByte(I2C_EERPOM, EEPROM_Address | (page >> 3 & 0x0E));   //发送设备地址+写信号
	#endif
	
	if(!I2C_WaitAck(I2C_EERPOM)){I2C_Stop(I2C_EERPOM); return false;}
	
	#if (EEPROM_SIZE == 64)
	I2C_SendByte(I2C_EERPOM, (page >> 3));   //设置低起始地址
	I2C_WaitAck(I2C_EERPOM);
	I2C_SendByte(I2C_EERPOM, (page << 5) | offset );   //设置低起始地址
	#elif (EEPROM_SIZE == 16)
	I2C_SendByte(I2C_EERPOM, (page << 4 & 0xff) | offset );   //设置低起始地址
	#endif
	
	I2C_WaitAck(I2C_EERPOM);
	for(int i=0;i < len;i++) {
		I2C_SendByte(I2C_EERPOM, data[i]);
		I2C_WaitAck(I2C_EERPOM);
	}	
	I2C_Stop(I2C_EERPOM); 
	vTaskDelay(5 / portTICK_RATE_MS);
	return true;
}

/**
 * EERPOM写入数据
 *   address：写入数据的起始地址，范围0~2047，AT24C16地址最好为16的整数倍,AT24C64地址最好为32的整数倍
 *   data：需要存储的数据
 *   len：数据长度，范围1~2048
 */
bool EEPROM_Write(uint16_t address, uint8_t * data, uint16_t len)
{
	uint8_t size;
	#if (EEPROM_SIZE == 64)
	size = 32;
	#elif (EEPROM_SIZE == 16)
	size = 16;
	#endif
	bool result;

	uint8_t page = address / size; // 起始页码
	uint8_t offset = address % size; // 起始偏移地址
	uint8_t length = size - offset; // 第一次写入的数据长度
	if(length >= len) // 数据可以全部存储到第一页
	{
		result = EEPROM_Write_Page(page, offset, data, len);
	} 
	else 
	{
	  result = EEPROM_Write_Page(page, offset, data, length); // 将第一页不一定完完整的数据写入
		if(result) { // 上一次写入成功
			data += length;
			page++;
			uint8_t page_count = (len - length) / size + page; // 中间完整页码的最后一页
			for(; page <= page_count; page++, data+=size) 
			{
				result = EEPROM_Write_Page(page, 0, data, size); // 将中间页码数据写入
				if(!result) { return false;	}
			}
			uint8_t end_len = len - page_count * size - length;
			if(end_len > 0) // 如果最后还有剩余不足一页的数据
			{
			  result = EEPROM_Write_Page(page, 0, data, end_len); // 写入最后不满一页的数据
			}
		}
	}
	return result;
}

/**
 * EERPOM读取数据
 *   address：读取数据的起始地址，AT24C16范围0~2047, AT24C64范围0~8192
 *   data：读取的buf
 *   len：数据长度，当读取的数据超出EERPOM地址时会循环到首地址继续读取
 */
bool EEPROM_Read(uint16_t address, uint8_t * data, uint16_t len)
{
	uint8_t dev, reg1, reg2; 
	uint8_t a;
	
	#if (EEPROM_SIZE == 64)	
	dev = EEPROM_Address;
	reg1 = address >> 8;
	reg2 = address & 0xff;
	#elif (EEPROM_SIZE == 16)	
	dev = EEPROM_Address | (address >> 7 & 0x0E);
	reg1 = address & 0xff;
	#endif
	
	if(!I2C_Start(I2C_EERPOM)) {I2C_Stop(I2C_EERPOM); return false;}
	I2C_SendByte(I2C_EERPOM, dev);  //发送设备地址+写信号
	if(!I2C_WaitAck(I2C_EERPOM)){I2C_Stop(I2C_EERPOM); return false;}
	I2C_SendByte(I2C_EERPOM, reg1);   
	if(!I2C_WaitAck(I2C_EERPOM)){I2C_Stop(I2C_EERPOM); return false;}
	
	#if (EEPROM_SIZE == 64)	
	I2C_SendByte(I2C_EERPOM, reg2);   
	if(!I2C_WaitAck(I2C_EERPOM)){I2C_Stop(I2C_EERPOM); return false;}
	#endif
	
	I2C_Start(I2C_EERPOM);
	I2C_SendByte(I2C_EERPOM, dev | 1); // 读操作
	for(uint16_t i=0; i<len; i++)
	{
		I2C_Ack(I2C_EERPOM); //  发送读信号后第一次是否应该是等待应答？
	  data[i] = I2C_RadeByte(I2C_EERPOM);
	}
	I2C_NoAck(I2C_EERPOM);
	I2C_Stop(I2C_EERPOM);
	return true;
}

/**
 * EEPROM 测试
 */
void EEPROM_Test() 
{
	// 单页写入
	uint8_t read[255];
	uint8_t write[16];
	for(uint8_t i=0; i<16; i++) 
	{
		for(uint8_t j=0; j<16; j++)
		{
			write[j] = i*16+j;
		}
		EEPROM_Write_Page(i, 0, write, 16); // 第i页的0位置开始写入write数组
	}
	EEPROM_Read(0, read, 255);
	
	// 任意长度数据写入任意地址
	uint8_t c[64] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64};
	uint8_t c2[64];
	int count  = 50;
  EEPROM_Write(1800, c, count);
	EEPROM_Read(1800, c2, count);
		
//	for(int i=0;i<32;i++) {
//		printf("%d\r\n",c2[i]);
//	}
}

	