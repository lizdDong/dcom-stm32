/**
  ******************************************************************************
  * @file    dcom.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-3-10
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DCOM_H_
#define _DCOM_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "stm32f10x.h"                  // Device header


#define USE_RTOS_RT_THREAD    0

#if (USE_RTOS_RT_THREAD)
#include <rtthread.h>
#endif


#define DOK        0
#define DEOR       (-1)


int str_to_int(char *str);
float str_to_float(char *str);
int rounding_off(float v);
void systick_init(void);
void systick_callback(void);
uint32_t enter_critical(void);
void exit_critical(uint32_t priMask);
uint32_t systick_getCount(void);


#endif


/*********************************** End of file *****************************************/
