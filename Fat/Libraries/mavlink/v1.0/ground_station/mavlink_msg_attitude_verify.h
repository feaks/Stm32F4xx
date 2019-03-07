// MESSAGE attitude_verify PACKING

#define MAVLINK_MSG_ID_attitude_verify 182

typedef struct __mavlink_attitude_verify_t
{
 float angle_x; /*< angle_x -180.00~180.00*/
 float angle_y; /*< angle_y -180.00~180.00*/
 float angle_z; /*< angle_z -180.00~180.00*/
} mavlink_attitude_verify_t;

#define MAVLINK_MSG_ID_attitude_verify_LEN 12
#define MAVLINK_MSG_ID_182_LEN 12

#define MAVLINK_MSG_ID_attitude_verify_CRC 169
#define MAVLINK_MSG_ID_182_CRC 169



#define MAVLINK_MESSAGE_INFO_attitude_verify { \
	"attitude_verify", \
	3, \
	{  { "angle_x", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_attitude_verify_t, angle_x) }, \
         { "angle_y", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_attitude_verify_t, angle_y) }, \
         { "angle_z", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_attitude_verify_t, angle_z) }, \
         } \
}


/**
 * @brief Pack a attitude_verify message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param angle_x angle_x -180.00~180.00
 * @param angle_y angle_y -180.00~180.00
 * @param angle_z angle_z -180.00~180.00
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_attitude_verify_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       float angle_x, float angle_y, float angle_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_attitude_verify_LEN];
	_mav_put_float(buf, 0, angle_x);
	_mav_put_float(buf, 4, angle_y);
	_mav_put_float(buf, 8, angle_z);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_attitude_verify_LEN);
#else
	mavlink_attitude_verify_t packet;
	packet.angle_x = angle_x;
	packet.angle_y = angle_y;
	packet.angle_z = angle_z;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_attitude_verify_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_attitude_verify;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_attitude_verify_LEN, MAVLINK_MSG_ID_attitude_verify_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_attitude_verify_LEN);
#endif
}

/**
 * @brief Pack a attitude_verify message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param angle_x angle_x -180.00~180.00
 * @param angle_y angle_y -180.00~180.00
 * @param angle_z angle_z -180.00~180.00
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_attitude_verify_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           float angle_x,float angle_y,float angle_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_attitude_verify_LEN];
	_mav_put_float(buf, 0, angle_x);
	_mav_put_float(buf, 4, angle_y);
	_mav_put_float(buf, 8, angle_z);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_attitude_verify_LEN);
#else
	mavlink_attitude_verify_t packet;
	packet.angle_x = angle_x;
	packet.angle_y = angle_y;
	packet.angle_z = angle_z;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_attitude_verify_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_attitude_verify;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_attitude_verify_LEN, MAVLINK_MSG_ID_attitude_verify_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_attitude_verify_LEN);
#endif
}

/**
 * @brief Encode a attitude_verify struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param attitude_verify C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_attitude_verify_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_attitude_verify_t* attitude_verify)
{
	return mavlink_msg_attitude_verify_pack(system_id, component_id, msg, attitude_verify->angle_x, attitude_verify->angle_y, attitude_verify->angle_z);
}

/**
 * @brief Encode a attitude_verify struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param attitude_verify C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_attitude_verify_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_attitude_verify_t* attitude_verify)
{
	return mavlink_msg_attitude_verify_pack_chan(system_id, component_id, chan, msg, attitude_verify->angle_x, attitude_verify->angle_y, attitude_verify->angle_z);
}

/**
 * @brief Send a attitude_verify message
 * @param chan MAVLink channel to send the message
 *
 * @param angle_x angle_x -180.00~180.00
 * @param angle_y angle_y -180.00~180.00
 * @param angle_z angle_z -180.00~180.00
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_attitude_verify_send(mavlink_channel_t chan, float angle_x, float angle_y, float angle_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_attitude_verify_LEN];
	_mav_put_float(buf, 0, angle_x);
	_mav_put_float(buf, 4, angle_y);
	_mav_put_float(buf, 8, angle_z);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_verify, buf, MAVLINK_MSG_ID_attitude_verify_LEN, MAVLINK_MSG_ID_attitude_verify_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_verify, buf, MAVLINK_MSG_ID_attitude_verify_LEN);
#endif
#else
	mavlink_attitude_verify_t packet;
	packet.angle_x = angle_x;
	packet.angle_y = angle_y;
	packet.angle_z = angle_z;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_verify, (const char *)&packet, MAVLINK_MSG_ID_attitude_verify_LEN, MAVLINK_MSG_ID_attitude_verify_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_verify, (const char *)&packet, MAVLINK_MSG_ID_attitude_verify_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_attitude_verify_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_attitude_verify_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  float angle_x, float angle_y, float angle_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_float(buf, 0, angle_x);
	_mav_put_float(buf, 4, angle_y);
	_mav_put_float(buf, 8, angle_z);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_verify, buf, MAVLINK_MSG_ID_attitude_verify_LEN, MAVLINK_MSG_ID_attitude_verify_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_verify, buf, MAVLINK_MSG_ID_attitude_verify_LEN);
#endif
#else
	mavlink_attitude_verify_t *packet = (mavlink_attitude_verify_t *)msgbuf;
	packet->angle_x = angle_x;
	packet->angle_y = angle_y;
	packet->angle_z = angle_z;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_verify, (const char *)packet, MAVLINK_MSG_ID_attitude_verify_LEN, MAVLINK_MSG_ID_attitude_verify_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_verify, (const char *)packet, MAVLINK_MSG_ID_attitude_verify_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE attitude_verify UNPACKING


/**
 * @brief Get field angle_x from attitude_verify message
 *
 * @return angle_x -180.00~180.00
 */
static inline float mavlink_msg_attitude_verify_get_angle_x(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  0);
}

/**
 * @brief Get field angle_y from attitude_verify message
 *
 * @return angle_y -180.00~180.00
 */
static inline float mavlink_msg_attitude_verify_get_angle_y(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Get field angle_z from attitude_verify message
 *
 * @return angle_z -180.00~180.00
 */
static inline float mavlink_msg_attitude_verify_get_angle_z(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Decode a attitude_verify message into a struct
 *
 * @param msg The message to decode
 * @param attitude_verify C-struct to decode the message contents into
 */
static inline void mavlink_msg_attitude_verify_decode(const mavlink_message_t* msg, mavlink_attitude_verify_t* attitude_verify)
{
#if MAVLINK_NEED_BYTE_SWAP
	attitude_verify->angle_x = mavlink_msg_attitude_verify_get_angle_x(msg);
	attitude_verify->angle_y = mavlink_msg_attitude_verify_get_angle_y(msg);
	attitude_verify->angle_z = mavlink_msg_attitude_verify_get_angle_z(msg);
#else
	memcpy(attitude_verify, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_attitude_verify_LEN);
#endif
}
