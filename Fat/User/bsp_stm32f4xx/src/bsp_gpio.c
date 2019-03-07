#include "bsp_gpio.h"

/**初始化GPIO*****************************************************************
*GPIOx:GPIO模块                                                              *
*pin：GPIO引脚                                                               *
*IO_mode：0：输出   1：输入模式                                              *
*Init_GPIO(GPIOE,1,IO_OUT,Output_PP,GPIO_PULLUP); //初始化E1口为输出         *
*Init_GPIO(GPIOE,1,IO_IN,Input_MD,GPIO_PULLUP);  //初始化E1口为输入          *
*****************************************************************************/
void GPIO_Initialize(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode, GPIOOType_TypeDef GPIO_OType, GPIOPuPd_TypeDef GPIO_PuPd)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
  if(GPIOx==GPIOA)        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能GPIOA时钟
	else if(GPIOx==GPIOB)   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // 使能GPIOB时钟
	else if(GPIOx==GPIOC)   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // 使能GPIOC时钟
	else if(GPIOx==GPIOD)   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // 使能GPIOD时钟
  else if(GPIOx==GPIOE)   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); // 使能GPIOE时钟
 	else if(GPIOx==GPIOF)   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); // 使能GPIOF时钟
	else if(GPIOx==GPIOG)   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE); // 使能GPIOG时钟
	else if(GPIOx==GPIOH)		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE); // 使能GPIOH时钟
	else if(GPIOx==GPIOI)		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE); // 使能GPIOI时钟
	else if(GPIOx==GPIOJ)		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOJ, ENABLE); // 使能GPIOJ时钟
	else if(GPIOx==GPIOK)		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOK, ENABLE); // 使能GPIOK时钟
	
	if(GPIO_Mode == GPIO_Mode_OUT)
	{
		GPIO_InitStructure.GPIO_OType = GPIO_OType;
	}
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd; 
  GPIO_Init(GPIOx, &GPIO_InitStructure);//初始化GPIO
}

/*************************************************************
*设置引脚状态，输出高低电平																	 *
*GPIO_Set(GPIOE,GPIO_Pin_1,0);        //将E1口输出低电平     *
*************************************************************/
void GPIO_Set(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,unsigned char out)
{
		if(out==0) 	GPIO_ResetBits(GPIOx,GPIO_Pin);	
		else	GPIO_SetBits(GPIOx,GPIO_Pin);
}

/**************************************************************************
*获取引脚状态，输出高低电平                                               *
*unsigned char temp = GET_GPIO(GPIOE,GPIO_Pin_1);     //获取E1的引脚状态  *
**************************************************************************/
uint8_t GPIO_Get(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	 return GPIO_ReadInputDataBit(GPIOx, GPIO_Pin);
}

/**
 * LED初始化
 */
void LED_Init()
{
  GPIO_Initialize(GPIOE, GPIO_Pin_0, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP); //初始化E1口为输出
	GPIO_Initialize(GPIOE, GPIO_Pin_1, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP); //初始化E2口为输出
	GPIO_Initialize(GPIOE, GPIO_Pin_2, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP); //初始化E3口为输出
	GPIO_Initialize(GPIOE, GPIO_Pin_3, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP); //初始化E4口为输出
	GPIO_Initialize(GPIOC, GPIO_Pin_2, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP); //初始化E4口为输出
	GPIOC->BSRRH = GPIO_Pin_2;
}
