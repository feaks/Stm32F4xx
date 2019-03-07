#include "bsp_wdg.h"
#include "stm32f4xx_iwdg.h"
#include "math.h"

// 初始化独立看门狗    reset_time_ms : 看门狗复位时间ms
void IWDG_Init(uint16_t reset_time_ms)
{
	assert_param(reset_time_ms < 32768);
	uint8_t prer = 0;  // prer:分频数:0~7(只有低3位有效!)
	uint16_t reload = 0; // reload:自动重装载值,0~0xFFF.
	
	/* 分频因子=4*2^prer.但最大值只能是256!   时间计算(大概):Tout=((4*2^prer)*reload)/32 (ms). */
	if(reset_time_ms < 512)					prer = IWDG_Prescaler_4;
	else if(reset_time_ms < 1024) 	prer = IWDG_Prescaler_8;
	else if(reset_time_ms < 2048) 	prer = IWDG_Prescaler_16;
	else if(reset_time_ms < 4096) 	prer = IWDG_Prescaler_32;
	else if(reset_time_ms < 8192) 	prer = IWDG_Prescaler_64;
	else if(reset_time_ms < 16384) 	prer = IWDG_Prescaler_128;
	else if(reset_time_ms < 32768) 	prer = IWDG_Prescaler_256;
	
	reload = reset_time_ms * IWDG_CLOCK / ( 4 * pow(2, prer));
	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //使能对IWDG->PR IWDG->RLR的写
	
	IWDG_SetPrescaler(prer); //设置IWDG分频系数

	IWDG_SetReload(reload);   //设置IWDG装载值

	IWDG_ReloadCounter(); //reload
	
	IWDG_Enable();       //使能看门狗
}

// 喂独立看门狗
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}

/*********************************************************************
 * 复位条件：
 *     当递减计数器值小于0x40时复位
 *     在窗口之外重载递减计数器时复位
 * 提前唤醒中断：
 *     当递减计数器等于0x40时出发
 * 超时时间：
 *     Twwdg = (4096*2^WDGTB*(T[5:0+1]))/Fpclk;
 *       Twwdg ： WWDG超时时间（ms）
 *       Fpclk ： APB1的时钟频率（kHz）
 *       WDGTB ： WWDG的预分频系数
 *       T[5:0] ： 窗口看门狗的计数器低6位
 *********************************************************************/

// 保存WWDG计数器的设置值,默认为最大. 
uint8_t WWDG_CNT=0X7F;

// 初始化窗口看门狗    reset_time_ms : 复位时间    top_window_time ：喂狗的上窗口时间 （当top_window_time = reset_time_ms时，可以随时喂狗）
void WWDG_Init(uint8_t reset_time_ms, uint8_t top_window_time)
{
	assert_param(reset_time_ms < 50);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE); //使能窗口看门狗时钟
	
	uint32_t prer = 0; // 预分频系数
	uint8_t cnt = 0; // 计数初始值
	
	if(reset_time_ms < 7)					{ prer = WWDG_Prescaler_1; cnt = reset_time_ms * WWDG_PCLK / 1 + 0x3F; } // 最大值6.24ms  
	else if(reset_time_ms < 13) 	{ prer = WWDG_Prescaler_2; cnt = reset_time_ms * WWDG_PCLK / 2 + 0x3F; } // 最大值12.48ms
	else if(reset_time_ms < 25) 	{ prer = WWDG_Prescaler_4; cnt = reset_time_ms * WWDG_PCLK / 4 + 0x3F; } // 最大值24.97ms
	else { prer = WWDG_Prescaler_8; cnt = reset_time_ms * WWDG_PCLK / 8 + 0x3F; } // 最大值49.93ms

	WWDG_CNT = cnt & WWDG_CNT; // 初始化WWDG_CNT. 
	WWDG_SetPrescaler(prer); // 设置分频值
	WWDG_SetWindowValue(top_window_time+0x3F); //设置窗口值
	WWDG_Enable(WWDG_CNT);  //设置计数值并开启看门狗
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=WWDG_IRQn; // 窗口看门狗中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;  // 抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // 子优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE; // 使能窗口看门狗
	NVIC_Init(&NVIC_InitStructure);
	
	WWDG_ClearFlag(); // 清除提前唤醒中断标志位
  WWDG_EnableIT(); // 开启提前唤醒中断
}

//窗口看门狗中断服务程序 
void WWDG_IRQHandler(void)
{
	WWDG_ClearFlag(); // 清除提前唤醒中断标志位
	WWDG_SetCounter(WWDG_CNT); // 重设窗口看门狗值
	// do something

}
