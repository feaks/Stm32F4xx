#include "bsp_mavlink.h"
#include "config.h"
#include "queue.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdbool.h"
#include "math_extend.h"
#include "bsp_rc.h"
#include "PID_Attitude.h"
#include "PID_Position.h"
#include "bsp_gps.h"
#include "bsp_param.h"

#include "task_mavlink.h"

#include "main.h"

xSemaphoreHandle xMavlinkSendBinarySemaphore;

static uint8_t tx_buf[sizeof(mavlink_message_t)];
static uint8_t rx_buf[sizeof(mavlink_message_t)];
static uint16_t parse_index;
static mavlink_message_t rx_message;
static mavlink_status_t rx_status;

GPS_POINT piont;

MAVLINK mavlink;

MAVLINK_STATUS mavlink_status = {.signal_percent = 10000 };

void MAVLINK_DMA_Init(void)
{	
	vSemaphoreCreateBinary(xMavlinkSendBinarySemaphore);
	
	/* Enable DMA1 clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	/* DMA1 Stream3 Configuration */
	DMA_DeInit(DMA1_Stream3);
	while(DMA_GetCmdStatus(DMA1_Stream3) != DISABLE ){}						//等待DMA可配置
	DMA_InitTypeDef DMA_InitStruct_TX;
	DMA_InitStruct_TX.DMA_Channel = DMA_Channel_4;                // 通道4 
	DMA_InitStruct_TX.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;  
	DMA_InitStruct_TX.DMA_Memory0BaseAddr = (uint32_t)&tx_buf[0];
	DMA_InitStruct_TX.DMA_DIR = DMA_DIR_MemoryToPeripheral;       // 内存到外设模式
	DMA_InitStruct_TX.DMA_BufferSize = sizeof(mavlink_message_t);
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
	DMA_Init(DMA1_Stream3, &DMA_InitStruct_TX);
	
	/* Enable the DMA global Interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE); // transfer complete interrupt
	USART_DMACmd(USART3, USART_DMAReq_Tx , ENABLE);
	
	/* DMA1 Stream1 Configuration */
	DMA_DeInit(DMA1_Stream1);
	DMA_InitTypeDef DMA_InitStruct_RX;
	DMA_InitStruct_RX.DMA_Channel = DMA_Channel_4;                // 通道4 
	DMA_InitStruct_RX.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;  
	DMA_InitStruct_RX.DMA_Memory0BaseAddr = (uint32_t)&rx_buf[0];
	DMA_InitStruct_RX.DMA_DIR = DMA_DIR_PeripheralToMemory;       // 外设到内存模式
	DMA_InitStruct_RX.DMA_BufferSize = sizeof(mavlink_message_t);
	DMA_InitStruct_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 
	DMA_InitStruct_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
	DMA_InitStruct_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct_RX.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct_RX.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct_RX.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct_RX.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStruct_RX.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct_RX.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream1, &DMA_InitStruct_RX);
	USART_DMACmd(USART3, USART_DMAReq_Rx , ENABLE);
	DMA_Cmd(DMA1_Stream1, ENABLE);
}

// 使用DMA发送消息
void mavlink_send_with_dma(mavlink_message_t * msg)
{
	uint16_t len = mavlink_msg_to_send_buffer(tx_buf, msg);
	DMA_SetCurrDataCounter(DMA1_Stream3, len);          //数据传输量 
	DMA_Cmd(DMA1_Stream3, ENABLE);
}

// DMA发送完成中断
void DMA1_Stream3_IRQHandler()
{
	static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	if(DMA_GetFlagStatus(DMA1_Stream3, DMA_FLAG_TCIF3))
	{
		DMA_ClearFlag(DMA1_Stream3, DMA_FLAG_TCIF3);
		DMA_Cmd(DMA1_Stream3, DISABLE);
		xSemaphoreGiveFromISR( xMavlinkSendBinarySemaphore, &xHigherPriorityTaskWoken );
	}
	DMA_ClearFlag(DMA1_Stream3, DMA_FLAG_HTIF3 | DMA_FLAG_TCIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3); // 虽然只打开了DMA_IT_TC中断，但运行中同时会产生DMA_FLAG_HTIF3与DMA_FLAG_FEIF3中断，目前原因不明
}

// 解析Mavlink消息
void mavlink_parse_message(void)
{
	while(parse_index != sizeof(mavlink_message_t) - DMA1_Stream1->NDTR) 
	{
		uint8_t result = mavlink_parse_char(MAVLINK_COMM_0, rx_buf[parse_index], &rx_message, &rx_status);
		switch(result)
		{
			case MAVLINK_FRAMING_INCOMPLETE:
				break;
			case MAVLINK_FRAMING_OK:
				mavlink_handler_message(rx_message.msgid, &rx_message);
				mavlink_status.lost += (uint8_t)(rx_message.seq - mavlink_status.last_seq) - 1;
				mavlink_status.last_receive_time = xTaskGetTickCount() * portTICK_RATE_MS;
				mavlink_status.last_seq = rx_message.seq;
				mavlink_status.receive++;
				if(mavlink_status.last_receive_time - mavlink_status.last_signal_time >= 1000) // 1秒更新一次信号强度
				{
					mavlink_status.last_signal_time = mavlink_status.last_receive_time;
					mavlink_status.signal_percent = (uint32_t)(mavlink_status.receive * 10000) / (mavlink_status.receive + mavlink_status.lost);
					mavlink_status.receive = mavlink_status.lost = 0;
				}
				break;
			case MAVLINK_FRAMING_BAD_CRC:
				break;
		}
		if(++parse_index == sizeof(mavlink_message_t))
    {
		  parse_index = 0;
		}
	}
}

// 设置PID
static void mavlink_handler_set_pid_pack_180(mavlink_set_pid_t * pid)
{
	switch(pid->number)  
	{
		case STABLE_ROLL:
		{
			ctrl_stable.pid.roll.kp = pid->PID_P;
			ctrl_stable.pid.roll.ki = pid->PID_I;
			ctrl_stable.pid.roll.kd = pid->PID_D;
			Param_SavePID(STABLE_ROLL_ADDR, &ctrl_stable.pid.roll);
		}
		break;
		case STABLE_PITCH:
		{
			ctrl_stable.pid.pitch.kp = pid->PID_P;
			ctrl_stable.pid.pitch.ki = pid->PID_I;
			ctrl_stable.pid.pitch.kd = pid->PID_D;
			Param_SavePID(STABLE_PITCH_ADDR, &ctrl_stable.pid.pitch);
		}
		break;
		case STABLE_YAW:
		{
			ctrl_stable.pid.yaw.kp = pid->PID_P; 
			ctrl_stable.pid.yaw.ki = pid->PID_I; 
			ctrl_stable.pid.yaw.kd = pid->PID_D; 
			Param_SavePID(STABLE_YAW_ADDR, &ctrl_stable.pid.yaw);
		}
		break;
		case RATE_ROLL:
		{
			ctrl_rate.pid.roll.kp = pid->PID_P;
			ctrl_rate.pid.roll.ki = pid->PID_I;
			ctrl_rate.pid.roll.kd = pid->PID_D;
			Param_SavePID(RATE_ROLL_ADDR, &ctrl_rate.pid.roll);
		}
		break;
		case RATE_PITCH:
		{
			ctrl_rate.pid.pitch.kp = pid->PID_P;
			ctrl_rate.pid.pitch.ki = pid->PID_I;
			ctrl_rate.pid.pitch.kd = pid->PID_D;
			Param_SavePID(RATE_PITCH_ADDR, &ctrl_rate.pid.pitch);
		}
		break;
		case RATE_YAW:
		{
			ctrl_rate.pid.yaw.kp = pid->PID_P;
			ctrl_rate.pid.yaw.ki = pid->PID_I;
			ctrl_rate.pid.yaw.kd = pid->PID_D;		
			Param_SavePID(RATE_YAW_ADDR, &ctrl_rate.pid.yaw);
		}
		break;
		case POSITION_X:
		{
			ctrl_position.pid.x.kp = pid->PID_P;
			ctrl_position.pid.x.ki = pid->PID_I;
			ctrl_position.pid.x.kd = pid->PID_D;
			Param_SavePID(POSITION_X_ADDR, &ctrl_position.pid.x);
		}
		break;
		case POSITION_Y:
		{
			ctrl_position.pid.y.kp = pid->PID_P;
			ctrl_position.pid.y.ki = pid->PID_I;
			ctrl_position.pid.y.kd = pid->PID_D;
			Param_SavePID(POSITION_Y_ADDR, &ctrl_position.pid.y);
		}
		break;
		case POSITION_Z:
		{
			ctrl_position.pid.z.kp = pid->PID_P;
			ctrl_position.pid.z.ki = pid->PID_I;
			ctrl_position.pid.z.kd = pid->PID_D;
			Param_SavePID(POSITION_Z_ADDR, &ctrl_position.pid.z);
		}
		break;
		case SPEED_X:
		{
			ctrl_speed.pid.x.kp = pid->PID_P;
			ctrl_speed.pid.x.ki = pid->PID_I;
			ctrl_speed.pid.x.kd = pid->PID_D;
			Param_SavePID(SPEED_X_ADDR, &ctrl_speed.pid.x);
		}
		break;
		case SPEED_Y:
		{
			ctrl_speed.pid.y.kp = pid->PID_P;
			ctrl_speed.pid.y.ki = pid->PID_I;
			ctrl_speed.pid.y.kd = pid->PID_D;
			Param_SavePID(SPEED_Y_ADDR, &ctrl_speed.pid.y);
		}
		break;
		case SPEED_Z:
		{
			ctrl_speed.pid.z.kp = pid->PID_P;
			ctrl_speed.pid.z.ki = pid->PID_I;
			ctrl_speed.pid.z.kd = pid->PID_D;
			Param_SavePID(SPEED_Z_ADDR, &ctrl_speed.pid.z);
		}
		break; 
	}
}


static void mavlink_get_pid(uint8_t number, float kp, float ki, float kd)
{
	while(mavlink.ack[SET_PID])  vTaskDelay(5/portTICK_RATE_MS);
	mavlink.mavlink_set_pid.number = number;
	mavlink.mavlink_set_pid.PID_P = kp;
	mavlink.mavlink_set_pid.PID_I = ki;
	mavlink.mavlink_set_pid.PID_D = kd;
	mavlink.ack[SET_PID] = true;
}

// 获取PID
static void mavlink_handler_get_pid_pack_181(mavlink_get_pid_t * pid)
{
	if(pid->type == 1) // param
	{
		switch(pid->number)
		{
			case PID_ALL:
			{
				mavlink_get_pid(STABLE_ROLL,  ctrl_stable.pid.roll.kp,  ctrl_stable.pid.roll.ki,  ctrl_stable.pid.roll.kd);
				mavlink_get_pid(STABLE_PITCH, ctrl_stable.pid.pitch.kp, ctrl_stable.pid.pitch.ki, ctrl_stable.pid.pitch.kd);
				mavlink_get_pid(STABLE_YAW,   ctrl_stable.pid.yaw.kp,   ctrl_stable.pid.yaw.ki,   ctrl_stable.pid.yaw.kd);

				mavlink_get_pid(RATE_ROLL,  ctrl_rate.pid.roll.kp,  ctrl_rate.pid.roll.ki,  ctrl_rate.pid.roll.kd);
				mavlink_get_pid(RATE_PITCH, ctrl_rate.pid.pitch.kp, ctrl_rate.pid.pitch.ki, ctrl_rate.pid.pitch.kd);
				mavlink_get_pid(RATE_YAW,   ctrl_rate.pid.yaw.kp,   ctrl_rate.pid.yaw.ki,   ctrl_rate.pid.yaw.kd);

				mavlink_get_pid(POSITION_X, ctrl_position.pid.x.kp, ctrl_position.pid.x.ki, ctrl_position.pid.x.kd);
				mavlink_get_pid(POSITION_Y, ctrl_position.pid.y.kp, ctrl_position.pid.y.ki, ctrl_position.pid.y.kd);
				mavlink_get_pid(POSITION_Z, ctrl_position.pid.z.kp, ctrl_position.pid.z.ki, ctrl_position.pid.z.kd);

				mavlink_get_pid(SPEED_X, ctrl_speed.pid.x.kp, ctrl_speed.pid.x.ki, ctrl_speed.pid.x.kd);
				mavlink_get_pid(SPEED_Y, ctrl_speed.pid.y.kp, ctrl_speed.pid.y.ki, ctrl_speed.pid.y.kd);
				mavlink_get_pid(SPEED_Z, ctrl_speed.pid.z.kp, ctrl_speed.pid.z.ki, ctrl_speed.pid.z.kd);
			}
			break;
			case STABLE_ROLL:  mavlink_get_pid(STABLE_ROLL,  ctrl_stable.pid.roll.kp,  ctrl_stable.pid.roll.ki,  ctrl_stable.pid.roll.kd);	break;
			case STABLE_PITCH: mavlink_get_pid(STABLE_PITCH, ctrl_stable.pid.pitch.kp, ctrl_stable.pid.pitch.ki, ctrl_stable.pid.pitch.kd); break;
			case STABLE_YAW:   mavlink_get_pid(STABLE_YAW,   ctrl_stable.pid.yaw.kp,   ctrl_stable.pid.yaw.ki,   ctrl_stable.pid.yaw.kd);   break;
			case RATE_ROLL:    mavlink_get_pid(RATE_ROLL,    ctrl_rate.pid.roll.kp,    ctrl_rate.pid.roll.ki,    ctrl_rate.pid.roll.kd);		break;
			case RATE_PITCH:   mavlink_get_pid(RATE_PITCH,   ctrl_rate.pid.pitch.kp,   ctrl_rate.pid.pitch.ki,   ctrl_rate.pid.pitch.kd);	  break;
			case RATE_YAW:     mavlink_get_pid(RATE_YAW,     ctrl_rate.pid.yaw.kp,     ctrl_rate.pid.yaw.ki,     ctrl_rate.pid.yaw.kd);		  break;
			case POSITION_X:   mavlink_get_pid(POSITION_X,   ctrl_position.pid.x.kp,   ctrl_position.pid.x.ki,   ctrl_position.pid.x.kd);   break;
			case POSITION_Y:   mavlink_get_pid(POSITION_Y,   ctrl_position.pid.y.kp,   ctrl_position.pid.y.ki,   ctrl_position.pid.y.kd);   break;
			case POSITION_Z:   mavlink_get_pid(POSITION_Z,   ctrl_position.pid.z.kp,   ctrl_position.pid.z.ki,   ctrl_position.pid.z.kd);   break;
			case SPEED_X:      mavlink_get_pid(SPEED_X,      ctrl_speed.pid.x.kp,      ctrl_speed.pid.x.ki,      ctrl_speed.pid.x.kd);      break;
			case SPEED_Y:      mavlink_get_pid(SPEED_Y,      ctrl_speed.pid.y.kp,      ctrl_speed.pid.y.ki,      ctrl_speed.pid.y.kd);      break;
			case SPEED_Z:      mavlink_get_pid(SPEED_Z,      ctrl_speed.pid.z.kp,      ctrl_speed.pid.z.ki,      ctrl_speed.pid.z.kd);      break;		
		}
	}
	else if(pid->type == 2) // pid out
	{
		mavlink.mavlink_get_pid.number = pid->number;
		mavlink.mavlink_get_pid.type = pid->type;
	}		
}

// 处理地面站指令
static void mavlink_handler_command_long_pack_76(mavlink_message_t * msg)
{
//	mavlink_msg_command_long_pack(MAVLINK_SYSID, MAVLINK_COMPID, &msg, 0, 0, command_long->command, 0, command_long->param1, command_long->param2, command_long->param3, command_long->param4, command_long->param5, command_long->param6, command_long->param7);
	xQueueSendToBack(xMavlinkReceiveQueue, msg, MAVLINK_WAIT);
}

void mavlink_handler_message(uint8_t msgId, mavlink_message_t* rx_message)
{
	switch(msgId)
	{
		case MAVLINK_MSG_ID_SET_PID:      // 设置PID
			mavlink_handler_set_pid_pack_180((mavlink_set_pid_t *)rx_message->payload64);
			break;
		case MAVLINK_MSG_ID_GET_PID:      // 获取PID
			mavlink_handler_get_pid_pack_181((mavlink_get_pid_t *) rx_message->payload64);
			break;
		case MAVLINK_MSG_ID_COMMAND_LONG: // 指令
			mavlink_handler_command_long_pack_76(rx_message);
   		break;
		case MAVLINK_MSG_ID_SET_MODE:	    // 设置模式，标志这航点信息结束   
			{	
				gps_route = 1; // 路径设置完成标志
			}
			break;
	}
}
