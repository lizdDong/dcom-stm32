/**
  ******************************************************************************
  * @file    duart.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-7-7
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "duart.h"


#if (UART1_EN)
uint8_t g_aUart1TxFifoBuff[UART1_TX_FIFO_BUFF_SIZE];
uint8_t g_aUart1RxFifoBuff[UART1_RX_FIFO_BUFF_SIZE];
#if (UART1_USE_DMA)
uint8_t g_aUart1TxDmaBuff[UART1_TX_DMA_BUFF_SIZE];
uint8_t g_aUart1RxDmaBuff[UART1_RX_DMA_BUFF_SIZE];
#endif
uart_t g_sUart1;
#endif

#if (UART2_EN)
uint8_t g_aUart2TxFifoBuff[UART2_TX_FIFO_BUFF_SIZE];
uint8_t g_aUart2RxFifoBuff[UART2_RX_FIFO_BUFF_SIZE];
#if (UART2_USE_DMA)
uint8_t g_aUart2TxDmaBuff[UART2_TX_DMA_BUFF_SIZE];
uint8_t g_aUart2RxDmaBuff[UART2_RX_DMA_BUFF_SIZE];
#endif
uart_t g_sUart2;
#endif

#if (UART3_EN)
uint8_t g_aUart3TxFifoBuff[UART3_TX_FIFO_BUFF_SIZE];
uint8_t g_aUart3RxFifoBuff[UART3_RX_FIFO_BUFF_SIZE];
#if (UART3_USE_DMA)
uint8_t g_aUart3TxDmaBuff[UART3_TX_DMA_BUFF_SIZE];
uint8_t g_aUart3RxDmaBuff[UART3_RX_DMA_BUFF_SIZE];
#endif
uart_t g_sUart3;
#endif

static void uart_cfg(void);


/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void uart_fifo_lock(void *arg)
{
    uint32_t *pmask = (uint32_t *)arg;
    *pmask = enter_critical();
}

void uart_fifo_unlock(void *arg)
{
    uint32_t mask = *((uint32_t *)arg);
    exit_critical(mask);
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void uart_init(void)
{
    uart_cfg();

#if (UART1_EN)
    fifo_init(&g_sUart1.txFifo, g_aUart1TxFifoBuff, sizeof(g_aUart1TxFifoBuff), uart_fifo_lock, uart_fifo_unlock);
    fifo_init(&g_sUart1.rxFifo, g_aUart1RxFifoBuff, sizeof(g_aUart1RxFifoBuff), uart_fifo_lock, uart_fifo_unlock);
    g_sUart1.uartx = USART1;
#if (UART1_USE_DMA)
    g_sUart1.txDmaChannel = DMA1_Channel4;
    g_sUart1.rxDmaChannel = DMA1_Channel5;
    g_sUart1.pTxDmaBuff = g_aUart1TxDmaBuff;
    g_sUart1.pRxDmaBuff = g_aUart1RxDmaBuff;
    g_sUart1.rxDmaBuffSize = UART1_RX_DMA_BUFF_SIZE;
    g_sUart1.dmaLastRxCnt = 0;
    g_sUart1.dmaOnTransmit = 0;
#endif
    g_sUart1.txStart_callback = NULL;
    g_sUart1.txEnd_callback = NULL;
#endif

#if (UART2_EN)
    fifo_init(&g_sUart2.txFifo, g_aUart2TxFifoBuff, sizeof(g_aUart2TxFifoBuff), uart_fifo_lock, uart_fifo_unlock);
    fifo_init(&g_sUart2.rxFifo, g_aUart2RxFifoBuff, sizeof(g_aUart2RxFifoBuff), uart_fifo_lock, uart_fifo_unlock);
    g_sUart2.uartx = USART2;
#if (UART2_USE_DMA)
    g_sUart2.txDmaChannel = DMA1_Channel7;
    g_sUart2.rxDmaChannel = DMA1_Channel6;
    g_sUart2.pTxDmaBuff = g_aUart2TxDmaBuff;
    g_sUart2.pRxDmaBuff = g_aUart2RxDmaBuff;
    g_sUart2.rxDmaBuffSize = UART2_RX_DMA_BUFF_SIZE;
    g_sUart2.dmaLastRxCnt = 0;
    g_sUart2.dmaOnTransmit = 0;
#endif
    g_sUart2.txStart_callback = NULL;
    g_sUart2.txEnd_callback = NULL;
#endif

#if (UART3_EN)
    fifo_init(&g_sUart3.txFifo, g_aUart3TxFifoBuff, sizeof(g_aUart3TxFifoBuff), uart_fifo_lock, uart_fifo_unlock);
    fifo_init(&g_sUart3.rxFifo, g_aUart3RxFifoBuff, sizeof(g_aUart3RxFifoBuff), uart_fifo_lock, uart_fifo_unlock);
    g_sUart3.uartx = USART3;
#if (UART3_USE_DMA)
    g_sUart3.txDmaChannel = DMA1_Channel2;
    g_sUart3.rxDmaChannel = DMA1_Channel3;
    g_sUart3.pTxDmaBuff = g_aUart3TxDmaBuff;
    g_sUart3.pRxDmaBuff = g_aUart3RxDmaBuff;
    g_sUart3.rxDmaBuffSize = UART3_RX_DMA_BUFF_SIZE;
    g_sUart3.dmaLastRxCnt = 0;
    g_sUart3.dmaOnTransmit = 0;
#endif
    g_sUart3.txStart_callback = NULL;
    g_sUart3.txEnd_callback = NULL;
#endif
}




/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
static void uart_cfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
#if (UART1_USE_DMA || UART2_USE_DMA || UART3_USE_DMA)
    DMA_InitTypeDef DMA_InitStructure;
#endif
    
    USART_DeInit(USART1);
    USART_DeInit(USART2);
    USART_DeInit(USART3);
    USART_DeInit(UART4);
    USART_DeInit(UART5);

#if (UART1_EN)

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = UART1_BAUD;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    /* 配置串口中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

#if (UART1_USE_DMA)

    /* 使能DMA收发 */
    USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);

    /* 配置串口tx DMA */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel4);
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t) & (USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)g_aUart1TxDmaBuff;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralDST;    /* 传输方向:内存->外设 */
    DMA_InitStructure.DMA_BufferSize            = 0;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    //DMA_Cmd(DMA1_Channel4, ENABLE); // 将在发送函数中启用

    /* 配置串口rx DMA */
    DMA_DeInit(DMA1_Channel5);
    DMA_Cmd(DMA1_Channel5, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t) & (USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)g_aUart1RxDmaBuff;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralSRC;    /* 传输方向:外设->内存 */
    DMA_InitStructure.DMA_BufferSize            = UART1_RX_DMA_BUFF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel5, ENABLE);

    /* 配置DMA中断 */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;    /* UART1 DMA1Tx */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;    /* UART1 DMA1Rx */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 使能DMA发送完成中断 */
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    DMA_ClearFlag(DMA1_IT_TC4);

    /* 使能DMA半满、溢满中断 */
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC | DMA_IT_HT, ENABLE);
    DMA_ClearFlag(DMA1_IT_TC5);
    DMA_ClearFlag(DMA1_IT_HT5);

    /* 使能空闲中断 */
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
#else

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  /* 使能接收中断 */
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);  /* 此处不打开发送中断，发送中断使能在发送函数打开 */
    USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送完成标志，解决第1个字节无法正确发送出去的问题 */
#endif

    /* 使能串口 */
    USART_Cmd(USART1, ENABLE);
#endif

#if (UART2_EN)

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = UART2_BAUD;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    /* 配置串口中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
#if (UART2_USE_DMA)

    /* 使能DMA收发 */
    USART_DMACmd(USART2, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);

    /* 配置串口tx DMA */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel7);
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t) & (USART2->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)g_aUart2TxDmaBuff;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralDST;    /* 传输方向:内存->外设 */
    DMA_InitStructure.DMA_BufferSize            = 0;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);
    //DMA_Cmd(DMA1_Channel7, ENABLE); // 将在发送函数中启用

    /* 配置串口rx DMA */
    DMA_DeInit(DMA1_Channel6);
    DMA_Cmd(DMA1_Channel6, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t) & (USART2->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)g_aUart2RxDmaBuff;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralSRC;    /* 传输方向:外设->内存 */
    DMA_InitStructure.DMA_BufferSize            = UART2_RX_DMA_BUFF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel6, ENABLE);

    /* 配置DMA中断 */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;    /* UART2 DMA1Tx */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;    /* UART2 DMA1Rx */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 使能DMA发送完成中断 */
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
    DMA_ClearFlag(DMA1_IT_TC7);

    /* 使能DMA半满、溢满中断 */
    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC | DMA_IT_HT, ENABLE);
    DMA_ClearFlag(DMA1_IT_TC6);
    DMA_ClearFlag(DMA1_IT_HT6);

    /* 使能空闲中断 */
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
#else

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  /* 使能接收中断 */
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);  /* 此处不打开发送中断，发送中断使能在发送函数打开 */
    USART_ClearFlag(USART2, USART_FLAG_TC);     /* 清发送完成标志，解决第1个字节无法正确发送出去的问题 */
#endif

    USART_Cmd(USART2, ENABLE);    /* 使能串口 */

#endif

#if (UART3_EN)

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = UART3_BAUD;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    /* 配置串口中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
#if (UART3_USE_DMA)

    /* 使能DMA收发 */
    USART_DMACmd(USART3, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);

    /* 配置串口tx DMA */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel2);
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t) & (USART3->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)g_aUart3TxDmaBuff;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralDST;    /* 传输方向:内存->外设 */
    DMA_InitStructure.DMA_BufferSize            = 0;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    //DMA_Cmd(DMA1_Channel2, ENABLE); // 将在发送函数中启用

    /* 配置串口rx DMA */
    DMA_DeInit(DMA1_Channel3);
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t) & (USART3->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr        = (uint32_t)g_aUart3RxDmaBuff;
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralSRC;    /* 传输方向:外设->内存 */
    DMA_InitStructure.DMA_BufferSize            = UART3_RX_DMA_BUFF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                   = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel3, ENABLE);

    /* 配置DMA中断 */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;    /* UART3 DMA1Tx */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;    /* UART3 DMA1Rx */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 使能DMA发送完成中断 */
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
    DMA_ClearFlag(DMA1_IT_TC2);

    /* 使能DMA半满、溢满中断 */
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC | DMA_IT_HT, ENABLE);
    DMA_ClearFlag(DMA1_IT_TC3);
    DMA_ClearFlag(DMA1_IT_HT3);

    /* 使能空闲中断 */
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
#else

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  /* 使能接收中断 */
    USART_ITConfig(USART3, USART_IT_TXE, DISABLE);  /* 此处不打开发送中断，发送中断使能在发送函数打开 */
    USART_ClearFlag(USART3, USART_FLAG_TC);     /* 清发送完成标志，解决第1个字节无法正确发送出去的问题 */
#endif

    USART_Cmd(USART3, ENABLE);    /* 使能串口 */

#endif
}

#if (UART1_USE_DMA || UART2_USE_DMA || UART3_USE_DMA)
/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void uart_dmaTransmit(uart_t *uart)
{
    uint32_t cnt;

    cnt = fifo_read(&uart->txFifo, uart->pTxDmaBuff, UART1_TX_DMA_BUFF_SIZE, 0);
    if(cnt > 0)
    {
        uart->dmaOnTransmit = 1;
        uart->txDmaChannel->CNDTR = cnt;
        DMA_Cmd(uart->txDmaChannel, ENABLE);
    }
    else
    {
        uart->dmaOnTransmit = 0;
    }
}

/**
 ****************************************************************************
 * @brief  串口 DMA发送中断处理函数
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
#if (UART1_USE_DMA)
void DMA1_Channel4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC4))
    {
        DMA_ClearFlag(DMA1_FLAG_TC4);
        DMA_Cmd(DMA1_Channel4, DISABLE);
        uart_dmaTransmit(&g_sUart1);
    }
}
#endif

#if (UART2_USE_DMA)
void DMA1_Channel7_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC7))
    {
        DMA_ClearFlag(DMA1_FLAG_TC7);
        DMA_Cmd(DMA1_Channel7, DISABLE);
        uart_dmaTransmit(&g_sUart2);
    }
}
#endif

#if (UART3_USE_DMA)
void DMA1_Channel2_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC2))
    {
        DMA_ClearFlag(DMA1_FLAG_TC2);
        DMA_Cmd(DMA1_Channel2, DISABLE);
        uart_dmaTransmit(&g_sUart3);
    }
}
#endif

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void uart_dmaRxCplt(uart_t *uart)
{
    uint32_t cnt;
    
    cnt = uart->rxDmaBuffSize - uart->dmaLastRxCnt;
    cnt = fifo_write(&uart->rxFifo, &uart->pRxDmaBuff[uart->dmaLastRxCnt], cnt, 0);
    uart->dmaLastRxCnt = 0;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void uart_dmaRxHalfCplt(uart_t *uart)
{
    uint32_t cnt;
    
    cnt = uart->rxDmaBuffSize - DMA_GetCurrDataCounter(uart->rxDmaChannel) - uart->dmaLastRxCnt;
    cnt = fifo_write(&uart->rxFifo, &uart->pRxDmaBuff[uart->dmaLastRxCnt], cnt, 0);
    uart->dmaLastRxCnt += cnt;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void uart_rxIdle(uart_t *uart)
{
    uint32_t cnt;
    
    cnt = uart->rxDmaBuffSize - DMA_GetCurrDataCounter(uart->rxDmaChannel) - uart->dmaLastRxCnt;
    if(cnt > 0)
    {
        cnt = fifo_write(&uart->rxFifo, &uart->pRxDmaBuff[uart->dmaLastRxCnt], cnt, 0);
        uart->dmaLastRxCnt += cnt;
    }
}
#endif

/**
 ****************************************************************************
 * @brief  串口 DMA接收中断处理函数
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
#if (UART1_USE_DMA)
void DMA1_Channel5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC5))
    {
        DMA_ClearFlag(DMA1_FLAG_TC5);
        uart_dmaRxCplt(&g_sUart1);
    }
    if(DMA_GetITStatus(DMA1_IT_HT5))
    {
        DMA_ClearFlag(DMA1_FLAG_HT5);
        uart_dmaRxHalfCplt(&g_sUart1);
    }
}
#endif

#if (UART2_USE_DMA)
void DMA1_Channel6_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC6))
    {
        DMA_ClearFlag(DMA1_FLAG_TC6);
        uart_dmaRxCplt(&g_sUart2);
    }
    if(DMA_GetITStatus(DMA1_IT_HT6))
    {
        DMA_ClearFlag(DMA1_FLAG_HT6);
        uart_dmaRxHalfCplt(&g_sUart2);
    }
}
#endif

#if (UART3_USE_DMA)
void DMA1_Channel3_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC3))
    {
        DMA_ClearFlag(DMA1_FLAG_TC3);
        uart_dmaRxCplt(&g_sUart3);
    }
    if(DMA_GetITStatus(DMA1_IT_HT3))
    {
        DMA_ClearFlag(DMA1_FLAG_HT3);
        uart_dmaRxHalfCplt(&g_sUart3);
    }
}
#endif

/**
 ****************************************************************************
 * @brief  Referred to armfly's code.
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
static void uart_irq(uart_t *uart)
{
    /* 处理接收中断  */
    if(USART_GetITStatus(uart->uartx, USART_IT_RXNE) != RESET)
    {
        uint8_t rxbyte;

        /* 从串口接收数据寄存器读取数据存放到接收FIFO */
        rxbyte = USART_ReceiveData(uart->uartx);
        fifo_write(&uart->rxFifo, &rxbyte, 1, 0);
    }
    
#if (UART1_USE_DMA || UART2_USE_DMA || UART3_USE_DMA)
    /* 处理空闲中断 */
    if(USART_GetITStatus(uart->uartx, USART_IT_IDLE) != RESET)
    {
		/* read the data to clear receive idle interrupt flag */
		USART_ReceiveData(uart->uartx);
        uart_rxIdle(uart);
    }
#endif

    /* 处理发送缓冲区空中断 */
    if(USART_GetITStatus(uart->uartx, USART_IT_TXE) != RESET)
    {
        uint16_t txcnt;
        uint8_t txbyte;

        /* 从发送FIFO取1个字节写入串口发送数据寄存器 */
        txcnt = fifo_read(&uart->txFifo, &txbyte, 1, 0);
        if(txcnt != 0)
        {
            USART_SendData(uart->uartx, txbyte);
        }
        else
        {
            /* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
            USART_ITConfig(uart->uartx, USART_IT_TXE, DISABLE);

            /* 使能数据发送完毕中断 */
            USART_ITConfig(uart->uartx, USART_IT_TC, ENABLE);
        }
    }
    /* 处理数据bit位全部发送完毕的中断 */
    else if(USART_GetITStatus(uart->uartx, USART_IT_TC) != RESET)
    {
        /* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
        USART_ITConfig(uart->uartx, USART_IT_TC, DISABLE);

        /* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
        if(uart->txEnd_callback != NULL)
        {
            uart->txEnd_callback();
        }
    }
}

/**
 ****************************************************************************
 * @brief  串口中断处理函数
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
#if (UART1_EN)
void USART1_IRQHandler(void)
{
    uart_irq(&g_sUart1);
}
#endif

#if (UART2_EN)
void USART2_IRQHandler(void)
{
    uart_irq(&g_sUart2);
}
#endif

#if (UART3_EN)
void USART3_IRQHandler(void)
{
    uart_irq(&g_sUart3);
}
#endif

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint16_t uart_write(uart_t *uart, uint8_t *buff, uint16_t len)
{
    uint16_t txcnt;

    txcnt = fifo_write(&uart->txFifo, buff, len, 100);
    if(txcnt > 0)
    {
        /* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为发送模式 */
        if(uart->txStart_callback != NULL)
        {
            uart->txStart_callback();
        }
        
        if(uart->uartx == USART1)
        {
        #if (UART1_USE_DMA)
            if(uart->dmaOnTransmit == 0)
            {
                uart_dmaTransmit(uart);
            }
        #else
            USART_ITConfig(uart->uartx, USART_IT_TXE, ENABLE);    // 启动发送空中断，在中断中完成发送
        #endif
        }
        else if(uart->uartx == USART2)
        {
        #if (UART2_USE_DMA)
            if(uart->dmaOnTransmit == 0)
            {
                uart_dmaTransmit(uart);
            }
        #else
            USART_ITConfig(uart->uartx, USART_IT_TXE, ENABLE);    // 启动发送空中断，在中断中完成发送
        #endif
        }
        else if(uart->uartx == USART3)
        {
        #if (UART3_USE_DMA)
            if(uart->dmaOnTransmit == 0)
            {
                uart_dmaTransmit(uart);
            }
        #else
            USART_ITConfig(uart->uartx, USART_IT_TXE, ENABLE);    // 启动发送空中断，在中断中完成发送
        #endif
        }


    }
    return txcnt;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint16_t uart_read(uart_t *uart, uint8_t *buff, uint16_t len)
{
    uint16_t txcnt;

    txcnt = fifo_read(&uart->rxFifo, buff, len, 0);
    return txcnt;
}


/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
static uart_t *whichUart(com_t comx)
{
#if (UART1_EN)
    if(comx == UART_COM1) return &g_sUart1;
#endif

#if (UART2_EN)
    if(comx == UART_COM2) return &g_sUart2;
#endif

#if (UART3_EN)
    if(comx == UART_COM3) return &g_sUart3;
#endif

    return NULL;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint16_t uart_sendChar(com_t comx, char ch)
{
    uart_t *uartx;
    uint16_t ret;
    uint8_t dat[1];

    dat[0] = ch;
    uartx = whichUart(comx);
    if(uartx == NULL)
        return 0;
    ret = uart_write(uartx, dat, 1);

    return ret;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint16_t uart_sendByte(com_t comx, uint8_t byte)
{
    uart_t *uartx;
    uint16_t ret;
    uint8_t dat[1];

    dat[0] = byte;
    uartx = whichUart(comx);
    if(uartx == NULL)
        return 0;
    ret = uart_write(uartx, dat, 1);

    return ret;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint16_t uart_sendStr(com_t comx, char *str)
{
    uart_t *uartx;
    uint16_t ret;

    uartx = whichUart(comx);
    if(uartx == NULL)
        return 0;
    ret = uart_write(uartx, (uint8_t *)str, strlen(str));

    return ret;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint16_t uart_sendData(com_t comx, uint8_t *dat, uint16_t len)
{
    uart_t *uartx;
    uint16_t ret;

    uartx = whichUart(comx);
    if(uartx == NULL)
        return 0;
    ret = uart_write(uartx, dat, len);

    return ret;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint16_t uart_getChar(com_t comx, char *ch)
{
    uart_t *uartx;
    uint16_t ret;

    uartx = whichUart(comx);
    if(uartx == NULL)
        return 0;
    ret = uart_read(uartx, (uint8_t *)ch, 1);

    return ret;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint16_t uart_getByte(com_t comx, uint8_t *byte)
{
    uart_t *uartx;
    uint16_t ret;

    uartx = whichUart(comx);
    if(uartx == NULL)
        return 0;
    ret = uart_read(uartx, byte, 1);

    return ret;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint16_t uart_getData(com_t comx, uint8_t *dat, uint16_t len)
{
    uart_t *uartx;
    uint16_t ret;

    uartx = whichUart(comx);
    if(uartx == NULL)
        return 0;
    ret = uart_read(uartx, dat, len);

    return ret;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
int fputc(int ch, FILE *f)
{
#if 1 /* 将需要printf的字符通过串口中断FIFO发送出去，printf函数会立即返回 */
    uart_sendChar(UART_COM1, (uint8_t)ch);

    return ch;
#else /* 采用阻塞方式发送每个字符,等待数据发送完毕 */
    /* 写一个字节到USART1 */
    USART_SendData(USART1, (uint8_t) ch);

    /* 等待发送结束 */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}

    return ch;
#endif
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
int fgetc(FILE *f)
{

#if 1 /* 从串口接收FIFO中取1个数据, 只有取到数据才返回 */
    uint8_t rxbyte;

    while(uart_getChar(UART_COM1, (char *)&rxbyte) == 0);

    return rxbyte;
#else
    /* 等待串口1输入数据 */
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(USART1);
#endif
}

/**
 ****************************************************************************
 * @brief  实现RT-Thread的输入函数
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
//#ifdef RT_USING_FINSH
#if 0
char rt_hw_console_getchar(void)
{
    char ucData;

    if(uart_getChar(UART_COM1, &ucData) == 1)
    {
        return ucData;
    }
    return (char)(-1);
}
#endif

/**
 ****************************************************************************
 * @brief  实现RT-Thread的输出函数
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
//#ifdef RT_USING_CONSOLE
#if 0
void rt_hw_console_output(const char *str)
{
    uint16_t i = 0;

    while(1)
    {
        if(*str == '\n')
        {
            uart_sendChar(UART_COM1, '\r');
        }
        uart_sendChar(UART_COM1, *str);
        if(*(++str) == '\0' || ++i == RT_CONSOLEBUF_SIZE)
        {
            break;
        }
    }
}
#endif

/******************************* End of file **********************************/

