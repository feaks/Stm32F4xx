#include "bsp_cli.h"

#include "bsp_uart.h"
#include "stdlib.h"
#include "string.h"

#define CMD_MAX_LEN  	6
#define ARG_MAX_LEN  	6


#define RX_LEN  			30
#define TX_LEN  			30
uint8_t  rx_buf[RX_LEN];
uint8_t  tx_buf[TX_LEN];
void UARTl_DMA_Init(void)
{

	/* Enable DMA2 clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	/* DMA2 Stream7  Configuration */
	DMA_DeInit(DMA2_Stream7);
	DMA_InitTypeDef DMA_InitStruct_TX;
	DMA_InitStruct_TX.DMA_Channel = DMA_Channel_4;                // 通道4 
	DMA_InitStruct_TX.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;  
	DMA_InitStruct_TX.DMA_Memory0BaseAddr = (uint32_t)&tx_buf[0];
	DMA_InitStruct_TX.DMA_DIR = DMA_DIR_MemoryToPeripheral;       // 内存到外设模式
	DMA_InitStruct_TX.DMA_BufferSize = sizeof(tx_buf);
  DMA_InitStruct_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 
	DMA_InitStruct_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
	DMA_InitStruct_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct_TX.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct_TX.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct_TX.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct_TX.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStruct_TX.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct_TX.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7, &DMA_InitStruct_TX);
	
	/* Enable the DMA global Interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE); // transfer complete interrupt
	USART_DMACmd(USART1, USART_DMAReq_Tx , ENABLE);
	DMA_Cmd(DMA2_Stream7, DISABLE);
	
	/* DMA12 Stream5 Configuration */
	DMA_DeInit(DMA2_Stream5);
	DMA_InitTypeDef DMA_InitStruct_RX;
	DMA_InitStruct_RX.DMA_Channel = DMA_Channel_4;                // 通道4 
	DMA_InitStruct_RX.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;  
	DMA_InitStruct_RX.DMA_Memory0BaseAddr = (uint32_t)&rx_buf[0];
	DMA_InitStruct_RX.DMA_DIR = DMA_DIR_PeripheralToMemory;       // 外设到内存模式
	DMA_InitStruct_RX.DMA_BufferSize = sizeof(rx_buf);
	DMA_InitStruct_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
	DMA_InitStruct_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct_RX.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct_RX.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct_RX.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct_RX.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStruct_RX.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct_RX.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream5, &DMA_InitStruct_RX);
	USART_DMACmd(USART1, USART_DMAReq_Rx , ENABLE);
	DMA_Cmd(DMA2_Stream5, ENABLE);
}
void UART1_TX_DMA_Config(uint8_t *BufferSRC,uint32_t BufferSize)
{
	DMA_InitTypeDef DMA_InitStruct_TX;
	
	DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_FEIF7 | DMA_FLAG_DMEIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_HTIF7 | DMA_FLAG_TCIF7);
	
	DMA_Cmd(DMA2_Stream7, DISABLE);
	/* DMA2 Stream7  Configuration */
	DMA_DeInit(DMA2_Stream7);

	DMA_InitStruct_TX.DMA_Channel = DMA_Channel_4;                // 通道4 
	DMA_InitStruct_TX.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;  
	DMA_InitStruct_TX.DMA_Memory0BaseAddr = (uint32_t)BufferSRC;
	DMA_InitStruct_TX.DMA_DIR = DMA_DIR_MemoryToPeripheral;       // 内存到外设模式
	DMA_InitStruct_TX.DMA_BufferSize = BufferSize;
  DMA_InitStruct_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 
	DMA_InitStruct_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
	DMA_InitStruct_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct_TX.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStruct_TX.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct_TX.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct_TX.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStruct_TX.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct_TX.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7, &DMA_InitStruct_TX);
	
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
	
	DMA_Cmd(DMA2_Stream7, ENABLE);
	
}

void UART1_DMA_Send( uint8_t *BufferSRC,uint32_t BufferSize)
{
	UART1_TX_DMA_Config(BufferSRC,BufferSize);
}

// DMA发送完成中断
void DMA2_Stream7_IRQHandler()
{
	if(DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF5))
	{
		DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7  );
		DMA_Cmd(DMA2_Stream7, DISABLE);
	}
	DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_HTIF7 | DMA_FLAG_TCIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_FEIF7 | DMA_FLAG_DMEIF7); // 虽然只打开了DMA_IT_TC中断，但运行中同时会产生DMA_FLAG_HTIF3与DMA_FLAG_FEIF3中断，目前原因不明
}

void CLI_Init(uint32_t baud)
{
	/* */
	
	UART1_Init(baud);
	
	UARTl_DMA_Init();
}

/* 从串口获取命令行*/
int32_t get_cmd(uint8_t *cmd,uint8_t *len)
{
	static uint8_t index = 0;
	static uint8_t current_cmd[30];
	
	static uint8_t current ;/* '\n' 的起始位置*/
	static uint8_t next;		/* '\n' 的下一个起始位置，*/
	
	while(index != RX_LEN - DMA2_Stream5->NDTR)
	{
		/* xunzhao meihangmingling */
		if(rx_buf[index] == '\n')
		{
			if(rx_buf[current] != '\n')
			{
				/* 是第一个命令*/
				current = index;
				/*复制字符串到cmd中*/
				*len = index +1;
				memcpy(current_cmd,rx_buf,*len);
				cmd = current_cmd;			
			}else
			{
				/*bushi*/
				next = index;
			}
		}
		/* */
		if(rx_buf[current] == '\n' && rx_buf[next] == '\n' && next != current)
		{ /* 复制字符串到充满到中*/
			if(next > current)
			{
				/* 字符串在数组中连续 */
				*len = next - current;
				memcpy(current_cmd,&rx_buf[current + 1],*len);
			}else
			{
				/* 字符串在数组中不连续 */
				/* 拷贝时无需拷贝开始的 '\n' */
				if( current !=  RX_LEN - 1)
				{
					/* 先拷贝数组尾部的字符串，后拷贝数组头部的字符串*/
					*len = RX_LEN - 1 - current;
					memcpy(current_cmd,&rx_buf[current + 1],*len);

					memcpy(&current_cmd[*len],&rx_buf,next +1);
					*len += next + 1;
				}
				else
				{
					*len = next + 1;
					memcpy(current_cmd,rx_buf,*len);
				}
			
			}
			current = next;
		}
		
		if(++index  == RX_LEN)
			index = 0;
	}
	return 1;
}
/*
 * 命令格式 :CMD  ARG
 * 如 cd /
 */
int32_t cmd_parse(uint8_t *str,uint8_t *cmd,uint8_t *arg)
{
	uint8_t command[CMD_MAX_LEN];
	uint8_t *temp = command;
	uint8_t *option;
	uint8_t args[ARG_MAX_LEN];
	/* 截取cmd*/
	while( *str != ' ' )
	{
		*(temp++) = *(str++);
	}
	/* 确认空格*/
	if( *(str++) != ' ')
		return -1;
	/* 将字符串尾转化为 '\0'*/
	*temp = '\0';
	temp = args;
	/*消除剩余空格*/
	while( *str == ' ')
	{
		(str++);
	}
	/* 获取参数*/
	while( *str != '\n')
	{
		*(temp++) = *(str++);
	}
	*temp = '\0' ;
	
	if( cmd == NULL || arg == NULL)
		return 0;
	
	memcpy( cmd , command , (strlen((const char *)command) + 1) );
	
	memcpy( arg , args , (strlen((const char *)args) + 1) );
}
int cmd_execute(uint8_t * cmd,uint8_t *args)
{
	
}
void poll_cwd(void)
{
	/* cong chuanhou huoqu cifuchuan*/
	
	
}

