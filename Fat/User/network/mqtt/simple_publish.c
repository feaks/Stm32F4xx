#include "main.h"
#include "mqtt.h"

//#define mqttClientId "8056f2f2c01d|securemode=3,signmethod=hmacsha1,timestamp=123456|"
//#define mqttUsername "sensor&a1IQnndIVtY"
//#define mqttPassword "0dac29262c34d38d795485f47c34a9db800f7bbd"
//#define mqttTopic    "/a1IQnndIVtY/sensor/sensor_data"
//#define MSG 	"helloworld "

#define mqttClientId "8056f2f2c01d|securemode=3,signmethod=hmacsha1,timestamp=123456|"
#define mqttUsername "admin"
#define mqttPassword "123456"
#define mqttTopic    "stm32"
#define MSG 	"helloworld "


static Socket_t open_sockes(struct freertos_sockaddr xMqttServerAddress);
static void publish_callback(void** unused, struct mqtt_response_publish *published);
static void Task_sync(void *p);

static	uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
static	uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */
void simple_publish(void)
{
	Socket_t xSocket=0;
	
	struct freertos_sockaddr xMqttServerAddress;
	

	/* create socket and connect to server */
	xMqttServerAddress.sin_port = FreeRTOS_htons( 1883 );
	xMqttServerAddress.sin_addr = FreeRTOS_inet_addr_quick( 192,168,31,245);
//	xMqttServerAddress.sin_addr = FreeRTOS_inet_addr_quick( 139,196,135,135);
	while(xSocket == 0)
		xSocket = open_sockes(xMqttServerAddress);/* create */

	/* setup a client */
	struct mqtt_client client;

	mqtt_init(&client, xSocket, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback);

	mqtt_connect(&client, mqttClientId, NULL, NULL, 0, mqttUsername, mqttPassword, MQTT_CONNECT_WILL_QOS_1 |MQTT_CONNECT_PASSWORD | MQTT_CONNECT_USER_NAME, 60);
	/* check that we don't have any errors */
	if (client.error != MQTT_OK) {
		printf("error: %s\n", mqtt_error_str(client.error));
	}
	xTaskCreate(Task_sync,"matt_sync",1024, &client, 2, NULL );
	/* subscribe */
//  mqtt_subscribe(&client, mqttTopic, 0);
	/* start publishing the time */
//	printf("listening for '%s' messages.\n",  mqttTopic);
		/* publish the time */
	mqtt_publish(&client, mqttTopic, MSG, strlen(MSG) + 1, MQTT_PUBLISH_QOS_1|MQTT_CONNECT_PASSWORD | MQTT_CONNECT_USER_NAME);

	/* check for errors */
	if (client.error != MQTT_OK) {
			printf("PUBILSH error: %s\n", mqtt_error_str(client.error));
	}
	printf("PUBILSH success: %s\n", mqtt_error_str(client.error));

	mqtt_subscribe(&client, "stm32", 1);
	if (client.error != MQTT_OK) {
			printf("subscribe error: %s\n", mqtt_error_str(client.error));
	}
	printf("subscribe success: %s\n", mqtt_error_str(client.error));
}

void publish_callback(void** unused, struct mqtt_response_publish *published) 
{
    /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char* topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    printf("Received publish('%s'): %s\n", topic_name, (const char*) published->application_message);

    free(topic_name);
	 
}
static Socket_t open_sockes(struct freertos_sockaddr xMqttServerAddress)
{
	Socket_t xSocket;
	TickType_t xReceiveTimeOut = pdMS_TO_TICKS( 2000 );
	TickType_t xSendTimeOut = pdMS_TO_TICKS( 1000 );
	
	xSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP );		
	configASSERT( xSocket != FREERTOS_INVALID_SOCKET );
	
	FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );
	FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_SNDTIMEO, &xSendTimeOut, sizeof( xSendTimeOut ) );
	
	FreeRTOS_bind(xSocket,&xMqttServerAddress,sizeof(xMqttServerAddress));
	if( FreeRTOS_connect( xSocket, &xMqttServerAddress, sizeof( xMqttServerAddress ) ) != 0 )
	{
		printf("err\n");
		return 0;
	}
	return xSocket;
}

void Task_sync(void *p)
{
	struct mqtt_client client = *(struct mqtt_client *)p;
	while(1)
	{
		mqtt_sync(&client);
		vTaskDelay(200);
	}
}