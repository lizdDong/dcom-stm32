/**
  ******************************************************************************
  * @file    dfifo.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-12-3
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DFIFO_H_
#define _DFIFO_H_

#include <stdint.h>

typedef void (*fifo_cb)(void *arg);

typedef struct
{
    uint8_t *pBuff;
    volatile uint32_t waitReadCount;
    uint32_t buffSize;
    uint8_t *pWirte;
    uint8_t *pRead;
    fifo_cb lock;
    fifo_cb unlock;
} fifo_t;

void fifo_init(fifo_t *fifo, uint8_t *pbuff, uint32_t size, fifo_cb lock, fifo_cb unlock);
uint32_t fifo_write(fifo_t *fifo, uint8_t *pbuff, uint32_t num, uint32_t timeout);
uint32_t fifo_read(fifo_t *fifo, uint8_t *pbuff, uint32_t num, uint32_t timeout);
uint32_t fifo_peep(fifo_t *fifo, uint8_t *pbuff, uint32_t num);
uint32_t fifo_waitReadCount(fifo_t *fifo);

#endif

/*********************************** End of file *****************************************/
