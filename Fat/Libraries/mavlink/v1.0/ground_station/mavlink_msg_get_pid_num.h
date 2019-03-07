// MESSAGE GET_PID_NUM PACKING

#define MAVLINK_MSG_ID_GET_PID_NUM 179

typedef struct __mavlink_get_pid_num_t
{
 uint32_t number; /*< 0:all   
												 1:stable_loop.pid.roll
												 2:stable_loop.pid.pitch
												 3:stable_loop.pid.yaw
												 4:rate_loop.pid.roll
												 5:rate_loop.pid.pitch
												 6:rate_loop.pid.yaw
												 7:height  
												 8:height velocity
												 9:stable.roll
												 10:stable.pitch
												 11:rate.roll
												 12:rate.pitch					
												 */
} mavlink_get_pid_num_t;

#define MAVLINK_MSG_ID_GET_PID_NUM_LEN 4
#define MAVLINK_MSG_ID_179_LEN 4

#define MAVLINK_MSG_ID_GET_PID_NUM_CRC 82
#define MAVLINK_MSG_ID_179_CRC 82



#define MAVLINK_MESSAGE_INFO_GET_PID_NUM { \
	"GET_PID_NUM", \
	1, \
	{  { "number", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_get_pid_num_t, number) }, \
         } \
}


/**
 * @brief Pack a get_pid_num message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param number 0:all   
												 1:stable_loop.pid.roll
												 2:stable_loop.pid.pitch
												 3:stable_loop.pid.yaw
												 4:rate_loop.pid.roll
												 5:rate_loop.pid.pitch
												 6:rate_loop.pid.yaw
												 7:height  
												 8:height velocity
												 9:stable.roll
												 10:stable.pitch
												 11:rate.roll
												 12:rate.pitch					
												 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_get_pid_num_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint32_t number)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GET_PID_NUM_LEN];
	_mav_put_uint32_t(buf, 0, number);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GET_PID_NUM_LEN);
#else
	mavlink_get_pid_num_t packet;
	packet.number = number;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GET_PID_NUM_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_GET_PID_NUM;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GET_PID_NUM_LEN, MAVLINK_MSG_ID_GET_PID_NUM_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GET_PID_NUM_LEN);
#endif
}

/**
 * @brief Pack a get_pid_num message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param number 0:all   
												 1:stable_loop.pid.roll
												 2:stable_loop.pid.pitch
												 3:stable_loop.pid.yaw
												 4:rate_loop.pid.roll
												 5:rate_loop.pid.pitch
												 6:rate_loop.pid.yaw
												 7:height  
												 8:height velocity
												 9:stable.roll
												 10:stable.pitch
												 11:rate.roll
												 12:rate.pitch					
												 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_get_pid_num_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint32_t number)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GET_PID_NUM_LEN];
	_mav_put_uint32_t(buf, 0, number);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GET_PID_NUM_LEN);
#else
	mavlink_get_pid_num_t packet;
	packet.number = number;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GET_PID_NUM_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_GET_PID_NUM;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GET_PID_NUM_LEN, MAVLINK_MSG_ID_GET_PID_NUM_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GET_PID_NUM_LEN);
#endif
}

/**
 * @brief Encode a get_pid_num struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param get_pid_num C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_get_pid_num_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_get_pid_num_t* get_pid_num)
{
	return mavlink_msg_get_pid_num_pack(system_id, component_id, msg, get_pid_num->number);
}

/**
 * @brief Encode a get_pid_num struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param get_pid_num C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_get_pid_num_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_get_pid_num_t* get_pid_num)
{
	return mavlink_msg_get_pid_num_pack_chan(system_id, component_id, chan, msg, get_pid_num->number);
}

/**
 * @brief Send a get_pid_num message
 * @param chan MAVLink channel to send the message
 *
 * @param number 0:all   
												 1:stable_loop.pid.roll
												 2:stable_loop.pid.pitch
												 3:stable_loop.pid.yaw
												 4:rate_loop.pid.roll
												 5:rate_loop.pid.pitch
												 6:rate_loop.pid.yaw
												 7:height  
												 8:height velocity
												 9:stable.roll
												 10:stable.pitch
												 11:rate.roll
												 12:rate.pitch					
												 
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_get_pid_num_send(mavlink_channel_t chan, uint32_t number)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GET_PID_NUM_LEN];
	_mav_put_uint32_t(buf, 0, number);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID_NUM, buf, MAVLINK_MSG_ID_GET_PID_NUM_LEN, MAVLINK_MSG_ID_GET_PID_NUM_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID_NUM, buf, MAVLINK_MSG_ID_GET_PID_NUM_LEN);
#endif
#else
	mavlink_get_pid_num_t packet;
	packet.number = number;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID_NUM, (const char *)&packet, MAVLINK_MSG_ID_GET_PID_NUM_LEN, MAVLINK_MSG_ID_GET_PID_NUM_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID_NUM, (const char *)&packet, MAVLINK_MSG_ID_GET_PID_NUM_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_GET_PID_NUM_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_get_pid_num_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t number)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, number);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID_NUM, buf, MAVLINK_MSG_ID_GET_PID_NUM_LEN, MAVLINK_MSG_ID_GET_PID_NUM_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID_NUM, buf, MAVLINK_MSG_ID_GET_PID_NUM_LEN);
#endif
#else
	mavlink_get_pid_num_t *packet = (mavlink_get_pid_num_t *)msgbuf;
	packet->number = number;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID_NUM, (const char *)packet, MAVLINK_MSG_ID_GET_PID_NUM_LEN, MAVLINK_MSG_ID_GET_PID_NUM_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID_NUM, (const char *)packet, MAVLINK_MSG_ID_GET_PID_NUM_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE GET_PID_NUM UNPACKING


/**
 * @brief Get field number from get_pid_num message
 *
 * @return 0:all   
												 1:stable_loop.pid.roll
												 2:stable_loop.pid.pitch
												 3:stable_loop.pid.yaw
												 4:rate_loop.pid.roll
												 5:rate_loop.pid.pitch
												 6:rate_loop.pid.yaw
												 7:height  
												 8:height velocity
												 9:stable.roll
												 10:stable.pitch
												 11:rate.roll
												 12:rate.pitch					
												 
 */
static inline uint32_t mavlink_msg_get_pid_num_get_number(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Decode a get_pid_num message into a struct
 *
 * @param msg The message to decode
 * @param get_pid_num C-struct to decode the message contents into
 */
static inline void mavlink_msg_get_pid_num_decode(const mavlink_message_t* msg, mavlink_get_pid_num_t* get_pid_num)
{
#if MAVLINK_NEED_BYTE_SWAP
	get_pid_num->number = mavlink_msg_get_pid_num_get_number(msg);
#else
	memcpy(get_pid_num, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_GET_PID_NUM_LEN);
#endif
}
