/**
  ******************************************************************************
  * @file    diap.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-6-16
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DIAP_H_
#define _DIAP_H_

#include <stdint.h>


typedef struct
{
    char fileName[32];
    uint32_t fileSize;
    uint32_t completedSize;
    uint8_t progress;
    uint8_t fStart;
    uint32_t writeAddr;
} iap_t;

extern iap_t g_sIap;

void iap_start(uint8_t *pkg);
int8_t iap_writeFlash(uint8_t *pkg, uint16_t pkgSize);
void iap_end(void);


#endif
