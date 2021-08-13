/**
  ******************************************************************************
  * @file    duart.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-7-7
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#ifndef _DUART_H_
#define _DUART_H_

#include <stdint.h>
#include "stm32f10x.h"
#include "dfifo.h"

#define UART1_EN                    1
#define UART1_USE_DMA               1
#define UART1_BAUD                  115200
#define UART1_TX_FIFO_BUFF_SIZE     256
#define UART1_RX_FIFO_BUFF_SIZE     256
#define UART1_TX_DMA_BUFF_SIZE      64
#define UART1_RX_DMA_BUFF_SIZE      64

#define UART2_EN                    0
#define UART2_USE_DMA               0
#define UART2_BAUD                  115200
#define UART2_TX_FIFO_BUFF_SIZE     256
#define UART2_RX_FIFO_BUFF_SIZE     256
#define UART2_TX_DMA_BUFF_SIZE      64
#define UART2_RX_DMA_BUFF_SIZE      64

#define UART3_EN                    0
#define UART3_USE_DMA               0
#define UART3_BAUD                  115200
#define UART3_TX_FIFO_BUFF_SIZE     256
#define UART3_RX_FIFO_BUFF_SIZE     256
#define UART3_TX_DMA_BUFF_SIZE      64
#define UART3_RX_DMA_BUFF_SIZE      64

typedef enum
{
    UART_COM1 = 1,
    UART_COM2,
    UART_COM3,
    UART_COM4,
    UART_COM5,
    UART_COM6
} com_t;

typedef struct
{
    USART_TypeDef *uartx;
#if (UART1_USE_DMA || UART2_USE_DMA || UART3_USE_DMA)
    DMA_Channel_TypeDef *txDmaChannel;
    DMA_Channel_TypeDef *rxDmaChannel;
    uint8_t *pTxDmaBuff;
    uint8_t *pRxDmaBuff;
    uint32_t rxDmaBuffSize;
    uint32_t dmaLastRxCnt;
    uint8_t dmaOnTransmit;
#endif
    fifo_t txFifo;
    fifo_t rxFifo;
    void (*txStart_callback)(void);
    void (*txEnd_callback)(void);
} uart_t;



void uart_init(void);

uint16_t uart_write(uart_t *uart, uint8_t *buff, uint16_t len);
uint16_t uart_read(uart_t *uart, uint8_t *buff, uint16_t len);
uint16_t uart_sendChar(com_t comx, char c);
uint16_t uart_sendByte(com_t comx, uint8_t byte);
uint16_t uart_sendStr(com_t comx, char *str);
uint16_t uart_sendData(com_t comx, uint8_t *dat, uint16_t len);
uint16_t uart_getChar(com_t comx, char *c);
uint16_t uart_getByte(com_t comx, uint8_t *byte);
uint16_t uart_getData(com_t comx, uint8_t *dat, uint16_t len);



#endif


/******************************* End of file **********************************/
