#include "bsp_random.h"
#include "stm32f4xx_rng.h"

static uint32_t random32bit;

// 随机数生成器初始化
void Random_Init()
{
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE); // 使能RNG时钟源

	RNG_Cmd(ENABLE); // 使能RNG

//	NVIC_InitTypeDef   NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = HASH_RNG_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	RNG_ITConfig(ENABLE); // 使能后开始生成随机数，只要生成的随机数被读取过就会立即重新生成随机数
}

// 阻塞的方式获取随机数，随机数生成时间很短，一般需要2个时钟周期，所以一般是使用阻塞的方式读取随机数
uint32_t Random_GetNumber()
{
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET);
	
	return RNG_GetRandomNumber();
}

// 随机数产生中断处理
void HASH_RNG_IRQHandler(void)
{
	if(RNG_GetITStatus(RNG_IT_CEI) == SET) // 时钟错误
	{
		RNG_ClearITPendingBit(RNG_IT_CEI);
	} 
	else if(RNG_GetITStatus(RNG_IT_SEI) == SET) // 种子错误
	{
		RNG_ClearITPendingBit(RNG_IT_SEI);
	} 
	else
	{
		RNG_ITConfig(ENABLE); // 禁止随机数产生中断
		random32bit = RNG_GetRandomNumber(); // 获取随机数
	}
}