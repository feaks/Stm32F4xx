#include "bsp_lan8720.h"

#include "lwip/netif.h"

/*
		ETH_MDIO -------------------------> PA2
	  ETH_MDC --------------------------> PC1
	  ETH_RMII_REF_CLK------------------> PA1
	  ETH_RMII_CRS_DV ------------------> PA7
	  ETH_RMII_RXD0 --------------------> PC4
	  ETH_RMII_RXD1 --------------------> PC5
	  ETH_RMII_TX_EN -------------------> PG11
	  ETH_RMII_TXD0 --------------------> PG13
	  ETH_RMII_TXD1 --------------------> PG14
	  ETH_RESET-------------------------> PD3
*/

__IO uint32_t  EthStatus = 0;		
extern xSemaphoreHandle s_xSemaphore;
extern struct netif gnetif;
static void LAN8720_Reset(uint8_t state)
{
	if(state ==1)
	{
		GPIO_SetBits(GPIOD , GPIO_Pin_3);
	}
	else if(state ==0)
	{
		GPIO_ResetBits(GPIOD , GPIO_Pin_3);
	}
}


void LAN8720_Init(void)
{
	
	NVIC_InitTypeDef NVIC_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOG , ENABLE);	//启动端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);   //启动SYSCFG时钟
	
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);	//LAN8720只能支持RMII
	
	//GPIOA
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOA , &GPIO_InitStruct);
	
	//GPIOC
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC , &GPIO_InitStruct);
	
	//GPIOG
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOG , &GPIO_InitStruct);
	
	//GPIOD
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;	//设定具有普通IO口的输出功能
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;	//设定为推完输出
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	//AF --- GPIOA GPIOC GPIOG
	
	GPIO_PinAFConfig(GPIOA , GPIO_PinSource1 , GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA , GPIO_PinSource2 , GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA , GPIO_PinSource7 , GPIO_AF_ETH);
	
	GPIO_PinAFConfig(GPIOC , GPIO_PinSource1 , GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC , GPIO_PinSource4 , GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC , GPIO_PinSource5 , GPIO_AF_ETH);
	
	GPIO_PinAFConfig(GPIOG , GPIO_PinSource11 , GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG , GPIO_PinSource13 , GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG , GPIO_PinSource14 , GPIO_AF_ETH);
	
	NVIC_InitStruct.NVIC_IRQChannel = ETH_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	LAN8720_Reset(0);
	vTaskDelay(50);
	LAN8720_Reset(1);
}

void ETH_MAC_DMA_Config(void)
{
	uint32_t state;
	ETH_InitTypeDef ETH_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx | RCC_AHB1Periph_ETH_MAC_Rx , ENABLE);	//启动MAC  MAC_Tx MAC_Rx
	
	ETH_DeInit(); 
	ETH_SoftwareReset();
	while(ETH_GetSoftwareResetStatus() == SET);
	ETH_StructInit(&ETH_InitStruct);
	
	ETH_InitStruct.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;	//使能地址对齐节拍 (用于DMA数据传输)
	ETH_InitStruct.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;	//禁止使用PAD和FCS字段自动去除功能
	ETH_InitStruct.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;	//启动自适应功能，系统会选择最优工作模式
	ETH_InitStruct.ETH_BackOffLimit = ETH_BackOffLimit_10;	//后退限制 在发送冲突后重新安排发送的延迟时间  一般选择最大值10
	ETH_InitStruct.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;	//接收广播帧
	ETH_InitStruct.ETH_CarrierSense = ETH_CarrierSense_Enable;	//使能载波监听功能
	ETH_InitStruct.ETH_ChecksumOffload = ETH_ChecksumOffload_Disable;	//禁用IPv4校验和减荷功能
	ETH_InitStruct.ETH_DeferralCheck = ETH_DeferralCheck_Disable;	//不检查延迟
	ETH_InitStruct.ETH_DescriptorSkipLength = 0;	//描述符跳过长度
	ETH_InitStruct.ETH_DestinationAddrFilter = ETH_DestinationAddrFilter_Normal;	//选择正常的目标地址过滤功能
	ETH_InitStruct.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;		//DMA仲裁 控制Rx与Tx优先级
	ETH_InitStruct.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;	//开启丢弃TCP/ip校验错误帧
	ETH_InitStruct.ETH_FixedBurst = ETH_FixedBurst_Enable;	//开启固定突发功能
	ETH_InitStruct.ETH_FlushReceivedFrame = ETH_FlushReceivedFrame_Enable;	//使能刷新接收帧
	ETH_InitStruct.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;	//禁止转发错误帧
	ETH_InitStruct.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;	//不转发过小的好帧
	ETH_InitStruct.ETH_HashTableHigh = 0;
	ETH_InitStruct.ETH_HashTableLow = 0;
	ETH_InitStruct.ETH_InterFrameGap = ETH_InterFrameGap_96Bit;	//设定发送帧之间的最小间隔  
	ETH_InitStruct.ETH_Jabber = ETH_Jabber_Enable;	//使能jabber定时器
	ETH_InitStruct.ETH_LoopbackMode = ETH_LoopbackMode_Disable;	//关闭回送模式
	ETH_InitStruct.ETH_Mode =ETH_Mode_FullDuplex ;	//设定模式为全双工
	ETH_InitStruct.ETH_MulticastFramesFilter =ETH_MulticastFramesFilter_PerfectHashTable ;	//多播源地址过滤模式为 完美过滤
	ETH_InitStruct.ETH_PassControlFrames = ETH_PassControlFrames_BlockAll;	//禁止转发所有控制帧
	ETH_InitStruct.ETH_PauseLowThreshold = ETH_PauseLowThreshold_Minus4;	//选择暂停时间减去4个间隔
	ETH_InitStruct.ETH_PauseTime = 0;	//保存发送控制帧中暂停时间
	ETH_InitStruct.ETH_PromiscuousMode = ETH_PromiscuousMode_Enable;	//启动混合模式
	ETH_InitStruct.ETH_ReceiveAll = ETH_ReceiveAll_Disable;	//不接收所有的MAC帧
	ETH_InitStruct.ETH_ReceiveFlowControl = ETH_ReceiveFlowControl_Disable;	//禁止接收流控制
	ETH_InitStruct.ETH_ReceiveOwn = ETH_ReceiveOwn_Enable;	//接收自身帧功能选择
	ETH_InitStruct.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;	//启动接收存储并转发功能
	ETH_InitStruct.ETH_ReceiveThresholdControl = ETH_ReceiveThresholdControl_64Bytes;	//设定接收阈值字节数
	ETH_InitStruct.ETH_RetryTransmission = ETH_RetryTransmission_Enable;	//启动传输重试功能
	ETH_InitStruct.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;	//DMA突发接收字节长度
	ETH_InitStruct.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;	//使能处理第二帧的功能
	ETH_InitStruct.ETH_SourceAddrFilter = ETH_SourceAddrFilter_Disable;	//禁止源地址过滤
	ETH_InitStruct.ETH_Speed = ETH_Speed_10M;	//设定以太网速度-->10Mbps
	ETH_InitStruct.ETH_TransmitFlowControl = ETH_TransmitFlowControl_Disable;	//禁止发送流控制
	ETH_InitStruct.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;	//使能接收存储并转发功能
	ETH_InitStruct.ETH_TransmitThresholdControl = ETH_TransmitThresholdControl_64Bytes;	//发送阈值控制
	ETH_InitStruct.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;	//DMA突发发送数据长度
	ETH_InitStruct.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_PerfectHashTable;	//单播过滤
	ETH_InitStruct.ETH_UnicastPauseFrameDetect = ETH_UnicastPauseFrameDetect_Disable;	//禁止单播暂停帧检测
	ETH_InitStruct.ETH_VLANTagComparison = ETH_VLANTagComparison_16Bit;	//VLAN标记比较
	ETH_InitStruct.ETH_VLANTagIdentifier = 0;
	ETH_InitStruct.ETH_Watchdog = ETH_Watchdog_Enable;	//启动以太网看门狗
	ETH_InitStruct.ETH_ZeroQuantaPause = ETH_ZeroQuantaPause_Disable;	//禁止零时间片暂停
	state = ETH_Init(&ETH_InitStruct , LAN8720_PHY_ADDRESS);
	
	if(state == ETH_SUCCESS)
	{
		ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);
	}
}


void ETH_BSP_Config(void)
{
	/* 端口引脚初始化 */
	LAN8720_Init();	
	
	/* ETH 初始化 */
	ETH_MAC_DMA_Config();
	
//	xTaskCreate(Eth_Link_IT_task , "ETH_Link" , configMINIMAL_STACK_SIZE , NULL , configMAX_PRIORITIES -3 , NULL);
}



void ETH_IRQHandler(void)
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  /* Frame received */
  if ( ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) == SET) 
  {
    /* Give the semaphore to wakeup LwIP task */
    xSemaphoreGiveFromISR( s_xSemaphore, &xHigherPriorityTaskWoken );
  }

  /* Clear the interrupt flags. */
  /* Clear the Eth DMA Rx IT pending bits */
  ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
  ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);

  /* Switch tasks if necessary. */	
  if( xHigherPriorityTaskWoken != pdFALSE )
  {
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  }
}


void Eth_Link_IT_task( void * pvParameters )
{
	for(;;)
	{
		vTaskDelay(500 / portTICK_PERIOD_MS);
		uint16_t i = ETH_ReadPHYRegister((uint16_t) LAN8720_PHY_ADDRESS, PHY_SR);
		
		if(i & ETH_LINK_FLAG)
		{
			/* The Device link up */
			netif_set_link_up(&gnetif);
		}
		else
		{
			/* The Device link down */
			netif_set_link_up(&gnetif);
		}
	}
}

