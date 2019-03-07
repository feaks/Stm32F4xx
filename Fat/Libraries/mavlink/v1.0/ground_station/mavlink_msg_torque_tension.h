// MESSAGE torque_tension PACKING

#define MAVLINK_MSG_ID_torque_tension 183

typedef struct __mavlink_torque_tension_t
{
 uint32_t voltage; /*< voltage 0-20V * 1000*/
 uint32_t current; /*< current 0-50A * 1000 */
 uint32_t rpm; /*< rpm 0-26000rpm * 1000 */
 uint32_t torque; /*< torque 0-1.000N*m * 1000 */
 uint32_t tension; /*< tension 0-10.000kg * 1000 */
 uint32_t throttle; /*< throttle 0-2000ms */
} mavlink_torque_tension_t;

#define MAVLINK_MSG_ID_torque_tension_LEN 24
#define MAVLINK_MSG_ID_183_LEN 24

#define MAVLINK_MSG_ID_torque_tension_CRC 97
#define MAVLINK_MSG_ID_183_CRC 97



#define MAVLINK_MESSAGE_INFO_torque_tension { \
	"torque_tension", \
	6, \
	{  { "voltage", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_torque_tension_t, voltage) }, \
         { "current", NULL, MAVLINK_TYPE_UINT32_T, 0, 4, offsetof(mavlink_torque_tension_t, current) }, \
         { "rpm", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_torque_tension_t, rpm) }, \
         { "torque", NULL, MAVLINK_TYPE_UINT32_T, 0, 12, offsetof(mavlink_torque_tension_t, torque) }, \
         { "tension", NULL, MAVLINK_TYPE_UINT32_T, 0, 16, offsetof(mavlink_torque_tension_t, tension) }, \
         { "throttle", NULL, MAVLINK_TYPE_UINT32_T, 0, 20, offsetof(mavlink_torque_tension_t, throttle) }, \
         } \
}


/**
 * @brief Pack a torque_tension message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param voltage voltage 0-20V * 1000
 * @param current current 0-50A * 1000 
 * @param rpm rpm 0-26000rpm * 1000 
 * @param torque torque 0-1.000N*m * 1000 
 * @param tension tension 0-10.000kg * 1000 
 * @param throttle throttle 0-2000ms 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_torque_tension_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint32_t voltage, uint32_t current, uint32_t rpm, uint32_t torque, uint32_t tension, uint32_t throttle)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_torque_tension_LEN];
	_mav_put_uint32_t(buf, 0, voltage);
	_mav_put_uint32_t(buf, 4, current);
	_mav_put_uint32_t(buf, 8, rpm);
	_mav_put_uint32_t(buf, 12, torque);
	_mav_put_uint32_t(buf, 16, tension);
	_mav_put_uint32_t(buf, 20, throttle);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_torque_tension_LEN);
#else
	mavlink_torque_tension_t packet;
	packet.voltage = voltage;
	packet.current = current;
	packet.rpm = rpm;
	packet.torque = torque;
	packet.tension = tension;
	packet.throttle = throttle;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_torque_tension_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_torque_tension;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_torque_tension_LEN, MAVLINK_MSG_ID_torque_tension_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_torque_tension_LEN);
#endif
}

/**
 * @brief Pack a torque_tension message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param voltage voltage 0-20V * 1000
 * @param current current 0-50A * 1000 
 * @param rpm rpm 0-26000rpm * 1000 
 * @param torque torque 0-1.000N*m * 1000 
 * @param tension tension 0-10.000kg * 1000 
 * @param throttle throttle 0-2000ms 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_torque_tension_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint32_t voltage,uint32_t current,uint32_t rpm,uint32_t torque,uint32_t tension,uint32_t throttle)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_torque_tension_LEN];
	_mav_put_uint32_t(buf, 0, voltage);
	_mav_put_uint32_t(buf, 4, current);
	_mav_put_uint32_t(buf, 8, rpm);
	_mav_put_uint32_t(buf, 12, torque);
	_mav_put_uint32_t(buf, 16, tension);
	_mav_put_uint32_t(buf, 20, throttle);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_torque_tension_LEN);
#else
	mavlink_torque_tension_t packet;
	packet.voltage = voltage;
	packet.current = current;
	packet.rpm = rpm;
	packet.torque = torque;
	packet.tension = tension;
	packet.throttle = throttle;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_torque_tension_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_torque_tension;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_torque_tension_LEN, MAVLINK_MSG_ID_torque_tension_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_torque_tension_LEN);
#endif
}

/**
 * @brief Encode a torque_tension struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param torque_tension C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_torque_tension_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_torque_tension_t* torque_tension)
{
	return mavlink_msg_torque_tension_pack(system_id, component_id, msg, torque_tension->voltage, torque_tension->current, torque_tension->rpm, torque_tension->torque, torque_tension->tension, torque_tension->throttle);
}

/**
 * @brief Encode a torque_tension struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param torque_tension C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_torque_tension_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_torque_tension_t* torque_tension)
{
	return mavlink_msg_torque_tension_pack_chan(system_id, component_id, chan, msg, torque_tension->voltage, torque_tension->current, torque_tension->rpm, torque_tension->torque, torque_tension->tension, torque_tension->throttle);
}

/**
 * @brief Send a torque_tension message
 * @param chan MAVLink channel to send the message
 *
 * @param voltage voltage 0-20V * 1000
 * @param current current 0-50A * 1000 
 * @param rpm rpm 0-26000rpm * 1000 
 * @param torque torque 0-1.000N*m * 1000 
 * @param tension tension 0-10.000kg * 1000 
 * @param throttle throttle 0-2000ms 
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_torque_tension_send(mavlink_channel_t chan, uint32_t voltage, uint32_t current, uint32_t rpm, uint32_t torque, uint32_t tension, uint32_t throttle)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_torque_tension_LEN];
	_mav_put_uint32_t(buf, 0, voltage);
	_mav_put_uint32_t(buf, 4, current);
	_mav_put_uint32_t(buf, 8, rpm);
	_mav_put_uint32_t(buf, 12, torque);
	_mav_put_uint32_t(buf, 16, tension);
	_mav_put_uint32_t(buf, 20, throttle);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension, buf, MAVLINK_MSG_ID_torque_tension_LEN, MAVLINK_MSG_ID_torque_tension_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension, buf, MAVLINK_MSG_ID_torque_tension_LEN);
#endif
#else
	mavlink_torque_tension_t packet;
	packet.voltage = voltage;
	packet.current = current;
	packet.rpm = rpm;
	packet.torque = torque;
	packet.tension = tension;
	packet.throttle = throttle;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension, (const char *)&packet, MAVLINK_MSG_ID_torque_tension_LEN, MAVLINK_MSG_ID_torque_tension_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension, (const char *)&packet, MAVLINK_MSG_ID_torque_tension_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_torque_tension_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_torque_tension_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t voltage, uint32_t current, uint32_t rpm, uint32_t torque, uint32_t tension, uint32_t throttle)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, voltage);
	_mav_put_uint32_t(buf, 4, current);
	_mav_put_uint32_t(buf, 8, rpm);
	_mav_put_uint32_t(buf, 12, torque);
	_mav_put_uint32_t(buf, 16, tension);
	_mav_put_uint32_t(buf, 20, throttle);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension, buf, MAVLINK_MSG_ID_torque_tension_LEN, MAVLINK_MSG_ID_torque_tension_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension, buf, MAVLINK_MSG_ID_torque_tension_LEN);
#endif
#else
	mavlink_torque_tension_t *packet = (mavlink_torque_tension_t *)msgbuf;
	packet->voltage = voltage;
	packet->current = current;
	packet->rpm = rpm;
	packet->torque = torque;
	packet->tension = tension;
	packet->throttle = throttle;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension, (const char *)packet, MAVLINK_MSG_ID_torque_tension_LEN, MAVLINK_MSG_ID_torque_tension_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_torque_tension, (const char *)packet, MAVLINK_MSG_ID_torque_tension_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE torque_tension UNPACKING


/**
 * @brief Get field voltage from torque_tension message
 *
 * @return voltage 0-20V * 1000
 */
static inline uint32_t mavlink_msg_torque_tension_get_voltage(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field current from torque_tension message
 *
 * @return current 0-50A * 1000 
 */
static inline uint32_t mavlink_msg_torque_tension_get_current(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  4);
}

/**
 * @brief Get field rpm from torque_tension message
 *
 * @return rpm 0-26000rpm * 1000 
 */
static inline uint32_t mavlink_msg_torque_tension_get_rpm(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  8);
}

/**
 * @brief Get field torque from torque_tension message
 *
 * @return torque 0-1.000N*m * 1000 
 */
static inline uint32_t mavlink_msg_torque_tension_get_torque(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  12);
}

/**
 * @brief Get field tension from torque_tension message
 *
 * @return tension 0-10.000kg * 1000 
 */
static inline uint32_t mavlink_msg_torque_tension_get_tension(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  16);
}

/**
 * @brief Get field throttle from torque_tension message
 *
 * @return throttle 0-2000ms 
 */
static inline uint32_t mavlink_msg_torque_tension_get_throttle(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  20);
}

/**
 * @brief Decode a torque_tension message into a struct
 *
 * @param msg The message to decode
 * @param torque_tension C-struct to decode the message contents into
 */
static inline void mavlink_msg_torque_tension_decode(const mavlink_message_t* msg, mavlink_torque_tension_t* torque_tension)
{
#if MAVLINK_NEED_BYTE_SWAP
	torque_tension->voltage = mavlink_msg_torque_tension_get_voltage(msg);
	torque_tension->current = mavlink_msg_torque_tension_get_current(msg);
	torque_tension->rpm = mavlink_msg_torque_tension_get_rpm(msg);
	torque_tension->torque = mavlink_msg_torque_tension_get_torque(msg);
	torque_tension->tension = mavlink_msg_torque_tension_get_tension(msg);
	torque_tension->throttle = mavlink_msg_torque_tension_get_throttle(msg);
#else
	memcpy(torque_tension, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_torque_tension_LEN);
#endif
}
