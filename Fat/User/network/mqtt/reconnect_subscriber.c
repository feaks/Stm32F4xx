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
#define mqttMSG 	"helloworld "

uint32_t connect=0;
/**
 * @brief A structure that I will use to keep track of some data needed 
 *        to setup the connection to the broker.
 * 
 * An instance of this struct will be created in my \c main(). Then, whenever
 * \ref reconnect_client is called, this instance will be passed. 
 */
struct reconnect_state_t {
    struct freertos_sockaddr addr;
    const char* topic;
    uint8_t* sendbuf;
    size_t sendbufsz;
    uint8_t* recvbuf;
    size_t recvbufsz;
};


void publish_callback(void** unused, struct mqtt_response_publish *published) ;
void publish_msg(struct mqtt_client* client,char * topic,char *msg,uint32_t len);


static	uint8_t sendbuf[2048];
static	uint8_t recvbuf[1024];
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
		FreeRTOS_closesocket(xSocket);
		return 0;
	}
	return xSocket;
}

static void Task_sync(void *p)
{
	struct mqtt_client client = *(struct mqtt_client *)p;
	while(1)
	{
		mqtt_sync(&client);
		vTaskDelay(200);
	}
}

static void reconnect_client(struct mqtt_client* client, void **reconnect_state_vptr)
{
    struct reconnect_state_t *reconnect_state = *((struct reconnect_state_t**) reconnect_state_vptr);

    /* Close the clients socket if this isn't the initial reconnect call */
    if (client->error != MQTT_ERROR_INITIAL_RECONNECT) {
        FreeRTOS_closesocket(client->socketfd);
    }

    /* Perform error handling here. */
    if (client->error != MQTT_ERROR_INITIAL_RECONNECT) {
        printf("reconnect_client: called while client was in error state \"%s\"\n", 
               mqtt_error_str(client->error)
        );
    }

    /* Open a new socket. */
    Socket_t sockfd = 0;
			
    while(sockfd == 0)
			sockfd = open_sockes(reconnect_state->addr);

    /* Reinitialize the client. */
    mqtt_reinit(client, sockfd, 
                reconnect_state->sendbuf, reconnect_state->sendbufsz,
                reconnect_state->recvbuf, reconnect_state->recvbufsz
    );
    
		
		
    /* Send connection request to the broker. */
    mqtt_connect(client, "subscribing_client", NULL, NULL, 0, NULL, NULL, 0, 400);
	
    /* Subscribe to the topic. */
    mqtt_subscribe(client, reconnect_state->topic, 0);
}

void reconnect_subscriber(void)
{
	struct reconnect_state_t reconnect_state;
	reconnect_state.addr.sin_addr = FreeRTOS_inet_addr_quick( 192,168,31,245);
	reconnect_state.addr.sin_port = FreeRTOS_htons( 1883 );
	reconnect_state.addr.sin_len = sizeof(struct freertos_sockaddr);
	reconnect_state.topic = mqttTopic;

	reconnect_state.sendbuf = sendbuf;
	reconnect_state.sendbufsz = sizeof(sendbuf);
	reconnect_state.recvbuf = recvbuf;
	reconnect_state.recvbufsz = sizeof(recvbuf);
	
	/* setup a client */
	struct mqtt_client client;
	mqtt_init_reconnect(&client, 
											reconnect_client, &reconnect_state, 
											publish_callback
	);
	
	/* start a thread to refresh the client (handle egress and ingree client traffic) */
	xTaskCreate(Task_sync,"matt_sync",500, &client, 2, NULL );
	
	/* start publishing the time */
	printf("listening for '%s' messages.\n", mqttTopic);
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

void publish_msg(struct mqtt_client* client,char * topic,char *msg,uint32_t len)
{
	static struct mqtt_client *client1 = NULL;
	
	if(client != NULL )
		client1 = client;
	if(client1 != NULL)
		mqtt_publish(client1, topic, msg, len, MQTT_PUBLISH_QOS_1|MQTT_CONNECT_PASSWORD | MQTT_CONNECT_USER_NAME);
	
		/* check for errors */
	if (client1->error != MQTT_OK) {
			printf("PUBILSH error: %s\n", mqtt_error_str(client1->error));
	}
	printf("PUBILSH success: %s\n", mqtt_error_str(client1->error));
}