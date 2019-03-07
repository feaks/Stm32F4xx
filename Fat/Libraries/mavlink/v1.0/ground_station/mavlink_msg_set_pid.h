// MESSAGE SET_PID PACKING

#define MAVLINK_MSG_ID_SET_PID 180

typedef struct __mavlink_set_pid_t
{
 uint32_t number; /*< 1:stable_loop.pid.roll
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
												*/
 float PID_P; /*< SET PID_P*/
 float PID_I; /*< SET PID_I*/
 float PID_D; /*< SET PID_D*/
} mavlink_set_pid_t;

#define MAVLINK_MSG_ID_SET_PID_LEN 16
#define MAVLINK_MSG_ID_180_LEN 16

#define MAVLINK_MSG_ID_SET_PID_CRC 116
#define MAVLINK_MSG_ID_180_CRC 116



#define MAVLINK_MESSAGE_INFO_SET_PID { \
	"SET_PID", \
	4, \
	{  { "number", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_set_pid_t, number) }, \
         { "PID_P", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_set_pid_t, PID_P) }, \
         { "PID_I", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_set_pid_t, PID_I) }, \
         { "PID_D", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_set_pid_t, PID_D) }, \
         } \
}


/**
 * @brief Pack a set_pid message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param number 1:stable_loop.pid.roll
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
												
 * @param PID_P SET PID_P
 * @param PID_I SET PID_I
 * @param PID_D SET PID_D
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_set_pid_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint32_t number, float PID_P, float PID_I, float PID_D)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SET_PID_LEN];
	_mav_put_uint32_t(buf, 0, number);
	_mav_put_float(buf, 4, PID_P);
	_mav_put_float(buf, 8, PID_I);
	_mav_put_float(buf, 12, PID_D);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SET_PID_LEN);
#else
	mavlink_set_pid_t packet;
	packet.number = number;
	packet.PID_P = PID_P;
	packet.PID_I = PID_I;
	packet.PID_D = PID_D;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SET_PID_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_SET_PID;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_SET_PID_LEN, MAVLINK_MSG_ID_SET_PID_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_SET_PID_LEN);
#endif
}

/**
 * @brief Pack a set_pid message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param number 1:stable_loop.pid.roll
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
												
 * @param PID_P SET PID_P
 * @param PID_I SET PID_I
 * @param PID_D SET PID_D
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_set_pid_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint32_t number,float PID_P,float PID_I,float PID_D)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SET_PID_LEN];
	_mav_put_uint32_t(buf, 0, number);
	_mav_put_float(buf, 4, PID_P);
	_mav_put_float(buf, 8, PID_I);
	_mav_put_float(buf, 12, PID_D);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SET_PID_LEN);
#else
	mavlink_set_pid_t packet;
	packet.number = number;
	packet.PID_P = PID_P;
	packet.PID_I = PID_I;
	packet.PID_D = PID_D;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SET_PID_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_SET_PID;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_SET_PID_LEN, MAVLINK_MSG_ID_SET_PID_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_SET_PID_LEN);
#endif
}

/**
 * @brief Encode a set_pid struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param set_pid C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_set_pid_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_set_pid_t* set_pid)
{
	return mavlink_msg_set_pid_pack(system_id, component_id, msg, set_pid->number, set_pid->PID_P, set_pid->PID_I, set_pid->PID_D);
}

/**
 * @brief Encode a set_pid struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param set_pid C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_set_pid_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_set_pid_t* set_pid)
{
	return mavlink_msg_set_pid_pack_chan(system_id, component_id, chan, msg, set_pid->number, set_pid->PID_P, set_pid->PID_I, set_pid->PID_D);
}

/**
 * @brief Send a set_pid message
 * @param chan MAVLink channel to send the message
 *
 * @param number 1:stable_loop.pid.roll
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
												
 * @param PID_P SET PID_P
 * @param PID_I SET PID_I
 * @param PID_D SET PID_D
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_set_pid_send(mavlink_channel_t chan, uint32_t number, float PID_P, float PID_I, float PID_D)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SET_PID_LEN];
	_mav_put_uint32_t(buf, 0, number);
	_mav_put_float(buf, 4, PID_P);
	_mav_put_float(buf, 8, PID_I);
	_mav_put_float(buf, 12, PID_D);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_PID, buf, MAVLINK_MSG_ID_SET_PID_LEN, MAVLINK_MSG_ID_SET_PID_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_PID, buf, MAVLINK_MSG_ID_SET_PID_LEN);
#endif
#else
	mavlink_set_pid_t packet;
	packet.number = number;
	packet.PID_P = PID_P;
	packet.PID_I = PID_I;
	packet.PID_D = PID_D;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_PID, (const char *)&packet, MAVLINK_MSG_ID_SET_PID_LEN, MAVLINK_MSG_ID_SET_PID_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_PID, (const char *)&packet, MAVLINK_MSG_ID_SET_PID_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_SET_PID_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_set_pid_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t number, float PID_P, float PID_I, float PID_D)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, number);
	_mav_put_float(buf, 4, PID_P);
	_mav_put_float(buf, 8, PID_I);
	_mav_put_float(buf, 12, PID_D);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_PID, buf, MAVLINK_MSG_ID_SET_PID_LEN, MAVLINK_MSG_ID_SET_PID_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_PID, buf, MAVLINK_MSG_ID_SET_PID_LEN);
#endif
#else
	mavlink_set_pid_t *packet = (mavlink_set_pid_t *)msgbuf;
	packet->number = number;
	packet->PID_P = PID_P;
	packet->PID_I = PID_I;
	packet->PID_D = PID_D;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_PID, (const char *)packet, MAVLINK_MSG_ID_SET_PID_LEN, MAVLINK_MSG_ID_SET_PID_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SET_PID, (const char *)packet, MAVLINK_MSG_ID_SET_PID_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE SET_PID UNPACKING


/**
 * @brief Get field number from set_pid message
 *
 * @return 1:stable_loop.pid.roll
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
												
 */
static inline uint32_t mavlink_msg_set_pid_get_number(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field PID_P from set_pid message
 *
 * @return SET PID_P
 */
static inline float mavlink_msg_set_pid_get_PID_P(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Get field PID_I from set_pid message
 *
 * @return SET PID_I
 */
static inline float mavlink_msg_set_pid_get_PID_I(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field PID_D from set_pid message
 *
 * @return SET PID_D
 */
static inline float mavlink_msg_set_pid_get_PID_D(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Decode a set_pid message into a struct
 *
 * @param msg The message to decode
 * @param set_pid C-struct to decode the message contents into
 */
static inline void mavlink_msg_set_pid_decode(const mavlink_message_t* msg, mavlink_set_pid_t* set_pid)
{
#if MAVLINK_NEED_BYTE_SWAP
	set_pid->number = mavlink_msg_set_pid_get_number(msg);
	set_pid->PID_P = mavlink_msg_set_pid_get_PID_P(msg);
	set_pid->PID_I = mavlink_msg_set_pid_get_PID_I(msg);
	set_pid->PID_D = mavlink_msg_set_pid_get_PID_D(msg);
#else
	memcpy(set_pid, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_SET_PID_LEN);
#endif
}
