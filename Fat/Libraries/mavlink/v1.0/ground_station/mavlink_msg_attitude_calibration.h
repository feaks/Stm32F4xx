// MESSAGE attitude_calibration PACKING

#define MAVLINK_MSG_ID_attitude_calibration 185

typedef struct __mavlink_attitude_calibration_t
{
 int16_t ctrl_auto; /*< auto calibration 0:AUTO_NORMAL  1:AUTO_ENABLE  2:AUTO_DISABLE*/
 int16_t ctrl_x; /*< ctrl_x -10000~10000 for -180~180*/
 int16_t ctrl_y; /*< ctrl_y -10000~10000 for -180~180*/
 int16_t ctrl_z; /*< ctrl_z -10000~10000 for -180~180*/
 uint8_t calibration_flag; /*< calibration flag 0~10*/
} mavlink_attitude_calibration_t;

#define MAVLINK_MSG_ID_attitude_calibration_LEN 9
#define MAVLINK_MSG_ID_185_LEN 9

#define MAVLINK_MSG_ID_attitude_calibration_CRC 161
#define MAVLINK_MSG_ID_185_CRC 161



#define MAVLINK_MESSAGE_INFO_attitude_calibration { \
	"attitude_calibration", \
	5, \
	{  { "ctrl_auto", NULL, MAVLINK_TYPE_INT16_T, 0, 0, offsetof(mavlink_attitude_calibration_t, ctrl_auto) }, \
         { "ctrl_x", NULL, MAVLINK_TYPE_INT16_T, 0, 2, offsetof(mavlink_attitude_calibration_t, ctrl_x) }, \
         { "ctrl_y", NULL, MAVLINK_TYPE_INT16_T, 0, 4, offsetof(mavlink_attitude_calibration_t, ctrl_y) }, \
         { "ctrl_z", NULL, MAVLINK_TYPE_INT16_T, 0, 6, offsetof(mavlink_attitude_calibration_t, ctrl_z) }, \
         { "calibration_flag", NULL, MAVLINK_TYPE_UINT8_T, 0, 8, offsetof(mavlink_attitude_calibration_t, calibration_flag) }, \
         } \
}


/**
 * @brief Pack a attitude_calibration message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param ctrl_auto auto calibration 0:AUTO_NORMAL  1:AUTO_ENABLE  2:AUTO_DISABLE
 * @param calibration_flag calibration flag 0~10
 * @param ctrl_x ctrl_x -10000~10000 for -180~180
 * @param ctrl_y ctrl_y -10000~10000 for -180~180
 * @param ctrl_z ctrl_z -10000~10000 for -180~180
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_attitude_calibration_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       int16_t ctrl_auto, uint8_t calibration_flag, int16_t ctrl_x, int16_t ctrl_y, int16_t ctrl_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_attitude_calibration_LEN];
	_mav_put_int16_t(buf, 0, ctrl_auto);
	_mav_put_int16_t(buf, 2, ctrl_x);
	_mav_put_int16_t(buf, 4, ctrl_y);
	_mav_put_int16_t(buf, 6, ctrl_z);
	_mav_put_uint8_t(buf, 8, calibration_flag);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_attitude_calibration_LEN);
#else
	mavlink_attitude_calibration_t packet;
	packet.ctrl_auto = ctrl_auto;
	packet.ctrl_x = ctrl_x;
	packet.ctrl_y = ctrl_y;
	packet.ctrl_z = ctrl_z;
	packet.calibration_flag = calibration_flag;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_attitude_calibration_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_attitude_calibration;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_attitude_calibration_LEN, MAVLINK_MSG_ID_attitude_calibration_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_attitude_calibration_LEN);
#endif
}

/**
 * @brief Pack a attitude_calibration message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param ctrl_auto auto calibration 0:AUTO_NORMAL  1:AUTO_ENABLE  2:AUTO_DISABLE
 * @param calibration_flag calibration flag 0~10
 * @param ctrl_x ctrl_x -10000~10000 for -180~180
 * @param ctrl_y ctrl_y -10000~10000 for -180~180
 * @param ctrl_z ctrl_z -10000~10000 for -180~180
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_attitude_calibration_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           int16_t ctrl_auto,uint8_t calibration_flag,int16_t ctrl_x,int16_t ctrl_y,int16_t ctrl_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_attitude_calibration_LEN];
	_mav_put_int16_t(buf, 0, ctrl_auto);
	_mav_put_int16_t(buf, 2, ctrl_x);
	_mav_put_int16_t(buf, 4, ctrl_y);
	_mav_put_int16_t(buf, 6, ctrl_z);
	_mav_put_uint8_t(buf, 8, calibration_flag);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_attitude_calibration_LEN);
#else
	mavlink_attitude_calibration_t packet;
	packet.ctrl_auto = ctrl_auto;
	packet.ctrl_x = ctrl_x;
	packet.ctrl_y = ctrl_y;
	packet.ctrl_z = ctrl_z;
	packet.calibration_flag = calibration_flag;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_attitude_calibration_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_attitude_calibration;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_attitude_calibration_LEN, MAVLINK_MSG_ID_attitude_calibration_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_attitude_calibration_LEN);
#endif
}

/**
 * @brief Encode a attitude_calibration struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param attitude_calibration C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_attitude_calibration_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_attitude_calibration_t* attitude_calibration)
{
	return mavlink_msg_attitude_calibration_pack(system_id, component_id, msg, attitude_calibration->ctrl_auto, attitude_calibration->calibration_flag, attitude_calibration->ctrl_x, attitude_calibration->ctrl_y, attitude_calibration->ctrl_z);
}

/**
 * @brief Encode a attitude_calibration struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param attitude_calibration C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_attitude_calibration_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_attitude_calibration_t* attitude_calibration)
{
	return mavlink_msg_attitude_calibration_pack_chan(system_id, component_id, chan, msg, attitude_calibration->ctrl_auto, attitude_calibration->calibration_flag, attitude_calibration->ctrl_x, attitude_calibration->ctrl_y, attitude_calibration->ctrl_z);
}

/**
 * @brief Send a attitude_calibration message
 * @param chan MAVLink channel to send the message
 *
 * @param ctrl_auto auto calibration 0:AUTO_NORMAL  1:AUTO_ENABLE  2:AUTO_DISABLE
 * @param calibration_flag calibration flag 0~10
 * @param ctrl_x ctrl_x -10000~10000 for -180~180
 * @param ctrl_y ctrl_y -10000~10000 for -180~180
 * @param ctrl_z ctrl_z -10000~10000 for -180~180
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_attitude_calibration_send(mavlink_channel_t chan, int16_t ctrl_auto, uint8_t calibration_flag, int16_t ctrl_x, int16_t ctrl_y, int16_t ctrl_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_attitude_calibration_LEN];
	_mav_put_int16_t(buf, 0, ctrl_auto);
	_mav_put_int16_t(buf, 2, ctrl_x);
	_mav_put_int16_t(buf, 4, ctrl_y);
	_mav_put_int16_t(buf, 6, ctrl_z);
	_mav_put_uint8_t(buf, 8, calibration_flag);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_calibration, buf, MAVLINK_MSG_ID_attitude_calibration_LEN, MAVLINK_MSG_ID_attitude_calibration_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_calibration, buf, MAVLINK_MSG_ID_attitude_calibration_LEN);
#endif
#else
	mavlink_attitude_calibration_t packet;
	packet.ctrl_auto = ctrl_auto;
	packet.ctrl_x = ctrl_x;
	packet.ctrl_y = ctrl_y;
	packet.ctrl_z = ctrl_z;
	packet.calibration_flag = calibration_flag;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_calibration, (const char *)&packet, MAVLINK_MSG_ID_attitude_calibration_LEN, MAVLINK_MSG_ID_attitude_calibration_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_calibration, (const char *)&packet, MAVLINK_MSG_ID_attitude_calibration_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_attitude_calibration_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_attitude_calibration_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  int16_t ctrl_auto, uint8_t calibration_flag, int16_t ctrl_x, int16_t ctrl_y, int16_t ctrl_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_int16_t(buf, 0, ctrl_auto);
	_mav_put_int16_t(buf, 2, ctrl_x);
	_mav_put_int16_t(buf, 4, ctrl_y);
	_mav_put_int16_t(buf, 6, ctrl_z);
	_mav_put_uint8_t(buf, 8, calibration_flag);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_calibration, buf, MAVLINK_MSG_ID_attitude_calibration_LEN, MAVLINK_MSG_ID_attitude_calibration_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_calibration, buf, MAVLINK_MSG_ID_attitude_calibration_LEN);
#endif
#else
	mavlink_attitude_calibration_t *packet = (mavlink_attitude_calibration_t *)msgbuf;
	packet->ctrl_auto = ctrl_auto;
	packet->ctrl_x = ctrl_x;
	packet->ctrl_y = ctrl_y;
	packet->ctrl_z = ctrl_z;
	packet->calibration_flag = calibration_flag;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_calibration, (const char *)packet, MAVLINK_MSG_ID_attitude_calibration_LEN, MAVLINK_MSG_ID_attitude_calibration_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_attitude_calibration, (const char *)packet, MAVLINK_MSG_ID_attitude_calibration_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE attitude_calibration UNPACKING


/**
 * @brief Get field ctrl_auto from attitude_calibration message
 *
 * @return auto calibration 0:AUTO_NORMAL  1:AUTO_ENABLE  2:AUTO_DISABLE
 */
static inline int16_t mavlink_msg_attitude_calibration_get_ctrl_auto(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  0);
}

/**
 * @brief Get field calibration_flag from attitude_calibration message
 *
 * @return calibration flag 0~10
 */
static inline uint8_t mavlink_msg_attitude_calibration_get_calibration_flag(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  8);
}

/**
 * @brief Get field ctrl_x from attitude_calibration message
 *
 * @return ctrl_x -10000~10000 for -180~180
 */
static inline int16_t mavlink_msg_attitude_calibration_get_ctrl_x(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  2);
}

/**
 * @brief Get field ctrl_y from attitude_calibration message
 *
 * @return ctrl_y -10000~10000 for -180~180
 */
static inline int16_t mavlink_msg_attitude_calibration_get_ctrl_y(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  4);
}

/**
 * @brief Get field ctrl_z from attitude_calibration message
 *
 * @return ctrl_z -10000~10000 for -180~180
 */
static inline int16_t mavlink_msg_attitude_calibration_get_ctrl_z(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  6);
}

/**
 * @brief Decode a attitude_calibration message into a struct
 *
 * @param msg The message to decode
 * @param attitude_calibration C-struct to decode the message contents into
 */
static inline void mavlink_msg_attitude_calibration_decode(const mavlink_message_t* msg, mavlink_attitude_calibration_t* attitude_calibration)
{
#if MAVLINK_NEED_BYTE_SWAP
	attitude_calibration->ctrl_auto = mavlink_msg_attitude_calibration_get_ctrl_auto(msg);
	attitude_calibration->ctrl_x = mavlink_msg_attitude_calibration_get_ctrl_x(msg);
	attitude_calibration->ctrl_y = mavlink_msg_attitude_calibration_get_ctrl_y(msg);
	attitude_calibration->ctrl_z = mavlink_msg_attitude_calibration_get_ctrl_z(msg);
	attitude_calibration->calibration_flag = mavlink_msg_attitude_calibration_get_calibration_flag(msg);
#else
	memcpy(attitude_calibration, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_attitude_calibration_LEN);
#endif
}
