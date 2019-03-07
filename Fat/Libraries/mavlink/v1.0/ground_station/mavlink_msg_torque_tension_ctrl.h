// MESSAGE torque_tension_ctrl PACKING

#define MAVLINK_MSG_ID_torque_tension_ctrl 184

typedef struct __mavlink_torque_tension_ctrl_t
{
 int16_t auto_increase; /*< auto increase 0:AUTO_NORMAL  1:AUTO_ENABLE  2:AUTO_DISABLE*/
 uint16_t period; /*< period 0-20000us*/
 uint16_t throttle; /*< throttle 0-20000us*/
} mavlink_torque_tension_ctrl_t;

#define MAVLINK_MSG_ID_torque_tension_ctrl_LEN 6
#define MAVLINK_MSG_ID_184_LEN 6

#define MAVLINK_MSG_ID_torque_tension_ctrl_CRC 224
#define MAVLINK_MSG_ID_184_CRC 224



#define MAVLINK_MESSAGE_INFO_torque_tension_ctrl { \
	"torque_tension_ctrl", \
	3, \
	{  { "auto_increase", NULL, MAVLINK_TYPE_INT16_T, 0, 0, offsetof(mavlink_torque_tension_ctrl_t, auto_increase) }, \
         { "period", NULL, MAVLINK_TYPE_UINT16_T, 0, 2, offsetof(mavlink_torque_tension_ctrl_t, period) }, \
         { "throttle", NULL, MAVLINK_TYPE_UINT16_T, 0, 4, offsetof(mavlink_torque_tension_ctrl_t, throttle) }, \
         } \
}


/**
 * @brief Pack a torque_tension_ctrl message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param auto_increase auto increase 0:AUTO_NORMAL  1:AUTO_ENABLE  2:AUTO_DISABLE
 * @param period period 0-20000us
 * @param throttle throttle 0-20000us
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_torque_tension_ctrl_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       int16_t auto_increase, uint16_t period, uint16_t throttle)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_torque_tension_ctrl_LEN];
	_mav_put_int16_t(buf, 0, auto_increase);
	_mav_put_uint16_t(buf, 2, period);
	_mav_put_uint16_t(buf, 4, throttle);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_torque_tension_ctrl_LEN);
#else
	mavlink_torque_tension_ctrl_t packet;
	packet.auto_increase = auto_increase;
	packet.period = period;
	packet.throttle = throttle;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_torque_tension_ctrl_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_torque_tension_ctrl;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_torque_tension_ctrl_LEN, MAVLINK_MSG_ID_torque_tension_ctrl_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_torque_tension_ctrl_LEN);
#endif
}

/**
 * @brief Pack a torque_tension_ctrl message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param auto_increase auto increase 0:AUTO_NORMAL  1:AUTO_ENABLE  2:AUTO_DISABLE
 * @param period period 0-20000us
 * @param throttle throttle 0-20000us
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_torque_tension_ctrl_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           int16_t auto_increase,uint16_t period,uint16_t throttle)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_torque_tension_ctrl_LEN];
	_mav_put_int16_t(buf, 0, auto_increase);
	_mav_put_uint16_t(buf, 2, period);
	_mav_put_uint16_t(buf, 4, throttle);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_torque_tension_ctrl_LEN);
#else
	mavlink_torque_tension_ctrl_t packet;
	packet.auto_increase = auto_increase;
	packet.period = period;
	packet.throttle = throttle;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_torque_tension_ctrl_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_torque_tension_ctrl;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_torque_tension_ctrl_LEN, MAVLINK_MSG_ID_torque_tension_ctrl_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_torque_tension_ctrl_LEN);
#endif
}

/**
 * @brief Encode a torque_tension_ctrl struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param torque_tension_ctrl C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_torque_tension_ctrl_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_torque_tension_ctrl_t* torque_tension_ctrl)
{
	return mavlink_msg_torque_tension_ctrl_pack(system_id, component_id, msg, torque_tension_ctrl->auto_increase, torque_tension_ctrl->period, torque_tension_ctrl->throttle);
}

/**
 * @brief Encode a torque_tension_ctrl struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param torque_tension_ctrl C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_torque_tension_ctrl_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_torque_tension_ctrl_t* torque_tension_ctrl)
{
	return mavlink_msg_torque_tension_ctrl_pack_chan(system_id, component_id, chan, msg, torque_tension_ctrl->auto_increase, torque_tension_ctrl->period, torque_tension_ctrl->throttle);
}

/**
 * @brief Send a torque_tension_ctrl message
 * @param chan MAVLink channel to send the message
 *
 * @param auto_increase auto increase 0:AUTO_NORMAL  1:AUTO_ENABLE  2:AUTO_DISABLE
 * @param period period 0-20000us
 * @param throttle throttle 0-20000us
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_torque_tension_ctrl_send(mavlink_channel_t chan, int16_t auto_increase, uint16_t period, uint16_t throttle)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_torque_tension_ctrl_LEN];
	_mav_put_int16_t(buf, 0, auto_increase);
	_mav_put_uint16_t(buf, 2, period);
	_mav_put_uint16_t(buf, 4, throttle);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension_ctrl, buf, MAVLINK_MSG_ID_torque_tension_ctrl_LEN, MAVLINK_MSG_ID_torque_tension_ctrl_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension_ctrl, buf, MAVLINK_MSG_ID_torque_tension_ctrl_LEN);
#endif
#else
	mavlink_torque_tension_ctrl_t packet;
	packet.auto_increase = auto_increase;
	packet.period = period;
	packet.throttle = throttle;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension_ctrl, (const char *)&packet, MAVLINK_MSG_ID_torque_tension_ctrl_LEN, MAVLINK_MSG_ID_torque_tension_ctrl_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension_ctrl, (const char *)&packet, MAVLINK_MSG_ID_torque_tension_ctrl_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_torque_tension_ctrl_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_torque_tension_ctrl_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  int16_t auto_increase, uint16_t period, uint16_t throttle)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_int16_t(buf, 0, auto_increase);
	_mav_put_uint16_t(buf, 2, period);
	_mav_put_uint16_t(buf, 4, throttle);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension_ctrl, buf, MAVLINK_MSG_ID_torque_tension_ctrl_LEN, MAVLINK_MSG_ID_torque_tension_ctrl_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension_ctrl, buf, MAVLINK_MSG_ID_torque_tension_ctrl_LEN);
#endif
#else
	mavlink_torque_tension_ctrl_t *packet = (mavlink_torque_tension_ctrl_t *)msgbuf;
	packet->auto_increase = auto_increase;
	packet->period = period;
	packet->throttle = throttle;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension_ctrl, (const char *)packet, MAVLINK_MSG_ID_torque_tension_ctrl_LEN, MAVLINK_MSG_ID_torque_tension_ctrl_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension_ctrl, (const char *)packet, MAVLINK_MSG_ID_torque_tension_ctrl_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE torque_tension_ctrl UNPACKING


/**
 * @brief Get field auto_increase from torque_tension_ctrl message
 *
 * @return auto increase 0:AUTO_NORMAL  1:AUTO_ENABLE  2:AUTO_DISABLE
 */
static inline int16_t mavlink_msg_torque_tension_ctrl_get_auto_increase(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  0);
}

/**
 * @brief Get field period from torque_tension_ctrl message
 *
 * @return period 0-20000us
 */
static inline uint16_t mavlink_msg_torque_tension_ctrl_get_period(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  2);
}

/**
 * @brief Get field throttle from torque_tension_ctrl message
 *
 * @return throttle 0-20000us
 */
static inline uint16_t mavlink_msg_torque_tension_ctrl_get_throttle(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  4);
}

/**
 * @brief Decode a torque_tension_ctrl message into a struct
 *
 * @param msg The message to decode
 * @param torque_tension_ctrl C-struct to decode the message contents into
 */
static inline void mavlink_msg_torque_tension_ctrl_decode(const mavlink_message_t* msg, mavlink_torque_tension_ctrl_t* torque_tension_ctrl)
{
#if MAVLINK_NEED_BYTE_SWAP
	torque_tension_ctrl->auto_increase = mavlink_msg_torque_tension_ctrl_get_auto_increase(msg);
	torque_tension_ctrl->period = mavlink_msg_torque_tension_ctrl_get_period(msg);
	torque_tension_ctrl->throttle = mavlink_msg_torque_tension_ctrl_get_throttle(msg);
#else
	memcpy(torque_tension_ctrl, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_torque_tension_ctrl_LEN);
#endif
}
