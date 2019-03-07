#ifndef __BSP_MS5611_H__
#define __BSP_MS5611_H__

#include "stdint.h"
#include "stdbool.h"

#define MS5611_ADDR 0xEE		// MPL3115A2 7-bit I2C address 0x60 is fixed

#define CMD_RESET               0x1E // ADC reset command
#define CMD_ADC_READ            0x00 // ADC read command
#define CMD_ADC_CONV            0x40 // ADC conversion command
#define CMD_ADC_D1              0x00 // ADC D1 conversion
#define CMD_ADC_D2              0x10 // ADC D2 conversion
#define CMD_ADC_256             0x00 // ADC OSR=256
#define CMD_ADC_512             0x02 // ADC OSR=512
#define CMD_ADC_1024            0x04 // ADC OSR=1024
#define CMD_ADC_2048            0x06 // ADC OSR=2048
#define CMD_ADC_4096            0x08 // ADC OSR=4096
#define CMD_PROM_RD             0xA0 // Prom read command
#define PROM_CNT                8

#define MS5611_OSR_4096_CONV_DELAY       8220   // 8.22ms
#define MS5611_OSR_2018_CONV_DELAY       4130   // 4.13ms
#define MS5611_OSR_1024_CONV_DELAY       2080   
#define MS5611_OSR_512_CONV_DELAY        1060   
#define MS5611_OSR_256_CONV_DELAY        540    



typedef struct 
{
	uint32_t ms5611_ut;  // static result of temperature measurement
	uint32_t ms5611_up;  // static result of pressure measurement
	int32_t dT;
	int64_t off;
	int64_t sens;
	int32_t delt;
	int32_t pressure_raw;		// 气压原始值
	float pressure;				// 气压值
	float altitube_raw;			// 高度原始值
	float altitube;				// 高度值
	float altitube2;			// 高度值
	int32_t temperature; // ??
	uint16_t C[PROM_CNT];   // on-chip ROM

	bool state;							// ????? (true:??    false:???)
	bool calibration;       // ????
	bool calibrating;       // ??????
	bool is_calibrated ;    // ??????
	
} MS5611;

extern MS5611 ms5611;

void MS5611_Init(void);  
void MS5611_Calculate(void);
uint8_t MS5611_Update(void);

/**
	prom[8]
		prom.factory_setup
		prom.c1_pressure_sens			40127
		prom.c2_pressure_offset			36924
		prom.c3_temp_coeff_pres_sens	23317
		prom.c4_temp_coeff_pres_offset	23282
		prom.c5_reference_temp			33464
		prom.c6_temp_coeff_temp			28312
		prom.serial_and_crc
*/
#endif
