/**
  ******************************************************************************
  * @file    diwdg.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-3-10
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DIWDG_H_
#define _DIWDG_H_

#include <stdint.h>

void iwdg_init(uint8_t prer, uint16_t rlr);
void iwdg_feed(void);


#endif


/*********************************** End of file *****************************************/
