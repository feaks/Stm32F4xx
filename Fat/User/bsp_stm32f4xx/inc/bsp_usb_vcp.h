#ifndef _BSP_USB_VCP_
#define _BSP_USB_VCP_

#include "stdint.h"

#include "bsp_mavlink.h"

enum {
	USBD_Status_Init,
  USBD_Status_Reset,
  USBD_Status_Configured,
  USBD_Status_Suspended,
  USBD_Status_Resumed,
  USBD_Status_Connected,
  USBD_Status_Disconnected
};

typedef struct 
{
	uint8_t usbd_status;
} USB_VCP;

extern USB_VCP usb_vcp;
extern uint8_t usb_rx_buf[sizeof(mavlink_message_t)];
extern uint16_t usb_rx_index, usb_parse_index;

void USB_VCP_Init(void);
void mavlink_send_with_usb(mavlink_message_t * msg);
void mavlink_parse_usb_message(void);
	
#endif
