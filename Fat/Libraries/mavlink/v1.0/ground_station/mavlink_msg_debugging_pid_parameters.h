// MESSAGE debugging_PID_parameters PACKING

#define MAVLINK_MSG_ID_debugging_PID_parameters 188

typedef struct __mavlink_debugging_pid_parameters_t
{
 uint64_t time; /*< milliseconds since system boot*/
 uint32_t number; /*<  1:stable_loop.roll
												2:stable_loop.pitch
												3:stable_loop.yaw
												4:rate_loop.roll
												5:rate_loop.pitch
												6:rate_loop.yaw
												7:location.pid.x  
												8:location.pid.y
												9:location.pid.z
												10:speed.pid.x
												11:speed.pid.y
												12:speed.pid.z
												 */
 float expect; /*< expect*/
 float real; /*< real*/
 float err; /*< err*/
 float err_filter; /*< err_filter*/
 float err_P; /*< P out*/
 float err_I; /*< I out*/
 float err_D; /*< D out*/
 float reserved; /*< reserved*/
} mavlink_debugging_pid_parameters_t;

#define MAVLINK_MSG_ID_debugging_PID_parameters_LEN 44
#define MAVLINK_MSG_ID_188_LEN 44

#define MAVLINK_MSG_ID_debugging_PID_parameters_CRC 143
#define MAVLINK_MSG_ID_188_CRC 143



#define MAVLINK_MESSAGE_INFO_debugging_PID_parameters { \
	"debugging_PID_parameters", \
	10, \
	{  { "time", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_debugging_pid_parameters_t, time) }, \
         { "number", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_debugging_pid_parameters_t, number) }, \
         { "expect", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_debugging_pid_parameters_t, expect) }, \
         { "real", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_debugging_pid_parameters_t, real) }, \
         { "err", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_debugging_pid_parameters_t, err) }, \
         { "err_filter", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_debugging_pid_parameters_t, err_filter) }, \
         { "err_P", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_debugging_pid_parameters_t, err_P) }, \
         { "err_I", NULL, MAVLINK_TYPE_FLOAT, 0, 32, offsetof(mavlink_debugging_pid_parameters_t, err_I) }, \
         { "err_D", NULL, MAVLINK_TYPE_FLOAT, 0, 36, offsetof(mavlink_debugging_pid_parameters_t, err_D) }, \
         { "reserved", NULL, MAVLINK_TYPE_FLOAT, 0, 40, offsetof(mavlink_debugging_pid_parameters_t, reserved) }, \
         } \
}


/**
 * @brief Pack a debugging_pid_parameters message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time milliseconds since system boot
 * @param number  1:stable_loop.roll
												2:stable_loop.pitch
												3:stable_loop.yaw
												4:rate_loop.roll
												5:rate_loop.pitch
												6:rate_loop.yaw
												7:location.pid.x  
												8:location.pid.y
												9:location.pid.z
												10:speed.pid.x
												11:speed.pid.y
												12:speed.pid.z
												 
 * @param expect expect
 * @param real real
 * @param err err
 * @param err_filter err_filter
 * @param err_P P out
 * @param err_I I out
 * @param err_D D out
 * @param reserved reserved
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_debugging_pid_parameters_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint64_t time, uint32_t number, float expect, float real, float err, float err_filter, float err_P, float err_I, float err_D, float reserved)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_debugging_PID_parameters_LEN];
	_mav_put_uint64_t(buf, 0, time);
	_mav_put_uint32_t(buf, 8, number);
	_mav_put_float(buf, 12, expect);
	_mav_put_float(buf, 16, real);
	_mav_put_float(buf, 20, err);
	_mav_put_float(buf, 24, err_filter);
	_mav_put_float(buf, 28, err_P);
	_mav_put_float(buf, 32, err_I);
	_mav_put_float(buf, 36, err_D);
	_mav_put_float(buf, 40, reserved);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_debugging_PID_parameters_LEN);
#else
	mavlink_debugging_pid_parameters_t packet;
	packet.time = time;
	packet.number = number;
	packet.expect = expect;
	packet.real = real;
	packet.err = err;
	packet.err_filter = err_filter;
	packet.err_P = err_P;
	packet.err_I = err_I;
	packet.err_D = err_D;
	packet.reserved = reserved;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_debugging_PID_parameters_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_debugging_PID_parameters;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_debugging_PID_parameters_LEN, MAVLINK_MSG_ID_debugging_PID_parameters_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_debugging_PID_parameters_LEN);
#endif
}

/**
 * @brief Pack a debugging_pid_parameters message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time milliseconds since system boot
 * @param number  1:stable_loop.roll
												2:stable_loop.pitch
												3:stable_loop.yaw
												4:rate_loop.roll
												5:rate_loop.pitch
												6:rate_loop.yaw
												7:location.pid.x  
												8:location.pid.y
												9:location.pid.z
												10:speed.pid.x
												11:speed.pid.y
												12:speed.pid.z
												 
 * @param expect expect
 * @param real real
 * @param err err
 * @param err_filter err_filter
 * @param err_P P out
 * @param err_I I out
 * @param err_D D out
 * @param reserved reserved
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_debugging_pid_parameters_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint64_t time,uint32_t number,float expect,float real,float err,float err_filter,float err_P,float err_I,float err_D,float reserved)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_debugging_PID_parameters_LEN];
	_mav_put_uint64_t(buf, 0, time);
	_mav_put_uint32_t(buf, 8, number);
	_mav_put_float(buf, 12, expect);
	_mav_put_float(buf, 16, real);
	_mav_put_float(buf, 20, err);
	_mav_put_float(buf, 24, err_filter);
	_mav_put_float(buf, 28, err_P);
	_mav_put_float(buf, 32, err_I);
	_mav_put_float(buf, 36, err_D);
	_mav_put_float(buf, 40, reserved);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_debugging_PID_parameters_LEN);
#else
	mavlink_debugging_pid_parameters_t packet;
	packet.time = time;
	packet.number = number;
	packet.expect = expect;
	packet.real = real;
	packet.err = err;
	packet.err_filter = err_filter;
	packet.err_P = err_P;
	packet.err_I = err_I;
	packet.err_D = err_D;
	packet.reserved = reserved;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_debugging_PID_parameters_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_debugging_PID_parameters;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_debugging_PID_parameters_LEN, MAVLINK_MSG_ID_debugging_PID_parameters_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_debugging_PID_parameters_LEN);
#endif
}

/**
 * @brief Encode a debugging_pid_parameters struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param debugging_pid_parameters C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_debugging_pid_parameters_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_debugging_pid_parameters_t* debugging_pid_parameters)
{
	return mavlink_msg_debugging_pid_parameters_pack(system_id, component_id, msg, debugging_pid_parameters->time, debugging_pid_parameters->number, debugging_pid_parameters->expect, debugging_pid_parameters->real, debugging_pid_parameters->err, debugging_pid_parameters->err_filter, debugging_pid_parameters->err_P, debugging_pid_parameters->err_I, debugging_pid_parameters->err_D, debugging_pid_parameters->reserved);
}

/**
 * @brief Encode a debugging_pid_parameters struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param debugging_pid_parameters C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_debugging_pid_parameters_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_debugging_pid_parameters_t* debugging_pid_parameters)
{
	return mavlink_msg_debugging_pid_parameters_pack_chan(system_id, component_id, chan, msg, debugging_pid_parameters->time, debugging_pid_parameters->number, debugging_pid_parameters->expect, debugging_pid_parameters->real, debugging_pid_parameters->err, debugging_pid_parameters->err_filter, debugging_pid_parameters->err_P, debugging_pid_parameters->err_I, debugging_pid_parameters->err_D, debugging_pid_parameters->reserved);
}

/**
 * @brief Send a debugging_pid_parameters message
 * @param chan MAVLink channel to send the message
 *
 * @param time milliseconds since system boot
 * @param number  1:stable_loop.roll
												2:stable_loop.pitch
												3:stable_loop.yaw
												4:rate_loop.roll
												5:rate_loop.pitch
												6:rate_loop.yaw
												7:location.pid.x  
												8:location.pid.y
												9:location.pid.z
												10:speed.pid.x
												11:speed.pid.y
												12:speed.pid.z
												 
 * @param expect expect
 * @param real real
 * @param err err
 * @param err_filter err_filter
 * @param err_P P out
 * @param err_I I out
 * @param err_D D out
 * @param reserved reserved
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_debugging_pid_parameters_send(mavlink_channel_t chan, uint64_t time, uint32_t number, float expect, float real, float err, float err_filter, float err_P, float err_I, float err_D, float reserved)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_debugging_PID_parameters_LEN];
	_mav_put_uint64_t(buf, 0, time);
	_mav_put_uint32_t(buf, 8, number);
	_mav_put_float(buf, 12, expect);
	_mav_put_float(buf, 16, real);
	_mav_put_float(buf, 20, err);
	_mav_put_float(buf, 24, err_filter);
	_mav_put_float(buf, 28, err_P);
	_mav_put_float(buf, 32, err_I);
	_mav_put_float(buf, 36, err_D);
	_mav_put_float(buf, 40, reserved);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_debugging_PID_parameters, buf, MAVLINK_MSG_ID_debugging_PID_parameters_LEN, MAVLINK_MSG_ID_debugging_PID_parameters_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_debugging_PID_parameters, buf, MAVLINK_MSG_ID_debugging_PID_parameters_LEN);
#endif
#else
	mavlink_debugging_pid_parameters_t packet;
	packet.time = time;
	packet.number = number;
	packet.expect = expect;
	packet.real = real;
	packet.err = err;
	packet.err_filter = err_filter;
	packet.err_P = err_P;
	packet.err_I = err_I;
	packet.err_D = err_D;
	packet.reserved = reserved;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_debugging_PID_parameters, (const char *)&packet, MAVLINK_MSG_ID_debugging_PID_parameters_LEN, MAVLINK_MSG_ID_debugging_PID_parameters_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_debugging_PID_parameters, (const char *)&packet, MAVLINK_MSG_ID_debugging_PID_parameters_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_debugging_PID_parameters_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_debugging_pid_parameters_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time, uint32_t number, float expect, float real, float err, float err_filter, float err_P, float err_I, float err_D, float reserved)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint64_t(buf, 0, time);
	_mav_put_uint32_t(buf, 8, number);
	_mav_put_float(buf, 12, expect);
	_mav_put_float(buf, 16, real);
	_mav_put_float(buf, 20, err);
	_mav_put_float(buf, 24, err_filter);
	_mav_put_float(buf, 28, err_P);
	_mav_put_float(buf, 32, err_I);
	_mav_put_float(buf, 36, err_D);
	_mav_put_float(buf, 40, reserved);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_debugging_PID_parameters, buf, MAVLINK_MSG_ID_debugging_PID_parameters_LEN, MAVLINK_MSG_ID_debugging_PID_parameters_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_debugging_PID_parameters, buf, MAVLINK_MSG_ID_debugging_PID_parameters_LEN);
#endif
#else
	mavlink_debugging_pid_parameters_t *packet = (mavlink_debugging_pid_parameters_t *)msgbuf;
	packet->time = time;
	packet->number = number;
	packet->expect = expect;
	packet->real = real;
	packet->err = err;
	packet->err_filter = err_filter;
	packet->err_P = err_P;
	packet->err_I = err_I;
	packet->err_D = err_D;
	packet->reserved = reserved;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_debugging_PID_parameters, (const char *)packet, MAVLINK_MSG_ID_debugging_PID_parameters_LEN, MAVLINK_MSG_ID_debugging_PID_parameters_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_debugging_PID_parameters, (const char *)packet, MAVLINK_MSG_ID_debugging_PID_parameters_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE debugging_PID_parameters UNPACKING


/**
 * @brief Get field time from debugging_pid_parameters message
 *
 * @return milliseconds since system boot
 */
static inline uint64_t mavlink_msg_debugging_pid_parameters_get_time(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field number from debugging_pid_parameters message
 *
 * @return  1:stable_loop.roll
												2:stable_loop.pitch
												3:stable_loop.yaw
												4:rate_loop.roll
												5:rate_loop.pitch
												6:rate_loop.yaw
												7:location.pid.x  
												8:location.pid.y
												9:location.pid.z
												10:speed.pid.x
												11:speed.pid.y
												12:speed.pid.z
												 
 */
static inline uint32_t mavlink_msg_debugging_pid_parameters_get_number(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  8);
}

/**
 * @brief Get field expect from debugging_pid_parameters message
 *
 * @return expect
 */
static inline float mavlink_msg_debugging_pid_parameters_get_expect(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field real from debugging_pid_parameters message
 *
 * @return real
 */
static inline float mavlink_msg_debugging_pid_parameters_get_real(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field err from debugging_pid_parameters message
 *
 * @return err
 */
static inline float mavlink_msg_debugging_pid_parameters_get_err(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  20);
}

/**
 * @brief Get field err_filter from debugging_pid_parameters message
 *
 * @return err_filter
 */
static inline float mavlink_msg_debugging_pid_parameters_get_err_filter(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  24);
}

/**
 * @brief Get field err_P from debugging_pid_parameters message
 *
 * @return P out
 */
static inline float mavlink_msg_debugging_pid_parameters_get_err_P(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  28);
}

/**
 * @brief Get field err_I from debugging_pid_parameters message
 *
 * @return I out
 */
static inline float mavlink_msg_debugging_pid_parameters_get_err_I(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  32);
}

/**
 * @brief Get field err_D from debugging_pid_parameters message
 *
 * @return D out
 */
static inline float mavlink_msg_debugging_pid_parameters_get_err_D(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  36);
}

/**
 * @brief Get field reserved from debugging_pid_parameters message
 *
 * @return reserved
 */
static inline float mavlink_msg_debugging_pid_parameters_get_reserved(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  40);
}

/**
 * @brief Decode a debugging_pid_parameters message into a struct
 *
 * @param msg The message to decode
 * @param debugging_pid_parameters C-struct to decode the message contents into
 */
static inline void mavlink_msg_debugging_pid_parameters_decode(const mavlink_message_t* msg, mavlink_debugging_pid_parameters_t* debugging_pid_parameters)
{
#if MAVLINK_NEED_BYTE_SWAP
	debugging_pid_parameters->time = mavlink_msg_debugging_pid_parameters_get_time(msg);
	debugging_pid_parameters->number = mavlink_msg_debugging_pid_parameters_get_number(msg);
	debugging_pid_parameters->expect = mavlink_msg_debugging_pid_parameters_get_expect(msg);
	debugging_pid_parameters->real = mavlink_msg_debugging_pid_parameters_get_real(msg);
	debugging_pid_parameters->err = mavlink_msg_debugging_pid_parameters_get_err(msg);
	debugging_pid_parameters->err_filter = mavlink_msg_debugging_pid_parameters_get_err_filter(msg);
	debugging_pid_parameters->err_P = mavlink_msg_debugging_pid_parameters_get_err_P(msg);
	debugging_pid_parameters->err_I = mavlink_msg_debugging_pid_parameters_get_err_I(msg);
	debugging_pid_parameters->err_D = mavlink_msg_debugging_pid_parameters_get_err_D(msg);
	debugging_pid_parameters->reserved = mavlink_msg_debugging_pid_parameters_get_reserved(msg);
#else
	memcpy(debugging_pid_parameters, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_debugging_PID_parameters_LEN);
#endif
}
