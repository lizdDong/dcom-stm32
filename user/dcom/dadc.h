/**
  ******************************************************************************
  * @file    dev_adc.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-4-12
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DEV_ADC_H_
#define _DEV_ADC_H_

#include <stdint.h>


#define ADC_VREF       1.2000f     //内部基准电压值

#define DEV_ADC1       1
#define DEV_ADC3       3

#define ADC_CHANNEL_NUM     2
#define ADC_SAMPLING_NUM    240

#define ADC3_CHANNEL_NUM    2
#define ADC3_SAMPLING_NUM   400

#define ADC_DUALMODE_CHANNEL_NUM    6
#define ADC_DUALMODE_SAMPLING_NUM   240


// 卡尔曼滤波器参数结构体
typedef struct
{
    float lastP;        // 上次估算协方差
    float nowP;         // 当前估算协方差
    float kg;           // 卡尔曼增益
    float Q;            // 过程噪声协方差
    float R;            // 观测噪声协方差
    float out;          // 卡尔曼滤波器输出
} kfp_t; // Kalman Filter parameter


extern volatile uint16_t g_aAdc1Value[ADC_SAMPLING_NUM][ADC_CHANNEL_NUM];
extern volatile uint16_t g_aAdc3Value[ADC3_SAMPLING_NUM][ADC3_CHANNEL_NUM];
extern volatile uint32_t g_aAdcDualConvValue[ADC_DUALMODE_SAMPLING_NUM][ADC_DUALMODE_CHANNEL_NUM];

extern uint8_t g_fAdcConvOver;

void adc1_init(void);
void adc3_init(void);
void adc12_init_dualMode(void);

uint16_t filter_maxAvg(uint16_t *array, uint16_t num, uint16_t avg_num);

uint16_t adc_getValue(uint8_t adcx, uint8_t n);
uint16_t adc3_getMaxValue(void);

uint16_t adc_getVrefValue(void);
float adc_getCpuTemp(void);

void kalman_paramInit(kfp_t *kfp, float lastP, float nowP, float kg, float Q, float R);
float kalman_filter(kfp_t *kfp, float input);


#endif


