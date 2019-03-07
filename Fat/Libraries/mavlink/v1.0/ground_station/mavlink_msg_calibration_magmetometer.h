// MESSAGE CALIBRATION_MAGMETOMETER PACKING

#define MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER 187

typedef struct __mavlink_calibration_magmetometer_t
{
 int16_t mag_x_min; /*< mag_x_min*/
 int16_t mag_x_max; /*< mag_x_max*/
 int16_t mag_y_min; /*< mag_y_min*/
 int16_t mag_y_max; /*< mag_y_max*/
 int16_t mag_z_min; /*< mag_z_min*/
 int16_t mag_z_max; /*< mag_z_max*/
} mavlink_calibration_magmetometer_t;

#define MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN 12
#define MAVLINK_MSG_ID_187_LEN 12

#define MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_CRC 26
#define MAVLINK_MSG_ID_187_CRC 26



#define MAVLINK_MESSAGE_INFO_CALIBRATION_MAGMETOMETER { \
	"CALIBRATION_MAGMETOMETER", \
	6, \
	{  { "mag_x_min", NULL, MAVLINK_TYPE_INT16_T, 0, 0, offsetof(mavlink_calibration_magmetometer_t, mag_x_min) }, \
         { "mag_x_max", NULL, MAVLINK_TYPE_INT16_T, 0, 2, offsetof(mavlink_calibration_magmetometer_t, mag_x_max) }, \
         { "mag_y_min", NULL, MAVLINK_TYPE_INT16_T, 0, 4, offsetof(mavlink_calibration_magmetometer_t, mag_y_min) }, \
         { "mag_y_max", NULL, MAVLINK_TYPE_INT16_T, 0, 6, offsetof(mavlink_calibration_magmetometer_t, mag_y_max) }, \
         { "mag_z_min", NULL, MAVLINK_TYPE_INT16_T, 0, 8, offsetof(mavlink_calibration_magmetometer_t, mag_z_min) }, \
         { "mag_z_max", NULL, MAVLINK_TYPE_INT16_T, 0, 10, offsetof(mavlink_calibration_magmetometer_t, mag_z_max) }, \
         } \
}


/**
 * @brief Pack a calibration_magmetometer message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param mag_x_min mag_x_min
 * @param mag_x_max mag_x_max
 * @param mag_y_min mag_y_min
 * @param mag_y_max mag_y_max
 * @param mag_z_min mag_z_min
 * @param mag_z_max mag_z_max
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_calibration_magmetometer_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       int16_t mag_x_min, int16_t mag_x_max, int16_t mag_y_min, int16_t mag_y_max, int16_t mag_z_min, int16_t mag_z_max)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN];
	_mav_put_int16_t(buf, 0, mag_x_min);
	_mav_put_int16_t(buf, 2, mag_x_max);
	_mav_put_int16_t(buf, 4, mag_y_min);
	_mav_put_int16_t(buf, 6, mag_y_max);
	_mav_put_int16_t(buf, 8, mag_z_min);
	_mav_put_int16_t(buf, 10, mag_z_max);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN);
#else
	mavlink_calibration_magmetometer_t packet;
	packet.mag_x_min = mag_x_min;
	packet.mag_x_max = mag_x_max;
	packet.mag_y_min = mag_y_min;
	packet.mag_y_max = mag_y_max;
	packet.mag_z_min = mag_z_min;
	packet.mag_z_max = mag_z_max;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN);
#endif
}

/**
 * @brief Pack a calibration_magmetometer message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param mag_x_min mag_x_min
 * @param mag_x_max mag_x_max
 * @param mag_y_min mag_y_min
 * @param mag_y_max mag_y_max
 * @param mag_z_min mag_z_min
 * @param mag_z_max mag_z_max
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_calibration_magmetometer_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           int16_t mag_x_min,int16_t mag_x_max,int16_t mag_y_min,int16_t mag_y_max,int16_t mag_z_min,int16_t mag_z_max)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN];
	_mav_put_int16_t(buf, 0, mag_x_min);
	_mav_put_int16_t(buf, 2, mag_x_max);
	_mav_put_int16_t(buf, 4, mag_y_min);
	_mav_put_int16_t(buf, 6, mag_y_max);
	_mav_put_int16_t(buf, 8, mag_z_min);
	_mav_put_int16_t(buf, 10, mag_z_max);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN);
#else
	mavlink_calibration_magmetometer_t packet;
	packet.mag_x_min = mag_x_min;
	packet.mag_x_max = mag_x_max;
	packet.mag_y_min = mag_y_min;
	packet.mag_y_max = mag_y_max;
	packet.mag_z_min = mag_z_min;
	packet.mag_z_max = mag_z_max;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN);
#endif
}

/**
 * @brief Encode a calibration_magmetometer struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param calibration_magmetometer C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_calibration_magmetometer_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_calibration_magmetometer_t* calibration_magmetometer)
{
	return mavlink_msg_calibration_magmetometer_pack(system_id, component_id, msg, calibration_magmetometer->mag_x_min, calibration_magmetometer->mag_x_max, calibration_magmetometer->mag_y_min, calibration_magmetometer->mag_y_max, calibration_magmetometer->mag_z_min, calibration_magmetometer->mag_z_max);
}

/**
 * @brief Encode a calibration_magmetometer struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param calibration_magmetometer C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_calibration_magmetometer_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_calibration_magmetometer_t* calibration_magmetometer)
{
	return mavlink_msg_calibration_magmetometer_pack_chan(system_id, component_id, chan, msg, calibration_magmetometer->mag_x_min, calibration_magmetometer->mag_x_max, calibration_magmetometer->mag_y_min, calibration_magmetometer->mag_y_max, calibration_magmetometer->mag_z_min, calibration_magmetometer->mag_z_max);
}

/**
 * @brief Send a calibration_magmetometer message
 * @param chan MAVLink channel to send the message
 *
 * @param mag_x_min mag_x_min
 * @param mag_x_max mag_x_max
 * @param mag_y_min mag_y_min
 * @param mag_y_max mag_y_max
 * @param mag_z_min mag_z_min
 * @param mag_z_max mag_z_max
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_calibration_magmetometer_send(mavlink_channel_t chan, int16_t mag_x_min, int16_t mag_x_max, int16_t mag_y_min, int16_t mag_y_max, int16_t mag_z_min, int16_t mag_z_max)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN];
	_mav_put_int16_t(buf, 0, mag_x_min);
	_mav_put_int16_t(buf, 2, mag_x_max);
	_mav_put_int16_t(buf, 4, mag_y_min);
	_mav_put_int16_t(buf, 6, mag_y_max);
	_mav_put_int16_t(buf, 8, mag_z_min);
	_mav_put_int16_t(buf, 10, mag_z_max);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER, buf, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER, buf, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN);
#endif
#else
	mavlink_calibration_magmetometer_t packet;
	packet.mag_x_min = mag_x_min;
	packet.mag_x_max = mag_x_max;
	packet.mag_y_min = mag_y_min;
	packet.mag_y_max = mag_y_max;
	packet.mag_z_min = mag_z_min;
	packet.mag_z_max = mag_z_max;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER, (const char *)&packet, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER, (const char *)&packet, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_calibration_magmetometer_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  int16_t mag_x_min, int16_t mag_x_max, int16_t mag_y_min, int16_t mag_y_max, int16_t mag_z_min, int16_t mag_z_max)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_int16_t(buf, 0, mag_x_min);
	_mav_put_int16_t(buf, 2, mag_x_max);
	_mav_put_int16_t(buf, 4, mag_y_min);
	_mav_put_int16_t(buf, 6, mag_y_max);
	_mav_put_int16_t(buf, 8, mag_z_min);
	_mav_put_int16_t(buf, 10, mag_z_max);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER, buf, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER, buf, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN);
#endif
#else
	mavlink_calibration_magmetometer_t *packet = (mavlink_calibration_magmetometer_t *)msgbuf;
	packet->mag_x_min = mag_x_min;
	packet->mag_x_max = mag_x_max;
	packet->mag_y_min = mag_y_min;
	packet->mag_y_max = mag_y_max;
	packet->mag_z_min = mag_z_min;
	packet->mag_z_max = mag_z_max;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER, (const char *)packet, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER, (const char *)packet, MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE CALIBRATION_MAGMETOMETER UNPACKING


/**
 * @brief Get field mag_x_min from calibration_magmetometer message
 *
 * @return mag_x_min
 */
static inline int16_t mavlink_msg_calibration_magmetometer_get_mag_x_min(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  0);
}

/**
 * @brief Get field mag_x_max from calibration_magmetometer message
 *
 * @return mag_x_max
 */
static inline int16_t mavlink_msg_calibration_magmetometer_get_mag_x_max(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  2);
}

/**
 * @brief Get field mag_y_min from calibration_magmetometer message
 *
 * @return mag_y_min
 */
static inline int16_t mavlink_msg_calibration_magmetometer_get_mag_y_min(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  4);
}

/**
 * @brief Get field mag_y_max from calibration_magmetometer message
 *
 * @return mag_y_max
 */
static inline int16_t mavlink_msg_calibration_magmetometer_get_mag_y_max(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  6);
}

/**
 * @brief Get field mag_z_min from calibration_magmetometer message
 *
 * @return mag_z_min
 */
static inline int16_t mavlink_msg_calibration_magmetometer_get_mag_z_min(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  8);
}

/**
 * @brief Get field mag_z_max from calibration_magmetometer message
 *
 * @return mag_z_max
 */
static inline int16_t mavlink_msg_calibration_magmetometer_get_mag_z_max(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  10);
}

/**
 * @brief Decode a calibration_magmetometer message into a struct
 *
 * @param msg The message to decode
 * @param calibration_magmetometer C-struct to decode the message contents into
 */
static inline void mavlink_msg_calibration_magmetometer_decode(const mavlink_message_t* msg, mavlink_calibration_magmetometer_t* calibration_magmetometer)
{
#if MAVLINK_NEED_BYTE_SWAP
	calibration_magmetometer->mag_x_min = mavlink_msg_calibration_magmetometer_get_mag_x_min(msg);
	calibration_magmetometer->mag_x_max = mavlink_msg_calibration_magmetometer_get_mag_x_max(msg);
	calibration_magmetometer->mag_y_min = mavlink_msg_calibration_magmetometer_get_mag_y_min(msg);
	calibration_magmetometer->mag_y_max = mavlink_msg_calibration_magmetometer_get_mag_y_max(msg);
	calibration_magmetometer->mag_z_min = mavlink_msg_calibration_magmetometer_get_mag_z_min(msg);
	calibration_magmetometer->mag_z_max = mavlink_msg_calibration_magmetometer_get_mag_z_max(msg);
#else
	memcpy(calibration_magmetometer, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_CALIBRATION_MAGMETOMETER_LEN);
#endif
}
