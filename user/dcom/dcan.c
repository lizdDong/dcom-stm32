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

    /* ���ù��ܺ�GPIOB�˿�ʱ��ʹ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);

    /* CAN1 ģ��ʱ��ʹ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

#if(0)
    //��ӳ��I/O
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

    /* Configure CAN pin: RX */  // PB8
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure CAN pin: TX */   // PB9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#else
    /* Configure CAN pin: RX */  // PA11
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure CAN pin: TX */   // PA12
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //�����������
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
    CAN_DeInit(CAN1);                       //������CAN��ȫ���Ĵ�������Ϊȱʡֵ
    CAN_StructInit(&CAN_InitStructure);     //��CAN_InitStruct�е�ÿһ��������ȱʡֵ����

    /* CAN cell init */
    CAN_InitStructure.CAN_TTCM = DISABLE;       //û��ʹ��ʱ�䴥��ģʽ
    CAN_InitStructure.CAN_ABOM = DISABLE;       //û��ʹ���Զ����߹���
    CAN_InitStructure.CAN_AWUM = DISABLE;       //û��ʹ���Զ�����ģʽ
    CAN_InitStructure.CAN_NART = DISABLE;       //û��ʹ�ܷ��Զ��ش�ģʽ
    CAN_InitStructure.CAN_RFLM = DISABLE;       //û��ʹ�ܽ���FIFO����ģʽ
    CAN_InitStructure.CAN_TXFP = DISABLE;       //û��ʹ�ܷ���FIFO���ȼ�
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; //CAN����Ϊ����ģʽ
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;    //����ͬ����Ծ���1��ʱ�䵥λ
    CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;    //ʱ���1Ϊ5��ʱ�䵥λ
    CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;    //ʱ���2Ϊ3��ʱ�䵥λ
    CAN_InitStructure.CAN_Prescaler = 16;       //��Ƶϵ��Ϊ32
    CAN_Init(CAN1, &CAN_InitStructure);         //������Ϊ��72000/2/(1+5+3)/16=250K ��������Ϊ250KBPs

    // CAN filter init ������
    CAN_FilterInitStructure.CAN_FilterNumber = 1;                   //ָ��������Ϊ1
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; //ָ��������Ϊ��ʶ������λģʽ
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; //������λ��Ϊ32λ

    CAN_FilterInitStructure.CAN_FilterIdHigh = 0X0000;              //Ҫ���˵�ID��λ
    CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)0x1314 << 3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF; //Ҫ���˵�ID��λ

    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;          //���������α�ʶ���ĸ�16λֵ
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;           //���������α�ʶ���ĵ�16λֵ
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;   //�趨��ָ���������FIFOΪ0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;          //ʹ�ܹ�����
    CAN_FilterInit(&CAN_FilterInitStructure);                       //������Ĳ�����ʼ��������

    /* CAN FIFO0 message pending interrupt enable */
    if(1)
    {
        // DMA��������ж�����
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);                        //ʹ��FIFO0��Ϣ�Һ��ж�
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
    

    tx_msg.StdId = (id >> 16) & 0x7FF;           //��׼��ʶ��
    tx_msg.ExtId = id & 0x1FFFFFFF;              //��չ��ʶ��, ���Ը��ĸñ�ʶ����ʾ���ֲ�ͬ�ӻ�
    tx_msg.IDE = CAN_ID_EXT;                     //ʹ����չ��ʶ��
    tx_msg.RTR = CAN_RTR_DATA;                   //Ϊ����֡
    
    if(len > 8)
    {
        uint16_t m, n;
        
        m = len / 8;
        n = len % 8;
        
        for(i = 0; i < m; i++)
        {
            memcpy(&tx_msg.Data[0], &data[i * 8], 8);
            tx_msg.DLC = 8;                            //��Ϣ�����ݳ���

            //��������
            mbox = CAN_Transmit(CAN1, &tx_msg);
            while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
                i++;    //�ȴ����ͽ���
            if(i >= 0XFFF)
                return 0;
        }
        
        if(n > 0)
        {
            memcpy(&tx_msg.Data[0], &data[m * 8], n);
            tx_msg.DLC = n;                            //��Ϣ�����ݳ���

            //��������
            mbox = CAN_Transmit(CAN1, &tx_msg);
            while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
                i++;    //�ȴ����ͽ���
            if(i >= 0XFFF)
                return 0;
        }
        return 1;
    }
    
    tx_msg.DLC = len;                            //��Ϣ�����ݳ���

    //��������
    mbox = CAN_Transmit(CAN1, &tx_msg);
    while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
        i++;    //�ȴ����ͽ���
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
 * @brief  USB�жϺ�CAN�����жϷ������USB��CAN����I/O������ֻ�õ�CAN���ж�
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    g_CanRxMsg.DLC = 0;

    CAN_Receive(CAN1, CAN_FIFO0, &g_CanRxMsg); //����FIFO0�е�����
    
    fifo_write(&g_sCan.rxFifo, g_CanRxMsg.Data, g_CanRxMsg.DLC);
}







/*********************************** End of file *****************************************/
