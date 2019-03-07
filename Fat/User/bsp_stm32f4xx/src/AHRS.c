#include "AHRS.h"
#include "PID_Attitude.h"
#include "Throttle.h"
#include "bsp_rc.h"
#include "bsp_mpu6050.h"
#include "bsp_hmc5883l.h"

/* NOTE : How to tune the gain? First of all, stick with this default gain. And let the quad in stable place.
   Log the steady state reponse of filter. If it is too slow, increase KpDef.
   If you are flying from ground to high altitude in short amount of time, please increase KiDef which
   will compensate gyro bias which depends on temperature and vibration of your vehicle */
					
#define KpDef (0.5f) 		 //! This parameter will give you about 15 seconds convergence time.
												 //! You can set this gain higher if you want more fast response.
												 //! But note that higher gain will give you also higher overshoot.
#define KiDef (0.025f) 	 //! This gain will incorporate slow time-varying bias (e.g., temperature change)
												 //! This gain is depend on your vehicle status.

volatile float AHRS_Kp = KpDef;    // proportional gain (Kp)  
volatile float AHRS_Ki = KiDef;    // integral gain (Ki)  
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;   // quaternion of sensor frame relative to auxiliary frame  
volatile float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f; // integral error terms scaled by Ki

static float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

static float Rot_matrix[9];				// 方向余弦矩阵

Status status;							// 飞机状态，加速度、速度、位置、姿态角

//! Using accelerometer, sense the gravity vector.
//! Using magnetometer, sense yaw.
// 使用加速度传感器测量重力向量，使用磁力计测量方向
void AHRSinit(float ax, float ay, float az, float mx, float my, float mz)
{
	float initialRoll, initialPitch;
	float cosRoll, sinRoll, cosPitch, sinPitch;
	float magX, magY;
	float initialHdg, cosHeading, sinHeading;

	initialRoll = fast_atan2(-ax, az);
	initialPitch = fast_atan2(ay, az);

	cosRoll = __cos(initialRoll);
	sinRoll = __sin(initialRoll);
	cosPitch = __cos(initialPitch);
	sinPitch = __sin(initialPitch);

	// X-Y
	// magX = mx * cosRoll + mz * sinRoll;
	// magY = mx * sinPitch * sinRoll + my * cosPitch - mz * sinPitch * cosRoll;

	// Y-X
	magX = mx * cosRoll + my * sinPitch * sinRoll + mz * cosPitch * sinRoll;
	magY = my * cosPitch - mz * sinPitch;

	initialHdg = fast_atan2(magX, magY);

	cosRoll = __cos(initialRoll * 0.5f);
	sinRoll = __sin(initialRoll * 0.5f);

	cosPitch = __cos(initialPitch * 0.5f);
	sinPitch = __sin(initialPitch * 0.5f);

	cosHeading = __cos(initialHdg * 0.5f);
	sinHeading = __sin(initialHdg * 0.5f);

	q0 = cosPitch * cosRoll * cosHeading + sinPitch * sinRoll * sinHeading;
	q1 = sinPitch * cosRoll * cosHeading - cosPitch * sinRoll * sinHeading;
	q2 = cosPitch * sinRoll * cosHeading + sinPitch * cosRoll * sinHeading;
	q3 = cosPitch * cosRoll * sinHeading - sinPitch * sinRoll * cosHeading;

	// auxillary variables to reduce number of repeated operations, for 1st pass
	q0q0 = q0 * q0;
	q0q1 = q0 * q1;
	q0q2 = q0 * q2;
	q0q3 = q0 * q3;
	q1q1 = q1 * q1;
	q1q2 = q1 * q2;
	q1q3 = q1 * q3;
	q2q2 = q2 * q2;
	q2q3 = q2 * q3;
	q3q3 = q3 * q3;
		
	// Convert q->R, This R converts inertial frame to body frame.
	Rot_matrix[0] = q0q0 + q1q1 - q2q2 - q3q3;// 11
	Rot_matrix[1] = 2.f * (q1q2 + q0q3);			// 12
	Rot_matrix[2] = 2.f * (q1q3 - q0q2);			// 13
	Rot_matrix[3] = 2.f * (q1q2 - q0q3);			// 21
	Rot_matrix[4] = q0q0 - q1q1 + q2q2 - q3q3;// 22
	Rot_matrix[5] = 2.f * (q2q3 + q0q1);			// 23
	Rot_matrix[6] = 2.f * (q1q3 + q0q2);			// 31
	Rot_matrix[7] = 2.f * (q2q3 - q0q1);			// 32
	Rot_matrix[8] = q0q0 - q1q1 - q2q2 + q3q3;// 33
}

void AHRS(float dt)
{
	float recipNorm;
	float ex = 0.0f, ey = 0.0f, ez = 0.0f;
	
	float ax = mpu6050.acc.x;
	float ay = mpu6050.acc.y;
	float az = mpu6050.acc.z;
	float gx = mpu6050.gyro.x;
	float gy = mpu6050.gyro.y;
	float gz = mpu6050.gyro.z;
	float mx = hmc5883l.mag.x;
	float my = hmc5883l.mag.y;
	float mz = hmc5883l.mag.z;
	
	static bool bFilterInit = false;
	// Make filter converge to initial solution faster
	// This function assumes you are in static position.
	// WARNING : in case air reboot, this can cause problem. But this is very unlikely happen.
	if(bFilterInit == false || hmc5883l.calibrating || mpu6050.calibrating) {
		AHRSinit(ax, ay, az, mx, my, mz);
		bFilterInit = true;
		return;
	}

	// 大地速度估计
	status.acc.x = (ax * Rot_matrix[0] + ay * Rot_matrix[3] + az * Rot_matrix[6]) * G;
	status.acc.y = (ax * Rot_matrix[1] + ay * Rot_matrix[4] + az * Rot_matrix[7]) * G;	
	status.acc.z = (ax * Rot_matrix[2] + ay * Rot_matrix[5] + az * Rot_matrix[8] - 1) * G;

	if(rc.channel[2].enable)
	{
		status.acc.x -= status.acc_offset.x * 0.005f;
		status.acc.y -= status.acc_offset.y * 0.005f;
		status.acc.z -= status.acc_offset.z * 0.005f;
	}
	
	status.position.x += status.speed.x*dt + status.acc.x * dt * dt * 0.5f;
	status.position.y += status.speed.y*dt + status.acc.y * dt * dt * 0.5f;
	if(rc.channel[0].radio_out)	
	{
		status.position.x = 0;
		status.speed.x = 0;
	}
	if(rc.channel[1].radio_out)	
	{
		status.position.y = 0;
		status.speed.y = 0;
	}
	
	status.speed.x += status.acc.x * dt;
	status.speed.y += status.acc.y * dt;
	status.speed.z += status.acc.z * dt;

	// Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
	// 如果地磁传感器各轴的数均是0，那么忽略该地磁数据。否则在地磁数据归一化处理的时候，会导致除以0的错误。
	if(mx * my * mz != 0.0f) {
		// Normalise magnetometer measurement
		// 把地磁的数据进行归一化处理。
		recipNorm = sqrtf3(mx, my, mz);
		mx /= recipNorm;
		my /= recipNorm;
		mz /= recipNorm;
		
		// Reference direction of Earth’s magnetic field
		// 地球磁场的参考方向
		float hx = mx * Rot_matrix[0] + my * Rot_matrix[3] + mz * Rot_matrix[6];
		float hy = mx * Rot_matrix[1] + my * Rot_matrix[4] + mz * Rot_matrix[7];
		float by = sqrtf2(hx, hy);
		float bz = mx * Rot_matrix[2] + my * Rot_matrix[5] + mz * Rot_matrix[8];

		// Estimated direction of magnetic field
		// 估计地球磁场方向
		float wx = by * Rot_matrix[1] + bz * Rot_matrix[2];
		float wy = by * Rot_matrix[4] + bz * Rot_matrix[5];
		float wz = by * Rot_matrix[7] + bz * Rot_matrix[8];

		// Error is sum of cross product between estimated direction and measured direction of field vectors
		// 误差是估计方向和场矢量测量方向的矢量积的总和
		ex += my * wz - mz * wy;
		ey += mz * wx - mx * wz;
		ez += mx * wy - my * wx;
	}

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	// 如果加速计各轴的数均是0，那么忽略该加速度数据。否则在加速计数据归一化处理的时候，会导致除以0的错误。
	if(ax * ay * az != 0.0f) {
		
		// Normalise accelerometer measurement
		// 把加速度计的数据进行归一化处理。
		recipNorm = sqrtf3(ax, ay, az);
		ax /= recipNorm;
		ay /= recipNorm;
		az /= recipNorm;
		
		// Estimated direction of gravity field
		// 根据当前四元数的姿态值来估算出各重力分量Vx，Vy，Vz。
		float vx = Rot_matrix[2];
		float vy = Rot_matrix[5];
		float vz = Rot_matrix[8];

		// Error is sum of cross product between estimated direction and measured direction of field vectors
		// 使用叉积来计算重力误差。
		ex += ay * vz - az * vy;
		ey += az * vx - ax * vz;
		ez += ax * vy - ay * vx;
	}

	// Apply feedback only when valid data has been gathered from the accelerometer or magnetometer
	// 只有当从加速度计或磁计中收集有效数据时才应用反馈
	if(ex != 0.0f && ey != 0.0f && ez != 0.0f) {
		// Compute and apply integral feedback if enabled
		// 把上述计算得到的重力和磁力差进行积分运算，
		if(AHRS_Ki > 0.0f) {
			integralFBx += AHRS_Ki * ex * dt; // integral error scaled by Ki
			integralFBy += AHRS_Ki * ey * dt;
			integralFBz += AHRS_Ki * ez * dt;
			gx += integralFBx; // apply integral feedback
			gy += integralFBy;
			gz += integralFBz;
		} else {
			integralFBx = 0.0f; // prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		// 把上述计算得到的重力差和磁力差进行比例运算。
		gx += AHRS_Kp * ex;
		gy += AHRS_Kp * ey;
		gz += AHRS_Kp * ez;
	}
		
	// Time derivative of quaternion. q_dot = 0.5*q\otimes omega.
	//! q_k = q_{k-1} + dt*\dot{q}
	//! \dot{q} = 0.5*q \otimes P(\omega)
	float dq0 = 0.5f*(-q1 * gx - q2 * gy - q3 * gz);
	float dq1 = 0.5f*( q0 * gx + q2 * gz - q3 * gy);
	float dq2 = 0.5f*( q0 * gy - q1 * gz + q3 * gx);
	float dq3 = 0.5f*( q0 * gz + q1 * gy - q2 * gx); 

	q0 += dt * dq0;
	q1 += dt * dq1;
	q2 += dt * dq2;
	q3 += dt * dq3;
	
	// Normalise quaternion
	recipNorm = sqrtf4(q0, q1, q2, q3);
	q0 /= recipNorm;
	q1 /= recipNorm;
	q2 /= recipNorm;
	q3 /= recipNorm;
	
	// Auxiliary variables to avoid repeated arithmetic
	// 预先进行四元数数据运算，以避免重复运算带来的效率问题。
	q0q0 = q0 * q0;
	q0q1 = q0 * q1;
	q0q2 = q0 * q2;
	q0q3 = q0 * q3;
	q1q1 = q1 * q1;
	q1q2 = q1 * q2;
	q1q3 = q1 * q3;
	q2q2 = q2 * q2;
	q2q3 = q2 * q3;
	q3q3 = q3 * q3;
	
	// Convert q->R, This R converts inertial frame to body frame.
	Rot_matrix[0] = q0q0 + q1q1 - q2q2 - q3q3;// 11
	Rot_matrix[1] = 2.f * (q1q2 + q0q3);	// 12
	Rot_matrix[2] = 2.f * (q1q3 - q0q2);	// 13
	Rot_matrix[3] = 2.f * (q1q2 - q0q3);	// 21
	Rot_matrix[4] = q0q0 - q1q1 + q2q2 - q3q3;// 22
	Rot_matrix[5] = 2.f * (q2q3 + q0q1);	// 23
	Rot_matrix[6] = 2.f * (q1q3 + q0q2);	// 31
	Rot_matrix[7] = 2.f * (q2q3 - q0q1);	// 32
	Rot_matrix[8] = q0q0 - q1q1 - q2q2 + q3q3;// 33

//	Roll = fast_atan2(-Rot_matrix[2], Rot_matrix[8]);	//! Roll
//	Pitch = asin(Rot_matrix[5]);	//! Pitch
//	Yaw = fast_atan2(-Rot_matrix[3], Rot_matrix[4]);		//! Yaw
	
//	Roll = atan2f(Rot_matrix[5], Rot_matrix[8]);	//! Roll
//	Pitch = -asinf(Rot_matrix[2]);	//! Pitch
//	Yaw = atan2f(Rot_matrix[1], Rot_matrix[0]);		//! Yaw

	//四元数转换为欧拉角输出
	status.attitude.pitch = asin(Rot_matrix[5]);
	status.attitude.roll  = fast_atan2(-Rot_matrix[2], Rot_matrix[8]);
	status.attitude.yaw   = fast_atan2(-Rot_matrix[3], Rot_matrix[4]);
}
