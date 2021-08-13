/**
  ******************************************************************************
  * @file    dpwm.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-4-13
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DPWM_H_
#define _DPWM_H_

#include <stdint.h>

void pwm_TIM3_init(uint16_t prescaler, uint16_t period);
void pwm_TIM3_setDutyCycle(uint8_t channel, uint16_t duty_cycle);

#endif

