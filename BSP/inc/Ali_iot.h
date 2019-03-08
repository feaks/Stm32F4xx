
#ifndef __ALI_IOT_H_
#define __ALI_IOT_H_

#include "stm32f4xx.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"

/* other functions */
void Ali_Mqtt_Connect_accepted(mqtt_client_t *client);



/* Ali Mqtt callback functions */
void Ali_Mqtt_subscribe_CallBack_Func(void *arg, err_t err);
void Ali_Mqtt_publish_CallBack_Func(void *arg, const char *topic, u32_t tot_len);
void Ali_Mqtt_publishData_CallBack_Func(void *arg, err_t err);
void Ali_Mqtt_data_CallBack_Func(void *arg, const u8_t *data, u16_t len, u8_t flags);
void Ali_Mqtt_Connect_CallBack_Func(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);


void Ali_Mqtt_Connect(void);

#endif
