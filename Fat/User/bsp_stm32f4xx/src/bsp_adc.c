#include "bsp_adc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

static ADC_DATA adc_buf[10];
Battery battery;			// 电池信息

/*
*********************************************************************************************************
*	函 数 名: Battery_ADC_Init
*	功能说明: ADC初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Battery_ADC_Init(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
	
	/* 使能外设时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);

	/* DMA2 Stream0 Configuration */
	DMA_DeInit(DMA2_Stream0);
	while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){}
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;                // 通道0
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;  
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&adc_buf[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = sizeof(ADC_DATA)*10 / 2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);
	
	/* 配置引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; // PC4、5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;					 // 模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;			 // 浮空模式
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	

	/* ADC 公共部分配置 */
	// ADC采样频率： Sampling Time + Conversion Time = 480 + 12 cycles = 492cyc
  //							 Conversion Time = 21MHz / 492cyc = 42ksps.
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;	// 独立模式
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;	// 预分频  ADCCLK = PCLK2/4 = 84M/4 = 21MHZ 
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;	// 禁止DMA模式（多个ADC时有效，DMA切换方式）
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	// 两个采样阶段之间的延迟5个时钟（在双重或三重模式下使用）
	ADC_CommonInit(&ADC_CommonInitStructure); // 初始化

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;  // 12位模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;						// 扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			// 连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;  // 禁止触发检测，使用软件触发。也可以使用定时器 定时触发
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; // 禁止触发检测，所以此位无效。使用定时器触发时，此位设置触发通道
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 3; // 3个在转换规则序列中
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_480Cycles); // ADC1,ADC通道,480个周期,提高采样时间可以提高精确度
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 2, ADC_SampleTime_480Cycles); // ADC1,ADC通道,480个周期,提高采样时间可以提高精确度
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 3, ADC_SampleTime_480Cycles); // ADC1,ADC通道,480个周期,提高采样时间可以提高精确度
	// ADC_Channel_TempSensor通道为芯片内部通道，同样还有ADC_Channel_Vrefint、ADC_Channel_Vbat通道
	
	ADC_TempSensorVrefintCmd(ENABLE); /* 使能温度检测 */
	// ADC_VBATCmd(ENABLE); /* Enable VBAT channel */

  /* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	/* Enables or disables the ADC DMA request after last transfer in multi ADC mode */
	// ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
	
	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE); /* 使能 ADC1 */
	
	ADC_SoftwareStartConv(ADC1); /* 软件启动转换 */
}

void Get_Battery(void) 
{	
	uint16_t sum_current = 0;
	uint16_t sum_voltage = 0;
	uint16_t sum_temperature = 0;
	for(uint8_t i=0; i<10; i++) 
	{
		sum_current += adc_buf[i].current;
		sum_voltage += adc_buf[i].voltage;
		sum_temperature += adc_buf[i].temperature;
	}
	
	battery.current = sum_current / 10 * 3.3 / 4095;         
	battery.voltage = sum_voltage / 10 * 3.3 / 4095 * 10 + 0.25f;   // 加上0.25f是ADC采样结果与实际测量电压差0.25v
	
	/**************************** 
	 * 电池电量不应低于3.6V     *
	 * 4.2V  -------- 100%      *
	 * 3.95V --------  75%      *
	 * 3.85V --------  50%      *
	 * 3.73V --------  25%      *
	 * 3.5V  --------   0%      *
	 ***************************/
	
	battery.temperature = (sum_temperature / 10 * 3.3 / 4095 - 0.76)/0.0025 + 25;
	/*************************************************************************************************
  * VSENSE：温度传感器的当前输出电压，与ADC_DR 寄存器中的结果ADC_ConvertedValue之间的转换关系为： *
  * 					ADC_ConvertedValue * Vdd                                                             *
  * VSENSE = --------------------------                                                           *
  * 					Vdd_convert_value(0xFFF)                                                             *
  * ADC转换结束以后，读取ADC_DR寄存器中的结果，转换温度值计算公式如下：                           *
  * 				V25 - VSENSE                                                                           *
  * T(℃) = ------------ + 25                                                                     *
  * 				 Avg_Slope                                                                             *
  * V25： 温度传感器在25℃时 的输出电压，典型值0.76 V。                                        		 *
  * Avg_Slope：温度传感器输出电压和温度的关联参数，典型值2.5 mV/℃。                            	 *
  ************************************************************************************************/
}

/*********************************** 飞航科技 (END OF FILE) ***************************************/
