#ifndef _BSP_EEPROM_H_
#define _BSP_EEPROM_H_

#include "stdint.h"
#include "stdbool.h"

/**
 * AT24C16
 *    设备地址：0b 1 0 1 0 P2 P1 P0 R/W
 *    前4位为EEPROM固定地址0xa，中间三位用于页寻址，最后一位表示读写位，1：读   0：写
 *
 * AT24C16共128page*16byte/page=2048byte
 *    注：每次连续读写不能分页，如果超出此页，EEPROM会将超出的部分循环写入此页
 *
 *
 */
 
#define EEPROM_Address 0xA0

/**
 * AT24C16初始化A0,A1,A2
 */
void EEPROM_Init(void);

/**
 * EERPOM单字节写入数据
 *   REG_Address：要写入的寄存器地址（16K及以下低8位有效）
 *   REG_data：要写入的数据
 */
bool EEPROM_Write_Byte(uint16_t REG_Address,uint8_t REG_data);

/**
 * EERPOM单字节读寄存器
 *   REG_Address：读取的寄存器地址，范围0~2047
 */
uint8_t EEPROM_Read_Byte(uint16_t REG_Address);
	
/**
 * EERPOM单页写入数据
 *   page：写入的页码，范围0~127
 *   offset：写入该页的偏移量 范围0~15
 *   data：需要存储的数据
 *   len：数据长度，范围1~16
 */
bool EEPROM_Write_Page(uint8_t page, uint8_t offset, uint8_t * data, uint8_t len);

/**
 * EERPOM写入数据
 *   address：写入数据的起始地址，范围0~2047，地址最好为16的整数倍
 *   data：需要存储的数据
 *   len：数据长度，范围1~2048
 */
bool EEPROM_Write(uint16_t address, uint8_t * data, uint16_t len);

/**
 * EERPOM读取数据
 *   address：读取数据的起始地址，范围0~2047
 *   data：读取的buf
 *   len：数据长度，范围1~2048，当读取的数据超出EERPOM地址时会循环到首地址继续读取
 */
bool EEPROM_Read(uint16_t address, uint8_t * data, uint16_t len);

#endif
