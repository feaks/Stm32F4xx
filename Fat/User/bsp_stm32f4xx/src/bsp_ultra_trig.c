#include "bsp_ultra_trig.h"
#include "stm32f4xx_gpio.h"
#include "bsp_Ultrasonic.h"

void ULTRA_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
 
	// ECHO 初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 	// 使能SYSCFG时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);		// 使能GPIOA时钟 

  //GPIOA10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;							// 10脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;						// 输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			// 100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						// 上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);									// 初始化GPIO
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource10); // PA10 连接到中断线10
	
  /* 配置EXTI_Line10 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line10;							// LINE10
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			// 中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 边沿触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;								// 使能LINE10
  EXTI_Init(&EXTI_InitStructure);//配置
	
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		// 外部中断10
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	// 抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		// 子优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// 使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);													// 配置
	
	
	// TRIG 初始化
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能GPIOA时钟

  // GPIOA9初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 					// Trig脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 			// 普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			// 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 	// 100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 				// 上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure); 							// 初始化GPIO
}
//uint16_t distance;
//uint16_t count = 0;
void EXTI15_10_IRQHandler(void)
{
//	count++;
//	static uint16_t start=0, end=0;
//	if(EXTI_GetFlagStatus(EXTI_Line10) != RESET)
//	{
//		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == Bit_SET && ultra.state == START)
//		{
//			ultra.state = END;
//			start = TIM_GetCounter(TIM3);
//		}
//		else if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10) == Bit_RESET && ultra.state == END)
//		{
//			end = TIM_GetCounter(TIM3);
//			distance = (end > start ? end - start : 0xFFFF - start + end) * 17 / 100;//time * 340 / 1000 / 2;
//			if(distance<3000)
//			{
//				ultra.isReady = true;
//				ultra.distance = distance;
//				ultra.delta_distance = ultra.distance - ultra.last_distance;
//				ultra.last_distance = ultra.distance;
//			}
//		}
//		EXTI_ClearFlag(EXTI_Line10);
//	}
}

// 发出获取超声波信息
void ULTRA_Trig(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_9);
	uint32_t begin = TIM_GetCounter(TIM3);
	for(uint32_t end = TIM_GetCounter(TIM3); end - begin < 50; end = TIM_GetCounter(TIM3)) {
		if(end < begin)
			end += 0xFFFF;
	}
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
	ultra.state = START;
}