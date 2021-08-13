/**
  ******************************************************************************
  * @file    dled.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-3-10
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DLED_H_
#define _DLED_H_

#include <stdint.h>


#define LED_ALWAYS    0xFFFFFFFF

#define LED_ON        0
#define LED_OFF       1

#define LED_ALL       0xFF

#define LED_COUNTS      2
#define LED_BUZZER      0   // ·äÃùÆ÷
#define LED_SYNC        1   // Í¬²½ÐÅºÅ

int led_init(void);
void led_on(uint8_t n);
void led_off(uint8_t n);
void led_blink(uint8_t num, uint32_t on_ms, uint32_t off_ms, uint32_t times);
void led_blinkHandle(void);

#endif


/*********************************** End of file *****************************************/
