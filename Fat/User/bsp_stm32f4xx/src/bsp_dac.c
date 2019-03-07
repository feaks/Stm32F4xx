#include "bsp_dac.h"
#include "stm32f4xx_DMA.h"

/**
 *	DAC 通道
 *		STM32 支持两个 DAC 通道，可以使用独立或者双端模式。
 *		DAC 通道 1 使用 DAC_OUT1 (PA4) 做为输出
 *		DAC 通道 2 使用 DAC_OUT2 (PA5) 做为输出
 *DAC 触发
 *		DAC 转换器可以通过 DAC_Trigger_None 配置成非触发模式，一旦通过 DAC_SetChannel1Data() /
 *		DAC_SetChannel2Data()函数写数据到 DHRx 寄存器将产生 DAC_OUT1/DAC_OUT2 输出。
 *		DAC 可以通过下面三种方式进行触发
 *			1. 外部事件触发：通过 DAC_Trigger_Ext_IT9 将 EXTI Line 9 与任何 GPIOx_Pin9 相连接。相应的
 *				GPIOx_Pin9 引脚必须配置成输入模式。
 *			2.定时器触发：TIM2, TIM4, TIM5, TIM6, TIM7 和 TIM8（DAC_Trigger_T2_TRGO ，DAC_Trigger_T4_TRGO...）
 *				通过函数 TIM_SelectOutputTrigger()选择定时器触发事件。
 *			3. 通过 DAC_Trigger_Software 配置成软件触发。
 *	DAC 缓存模式特性
 *		每个 DAC 通道都支持 DAC 缓存模式以减少输出阻抗，从而不需要二外增加运算放大器来驱动外部负载 。
 *		使能 DAC 输出缓冲需要执行下面配置 DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
 *		在使用输出缓冲和不使用输出缓冲的情况下，输出阻抗的大小区别可通过参考数据手册获得。
 *DAC 生成波形
 *		通过 DAC_WaveGeneration_Noise 配置产生噪声。
 *		通过 DAC_WaveGeneration_Triangle 配置产生三角波
 * DAC 数据格式
 *		通过 DAC_Align_8b_R 配置成 8 位数据右对齐。
 *		通过 DAC_Align_12b_L 配置成 12 位数据左对齐。
 *		通过 DAC_Align_12b_R 配置成 12 位数据右对齐。
 *DAC 数据到电压值的转换
 *		DAC_OUTx = VREF+ * DOR / 4095
 *		DOR：表示 DAC 输出数据寄存器。
 *		VREF+：表示输入参考电压。
 *		举一个例子：我们希望 DAC_OUT1 输出 0.7V 的电压，可以通过下面的函数来实现
 *			DAC_SetChannel1Data(DAC_Align_12b_R, 868);
 *			假设 VREF+ = 3.3V, DAC_OUT1 = (3.3 * 868) / 4095 = 0.7V。
 *	DMA 请求
 *		通过函数 DAC_DMACmd()使能 DAC 的 DMA 通道 1。 DMA 请求的映射关系如下：
 * 	DAC 通道 1 映射到 DMA1 Stream5 channel7。
 * 	DAC 通道 2 映射到 DMA1 Stream6 channel7。
 *DAC 的驱动方法
 *		通过函数使能 DAC 时钟 RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE)。
 *		配置 DAC_OUTx (DAC_OUT1: PA4, DAC_OUT2: PA5)为模拟模式。
 *		通过 DAC_Init()初始化 DAC。
 *		通过函数 DAC_Cmd()使能 DAC。
 **/

/* 梯形波参考点 */
const uint8_t aEscalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};
/* 正弦波参考点 */
const uint16_t Sine12bit[32] = {2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056, 3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

// DAC初始化，由TIM6触发，要先初始化TIM6
void VOLTAGE_DAC_Init()
{
	DAC_InitTypeDef DAC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能引脚时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE); // DAC 外设时钟使能
	
	// DAC通道1、2输出引脚配置（PA.4  PA.5）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// DAC通道1配置
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO; // 触发方式为定时器6
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None; // 不生成波形
	// DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle; // 波形为三角波
	// DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Noise; // 波形为噪声波
	// DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_4095 // 三角波振幅（根据实际需求修改，值越大幅值越高）
	// DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits11_0; // 噪声屏蔽位（根据实际需求修改，值越大幅值越高）
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable; // 输出缓存使能
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle; // 波形为三角波
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	// DAC_SetChannel1Data(DAC_Align_12b_R, 0); // 设置DAC通道1 DHR12R 寄存器（非DMA模式时需要设置对其方式）
	
	// DMA1 时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	// DMA1_Stream5 通道7配置
	DMA_DeInit(DMA1_Stream5);
	DMA_InitStructure.DMA_Channel = DMA_Channel_7;
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC->DHR12R2; // 12位 对应 DMA_PeripheralDataSize_HalfWord
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&Sine12bit;
	DMA_InitStructure.DMA_BufferSize = sizeof(Sine12bit);
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);
	
	// 使能 DMA1_Stream5
	DMA_Cmd(DMA1_Stream5, ENABLE);
	
	// 使能 DAC 通道1
	DAC_Cmd(DAC_Channel_1, ENABLE);
	
	// 使能DAC通道1的DMA功能
	DAC_DMACmd(DAC_Channel_1, ENABLE);
}

// freq ：定时器输出频率（此处用作DAC触发频率）
void TIM6_Config(uint16_t freq)
{
	static uint16_t ACCURACY = 10000;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	// 使能TIM6时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); 

	// 计算分频系数
	uint32_t hz_set = ACCURACY * freq;
	uint16_t PrescalerValue = (uint16_t)((42000000 * 2) / hz_set) - 1; // 分频系数（当分频系数大于1时，APB2时钟频率会放大2倍）
	
	// 配置TIM6
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);	
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue; // 定时器分频
	TIM_TimeBaseStructure.TIM_Period = ACCURACY;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	
	// TIM6 TRGO 选择
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	
	// 使能TIM6
	TIM_Cmd(TIM6, ENABLE);
}