#ifndef _BSP_UART_H_
#define _BSP_UART_H_

#include "stdint.h"
#include "stm32f4xx_usart.h"

// 初始化函数，参数： 波特率（9600.115200。。。。。。。）
void UART1_Init(uint32_t baud);
void UART2_Init(uint32_t baud);
void UART3_Init(uint32_t baud);
void UART4_Init(uint32_t baud);
void UART5_Init(uint32_t baud);
void UART6_Init(uint32_t baud);
void UART7_Init(uint32_t baud);
void UART8_Init(uint32_t baud);

#define UART_Putc(USARTx, ch)		USART_SendData(USARTx, ch); \
																while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) != SET) // 等待发送完成

#define UART1_Putc(ch)		UART_Putc(USART1, ch)
#define UART2_Putc(ch)		UART_Putc(USART2, ch)
#define UART3_Putc(ch)		UART_Putc(USART3, ch)
#define UART4_Putc(ch)		UART_Putc(UART4,  ch)
#define UART5_Putc(ch)		UART_Putc(UART5,  ch)
#define UART6_Putc(ch)		UART_Putc(USART6, ch)
#define UART7_Putc(ch)		UART_Putc(UART7,  ch)
#define UART8_Putc(ch)		UART_Putc(UART8,  ch)

void Uart_Puts(USART_TypeDef* USARTx, char *STR);

#define UART1_Puts(str)			Uart_Puts(USART1, str)
#define UART2_Puts(str)			Uart_Puts(USART2, str)
#define UART3_Puts(str)			Uart_Puts(USART3, str)
#define UART4_Puts(str)			Uart_Puts(UART4,  str)
#define UART5_Puts(str)			Uart_Puts(UART5,  str)
#define UART6_Puts(str)			Uart_Puts(USART6, str)
#define UART7_Puts(str)			Uart_Puts(UART7,  str)
#define UART8_Puts(str)			Uart_Puts(UART8,  str)

void Uart_Write(USART_TypeDef* USARTx, const uint8_t* Data, const uint16_t Len);

#define UART1_Write(data, len)		Uart_Write(USART1, data, len)
#define UART2_Write(data, len)		Uart_Write(USART2, data, len)
#define UART3_Write(data, len)		Uart_Write(USART3, data, len)
#define UART4_Write(data, len)		Uart_Write(UART4,  data, len)
#define UART5_Write(data, len)		Uart_Write(UART5,  data, len)
#define UART6_Write(data, len)		Uart_Write(USART6, data, len)
#define UART7_Write(data, len)		Uart_Write(UART7,  data, len)
#define UART8_Write(data, len)		Uart_Write(UART8,  data, len)

#endif
