#include "bsp_eth.h"

#include "stm32f4xx_gpio.h"
#include "stm32f4x7_eth.h"

#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"


#include "NetworkBufferManagement.h"
#include "task.h"

#include "string.h"


void ETHERNET_NVICConfiguration(void)
{
	// LAN8720中断配置
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0X06;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0X00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

uint16_t pulValue;

u8 ETH_MACDMA_Config(void)
{
	u8 rval;
	ETH_InitTypeDef ETH_InitStructure; 
	
	//Ê¹ÄÜÒÔÌ«ÍøMACÒÔ¼°MAC½ÓÊÕºÍ·¢ËÍÊ±ÖÓ
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
                        
	ETH_DeInit();  								//AHB×ÜÏßÖØÆôÒÔÌ«Íø
	ETH_SoftwareReset();  						//Èí¼þÖØÆôÍøÂç
	while (ETH_GetSoftwareResetStatus() == SET);//µÈ´ýÈí¼þÖØÆôÍøÂçÍê³É 
	ETH_StructInit(&ETH_InitStructure); 	 	//³õÊ¼»¯ÍøÂçÎªÄ¬ÈÏÖµ  

	///ÍøÂçMAC²ÎÊýÉèÖÃ 
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;   			//¿ªÆôÍøÂç×ÔÊÊÓ¦¹¦ÄÜ
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;					//¹Ø±Õ·´À¡
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable; 		//¹Ø±ÕÖØ´«¹¦ÄÜ
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; 	//¹Ø±Õ×Ô¶¯È¥³ýPDA/CRC¹¦ÄÜ 
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;						//¹Ø±Õ½ÓÊÕËùÓÐµÄÖ¡
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;//ÔÊÐí½ÓÊÕËùÓÐ¹ã²¥Ö¡
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;			//¹Ø±Õ»ìºÏÄ£Ê½µÄµØÖ·¹ýÂË  
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;//¶ÔÓÚ×é²¥µØÖ·Ê¹ÓÃÍêÃÀµØÖ·¹ýÂË   
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;	//¶Ôµ¥²¥µØÖ·Ê¹ÓÃÍêÃÀµØÖ·¹ýÂË 
#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 			//¿ªÆôipv4ºÍTCP/UDP/ICMPµÄÖ¡Ð£ÑéºÍÐ¶ÔØ   
#endif
	//µ±ÎÒÃÇÊ¹ÓÃÖ¡Ð£ÑéºÍÐ¶ÔØ¹¦ÄÜµÄÊ±ºò£¬Ò»¶¨ÒªÊ¹ÄÜ´æ´¢×ª·¢Ä£Ê½,´æ´¢×ª·¢Ä£Ê½ÖÐÒª±£Ö¤Õû¸öÖ¡´æ´¢ÔÚFIFOÖÐ,
	//ÕâÑùMACÄÜ²åÈë/Ê¶±ð³öÖ¡Ð£ÑéÖµ,µ±ÕæÐ£ÑéÕýÈ·µÄÊ±ºòDMA¾Í¿ÉÒÔ´¦ÀíÖ¡,·ñÔò¾Í¶ªÆúµô¸ÃÖ¡
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; //¿ªÆô¶ªÆúTCP/IP´íÎóÖ¡
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;     //¿ªÆô½ÓÊÕÊý¾ÝµÄ´æ´¢×ª·¢Ä£Ê½    
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;   //¿ªÆô·¢ËÍÊý¾ÝµÄ´æ´¢×ª·¢Ä£Ê½  

	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;     	//½ûÖ¹×ª·¢´íÎóÖ¡  
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;	//²»×ª·¢¹ýÐ¡µÄºÃÖ¡ 
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;  		//´ò¿ª´¦ÀíµÚ¶þÖ¡¹¦ÄÜ
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;  	//¿ªÆôDMA´«ÊäµÄµØÖ·¶ÔÆë¹¦ÄÜ
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;            			//¿ªÆô¹Ì¶¨Í»·¢¹¦ÄÜ    
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;     		//DMA·¢ËÍµÄ×î´óÍ»·¢³¤¶ÈÎª32¸ö½ÚÅÄ   
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;			//DMA½ÓÊÕµÄ×î´óÍ»·¢³¤¶ÈÎª32¸ö½ÚÅÄ
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
	rval=ETH_Init(&ETH_InitStructure,LAN8720_PHY_ADDRESS);		//ÅäÖÃETH
	if(rval==ETH_SUCCESS)//ÅäÖÃ³É¹¦
	{
		ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);  	//Ê¹ÄÜÒÔÌ«Íø½ÓÊÕÖÐ¶Ï	
	}
	return rval;
}

u8 LAN8720_Init(void)
{
	u8 rval=0;
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOD, ENABLE);//Ê¹ÄÜGPIOÊ±ÖÓ RMII½Ó¿Ú
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);   //Ê¹ÄÜSYSCFGÊ±ÖÓ
  
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); //MACºÍPHYÖ®¼äÊ¹ÓÃRMII½Ó¿Ú

	/*ÍøÂçÒý½ÅÉèÖÃ RMII½Ó¿Ú
	  ETH_MDIO -------------------------> PA2
	  ETH_MDC --------------------------> PC1
	  ETH_RMII_REF_CLK------------------> PA1
	  ETH_RMII_CRS_DV ------------------> PA7
	  ETH_RMII_RXD0 --------------------> PC4
	  ETH_RMII_RXD1 --------------------> PC5
	  ETH_RMII_TX_EN -------------------> PG11
	  ETH_RMII_TXD0 --------------------> PG13
	  ETH_RMII_TXD1 --------------------> PG14
	  ETH_RESET-------------------------> PD3*/
					
	  //ÅäÖÃPA1 PA2 PA7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH); //Òý½Å¸´ÓÃµ½ÍøÂç½Ó¿ÚÉÏ
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

	//ÅäÖÃPC1,PC4 and PC5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH); //Òý½Å¸´ÓÃµ½ÍøÂç½Ó¿ÚÉÏ
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
                                
	//ÅäÖÃPG11, PG14 and PG13 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);
	
	//ÅäÖÃPD3ÎªÍÆÍêÊä³ö
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//ÍÆÍêÊä³ö
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	LAN8720_RESET(0);				// LAN8720复位
	vTaskDelay(100);					// 复位时RXER引脚（即PHTAD0引脚）浮空，内部带有下拉电阻，座椅SMI地址为0。
	LAN8720_RESET(1);				// LAN8720释放复位
	
	ETHERNET_NVICConfiguration();	//ÉèÖÃÖÐ¶ÏÓÅÏÈ¼¶
	rval=ETH_MACDMA_Config();		//ÅäÖÃMAC¼°DMA
	pulValue = ETH_ReadPHYRegister(0x00,( uint16_t )0x01 );
	return !rval;					//ETHµÄ¹æÔòÎª:0,Ê§°Ü;1,³É¹¦;ËùÒÔÒªÈ¡·´Ò»ÏÂ 
}





extern TaskHandle_t Task_IPHandler;


// 获取以太网速度模式
uint8_t LAN8720_GetSpeed()
{
	/**********************
	 * 001 : 10M  半双工  *
	 * 010 ：100M 半双工  *
	 * 101 ：10M  全双工  *
	 * 110 ：100M 全双工  *
	 *********************/
	return ((ETH_ReadPHYRegister(0x00,31)&0x1C)>>2); // 从LAN8720的31号寄存器中读取网络速度和双工模式
}
