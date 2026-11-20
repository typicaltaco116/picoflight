// Primary Header
#include <picoflight/estimation/madgwick.h>

// Local Headers
#include <picoflight/orientation/types.h>
#include <picoflight/sensors/imu.h>
#include <picoflight/core/utilities.h>

// Standard Headers
#include <math.h>
#include <stdbool.h>


static float _q0 = 1.0f;
static float _q1 = 0.0f;
static float _q2 = 0.0f;
static float _q3 = 0.0f;
static float _gradientDescentStepSize = 0.04f;

static float invSqrt(float x)
{
    return 1.0f / sqrtf(x);
}

void madgwickSetStepSize(float B_val)
{
    _gradientDescentStepSize = B_val;
}

platformState_t madgwickCompute(imuVectors_t imu, float dt)
{
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    float gx = imu.gyro.x;
    float gy = imu.gyro.y;
    float gz = imu.gyro.z;
    float ax = imu.accel.x;
    float ay = imu.accel.y;
    float az = imu.accel.z;

    gx = DEG_TO_RAD(gx);
    gy = DEG_TO_RAD(gy);
    gz = DEG_TO_RAD(gz);


    //Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-_q1 * gx - _q2 * gy - _q3 * gz);
    qDot2 = 0.5f * (_q0 * gx + _q2 * gz - _q3 * gy);
    qDot3 = 0.5f * (_q0 * gy - _q1 * gz + _q3 * gx);
    qDot4 = 0.5f * (_q0 * gz + _q1 * gy - _q2 * gx);

    //Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        //Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        //Auxiliary variables to avoid repeated arithmetic
        _2q0 = 2.0f * _q0;
        _2q1 = 2.0f * _q1;
        _2q2 = 2.0f * _q2;
        _2q3 = 2.0f * _q3;
        _4q0 = 4.0f * _q0;
        _4q1 = 4.0f * _q1;
        _4q2 = 4.0f * _q2;
        _8q1 = 8.0f * _q1;
        _8q2 = 8.0f * _q2;
        q0q0 = _q0 * _q0;
        q1q1 = _q1 * _q1;
        q2q2 = _q2 * _q2;
        q3q3 = _q3 * _q3;

        //Gradient decent algorithm corrective step
        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * _q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0f * q0q0 * _q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0f * q1q1 * _q3 - _2q1 * ax + 4.0f * q2q2 * _q3 - _2q2 * ay;

        recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); //normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        //Apply feedback step
        qDot1 -= _gradientDescentStepSize * s0;
        qDot2 -= _gradientDescentStepSize * s1;
        qDot3 -= _gradientDescentStepSize * s2;
        qDot4 -= _gradientDescentStepSize * s3;
    }

    //Integrate rate of change of quaternion to yield quaternion
    _q0 += qDot1 * dt;
    _q1 += qDot2 * dt;
    _q2 += qDot3 * dt;
    _q3 += qDot4 * dt;

    //Normalize quaternion
    recipNorm = invSqrt(_q0 * _q0 + _q1 * _q1 + _q2 * _q2 + _q3 * _q3);
    _q0 *= recipNorm;
    _q1 *= recipNorm;
    _q2 *= recipNorm;
    _q3 *= recipNorm;

    platformState_t state;
    state.omega.roll = imu.gyro.x; // I have no idea what axis this should be.
    state.omega.pitch = imu.gyro.y;
    state.omega.yaw = imu.gyro.z;

    //Compute angles
    state.theta.roll = atan2(_q0*_q1 + _q2*_q3, 0.5f - _q1*_q1 - _q2*_q2);
    state.theta.pitch = -asin(constrainf(-2.0f * (_q1*_q3 - _q0*_q2),-0.999999,0.999999));
    state.theta.yaw = -atan2(_q1*_q2 + _q0*_q3, 0.5f - _q2*_q2 - _q3*_q3);

    return state;
}
