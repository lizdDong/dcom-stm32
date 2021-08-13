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

// 多任务中互斥访问
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
 * @brief  ADC1初始化，规则转换，DMA传输，可选定时器触发或软件触发
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

    /* 使能 ADC1 GPIO DMA 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* 配置ADC1 GPIO端口 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 配置ADC1 DMA */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;       // 外设数据地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_aAdc1Value;       // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                    // 传输方向 外设->内存
    DMA_InitStructure.DMA_BufferSize = ADC_SAMPLING_NUM * ADC_CHANNEL_NUM;        // 内存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              // 外设自增模式（单外设禁止）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       // 配置内存地址自增模式（多内存允许）
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   // 外设数据宽度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;           // 内存数据宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;       // 循环传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;   // 通道优先级为高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;          // 关闭从内存到内存模式
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);       // 使能 DMA通道
#if(1)
    {
        // DMA传输完成中断配置
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);   // 使能DMA中断
    }
#endif

    /* 配置ADC1 通道 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;    // 独立ADC模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;          // 多通道扫描方式（多通道时打开）
    //ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // 打开连续转换模式（软件启动转换时打开）
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   // 关闭连续转换模式（外部触发启动转换时关闭）
    //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // 不使用外部触发模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;   // 使用外部触发模式（触发事件为 T2_CC2）
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      // 采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNEL_NUM;       // 要转换的通道数目
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_DMACmd(ADC1, ENABLE);               // 使能 ADC1的 DMA请求
    //ADC_TempSensorVrefintCmd(ENABLE);     // 使能内部温度传感器和参考值ADC

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);       //配置ADC时钟，为PCLK2的6分频，即12MHz
    /* 转换耗时计算：T = 1 / f * (ADC_SampleTime + 12.5) */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);

    /* ADC1 校准 */
    ADC_Cmd(ADC1, ENABLE);      // 使能 ADC外设
    ADC_ResetCalibration(ADC1);                     // 复位校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC1));     // 等待校准寄存器复位完成
    ADC_StartCalibration(ADC1);               // ADC校准
    while(ADC_GetCalibrationStatus(ADC1));    // 等待校准完成
    //ADC_SoftwareStartConvCmd(ADC1, ENABLE);   // 软件启动ADC转换
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);    // 外部触发模式启动ADC转换

    /* 配置为外部定时器触发 */
    adc1_timerTrigCfg(72, 100);   // 72000000/72/100 = 10kHz，每100us触发一次采集

#if (USE_RTOS_RT_THREAD)
    rt_mutex_init(&mutex_getadc, "getAdc", RT_IPC_FLAG_FIFO);
#endif
}

/**
 ****************************************************************************
 * @brief  ADC3初始化，规则转换，DMA传输，可选定时器触发或软件触发
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

    /* 使能 ADC3 GPIO DMA 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    /* 配置ADC3 GPIO端口 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 配置ADC3 DMA */
    DMA_DeInit(DMA2_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;         // 外设数据地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_aAdc3Value;         // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                      // 传输方向 外设->内存
    DMA_InitStructure.DMA_BufferSize = ADC3_SAMPLING_NUM * ADC3_CHANNEL_NUM;      // 内存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              // 外设自增模式（单外设禁止）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       // 配置内存地址自增模式（多内存允许）
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   // 外设数据宽度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;           // 内存数据宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;         // 循环传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;     // 通道优先级为高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;            // 关闭从内存到内存模式
    DMA_Init(DMA2_Channel5, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel5, ENABLE);   // 使能DMA

    //以下为DMA传输完成中断配置
#if(0)
    {
        NVIC_InitTypeDef NVIC_InitStructure;    /* 中断结构体在 misc.h 中定义 */

        NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        DMA_ITConfig(DMA2_Channel5, DMA_IT_TC, ENABLE);
    }
#endif

    /* 配置ADC3 通道 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;    // 独立ADC模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;          // 多通道扫描方式（多通道时打开）
    //ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // 打开连续转换模式（软件启动转换时打开）
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   // 关闭连续转换模式（外部触发启动转换时关闭）
    //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // 不使用外部触发模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T5_CC1;   // 使用外部触发模式（触发事件为 T5_CC1）
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      // 采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = ADC3_CHANNEL_NUM;      // 要转换的通道数目
    ADC_Init(ADC3, &ADC_InitStructure);
    ADC_DMACmd(ADC3, ENABLE);               // 使能 ADC1的 DMA请求

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);       //配置ADC时钟，为PCLK2的6分频，即12MHz
    /* 转换耗时计算：T = 1 / f * (ADC_SampleTime + 12.5) */
    ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 2, ADC_SampleTime_239Cycles5);

    /* ADC3 校准 */
    ADC_Cmd(ADC3, ENABLE);      // 使能 ADC外设
    ADC_ResetCalibration(ADC3);                     // 复位校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC3));     // 等待校准寄存器复位完成
    ADC_StartCalibration(ADC3);               // ADC校准
    while(ADC_GetCalibrationStatus(ADC3));    // 等待校准完成
    //ADC_SoftwareStartConvCmd(ADC3, ENABLE);   // 软件启动ADC转换
    ADC_ExternalTrigConvCmd(ADC3, ENABLE);    // 外部触发模式启动ADC转换

    /* 配置为定时器触发 */
    adc3_timerTrigCfg(72, 100);   // 72000000/72/100 = 10kHz，每100us触发一次采集

#if (USE_RTOS_RT_THREAD)
    //rt_mutex_init(&mutex_getadc, "getAdc", RT_IPC_FLAG_FIFO);
#endif
}
/**
 ****************************************************************************
 * @brief  ADC1和ADC2的双ADC模式初始化
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

    /* 使能 ADC1 ADC2 GPIO DMA 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* 配置 GPIO端口 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |  GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 配置 DMA */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;       // 外设数据地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_aAdcDualConvValue;       // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                    // 传输方向 外设->内存
    DMA_InitStructure.DMA_BufferSize = ADC_DUALMODE_SAMPLING_NUM * ADC_DUALMODE_CHANNEL_NUM;        // 内存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              // 外设自增模式（单外设禁止）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       // 配置内存地址自增模式（多内存允许）
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;   // 外设数据宽度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;           // 内存数据宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;       // 循环传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;   // 通道优先级为高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;          // 关闭从内存到内存模式
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);       // 使能 DMA通道

#if(1)
    {
        // DMA传输完成中断配置
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);   // 使能DMA中断
    }
#endif

    /* 注意： 在双 ADC 模式里，当转换配置成由外部事件触发时，用户必须将其设置成仅触发主 ADC ，从ADC 设置
              成软件触发，这样可以防止意外的触发从转换。但是，主和从 ADC 的外部触发必须同时被激活 */

    /* 配置ADC1 通道 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;      // ADC1和ADC2工作在同步规则模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;          // 多通道扫描方式（多通道时打开）
    //ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // 打开连续转换模式（软件启动转换时打开）
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   // 关闭连续转换模式（外部触发启动转换时关闭）
    //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // 不使用外部触发模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;   // 使用外部触发模式（触发事件为 T2_CC2）
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      // 采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = ADC_DUALMODE_CHANNEL_NUM;       // 要转换的通道数目
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_DMACmd(ADC1, ENABLE);               // 使能 ADC1的 DMA请求
    //ADC_TempSensorVrefintCmd(ENABLE);     // 使能内部温度传感器和参考值ADC

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);       //配置ADC时钟，为PCLK2的6分频，即12MHz
    /* 转换耗时计算：T = 1 / f * (ADC_SampleTime + 12.5) */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 5, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 6, ADC_SampleTime_55Cycles5);

    /* 配置ADC2 通道 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;      // ADC1和ADC2工作在同步规则模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;          // 多通道扫描方式（多通道时打开）
    //ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // 打开连续转换模式（软件启动转换时打开）
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   // 关闭连续转换模式（外部触发启动转换时关闭）
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // 双 ADC 模式下，从 ADC 必须设置成软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;      // 采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = ADC_DUALMODE_CHANNEL_NUM;       // 要转换的通道数目
    ADC_Init(ADC2, &ADC_InitStructure);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);       //配置ADC时钟，为PCLK2的6分频，即12MHz
    /* 转换耗时计算：T = 1 / f * (ADC_SampleTime + 12.5) */
    ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_15, 4, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_8, 5, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_14, 6, ADC_SampleTime_55Cycles5);

    /* ADC1 校准 */
    ADC_Cmd(ADC1, ENABLE);      // 使能 ADC外设
    ADC_ResetCalibration(ADC1);                     // 复位校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC1));     // 等待校准寄存器复位完成
    ADC_StartCalibration(ADC1);               // ADC校准
    while(ADC_GetCalibrationStatus(ADC1));    // 等待校准完成
    //ADC_SoftwareStartConvCmd(ADC1, ENABLE);   // 软件启动ADC转换
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);    // 外部触发模式启动ADC转换

    /* ADC2 校准 */
    ADC_Cmd(ADC2, ENABLE);      // 使能 ADC外设
    ADC_ResetCalibration(ADC2);                     // 复位校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC2));     // 等待校准寄存器复位完成
    ADC_StartCalibration(ADC2);               // ADC校准
    while(ADC_GetCalibrationStatus(ADC2));    // 等待校准完成
    ADC_ExternalTrigConvCmd(ADC2, ENABLE);    // 外部触发模式启动ADC转换

    /* 配置为定时器触发 */
    adc1_timerTrigCfg(72, 100);   // 72000000/72/100 = 10kHz，每100us触发一次采集
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

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);    //时钟使能

    //定时器TIM2初始化
    /* ADC外部触发的触发条件：当外部触发信号被选为ADC规则或注入转换时，
    只有它的上升沿可以启动转换。所以将定时器配置为PWM输出模式，这是重点。*/
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;          //预分频值
    TIM_TimeBaseStructure.TIM_Period = period - 1;                //自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //设置时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //选择定时器模式
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = period / 2;;                    //设置占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //输出极性
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

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);    //时钟使能

    //定时器TIM2初始化
    /* ADC外部触发的触发条件：当外部触发信号被选为ADC规则或注入转换时，
    只有它的上升沿可以启动转换。所以将定时器配置为PWM输出模式，这是重点。*/
    TIM_DeInit(TIM5);
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;          //预分频值
    TIM_TimeBaseStructure.TIM_Period = period - 1;                //自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //设置时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM向上计数模式
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //选择定时器模式
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = period / 2;;                    //设置占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //输出极性
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
 * @brief  元素位置交换
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
 * @brief  堆排序：（建立最大堆，有序区）
 * @author lizdDong
 * @note   从堆顶把根卸出来放在有序区之前，再恢复堆。
 * @param  None
 * @retval None
 ****************************************************************************
*/
static void max_heapify(uint16_t arr[], uint16_t start, uint16_t end)
{
    //建立父节点指标和子节点指标
    uint16_t dad = start;
    uint16_t son = dad * 2 + 1;
    while(son <= end)
    {
        //若子节点在范围内才做比较
        if(son + 1 <= end && arr[son] < arr[son + 1])  //先比较两个子节点指标，选择最大的
            son++;
        if(arr[dad] > arr[son])  //如果父节点大于子节点代表调整完成，直接跳出函数
            return;
        else
        {
            //否则交换父子內容再继续子节点与孙节点比較
            swap(&arr[dad], &arr[son]);
            dad = son;
            son = dad * 2 + 1;
        }
    }
}

/**
 ****************************************************************************
 * @brief  堆排序
 * @author lizdDong
 * @note   平均时间复杂度O(n*log2n)，空间复杂度O(1)
 * @param  None
 * @retval None
 ****************************************************************************
*/
void heapSort(uint16_t arr[], uint16_t n)
{
    int i;

    //初始化，i从最后一个父节点开始调整
    for(i = n / 2 - 1; i >= 0; i--)
        max_heapify(arr, i, n - 1);

    //先将第一个元素和已经排好的元素前一位做交换，再从新调整(刚调整的元素之前的元素)，直到排序完成
    for(i = n - 1; i > 0; i--)
    {
        swap(&arr[0], &arr[i]);
        max_heapify(arr, 0, i - 1);
    }
}

/**
 ****************************************************************************
 * @brief  部分中值平均滤波法（中值和均值滤波的结合体）
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

    // 采用堆排序法对数组进行排序 ,从小到大排序
    heapSort(array, num);

    // 返回中间部分数据的平均值
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
 * @brief  部分最大值平均滤波法
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

    // 采用堆排序法对数组进行排序 ,从小到大排序
    heapSort(array, num);

    // 返回最大部分数据的平均值
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
 * @brief  初始化卡尔曼滤波器参数
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
 * @brief  卡尔曼滤波器
 * @author lizdDong
 * @note   None
 * @param  kfp：卡尔曼结构体参数。input：需要滤波的测量值（即传感器的采集值）
 * @retval 滤波后的数值（最优值）
 ****************************************************************************
*/
float kalman_filter(kfp_t *kfp, float input)
{
    //预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差
    kfp->nowP = kfp->lastP + kfp->Q;

    //卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）
    kfp->kg = kfp->nowP / (kfp->nowP + kfp->R);

    //更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）
    //因为这一次的预测值就是上一次的输出值
    kfp->out = kfp->out + kfp->kg * (input - kfp->out);

    //更新协方差方程: 本次的系统协方差付给 kfp->lastP 威下一次运算准备。
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
        // 交流输入电流采用部分最大值平均滤波法
        value = filter_maxAvg(g_aAdcValueTemp, sam_num, sam_num * 0.1);    //返回部分最大值平均值
    }
    else
    {
        value = filter_medAvg(g_aAdcValueTemp, sam_num, sam_num * 0.6);    //返回中间部分数据的均值
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
    return adc_getValue(DEV_ADC1, 0);   //序号与ADC转换顺序配置有关
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

    adc_tmp = adc_getValue(DEV_ADC1, 1);    //序号与ADC转换顺序配置有关
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
