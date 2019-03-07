#ifndef _BSP_I2C_H_
#define _BSP_I2C_H_

#include <stdint.h>
#include <stdbool.h>
#include <stm32f4xx_gpio.h>

#define SCL_H i2c->SCL_GPIOx->BSRRL = i2c->SCL_Pin
#define SCL_L i2c->SCL_GPIOx->BSRRH = i2c->SCL_Pin
   
#define SDA_H i2c->SDA_GPIOx->BSRRL = i2c->SDA_Pin
#define SDA_L i2c->SDA_GPIOx->BSRRH = i2c->SDA_Pin

#define SCL_read i2c->SCL_GPIOx->IDR  & i2c->SCL_Pin
#define SDA_read i2c->SDA_GPIOx->IDR  & i2c->SDA_Pin

typedef struct
{
	GPIO_TypeDef * SCL_GPIOx;
	GPIO_TypeDef * SDA_GPIOx;
	uint16_t SCL_Pin;
	uint16_t SDA_Pin;
} I2C;

extern I2C *I2C_1, *I2C_2, *I2C_3, *I2C_4;

bool I2C_Start(I2C * i2c);

bool I2C_Stop(I2C * i2c);

void I2C_Ack(I2C * i2c);

void I2C_NoAck(I2C * i2c);

bool I2C_WaitAck(I2C * i2c);

void I2C_SendByte(I2C * i2c, uint8_t SendByte);

uint8_t I2C_RadeByte(I2C * i2c);

void I2C_GPIO_Config(void);

bool I2C_Write_REG(I2C * i2c, uint8_t SlaveAddress,uint8_t REG_Address, uint8_t REG_data);

uint8_t I2C_Read_REG(I2C * i2c, uint8_t SlaveAddress,uint8_t REG_Address);

bool I2C_CheckDevice(I2C * i2c, uint8_t SlaveAddress);

#endif
