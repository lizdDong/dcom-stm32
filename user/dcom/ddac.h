/**
  ******************************************************************************
  * @file    dev_dac.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-4-13
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DEV_DAC_H_
#define _DEV_DAC_H_

#include <stdint.h>

#define DAC_WAVE1_BUFF      200
#define DAC_WAVE2_BUFF      200


extern uint16_t g_aWave1[DAC_WAVE1_BUFF];
extern uint16_t g_aWave2[DAC_WAVE2_BUFF];

void dac_ch1_init_simple(void);
void dac_ch2_init_simple(void);
void dac_ch1_setVoltage(uint16_t vol);
void dac_ch2_setVoltage(uint16_t vol);

void dac_makeSinWave(uint16_t *buff, uint16_t size, int16_t offset_x, int16_t offset_y, uint16_t amplitude);
void dac_makeRectWave(uint16_t *buff, uint16_t size, int16_t offset_y, uint16_t amplitude, uint8_t duty);
void dac_ch1_init_waveOut(void);
void dac_ch2_init_waveOut(void);
void dac_ch1_waveOut(uint32_t freq);
void dac_ch2_waveOut(uint32_t freq);


#endif

