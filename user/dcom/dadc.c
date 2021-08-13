/**
  ******************************************************************************
  * @file    dadc.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-4-23
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "dadc.h"


__IO uint16_t g_aAdc1Value[ADC_SAMPLING_NUM][ADC_CHANNEL_NUM];
__IO uint16_t g_aAdc3Value[ADC3_SAMPLING_NUM][ADC3_CHANNEL_NUM];
__IO uint32_t g_aAdcDualConvValue[ADC_DUALMODE_SAMPLING_NUM][ADC_DUALMODE_CHANNEL_NUM];


#if (ADC_SAMPLING_NUM >= ADC3_SAMPLING_NUM)
static uint16_t g_aAdcValueTemp[ADC_SAMPLING_NUM];
#else
static uint16_t g_aAdcValueTemp[ADC3_SAMPLING_NUM];
#endif

uint8_t g_fAdcConvOver;

#if (USE_RTOS_RT_THREAD)

// �������л������
#include "rtthread.h"
#ifndef RT_USING_MUTEX
#error "Please define [RT_USING_MUTEX] at <rtconfig.h> "
#endif
static struct rt_mutex mutex_getadc;

#endif


static void adc1_timerTrigCfg(uint16_t prescaler, uint16_t period);
static void adc3_timerTrigCfg(uint16_t prescaler, uint16_t period);


/**
 ****************************************************************************
 * @brief  ADC1��ʼ��������ת����DMA���䣬��ѡ��ʱ���������������
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void adc1_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* ʹ�� ADC1 GPIO DMA ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* ����ADC1 GPIO�˿� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* ����ADC1 DMA */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;       // �������ݵ�ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_aAdc1Value;       // �ڴ��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                    // ���䷽�� ����->�ڴ�
    DMA_InitStructure.DMA_BufferSize = ADC_SAMPLING_NUM * ADC_CHANNEL_NUM;        // �ڴ��С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              // ��������ģʽ���������ֹ��
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       // �����ڴ��ַ����ģʽ�����ڴ�����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   // �������ݿ��
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;           // �ڴ����ݿ��
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;       // ѭ������ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;   // ͨ�����ȼ�Ϊ��
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;          // �رմ��ڴ浽�ڴ�ģʽ
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);       // ʹ�� DMAͨ��
#if(1)
    {
        // DMA��������ж�����
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);   // ʹ��DMA�ж�
    }
#endif

    /* ����ADC1 ͨ�� */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;    // ����ADCģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;          // ��ͨ��ɨ�跽ʽ����ͨ��ʱ�򿪣�
    //ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // ������ת��ģʽ���������ת��ʱ�򿪣�
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   // �ر�����ת��ģʽ���ⲿ��������ת��ʱ�رգ�
    //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // ��ʹ���ⲿ����ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;   // ʹ���ⲿ����ģʽ�������¼�Ϊ T2_CC2��
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      // �ɼ������Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNEL_NUM;       // Ҫת����ͨ����Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_DMACmd(ADC1, ENABLE);               // ʹ�� ADC1�� DMA����
    //ADC_TempSensorVrefintCmd(ENABLE);     // ʹ���ڲ��¶ȴ������Ͳο�ֵADC

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);       //����ADCʱ�ӣ�ΪPCLK2��6��Ƶ����12MHz
    /* ת����ʱ���㣺T = 1 / f * (ADC_SampleTime + 12.5) */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);

    /* ADC1 У׼ */
    ADC_Cmd(ADC1, ENABLE);      // ʹ�� ADC����
    ADC_ResetCalibration(ADC1);                     // ��λУ׼�Ĵ���
    while(ADC_GetResetCalibrationStatus(ADC1));     // �ȴ�У׼�Ĵ�����λ���
    ADC_StartCalibration(ADC1);               // ADCУ׼
    while(ADC_GetCalibrationStatus(ADC1));    // �ȴ�У׼���
    //ADC_SoftwareStartConvCmd(ADC1, ENABLE);   // �������ADCת��
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);    // �ⲿ����ģʽ����ADCת��

    /* ����Ϊ�ⲿ��ʱ������ */
    adc1_timerTrigCfg(72, 100);   // 72000000/72/100 = 10kHz��ÿ100us����һ�βɼ�

#if (USE_RTOS_RT_THREAD)
    rt_mutex_init(&mutex_getadc, "getAdc", RT_IPC_FLAG_FIFO);
#endif
}

/**
 ****************************************************************************
 * @brief  ADC3��ʼ��������ת����DMA���䣬��ѡ��ʱ���������������
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void adc3_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* ʹ�� ADC3 GPIO DMA ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    /* ����ADC3 GPIO�˿� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* ����ADC3 DMA */
    DMA_DeInit(DMA2_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;         // �������ݵ�ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_aAdc3Value;         // �ڴ��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      // ���䷽�� ����->�ڴ�
    DMA_InitStructure.DMA_BufferSize = ADC3_SAMPLING_NUM * ADC3_CHANNEL_NUM;      // �ڴ��С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              // ��������ģʽ���������ֹ��
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       // �����ڴ��ַ����ģʽ�����ڴ�����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   // �������ݿ��
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;           // �ڴ����ݿ��
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;         // ѭ������ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;     // ͨ�����ȼ�Ϊ��
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;            // �رմ��ڴ浽�ڴ�ģʽ
    DMA_Init(DMA2_Channel5, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel5, ENABLE);   // ʹ��DMA

    //����ΪDMA��������ж�����
#if(0)
    {
        NVIC_InitTypeDef NVIC_InitStructure;    /* �жϽṹ���� misc.h �ж��� */

        NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        DMA_ITConfig(DMA2_Channel5, DMA_IT_TC, ENABLE);
    }
#endif

    /* ����ADC3 ͨ�� */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;    // ����ADCģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;          // ��ͨ��ɨ�跽ʽ����ͨ��ʱ�򿪣�
    //ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // ������ת��ģʽ���������ת��ʱ�򿪣�
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   // �ر�����ת��ģʽ���ⲿ��������ת��ʱ�رգ�
    //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // ��ʹ���ⲿ����ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T5_CC1;   // ʹ���ⲿ����ģʽ�������¼�Ϊ T5_CC1��
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      // �ɼ������Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = ADC3_CHANNEL_NUM;      // Ҫת����ͨ����Ŀ
    ADC_Init(ADC3, &ADC_InitStructure);
    ADC_DMACmd(ADC3, ENABLE);               // ʹ�� ADC1�� DMA����

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);       //����ADCʱ�ӣ�ΪPCLK2��6��Ƶ����12MHz
    /* ת����ʱ���㣺T = 1 / f * (ADC_SampleTime + 12.5) */
    ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 2, ADC_SampleTime_239Cycles5);

    /* ADC3 У׼ */
    ADC_Cmd(ADC3, ENABLE);      // ʹ�� ADC����
    ADC_ResetCalibration(ADC3);                     // ��λУ׼�Ĵ���
    while(ADC_GetResetCalibrationStatus(ADC3));     // �ȴ�У׼�Ĵ�����λ���
    ADC_StartCalibration(ADC3);               // ADCУ׼
    while(ADC_GetCalibrationStatus(ADC3));    // �ȴ�У׼���
    //ADC_SoftwareStartConvCmd(ADC3, ENABLE);   // �������ADCת��
    ADC_ExternalTrigConvCmd(ADC3, ENABLE);    // �ⲿ����ģʽ����ADCת��

    /* ����Ϊ��ʱ������ */
    adc3_timerTrigCfg(72, 100);   // 72000000/72/100 = 10kHz��ÿ100us����һ�βɼ�

#if (USE_RTOS_RT_THREAD)
    //rt_mutex_init(&mutex_getadc, "getAdc", RT_IPC_FLAG_FIFO);
#endif
}
/**
 ****************************************************************************
 * @brief  ADC1��ADC2��˫ADCģʽ��ʼ��
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void adc12_init_dualMode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* ʹ�� ADC1 ADC2 GPIO DMA ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* ���� GPIO�˿� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |  GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* ���� DMA */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;       // �������ݵ�ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_aAdcDualConvValue;       // �ڴ��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                    // ���䷽�� ����->�ڴ�
    DMA_InitStructure.DMA_BufferSize = ADC_DUALMODE_SAMPLING_NUM * ADC_DUALMODE_CHANNEL_NUM;        // �ڴ��С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              // ��������ģʽ���������ֹ��
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       // �����ڴ��ַ����ģʽ�����ڴ�����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;   // �������ݿ��
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;           // �ڴ����ݿ��
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;       // ѭ������ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;   // ͨ�����ȼ�Ϊ��
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;          // �رմ��ڴ浽�ڴ�ģʽ
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);       // ʹ�� DMAͨ��

#if(1)
    {
        // DMA��������ж�����
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);   // ʹ��DMA�ж�
    }
#endif

    /* ע�⣺ ��˫ ADC ģʽ���ת�����ó����ⲿ�¼�����ʱ���û����뽫�����óɽ������� ADC ����ADC ����
              ������������������Է�ֹ����Ĵ�����ת�������ǣ����ʹ� ADC ���ⲿ��������ͬʱ������ */

    /* ����ADC1 ͨ�� */
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;      // ADC1��ADC2������ͬ������ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;          // ��ͨ��ɨ�跽ʽ����ͨ��ʱ�򿪣�
    //ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // ������ת��ģʽ���������ת��ʱ�򿪣�
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   // �ر�����ת��ģʽ���ⲿ��������ת��ʱ�رգ�
    //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // ��ʹ���ⲿ����ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;   // ʹ���ⲿ����ģʽ�������¼�Ϊ T2_CC2��
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      // �ɼ������Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = ADC_DUALMODE_CHANNEL_NUM;       // Ҫת����ͨ����Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_DMACmd(ADC1, ENABLE);               // ʹ�� ADC1�� DMA����
    //ADC_TempSensorVrefintCmd(ENABLE);     // ʹ���ڲ��¶ȴ������Ͳο�ֵADC

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);       //����ADCʱ�ӣ�ΪPCLK2��6��Ƶ����12MHz
    /* ת����ʱ���㣺T = 1 / f * (ADC_SampleTime + 12.5) */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 5, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 6, ADC_SampleTime_55Cycles5);

    /* ����ADC2 ͨ�� */
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;      // ADC1��ADC2������ͬ������ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;          // ��ͨ��ɨ�跽ʽ����ͨ��ʱ�򿪣�
    //ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // ������ת��ģʽ���������ת��ʱ�򿪣�
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   // �ر�����ת��ģʽ���ⲿ��������ת��ʱ�رգ�
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // ˫ ADC ģʽ�£��� ADC �������ó��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      // �ɼ������Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = ADC_DUALMODE_CHANNEL_NUM;       // Ҫת����ͨ����Ŀ
    ADC_Init(ADC2, &ADC_InitStructure);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);       //����ADCʱ�ӣ�ΪPCLK2��6��Ƶ����12MHz
    /* ת����ʱ���㣺T = 1 / f * (ADC_SampleTime + 12.5) */
    ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_15, 4, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_8, 5, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_14, 6, ADC_SampleTime_55Cycles5);

    /* ADC1 У׼ */
    ADC_Cmd(ADC1, ENABLE);      // ʹ�� ADC����
    ADC_ResetCalibration(ADC1);                     // ��λУ׼�Ĵ���
    while(ADC_GetResetCalibrationStatus(ADC1));     // �ȴ�У׼�Ĵ�����λ���
    ADC_StartCalibration(ADC1);               // ADCУ׼
    while(ADC_GetCalibrationStatus(ADC1));    // �ȴ�У׼���
    //ADC_SoftwareStartConvCmd(ADC1, ENABLE);   // �������ADCת��
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);    // �ⲿ����ģʽ����ADCת��

    /* ADC2 У׼ */
    ADC_Cmd(ADC2, ENABLE);      // ʹ�� ADC����
    ADC_ResetCalibration(ADC2);                     // ��λУ׼�Ĵ���
    while(ADC_GetResetCalibrationStatus(ADC2));     // �ȴ�У׼�Ĵ�����λ���
    ADC_StartCalibration(ADC2);               // ADCУ׼
    while(ADC_GetCalibrationStatus(ADC2));    // �ȴ�У׼���
    ADC_ExternalTrigConvCmd(ADC2, ENABLE);    // �ⲿ����ģʽ����ADCת��

    /* ����Ϊ��ʱ������ */
    adc1_timerTrigCfg(72, 100);   // 72000000/72/100 = 10kHz��ÿ100us����һ�βɼ�
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
static void adc1_timerTrigCfg(uint16_t prescaler, uint16_t period)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);    //ʱ��ʹ��

    //��ʱ��TIM2��ʼ��
    /* ADC�ⲿ�����Ĵ������������ⲿ�����źű�ѡΪADC�����ע��ת��ʱ��
    ֻ�����������ؿ�������ת�������Խ���ʱ������ΪPWM���ģʽ�������ص㡣*/
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;          //Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_Period = period - 1;                //�Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //����ʱ�ӷָ�
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //ѡ��ʱ��ģʽ
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = period / 2;;                    //����ռ�ձ�
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //�������
    TIM_OC2Init(TIM2, & TIM_OCInitStructure);

    TIM_Cmd(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
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
static void adc3_timerTrigCfg(uint16_t prescaler, uint16_t period)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);    //ʱ��ʹ��

    //��ʱ��TIM2��ʼ��
    /* ADC�ⲿ�����Ĵ������������ⲿ�����źű�ѡΪADC�����ע��ת��ʱ��
    ֻ�����������ؿ�������ת�������Խ���ʱ������ΪPWM���ģʽ�������ص㡣*/
    TIM_DeInit(TIM5);
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;          //Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_Period = period - 1;                //�Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //����ʱ�ӷָ�
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //ѡ��ʱ��ģʽ
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = period / 2;;                    //����ռ�ձ�
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //�������
    TIM_OC1Init(TIM5, & TIM_OCInitStructure);

    TIM_Cmd(TIM5, ENABLE);
    TIM_CtrlPWMOutputs(TIM5, ENABLE);
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
uint16_t filter_average(const uint16_t *array, uint16_t num)
{
    uint16_t i;
    uint32_t sum;
    uint16_t max, min, avg;

    for(i = 0, sum = 0, max = 0, min = 0xFFFF; i < num; i++)
    {
        sum += array[i];
        if(max < array[i])
        {
            max = array[i];
        }
        if(min > array[i])
        {
            min = array[i];
        }
    }
    sum -= max;
    sum -= min;
    avg = sum / (num - 2);

    return avg;
}

/**
 ****************************************************************************
 * @brief  Ԫ��λ�ý���
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
static void swap(uint16_t *a, uint16_t *b)
{
    uint16_t tmp;

    tmp = *a;
    *a  = *b;
    *b  = tmp;
    return;
}

/**
 ****************************************************************************
 * @brief  �����򣺣��������ѣ���������
 * @author lizdDong
 * @note   �ӶѶ��Ѹ�ж��������������֮ǰ���ٻָ��ѡ�
 * @param  None
 * @retval None
 ****************************************************************************
*/
static void max_heapify(uint16_t arr[], uint16_t start, uint16_t end)
{
    //�������ڵ�ָ����ӽڵ�ָ��
    uint16_t dad = start;
    uint16_t son = dad * 2 + 1;
    while(son <= end)
    {
        //���ӽڵ��ڷ�Χ�ڲ����Ƚ�
        if(son + 1 <= end && arr[son] < arr[son + 1])  //�ȱȽ������ӽڵ�ָ�꣬ѡ������
            son++;
        if(arr[dad] > arr[son])  //������ڵ�����ӽڵ���������ɣ�ֱ����������
            return;
        else
        {
            //���򽻻����Ӄ����ټ����ӽڵ�����ڵ���^
            swap(&arr[dad], &arr[son]);
            dad = son;
            son = dad * 2 + 1;
        }
    }
}

/**
 ****************************************************************************
 * @brief  ������
 * @author lizdDong
 * @note   ƽ��ʱ�临�Ӷ�O(n*log2n)���ռ临�Ӷ�O(1)
 * @param  None
 * @retval None
 ****************************************************************************
*/
void heapSort(uint16_t arr[], uint16_t n)
{
    int i;

    //��ʼ����i�����һ�����ڵ㿪ʼ����
    for(i = n / 2 - 1; i >= 0; i--)
        max_heapify(arr, i, n - 1);

    //�Ƚ���һ��Ԫ�غ��Ѿ��źõ�Ԫ��ǰһλ���������ٴ��µ���(�յ�����Ԫ��֮ǰ��Ԫ��)��ֱ���������
    for(i = n - 1; i > 0; i--)
    {
        swap(&arr[0], &arr[i]);
        max_heapify(arr, 0, i - 1);
    }
}

/**
 ****************************************************************************
 * @brief  ������ֵƽ���˲�������ֵ�;�ֵ�˲��Ľ���壩
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint16_t filter_medAvg(uint16_t *array, uint16_t num, uint16_t avg_num)
{
    uint16_t start_num, end_num, i;
    uint32_t sum;

    // ���ö����򷨶������������ ,��С��������
    heapSort(array, num);

    // �����м䲿�����ݵ�ƽ��ֵ
    start_num = (num - avg_num) / 2;
    end_num = start_num + avg_num;
    for(i = start_num, sum = 0; i < end_num; i++)
    {
        sum += array[i];
    }
    return sum / avg_num;
}

/**
 ****************************************************************************
 * @brief  �������ֵƽ���˲���
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint16_t filter_maxAvg(uint16_t *array, uint16_t num, uint16_t avg_num)
{
    uint16_t start_num, end_num, i, res;
    uint32_t sum;

    // ���ö����򷨶������������ ,��С��������
    heapSort(array, num);

    // ������󲿷����ݵ�ƽ��ֵ
    start_num = num - avg_num;
    end_num = num;
    for(i = start_num, sum = 0; i < end_num; i++)
    {
        sum += array[i];
    }
    res = sum / avg_num;
    return res;
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
uint16_t adc_findMax(uint16_t *array, uint16_t num)
{
    uint16_t max, i;

    for(i = 0, max = 0; i < num; i++)
    {
        if(max < array[i])
        {
            max = array[i];
        }
    }
    return max;
}


/**
 ****************************************************************************
 * @brief  ��ʼ���������˲�������
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void kalman_paramInit(kfp_t *kfp, float lastP, float nowP, float kg, float Q, float R)
{
    kfp->lastP = lastP;
    kfp->nowP = nowP;
    kfp->kg = kg;
    kfp->Q = Q;
    kfp->R = R;
    kfp->out = 0;
}

/**
 ****************************************************************************
 * @brief  �������˲���
 * @author lizdDong
 * @note   None
 * @param  kfp���������ṹ�������input����Ҫ�˲��Ĳ���ֵ�����������Ĳɼ�ֵ��
 * @retval �˲������ֵ������ֵ��
 ****************************************************************************
*/
float kalman_filter(kfp_t *kfp, float input)
{
    //Ԥ��Э����̣�kʱ��ϵͳ����Э���� = k-1ʱ�̵�ϵͳЭ���� + ��������Э����
    kfp->nowP = kfp->lastP + kfp->Q;

    //���������淽�̣����������� = kʱ��ϵͳ����Э���� / ��kʱ��ϵͳ����Э���� + �۲�����Э���
    kfp->kg = kfp->nowP / (kfp->nowP + kfp->R);

    //��������ֵ���̣�kʱ��״̬����������ֵ = ״̬������Ԥ��ֵ + ���������� * ������ֵ - ״̬������Ԥ��ֵ��
    //��Ϊ��һ�ε�Ԥ��ֵ������һ�ε����ֵ
    kfp->out = kfp->out + kfp->kg * (input - kfp->out);

    //����Э�����: ���ε�ϵͳЭ����� kfp->lastP ����һ������׼����
    kfp->lastP = (1 - kfp->kg) * kfp->nowP;
    return kfp->out;
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
uint16_t adc_getValue(uint8_t adcx, uint8_t n)
{
    uint16_t i, sam_num, value;

#if (USE_RTOS_RT_THREAD)
    rt_mutex_take(&mutex_getadc, RT_WAITING_FOREVER);
#endif
    if(adcx == DEV_ADC1)
    {
        sam_num = ADC_SAMPLING_NUM;

        for(i = 0; i < sam_num; i++)
        {
            g_aAdcValueTemp[i] = g_aAdc1Value[i][n];
        }
    }
    else if(adcx == DEV_ADC3)
    {
        sam_num = ADC3_SAMPLING_NUM;

        for(i = 0; i < sam_num; i++)
        {
            g_aAdcValueTemp[i] = g_aAdc3Value[i][n];
        }
    }

    if(1)
    {
        // ��������������ò������ֵƽ���˲���
        value = filter_maxAvg(g_aAdcValueTemp, sam_num, sam_num * 0.1);    //���ز������ֵƽ��ֵ
    }
    else
    {
        value = filter_medAvg(g_aAdcValueTemp, sam_num, sam_num * 0.6);    //�����м䲿�����ݵľ�ֵ
    }

#if (USE_RTOS_RT_THREAD)
    rt_mutex_release(&mutex_getadc);
#endif

    return value;
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
uint16_t adc_getVrefValue(void)
{
    return adc_getValue(DEV_ADC1, 0);   //�����ADCת��˳�������й�
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
float adc_getCpuTemp(void)
{
    uint16_t adc_tmp;
    //uint16_t adc_vref;

    adc_tmp = adc_getValue(DEV_ADC1, 1);    //�����ADCת��˳�������й�
    //adc_vref = adc_getVrefValue();
    //return (1.43 - (adc_tmp * ADC_VREF / adc_vref)) * 1000 / 4.3 + 25;
    return (1.43 - (adc_tmp * 3.3 / 4095)) * 1000 / 4.3 + 25;
}


extern struct rt_semaphore adc_finish_sem;

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void  DMA1_Channel1_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        TIM_Cmd(TIM2, DISABLE);
        //g_fAdcConvOver = 1;
        rt_sem_release(&adc_finish_sem);
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
void  DMA2_Channel4_5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA2_IT_TC5) != RESET)
    {
        DMA_ClearITPendingBit(DMA2_IT_TC5);
    }
}

/****************************** End of file ***********************************/
