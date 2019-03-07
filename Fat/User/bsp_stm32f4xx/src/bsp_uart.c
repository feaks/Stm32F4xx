#include <stdio.h>
#include "bsp_uart.h"
//#include "FreeRTOS_CLI.h"
#include "config.h"

#include "bsp_Ultrasonic.h"

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 

//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);	// 等待发送完成
	USART_SendData(USART1, (uint8_t) ch);  
	return ch;
}

int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET); // 等待接收数据
	return (int)USART_ReceiveData(USART1);
}

/******************************************************
*	函 数 名: UART1_Init                                *
*	功能说明: UART1初始化                               *
*	形    参: baud:波特率                               *
*	返 回 值: 无                                        *
******************************************************/  
void UART1_Init(uint32_t baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 配置GPIO */
	#if UART1_Mapping		/* TX = PA9   RX = PA10 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 			// 打开 GPIO 时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 			// 打开 UART 时钟
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); 	// 将 PA9 映射为 USART1_TX
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); 	// 将 PA10 映射为 USART1_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 					// GPIO电平切换速度
	
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#else								/* TX = PB6   RX = PB7  */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);				// 打开 GPIO 时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);      // 打开 UART 时钟
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);   // 将 PB6 映射为 USART1_TX
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);   // 将 PB7 映射为 USART1_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						// GPIO电平切换速度
		
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                // 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                // 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif
	
  /* USART1 初始化设置 */
	USART_InitStructure.USART_BaudRate = baud;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// 1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;						// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
  USART_Init(USART1, &USART_InitStructure); 										// 初始化串口1
	
  USART_Cmd(USART1, ENABLE);																		// 使能串口1 
	
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);			// 清除发送完成标志

	// 配置同步模式
	// void USART_ClockInit(USART_TypeDef* USARTx, USART_ClockInitTypeDef* USART_ClockInitStruct)
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);								// 开启相关中断

	/* USART1 NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;							// 串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;			// 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;						// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                               // 根据指定的参数初始化NVIC寄存器
#endif
}

//char cInputString[cmdMAX_INPUT_SIZE];
//int ucInputIndex = 0;
//char pcOutputString[configCOMMAND_INT_MAX_OUTPUT_SIZE];
/****************************************************
*	函 数 名: USART1_IRQHandler                      	*
*	功能说明: 串1中断服务程序                         *
*	形    参: 无                                     	*
*	返 回 值: 无                                     	*
*****************************************************/ 
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
//	u8 cRxedChar;
//	BaseType_t xReturned;
//	
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
//	{
//		cRxedChar =USART_ReceiveData(USART1);	//读取接收到的数据		
//		if(cRxedChar != pdPASS) 
//		{
//				if( cRxedChar == '\n' || cRxedChar == '\r' ) 
//				{
//					printf("\n\r");
//					if(ucInputIndex == 0) {} 
//					else 
//					{
//						do {
//						xReturned = FreeRTOS_CLIProcessCommand( cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );  //索引命令，执行回调函数
//						printf((char *)pcOutputString);   //显示回调函数处理结果
//						printf("\n\r");        //换行
//						} while(xReturned != pdFALSE);
//						
//						ucInputIndex = 0;
//						memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );
//						memset( pcOutputString, 0x00, configCOMMAND_INT_MAX_OUTPUT_SIZE );
//					}
//				} 
//				else if( cRxedChar == '\r' )
//				{
//					/* Ignore the character. */
//				}
//				else if( ( cRxedChar == '\b' ) || ( cRxedChar == cmdASCII_DEL ) ) 
//				{
//					printf("\b \b");
//					/* Backspace was pressed.  Erase the last character in the
//					string - if any. */
//					if( ucInputIndex > 0 )
//					{
//						ucInputIndex--;
//						cInputString[ ucInputIndex ] = '\0';
//					}
//				}
//				else
//				{
//					USART_SendData(USART1,cRxedChar);  //回显
//					/* A character was entered.  Add it to the string entered so
//					far.  When a \n is entered the complete	string will be
//					passed to the command interpreter. */
//					if( ( cRxedChar >= ' ' ) && ( cRxedChar <= '~' ) )
//					{
//						if( ucInputIndex < cmdMAX_INPUT_SIZE )
//						{
//							cInputString[ ucInputIndex ] = cRxedChar;   // 命令保存
//							ucInputIndex++;             //计算命令长度
//						}
//					}
//				}			
//		}
//  } 
} 

/******************************************************
*	函 数 名: UART2_Init                                *
*	功能说明: UART2初始化                               *
*	形    参: baud:波特率                               *
*	返 回 值: 无                                        *
******************************************************/   
void UART2_Init(uint32_t baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 配置GPIO */
	#if UART2_Mapping		/* TX = PA2   RX = PA3 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 			// 打开 GPIO 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 			// 打开 UART 时钟
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); 	// 将 PA2 映射为 USART2_TX
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); 	// 将 PA3 映射为 USART2_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 					// GPIO电平切换速度
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#else								/* TX = PD5   RX = PD6  */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);				// 打开 GPIO 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);      // 打开 UART 时钟
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);   // 将 PD5 映射为 USART2_TX
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);   // 将 PD6 映射为 USART2_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						// GPIO电平切换速度
		
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                // 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                // 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	#endif

  /* USART2 初始化设置 */
	USART_InitStructure.USART_BaudRate = baud;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// 1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;						// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
  USART_Init(USART2, &USART_InitStructure); 										// 初始化串口2
	
  USART_Cmd(USART2, ENABLE);																		// 使能串口2

	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);			// 清除发送完成标志

#if EN_USART2_RX	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);								// 开启相关中断

	/* USART2 NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;							// 串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;			// 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;						// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                               // 根据指定的参数初始化NVIC寄存器
#endif
}

/****************************************************
*	函 数 名: USART2_IRQHandler                      	*
*	功能说明: 串2中断服务程序                         *
*	形    参: 无                                     	*
*	返 回 值: 无                                     	*
*****************************************************/
void USART2_IRQHandler(void)                										//串2中断服务程序
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  				//接收中断
	{
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
		uint8_t data = USART_ReceiveData(USART2);										// 读取接收到的数据
		UART2_Putc(data);
  } 
} 

/******************************************************
*	函 数 名: UART3_Init                                *
*	功能说明: UART3初始化                               *
*	形    参: baud:波特率                               *
*	返 回 值: 无                                        *
******************************************************/   
void UART3_Init(uint32_t baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 配置GPIO */
	#if UART3_Mapping==1		/* TX = PB10   RX = PB11 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 			// 打开 GPIO 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 			// 打开 UART 时钟
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); 	// 将 PB10 映射为 USART3_TX
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); 	// 将 PB11 映射为 USART3_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 					// GPIO电平切换速度
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	#elif UART3_Mapping==0  /* TX = PD8   RX = PD9  */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);				// 打开 GPIO 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);      // 打开 UART 时钟
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);   // 将 PD8 映射为 USART3_TX
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);   // 将 PD9 映射为 USART3_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						// GPIO电平切换速度
		
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                // 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                // 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	#else  								 /* TX = PC10  RX = PC11  */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);				// 打开 GPIO 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);      // 打开 UART 时钟
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);  // 将 PD10 映射为 USART3_TX
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);  // 将 PD11 映射为 USART3_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						// GPIO电平切换速度
		
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                // 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                // 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	#endif
  
  /* USART3 初始化设置 */
	USART_InitStructure.USART_BaudRate = baud;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// 1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;						// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
  USART_Init(USART3, &USART_InitStructure); 										// 初始化串口3
	
  USART_Cmd(USART3, ENABLE);																		// 使能串口3
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET);			// 清除发送完成标志

#if EN_USART3_RX	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);								// 开启相关中断

	/* USART3 NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;							// 串口3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;			// 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;						// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                               // 根据指定的参数初始化NVIC寄存器
#endif
}

void USART3_IRQHandler(void)                										//串口3中断服务程序
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  				//接收中断
	{
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
		uint8_t data = USART_ReceiveData(USART3);										// 读取接收到的数据
		UART3_Putc(data);
  } 
} 

/******************************************************
*	函 数 名: UART4_Init                                *
*	功能说明: UART4初始化                               *
*	形    参: baud:波特率                               *
*	返 回 值: 无                                        *
******************************************************/ 
void UART4_Init(uint32_t baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 配置GPIO */
	#if UART4_Mapping		/* TX = PA0   RX = PA1 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 			// 打开 GPIO 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); 			// 打开 UART 时钟
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4); 	  // 将 PA0 映射为 UART4_TX
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4); 	  // 将 PA1 映射为 UART4_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 					// GPIO电平切换速度
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#else								/* TX = PC10  RX = PC11  */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);				// 打开 GPIO 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);       // 打开 UART 时钟
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);   // 将 PD10 映射为 USART4_TX
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);   // 将 PD11 映射为 USART4_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						// GPIO电平切换速度
		
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                // 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                // 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	#endif
	
  /* USART4 初始化设置 */
	USART_InitStructure.USART_BaudRate = baud;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// 1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;						// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
  USART_Init(UART4, &USART_InitStructure); 											// 初始化串口4
	
  USART_Cmd(UART4, ENABLE);																			// 使能串口4
	
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC) != SET);			// 清除发送完成标志

#if EN_UART4_RX	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);									// 开启相关中断

	/* USART4 NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;							// 串口4中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;			// 抢占优先级6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						// 子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                               // 根据指定的参数初始化NVIC寄存器
#endif
}

void UART4_IRQHandler(void)                											//串口4中断服务程序
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  				//接收中断
	{
		USART_ClearFlag(UART4, USART_FLAG_RXNE);
		uint8_t data = USART_ReceiveData(UART4);										//读取接收到的数据
		UART4_Putc(data);
	  //Ultra_Receive(data);		
  } 
} 

/******************************************************
*	函 数 名: UART5_Init                                *
*	功能说明: UART5初始化                               *
*	形    参: baud:波特率                               *
*	返 回 值: 无                                        *
******************************************************/ 
void UART5_Init(uint32_t baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 配置GPIO */
	#if UART4_Mapping==0		/* TX = PC12  RX = PD2 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE); 			// 打开 GPIO 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); 			// 打开 UART 时钟
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5); 	// 将 PC12 映射为 UART5_TX
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5); 	  // 将 PD2  映射为 UART5_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 					// GPIO电平切换速度
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	#endif
	
  /* USART5 初始化设置 */
	USART_InitStructure.USART_BaudRate = baud;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// 1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;						// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
  USART_Init(UART5, &USART_InitStructure); 											// 初始化串口5
	
  USART_Cmd(UART5, ENABLE);																			// 使能串口5
	
	while(USART_GetFlagStatus(UART5, USART_FLAG_TC) != SET);			// 清除发送完成标志

#if EN_UART5_RX	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);									// 开启相关中断

	/* USART5 NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;							// 串口5中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;			// 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;						// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                               // 根据指定的参数初始化NVIC寄存器
#endif
}

void UART5_IRQHandler(void)                											// 串口5中断服务程序
{
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  				// 接收中断
	{
		USART_ClearFlag(UART5, USART_FLAG_RXNE);
		uint8_t data = USART_ReceiveData(UART5);										// 读取接收到的数据
		UART5_Putc(data);
  } 
}

/******************************************************
*	函 数 名: UART6_Init                                *
*	功能说明: UART6初始化                               *
*	形    参: baud:波特率                               *
*	返 回 值: 无                                        *
******************************************************/ 
void UART6_Init(uint32_t baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 配置GPIO */
	#if UART6_Mapping		/* TX = PG14  RX = PG9 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE); 			// 打开 GPIO 时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); 			// 打开 UART 时钟
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6); 	// 将 PG14 映射为 UART6_TX
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6); 	// 将 PG9 映射为 UART6_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 					// GPIO电平切换速度
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_9;
		GPIO_Init(GPIOG, &GPIO_InitStructure);
	#else								/* TX = PC6   RX = PC7  */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 			// 打开 GPIO 时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); 			// 打开 UART 时钟
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); 	// 将 PC6 映射为 UART6_TX
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6); 	// 将 PC7 映射为 UART6_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 					// GPIO电平切换速度
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	#endif
	
  /* USART6 初始化设置 */
	USART_InitStructure.USART_BaudRate = baud;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// 1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;						// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
  USART_Init(USART6, &USART_InitStructure); 										// 初始化串口6
	
  USART_Cmd(USART6, ENABLE);																		// 使能串口6
	
	while(USART_GetFlagStatus(USART6, USART_FLAG_TC) != SET);			// 清除发送完成标志

#if EN_USART6_RX	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);								// 开启相关中断

	/* USART4 NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;							// 串口6中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;			// 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;						// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                               // 根据指定的参数初始化NVIC寄存器
#endif
}

void USART6_IRQHandler(void)                										// 串口6中断服务程序
{
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  				// 接收中断
	{
		USART_ClearFlag(USART6, USART_FLAG_RXNE);
		uint8_t data = USART_ReceiveData(USART6);										// 读取接收到的数据
		UART6_Putc(data);
  } 
}

/******************************************************
*	函 数 名: UART7_Init                                *
*	功能说明: UART7初始化                               *
*	形    参: baud:波特率                               *
*	返 回 值: 无                                        *
******************************************************/ 
void UART7_Init(uint32_t baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 配置GPIO */
	#if UART7_Mapping		/* TX = PE8   RX = PE7 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 			// 打开 GPIO 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE); 			// 打开 UART 时钟
		GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_UART7); 	  // 将 PE8 映射为 UART7_TX
		GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_UART7); 	  // 将 PE7 映射为 UART7_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 					// GPIO电平切换速度
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7;
		GPIO_Init(GPIOE, &GPIO_InitStructure);
	#else								/* TX = PF7   RX = PF6  */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 			// 打开 GPIO 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE); 			// 打开 UART 时钟
		GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF_UART7); 	  // 将 PF7 映射为 UART7_TX
		GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_UART7); 	  // 将 PF6 映射为 UART7_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 					// GPIO电平切换速度
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
		GPIO_Init(GPIOF, &GPIO_InitStructure);
	#endif
	
  /* USART7 初始化设置 */
	USART_InitStructure.USART_BaudRate = baud;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// 1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;						// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
  USART_Init(UART7, &USART_InitStructure); 											// 初始化串口7
	
  USART_Cmd(UART7, ENABLE);																			// 使能串口7
	
	while(USART_GetFlagStatus(UART7, USART_FLAG_TC) != SET);			// 清除发送完成标志

#if EN_USART7_RX	
	USART_ITConfig(UART7, USART_IT_RXNE, ENABLE);									// 开启相关中断

	/* USART7 NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = UART7_IRQn;							// 串口7中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;			// 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;						// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                               // 根据指定的参数初始化NVIC寄存器
#endif
}

void UART7_IRQHandler(void)                											// 串口7中断服务程序
{
	if(USART_GetITStatus(UART7, USART_IT_RXNE) != RESET)  				// 接收中断
	{
		USART_ClearFlag(UART7, USART_FLAG_RXNE);
		uint8_t data = USART_ReceiveData(UART7);										// 读取接收到的数据
		UART7_Putc(data);
  } 
}

/******************************************************
*	函 数 名: UART8_Init                                *
*	功能说明: UART8初始化                               *
*	形    参: baud:波特率                               *
*	返 回 值: 无                                        *
******************************************************/ 
void UART8_Init(uint32_t baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 配置GPIO */
	#if UART8_Mapping==0		/* TX = PE1   RX = PE0 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 			// 打开 GPIO 时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE); 			// 打开 UART 时钟
		GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_UART8); 	  // 将 PE1 映射为 UART8_TX
		GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_UART8); 	  // 将 PE0 映射为 UART8_RX
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 					// GPIO电平切换速度
		/* 配置 USART Tx 为复用功能 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							// 输出类型为推挽
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// 内部上拉电阻使能
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// 复用模式

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
		GPIO_Init(GPIOE, &GPIO_InitStructure);
	#endif
	
  /* USART8 初始化设置 */
	USART_InitStructure.USART_BaudRate = baud;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// 1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;						// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
  USART_Init(UART8, &USART_InitStructure); 											// 初始化串口8
	
  USART_Cmd(UART8, ENABLE);																			// 使能串口8
	
	while(USART_GetFlagStatus(UART8, USART_FLAG_TC) != SET);			// 清除发送完成标志

#if EN_UART8_RX	
	USART_ITConfig(UART8, USART_IT_RXNE, ENABLE);									// 开启相关中断

	/* UART8 NVIC配置 */
  NVIC_InitStructure.NVIC_IRQChannel = UART8_IRQn;							// 串口8中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;			// 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;						// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                               // 根据指定的参数初始化NVIC寄存器
#endif
}

void UART8_IRQHandler(void)                											// 串口8中断服务程序
{
	if(USART_GetITStatus(UART8, USART_IT_RXNE) != RESET)  				// 接收中断
	{
		USART_ClearFlag(UART8, USART_FLAG_RXNE);
		uint8_t data = USART_ReceiveData(UART8);										// 读取接收到的数据
		UART8_Putc(data);
  } 
}

/*
*******************************************************
*	函 数 名: Uart_Puts                                 *
*	功能说明: UART输出字符串                            *
*	形    参: 																					*
*				USARTx : UART                                 *
*       STR    : 输出的字符串													*
*	返 回 值: 无                                        *
*******************************************************
*/
void Uart_Puts(USART_TypeDef* USARTx, char *STR)
{
	while(*STR)
	{
		UART_Putc(USARTx, *STR++);
	}
}

/*
*******************************************************
*	函 数 名: Uart_Puts                                 *
*	功能说明: UART输出字节数组			                    *
*	形    参: 																					*
*				USARTx : UART                                 *
*       Data   : 输出的字节数组 									  	*
*       Len    : 输出的字节数组的长度  						  	*
*	返 回 值: 无                                        *
*******************************************************
*/
void Uart_Write(USART_TypeDef* USARTx, const uint8_t* Data, const uint16_t Len)
{
	for(uint16_t i=0; i<Len; i++)
	{
		UART_Putc(USARTx, Data[i]);
	}
}

/**
 * 1、对于同步模式，通过函数 USART_ClockInit()使能时钟和 polarity,phase 和 last bit
 * 
 * 2、为了获得更高的波特率，可以通过函数 USART_OverSampling8Cmd()设置 8 倍的过采样（默认是
 *    16 倍的过采样），这个函数需要在时钟函数 RCC_APBxPeriphClockCmd())之后，USART_Init()之前调用。
 */
