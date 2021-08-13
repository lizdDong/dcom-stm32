/**
  ******************************************************************************
  * @file    dcan.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-7-23
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DCAN_H_
#define _DCAN_H_

#include "dfifo.h"


#define CAN_RX_BUFF_SIZE    32

#define CAN_REMOTE_ID       0x00001314

typedef struct
{
    fifo_t rxFifo;
    void (*rx_callback)(void);
} can_t;


void can_init(void);
uint8_t can_sendExtData(uint32_t id, uint8_t *data, uint16_t len);
uint16_t can_get(uint8_t *buff, uint16_t n);

#endif


/*********************************** End of file *****************************************/
