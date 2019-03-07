/*
*********************************************************************************************************
*	                                  
*	模块名称 : 滤波
*	文件名称 : include.h
*	版    本 : V1.0
*	说    明 : 
* 日		期 ：2016-2-20   
*		
*	Copyright(c)2016 武汉飞航科技有限公司 
*	技术讨论QQ群：363450228 淘宝官网：https://shop149667832.taobao.com
* 以下所有内容版权均属武汉飞航科技所有，未经允许不得用于商业用途。
* 欢迎各位使用并推广本程序，修改内容时必须保留飞航科技的版权声明。
*********************************************************************************************************
*/
#include "filter.h"
#include "math_extend.h"

#define BIQUAD_BANDWIDTH 1.9f     /* bandwidth in octaves */
#define BIQUAD_Q 1.0f / sqrtf(2.0f)     /* quality factor - butterworth*/

//一阶低通滤波器

// f_cut = cutoff frequency
void pt1FilterInit(pt1Filter_t *filter, uint8_t f_cut, float dT)
{
    filter->RC = 1.0f / ( 2.0f * PI * f_cut );
    filter->dT = dT;
}

float pt1FilterApply(pt1Filter_t *filter, float input)
{
    filter->state = filter->state + filter->dT / (filter->RC + filter->dT) * (input - filter->state);
    return filter->state;
}

float pt1FilterApply4(pt1Filter_t *filter, float input, uint16_t f_cut, float dT)
{
    // Pre calculate and store RC
    if (!filter->RC) {
        filter->RC = 1.0f / ( 2.0f * PI * f_cut );
    }

    filter->dT = dT;    // cache latest dT for possible use in pt1FilterApply
    filter->state = filter->state + dT / (filter->RC + dT) * (input - filter->state);
    return filter->state;
}

void pt1FilterReset(pt1Filter_t *filter, float input)
{
    filter->state = input;
}

// 二阶滤波器

float filterGetNotchQ(uint16_t centerFreq, uint16_t cutoff)
{
    const float octaves = log2f((float)centerFreq  / (float)cutoff) * 2;
    return sqrtf(powf(2, octaves)) / (powf(2, octaves) - 1);
}

void biquadFilterInitNotch(biquadFilter_t *filter, float dt, uint16_t filterFreq, uint16_t cutoffHz)
{
    float Q = filterGetNotchQ(filterFreq, cutoffHz);
    biquadFilterInit(filter, filterFreq, dt, Q, FILTER_NOTCH);
}

// sets up a biquad Filter
void biquadFilterInitLPF(biquadFilter_t *filter, uint16_t filterFreq, float dt)
{
    biquadFilterInit(filter, filterFreq, dt, BIQUAD_Q, FILTER_LPF);
}

void biquadFilterInit(biquadFilter_t *filter, uint16_t filterFreq, float dt, float Q, biquadFilterType_e filterType)
{
    // Check for Nyquist frequency and if it's not possible to initialize filter as requested - set to no filtering at all
    if (filterFreq < (1 / dt / 2)) {
        // setup variables
        const float sampleRate = 1.0f / dt;
        const float omega = 2.0f * PI * ((float)filterFreq) / sampleRate;
        const float sn = __sin(omega);
        const float cs = __cos(omega);
        const float alpha = sn / (2 * Q);

        float b0, b1, b2;
        switch (filterType) {
        case FILTER_LPF:
            b0 = (1 - cs) / 2;
            b1 = 1 - cs;
            b2 = (1 - cs) / 2;
            break;
        case FILTER_NOTCH:
            b0 =  1;
            b1 = -2 * cs;
            b2 =  1;
            break;
        }
        const float a0 =  1 + alpha;
        const float a1 = -2 * cs;
        const float a2 =  1 - alpha;

        // precompute the coefficients
        filter->b0 = b0 / a0;
        filter->b1 = b1 / a0;
        filter->b2 = b2 / a0;
        filter->a1 = a1 / a0;
        filter->a2 = a2 / a0;
    }
    else {
        // Not possible to filter frequencies above Nyquist frequency - passthrough
        filter->b0 = 1.0f;
        filter->b1 = 0.0f;
        filter->b2 = 0.0f;
        filter->a1 = 0.0f;
        filter->a2 = 0.0f;
    }

    // zero initial samples
    filter->d1 = filter->d2 = 0;
}

// Computes a biquad_t filter on a sample
float biquadFilterApply(biquadFilter_t *filter, float input)
{
    const float result = filter->b0 * input + filter->d1;
    filter->d1 = filter->b1 * input - filter->a1 * result + filter->d2;
    filter->d2 = filter->b2 * input - filter->a2 * result;
    return result;
}


/*
 * FIR filter
 */
void firFilterInit2(firFilter_t *filter, float *buf, uint8_t bufLength, const float *coeffs, uint8_t coeffsLength)
{
    filter->buf = buf;
    filter->bufLength = bufLength;
    filter->coeffs = coeffs;
    filter->coeffsLength = coeffsLength;
    memset(filter->buf, 0, sizeof(float) * filter->bufLength);
}

/*
 * FIR filter initialisation
 * If FIR filter is just used for averaging, coeffs can be set to NULL
 */
void firFilterInit(firFilter_t *filter, float *buf, uint8_t bufLength, const float *coeffs)
{
    firFilterInit2(filter, buf, bufLength, coeffs, bufLength);
}

void firFilterUpdate(firFilter_t *filter, float input)
{
    memmove(&filter->buf[1], &filter->buf[0], (filter->bufLength-1) * sizeof(float));
    filter->buf[0] = input;
}

float firFilterApply(const firFilter_t *filter)
{
    float ret = 0.0f;
    for (int ii = 0; ii < filter->coeffsLength; ++ii) {
        ret += filter->coeffs[ii] * filter->buf[ii];
    }
    return ret;
}

// PID
float PID_filter(PID_t* pid, float expect, float in, float dt)
{
	/* 期望赋值 */
	pid->expect = expect;
	
	/* 计算误差 */
	pid->err = pid->expect - in;
	
	/* 计算比例项 */
	pid->err_p =  pid->kp * pid->err;
	
	/* 计算微分项 */
	pid->err_d = pid->kd * (pid->err - pid->last_err) / dt;
	
	/* 记录历史数据 */
	pid->last_err = pid->err;
	
	/* 计算积分项 */
	pid->err_i += pid->ki * pid->err * dt;
	
	/* 误差积分限幅 */
	if(pid->limit != 0)
		pid->err_i = LIMIT(pid->err_i, -pid->limit, pid->limit);
	
	/* 计算输出量 */
	pid->out = pid->err_p + pid->err_i + pid->err_d;
}