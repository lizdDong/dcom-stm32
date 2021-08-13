/**
  ******************************************************************************
  * @file    drtc.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-5-5
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#ifndef _DRTC_H_
#define _DRTC_H_

#include "stm32f10x.h"

/* 选择RTC的时钟源 */
//#define RTC_CLOCK_SOURCE_LSE       /* LSE */
//#define RTC_CLOCK_SOURCE_LSI     /* LSI */ 


//时间结构体
typedef struct
{
    vu8 hour;
    vu8 min;
    vu8 sec;
    vu8 ms;
    //公历年月日周
    vu16 year;
    vu8  month;
    vu8  date;
    vu8  week;
} calendar_t;

extern calendar_t g_sCalendar;

void rtc_config(void);
void rtc_init(void);
int rtc_set(u16 year, u8 mon, u8 day, u8 hour, u8 min, u8 sec);
void rtc_get(void);

#endif



/*********************************** End of file *****************************************/
