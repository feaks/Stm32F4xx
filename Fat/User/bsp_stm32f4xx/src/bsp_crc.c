#include "bsp_crc.h"
#include "stdint.h"

// 使用CRC之前需要使能CRC时钟
// RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	
// Resets the CRC Data register (DR).
// 复位CRC数据寄存器
void CRC_ResetDR(void);

// Computes the 32-bit CRC of a given data word(32-bit).
// 根据当前CRC计算值与输入的Data数据计算CRC
uint32_t CRC_CalcCRC(uint32_t Data);

// Computes the 32-bit CRC of a given buffer of data word(32-bit).
// 根据当前CRC计算值与输入的多个Data数据计算CRC
uint32_t CRC_CalcBlockCRC(uint32_t pBuffer[], uint32_t BufferLength);

// Returns the current CRC value.
// 返回当前CRC计算结果
uint32_t CRC_GetCRC(void);

// Stores a 8-bit data in the Independent Data(ID) register.
// 存储一个8位数据在独立数据寄存器中
void CRC_SetIDRegister(uint8_t IDValue);

// Returns the 8-bit data stored in the Independent Data(ID) register
// 返回独立数据寄存器中的8位数据
uint8_t CRC_GetIDRegister(void);

/**
 * 简记式 ： 04C1DB7
 * 【例】假设使用的生成多项式是 G(X)=X3+X+1。 4 位的原始报文为 1010，求编码后的报文。
 *	解：
 *	1、将生成多项式 G(X)=X3+X+1 转换成对应的二进制除数 1011。
 *	2、此题生成多项式有 4 位（R+1）(注意：4 位的生成多项式计算所得的校验码为 3 位，R 为校验码
 *	位数)，要把原始报文 C(X)左移 3（R）位变成 1010 000
 *	3、用生成多项式对应的二进制数对左移 3 位后的原始报文进行模 2 除（高位对齐），相当于按位异或：
 *	1010000
 *	1011
 *	------------------
 *	0001000
 *	1011
 *	------------------
 *	011
 *	得到的余位 011，所以最终编码为：1010 011
 *	上述推算过程，有助于我们理解 CRC 的概念。但直接编程来实现上面的算法，不仅繁琐，效率也不
 *	高。实际上在工程中不会直接这样去计算和验证 CRC。下表中列出了一些见于标准的 CRC 资料(STM32
 *	使用的是 CRC-32)
 * https://gss1.bdstatic.com/9vo3dSag_xI4khGkpoWK1HF6hhy/baike/s%3D509/sign=9431bdf104087bf479ec57e9cbd2575e/0b46f21fbe096b637f80dcea0a338744eaf8aceb.jpg
 */