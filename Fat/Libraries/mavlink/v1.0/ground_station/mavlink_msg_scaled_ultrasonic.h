// MESSAGE SCALED_ULTRASONIC PACKING

#define MAVLINK_MSG_ID_SCALED_ULTRASONIC 186

typedef struct __mavlink_scaled_ultrasonic_t
{
 uint32_t time_boot_ms; /*< Timestamp (milliseconds since system boot)*/
 int16_t ultra; /*< ultrasonic data (mm)*/
} mavlink_scaled_ultrasonic_t;

#define MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN 6
#define MAVLINK_MSG_ID_186_LEN 6

#define MAVLINK_MSG_ID_SCALED_ULTRASONIC_CRC 50
#define MAVLINK_MSG_ID_186_CRC 50



#define MAVLINK_MESSAGE_INFO_SCALED_ULTRASONIC { \
	"SCALED_ULTRASONIC", \
	2, \
	{  { "time_boot_ms", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_scaled_ultrasonic_t, time_boot_ms) }, \
         { "ultra", NULL, MAVLINK_TYPE_INT16_T, 0, 4, offsetof(mavlink_scaled_ultrasonic_t, ultra) }, \
         } \
}


/**
 * @brief Pack a scaled_ultrasonic message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_boot_ms Timestamp (milliseconds since system boot)
 * @param ultra ultrasonic data (mm)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_scaled_ultrasonic_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint32_t time_boot_ms, int16_t ultra)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN];
	_mav_put_uint32_t(buf, 0, time_boot_ms);
	_mav_put_int16_t(buf, 4, ultra);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN);
#else
	mavlink_scaled_ultrasonic_t packet;
	packet.time_boot_ms = time_boot_ms;
	packet.ultra = ultra;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_SCALED_ULTRASONIC;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN, MAVLINK_MSG_ID_SCALED_ULTRASONIC_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN);
#endif
}

/**
 * @brief Pack a scaled_ultrasonic message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_boot_ms Timestamp (milliseconds since system boot)
 * @param ultra ultrasonic data (mm)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_scaled_ultrasonic_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint32_t time_boot_ms,int16_t ultra)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN];
	_mav_put_uint32_t(buf, 0, time_boot_ms);
	_mav_put_int16_t(buf, 4, ultra);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN);
#else
	mavlink_scaled_ultrasonic_t packet;
	packet.time_boot_ms = time_boot_ms;
	packet.ultra = ultra;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_SCALED_ULTRASONIC;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN, MAVLINK_MSG_ID_SCALED_ULTRASONIC_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN);
#endif
}

/**
 * @brief Encode a scaled_ultrasonic struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param scaled_ultrasonic C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_scaled_ultrasonic_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_scaled_ultrasonic_t* scaled_ultrasonic)
{
	return mavlink_msg_scaled_ultrasonic_pack(system_id, component_id, msg, scaled_ultrasonic->time_boot_ms, scaled_ultrasonic->ultra);
}

/**
 * @brief Encode a scaled_ultrasonic struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param scaled_ultrasonic C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_scaled_ultrasonic_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_scaled_ultrasonic_t* scaled_ultrasonic)
{
	return mavlink_msg_scaled_ultrasonic_pack_chan(system_id, component_id, chan, msg, scaled_ultrasonic->time_boot_ms, scaled_ultrasonic->ultra);
}

/**
 * @brief Send a scaled_ultrasonic message
 * @param chan MAVLink channel to send the message
 *
 * @param time_boot_ms Timestamp (milliseconds since system boot)
 * @param ultra ultrasonic data (mm)
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_scaled_ultrasonic_send(mavlink_channel_t chan, uint32_t time_boot_ms, int16_t ultra)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN];
	_mav_put_uint32_t(buf, 0, time_boot_ms);
	_mav_put_int16_t(buf, 4, ultra);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SCALED_ULTRASONIC, buf, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN, MAVLINK_MSG_ID_SCALED_ULTRASONIC_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SCALED_ULTRASONIC, buf, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN);
#endif
#else
	mavlink_scaled_ultrasonic_t packet;
	packet.time_boot_ms = time_boot_ms;
	packet.ultra = ultra;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SCALED_ULTRASONIC, (const char *)&packet, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN, MAVLINK_MSG_ID_SCALED_ULTRASONIC_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SCALED_ULTRASONIC, (const char *)&packet, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_scaled_ultrasonic_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t time_boot_ms, int16_t ultra)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, time_boot_ms);
	_mav_put_int16_t(buf, 4, ultra);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SCALED_ULTRASONIC, buf, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN, MAVLINK_MSG_ID_SCALED_ULTRASONIC_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SCALED_ULTRASONIC, buf, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN);
#endif
#else
	mavlink_scaled_ultrasonic_t *packet = (mavlink_scaled_ultrasonic_t *)msgbuf;
	packet->time_boot_ms = time_boot_ms;
	packet->ultra = ultra;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SCALED_ULTRASONIC, (const char *)packet, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN, MAVLINK_MSG_ID_SCALED_ULTRASONIC_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SCALED_ULTRASONIC, (const char *)packet, MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE SCALED_ULTRASONIC UNPACKING


/**
 * @brief Get field time_boot_ms from scaled_ultrasonic message
 *
 * @return Timestamp (milliseconds since system boot)
 */
static inline uint32_t mavlink_msg_scaled_ultrasonic_get_time_boot_ms(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field ultra from scaled_ultrasonic message
 *
 * @return ultrasonic data (mm)
 */
static inline int16_t mavlink_msg_scaled_ultrasonic_get_ultra(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  4);
}

/**
 * @brief Decode a scaled_ultrasonic message into a struct
 *
 * @param msg The message to decode
 * @param scaled_ultrasonic C-struct to decode the message contents into
 */
static inline void mavlink_msg_scaled_ultrasonic_decode(const mavlink_message_t* msg, mavlink_scaled_ultrasonic_t* scaled_ultrasonic)
{
#if MAVLINK_NEED_BYTE_SWAP
	scaled_ultrasonic->time_boot_ms = mavlink_msg_scaled_ultrasonic_get_time_boot_ms(msg);
	scaled_ultrasonic->ultra = mavlink_msg_scaled_ultrasonic_get_ultra(msg);
#else
	memcpy(scaled_ultrasonic, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_SCALED_ULTRASONIC_LEN);
#endif
}
