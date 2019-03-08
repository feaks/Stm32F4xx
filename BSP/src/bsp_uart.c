
#include "bsp_uart.h"
#include <stdio.h>

//重新指向数据流   printf();---> cmd的界面
// 								printf(); ---> usart1

#pragma import(__use_no_semihosting)   
                          
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;  
        
void _sys_exit(int x) 
{ 
	x = x; 
}

int fputc(int ch, FILE *f)	//输出重定向  printf
{      
	while(USART_GetFlagStatus(USART1 , USART_FLAG_TC) !=SET );
	USART1->DR = (uint8_t)ch;
	return ch;
}

int fgetc(FILE *f)				//输入重定向  scanf
{
	while(USART_GetFlagStatus(USART1 , USART_FLAG_RXNE) !=SET );
	return (int)USART1->DR;
}


void UART1_Init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOA , &GPIO_InitStruct);
	
	//端口映射
	GPIO_PinAFConfig(GPIOA , GPIO_PinSource9 ,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA , GPIO_PinSource10 ,GPIO_AF_USART1);
	
	USART_InitStruct.USART_BaudRate = baud;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1 , &USART_InitStruct);
	
	//启动中断的 NVIC配置
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);

/* 串口中断接收 */

	USART_ITConfig(USART1 , USART_IT_RXNE , ENABLE);
	
	USART_Cmd(USART1 , ENABLE);
	
	USART_ClearFlag(USART1 , USART_FLAG_TC);
}

//中断服务函数    中断服务句柄函数 (╯‵□′)╯︵┻━┻
void USART1_IRQHandler(void) 
{
	if(USART_GetITStatus(USART1 , USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART1 , USART_IT_RXNE);
//		(void) USART1->DR;	//空读语句
//		USART1->DR = USART1->DR;
//		while(USART_GetFlagStatus(USART1 , USART_FLAG_TC) !=SET);
	}
}
