#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include "bsp_gpio.h"

#define L3GD20H_CS(x)   GPIO_Set(GPIOA, GPIO_Pin_4,  x)
#define LSM303D_CS(x)		GPIO_Set(GPIOB, GPIO_Pin_2,  x)
#define MPU6000_CS(x)   GPIO_Set(GPIOD, GPIO_Pin_11, x)
#define MS5611_CS(x)    GPIO_Set(GPIOE, GPIO_Pin_8,  x)

void SPI1_Init(void);

uint8_t SPI1_ReadWriteByte(uint8_t Data);

#endif
