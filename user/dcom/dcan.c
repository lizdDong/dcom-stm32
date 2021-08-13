/**
  ******************************************************************************
  * @file    dcan.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-7-23
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "dcan.h"


static uint8_t g_aCanRxBuff[CAN_RX_BUFF_SIZE];
static can_t g_sCan;
static CanRxMsg g_CanRxMsg;


static void can_cfg(void);


/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void can_fifo_lock(void *arg)
{
    uint32_t *pmask = (uint32_t *)arg;
    *pmask = enter_critical();
}

void can_fifo_unlock(void *arg)
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
void can_init(void)
{
    can_cfg();
    
    fifo_init(&g_sCan.rxFifo, g_aCanRxBuff, sizeof(g_aCanRxBuff), can_fifo_lock, can_fifo_unlock);
    g_sCan.rx_callback = NULL;
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
static void can_cfg(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;
    CAN_InitTypeDef         CAN_InitStructure;
    CAN_FilterInitTypeDef   CAN_FilterInitStructure;

    /* 复用功能和GPIOB端口时钟使能*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);

    /* CAN1 模块时钟使能 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

#if(0)
    //重映射I/O
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

    /* Configure CAN pin: RX */  // PB8
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure CAN pin: TX */   // PB9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#else
    /* Configure CAN pin: RX */  // PA11
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure CAN pin: TX */   // PA12
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

    /* enabling interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* CAN register init */
    CAN_DeInit(CAN1);                       //将外设CAN的全部寄存器重设为缺省值
    CAN_StructInit(&CAN_InitStructure);     //把CAN_InitStruct中的每一个参数按缺省值填入

    /* CAN cell init */
    CAN_InitStructure.CAN_TTCM = DISABLE;       //没有使能时间触发模式
    CAN_InitStructure.CAN_ABOM = DISABLE;       //没有使能自动离线管理
    CAN_InitStructure.CAN_AWUM = DISABLE;       //没有使能自动唤醒模式
    CAN_InitStructure.CAN_NART = DISABLE;       //没有使能非自动重传模式
    CAN_InitStructure.CAN_RFLM = DISABLE;       //没有使能接收FIFO锁定模式
    CAN_InitStructure.CAN_TXFP = DISABLE;       //没有使能发送FIFO优先级
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; //CAN设置为正常模式
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;    //重新同步跳跃宽度1个时间单位
    CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;    //时间段1为5个时间单位
    CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;    //时间段2为3个时间单位
    CAN_InitStructure.CAN_Prescaler = 16;       //分频系数为32
    CAN_Init(CAN1, &CAN_InitStructure);         //波特率为：72000/2/(1+5+3)/16=250K 即波特率为250KBPs

    // CAN filter init 过滤器
    CAN_FilterInitStructure.CAN_FilterNumber = 1;                   //指定过滤器为1
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; //指定过滤器为标识符屏蔽位模式
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; //过滤器位宽为32位

    CAN_FilterInitStructure.CAN_FilterIdHigh = 0X0000;              //要过滤的ID高位
    CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)0x1314 << 3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF; //要过滤的ID低位

    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;          //过滤器屏蔽标识符的高16位值
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;           //过滤器屏蔽标识符的低16位值
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;   //设定了指向过滤器的FIFO为0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;          //使能过滤器
    CAN_FilterInit(&CAN_FilterInitStructure);                       //按上面的参数初始化过滤器

    /* CAN FIFO0 message pending interrupt enable */
    if(1)
    {
        // DMA传输完成中断配置
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);                        //使能FIFO0消息挂号中断
    }
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
uint8_t can_sendExtData(uint32_t id, uint8_t *data, uint16_t len)
{
    uint8_t mbox;
    uint16_t i = 0;
    CanTxMsg tx_msg;
    

    tx_msg.StdId = (id >> 16) & 0x7FF;           //标准标识符
    tx_msg.ExtId = id & 0x1FFFFFFF;              //扩展标识符, 可以更改该标识符以示区分不同从机
    tx_msg.IDE = CAN_ID_EXT;                     //使用扩展标识符
    tx_msg.RTR = CAN_RTR_DATA;                   //为数据帧
    
    if(len > 8)
    {
        uint16_t m, n;
        
        m = len / 8;
        n = len % 8;
        
        for(i = 0; i < m; i++)
        {
            memcpy(&tx_msg.Data[0], &data[i * 8], 8);
            tx_msg.DLC = 8;                            //消息的数据长度

            //发送数据
            mbox = CAN_Transmit(CAN1, &tx_msg);
            while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
                i++;    //等待发送结束
            if(i >= 0XFFF)
                return 0;
        }
        
        if(n > 0)
        {
            memcpy(&tx_msg.Data[0], &data[m * 8], n);
            tx_msg.DLC = n;                            //消息的数据长度

            //发送数据
            mbox = CAN_Transmit(CAN1, &tx_msg);
            while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
                i++;    //等待发送结束
            if(i >= 0XFFF)
                return 0;
        }
        return 1;
    }
    
    tx_msg.DLC = len;                            //消息的数据长度

    //发送数据
    mbox = CAN_Transmit(CAN1, &tx_msg);
    while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
        i++;    //等待发送结束
    if(i >= 0XFFF)
        return 0;
    
    return 1;
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
uint16_t can_get(uint8_t *buff, uint16_t n)
{
    return fifo_read(&g_sCan.rxFifo, buff, n);
}

/**
 ****************************************************************************
 * @brief  USB中断和CAN接收中断服务程序，USB跟CAN公用I/O，这里只用到CAN的中断
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    g_CanRxMsg.DLC = 0;

    CAN_Receive(CAN1, CAN_FIFO0, &g_CanRxMsg); //接收FIFO0中的数据
    
    fifo_write(&g_sCan.rxFifo, g_CanRxMsg.Data, g_CanRxMsg.DLC);
}







/*********************************** End of file *****************************************/
