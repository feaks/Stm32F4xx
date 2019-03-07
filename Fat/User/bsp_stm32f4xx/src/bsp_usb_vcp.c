#include "bsp_usb_vcp.h"
#include "usbd_core.h"
#include "usbd_desc.h" 
#include "usbd_cdc_core.h"
#include "usbd_cdc_vcp.h"
#include "usbd_usr.h" 
#include "usb_dcd_int.h"

USB_OTG_CORE_HANDLE    USB_OTG_dev;
USB_VCP usb_vcp;

static uint8_t usb_tx_buf[sizeof(mavlink_message_t)];
uint8_t usb_rx_buf[sizeof(mavlink_message_t)];
uint16_t usb_rx_index, usb_parse_index;
static mavlink_message_t usb_rx_message;
static mavlink_status_t usb_rx_status;

void USB_VCP_Init(void)
{
	USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb); // USB VCP初始化
}

// 使用USB发送消息
void mavlink_send_with_usb(mavlink_message_t * msg)
{	
	uint16_t len = mavlink_msg_to_send_buffer(usb_tx_buf, msg);
	VCP_DataTx(usb_tx_buf, len);
	xSemaphoreGive( xMavlinkSendBinarySemaphore);
}

// 解析USB Mavlink消息
void mavlink_parse_usb_message(void)
{
	while(usb_parse_index != usb_rx_index) {
		uint8_t result = mavlink_parse_char(MAVLINK_COMM_0, usb_rx_buf[usb_parse_index], &usb_rx_message, &usb_rx_status);
		switch(result)
		{
			case MAVLINK_FRAMING_INCOMPLETE:
				break;
			case MAVLINK_FRAMING_OK:
				mavlink_handler_message(usb_rx_message.msgid, &usb_rx_message);
				break;
			case MAVLINK_FRAMING_BAD_CRC:
				break;
		}
		if(usb_parse_index++ == sizeof(mavlink_message_t))
    {
		  usb_parse_index = 0;
		}
	}
}

// USB 中断
void OTG_FS_IRQHandler(void)
{
  	USBD_OTG_ISR_Handler(&USB_OTG_dev);
} 