// MESSAGE GET_PID PACKING

#define MAVLINK_MSG_ID_GET_PID 181

typedef struct __mavlink_get_pid_t
{
 uint32_t number; /*< 0:all
												 1:stable_loop.pid.roll
												 2:stable_loop.pid.pitch
												 3:stable_loop.pid.yaw
												 4:rate_loop.pid.roll
												 5:rate_loop.pid.pitch
												 6:rate_loop.pid.yaw
												 7:location.pid.x  
												 8:location.pid.y
												 9:location.pid.z
												 10:speed.pid.x
												 11:speed.pid.y
												 12:speed.pid.z
												 0xffffffff : none
												 */
 uint32_t type; /*< 
				1: param
				2: pid out
			*/
} mavlink_get_pid_t;

#define MAVLINK_MSG_ID_GET_PID_LEN 8
#define MAVLINK_MSG_ID_181_LEN 8

#define MAVLINK_MSG_ID_GET_PID_CRC 246
#define MAVLINK_MSG_ID_181_CRC 246



#define MAVLINK_MESSAGE_INFO_GET_PID { \
	"GET_PID", \
	2, \
	{  { "number", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_get_pid_t, number) }, \
         { "type", NULL, MAVLINK_TYPE_UINT32_T, 0, 4, offsetof(mavlink_get_pid_t, type) }, \
         } \
}


/**
 * @brief Pack a get_pid message
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
												 7:location.pid.x  
												 8:location.pid.y
												 9:location.pid.z
												 10:speed.pid.x
												 11:speed.pid.y
												 12:speed.pid.z
												 0xffffffff : none
												 
 * @param type 
				1: param
				2: pid out
			
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_get_pid_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint32_t number, uint32_t type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GET_PID_LEN];
	_mav_put_uint32_t(buf, 0, number);
	_mav_put_uint32_t(buf, 4, type);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GET_PID_LEN);
#else
	mavlink_get_pid_t packet;
	packet.number = number;
	packet.type = type;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GET_PID_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_GET_PID;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GET_PID_LEN, MAVLINK_MSG_ID_GET_PID_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GET_PID_LEN);
#endif
}

/**
 * @brief Pack a get_pid message on a channel
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
												 7:location.pid.x  
												 8:location.pid.y
												 9:location.pid.z
												 10:speed.pid.x
												 11:speed.pid.y
												 12:speed.pid.z
												 0xffffffff : none
												 
 * @param type 
				1: param
				2: pid out
			
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_get_pid_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint32_t number,uint32_t type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GET_PID_LEN];
	_mav_put_uint32_t(buf, 0, number);
	_mav_put_uint32_t(buf, 4, type);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GET_PID_LEN);
#else
	mavlink_get_pid_t packet;
	packet.number = number;
	packet.type = type;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GET_PID_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_GET_PID;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GET_PID_LEN, MAVLINK_MSG_ID_GET_PID_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GET_PID_LEN);
#endif
}

/**
 * @brief Encode a get_pid struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param get_pid C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_get_pid_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_get_pid_t* get_pid)
{
	return mavlink_msg_get_pid_pack(system_id, component_id, msg, get_pid->number, get_pid->type);
}

/**
 * @brief Encode a get_pid struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param get_pid C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_get_pid_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_get_pid_t* get_pid)
{
	return mavlink_msg_get_pid_pack_chan(system_id, component_id, chan, msg, get_pid->number, get_pid->type);
}

/**
 * @brief Send a get_pid message
 * @param chan MAVLink channel to send the message
 *
 * @param number 0:all
												 1:stable_loop.pid.roll
												 2:stable_loop.pid.pitch
												 3:stable_loop.pid.yaw
												 4:rate_loop.pid.roll
												 5:rate_loop.pid.pitch
												 6:rate_loop.pid.yaw
												 7:location.pid.x  
												 8:location.pid.y
												 9:location.pid.z
												 10:speed.pid.x
												 11:speed.pid.y
												 12:speed.pid.z
												 0xffffffff : none
												 
 * @param type 
				1: param
				2: pid out
			
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_get_pid_send(mavlink_channel_t chan, uint32_t number, uint32_t type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GET_PID_LEN];
	_mav_put_uint32_t(buf, 0, number);
	_mav_put_uint32_t(buf, 4, type);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID, buf, MAVLINK_MSG_ID_GET_PID_LEN, MAVLINK_MSG_ID_GET_PID_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID, buf, MAVLINK_MSG_ID_GET_PID_LEN);
#endif
#else
	mavlink_get_pid_t packet;
	packet.number = number;
	packet.type = type;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID, (const char *)&packet, MAVLINK_MSG_ID_GET_PID_LEN, MAVLINK_MSG_ID_GET_PID_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID, (const char *)&packet, MAVLINK_MSG_ID_GET_PID_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_GET_PID_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_get_pid_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t number, uint32_t type)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, number);
	_mav_put_uint32_t(buf, 4, type);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID, buf, MAVLINK_MSG_ID_GET_PID_LEN, MAVLINK_MSG_ID_GET_PID_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID, buf, MAVLINK_MSG_ID_GET_PID_LEN);
#endif
#else
	mavlink_get_pid_t *packet = (mavlink_get_pid_t *)msgbuf;
	packet->number = number;
	packet->type = type;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID, (const char *)packet, MAVLINK_MSG_ID_GET_PID_LEN, MAVLINK_MSG_ID_GET_PID_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_PID, (const char *)packet, MAVLINK_MSG_ID_GET_PID_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE GET_PID UNPACKING


/**
 * @brief Get field number from get_pid message
 *
 * @return 0:all
												 1:stable_loop.pid.roll
												 2:stable_loop.pid.pitch
												 3:stable_loop.pid.yaw
												 4:rate_loop.pid.roll
												 5:rate_loop.pid.pitch
												 6:rate_loop.pid.yaw
												 7:location.pid.x  
												 8:location.pid.y
												 9:location.pid.z
												 10:speed.pid.x
												 11:speed.pid.y
												 12:speed.pid.z
												 0xffffffff : none
												 
 */
static inline uint32_t mavlink_msg_get_pid_get_number(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field type from get_pid message
 *
 * @return 
				1: param
				2: pid out
			
 */
static inline uint32_t mavlink_msg_get_pid_get_type(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  4);
}

/**
 * @brief Decode a get_pid message into a struct
 *
 * @param msg The message to decode
 * @param get_pid C-struct to decode the message contents into
 */
static inline void mavlink_msg_get_pid_decode(const mavlink_message_t* msg, mavlink_get_pid_t* get_pid)
{
#if MAVLINK_NEED_BYTE_SWAP
	get_pid->number = mavlink_msg_get_pid_get_number(msg);
	get_pid->type = mavlink_msg_get_pid_get_type(msg);
#else
	memcpy(get_pid, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_GET_PID_LEN);
#endif
}
