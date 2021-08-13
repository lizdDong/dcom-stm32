/**
  ******************************************************************************
  * @file    ddac.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-4-13
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "ddac.h"


uint16_t g_aWave1[DAC_WAVE1_BUFF];
uint16_t g_aWave2[DAC_WAVE2_BUFF];

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void dac_ch1_init_simple(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     //ʹ��PORTAͨ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);       //ʹ��DACͨ��ʱ��

    /* ����DAC1 */
    GPIO_SetBits(GPIOA, GPIO_Pin_4);                    //PA.4 �����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       //ģ������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           //�˿�����
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    DAC_InitType.DAC_Trigger = DAC_Trigger_None;                         //��ʹ�ô������� TEN1=0
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;           //��ʹ�ò��η���
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; //���Ρ���ֵ����
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;            //DAC1�������ر� BOFF1=1
    DAC_Init(DAC_Channel_1, &DAC_InitType);      //��ʼ��DACͨ��1

    DAC_Cmd(DAC_Channel_1, ENABLE);              //ʹ��DAC1

    DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
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
void dac_ch2_init_simple(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitType;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     //ʹ��PORTAͨ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);       //ʹ��DACͨ��ʱ��

    /* ����DAC2 */
    GPIO_SetBits(GPIOA, GPIO_Pin_5);                    //PA.5 �����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       //ģ������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;           //�˿�����
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    DAC_InitType.DAC_Trigger = DAC_Trigger_None;                         //��ʹ�ô������� TEN1=0
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;           //��ʹ�ò��η���
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; //���Ρ���ֵ����
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;            //DAC2�������ر� BOFF1=1
    DAC_Init(DAC_Channel_2, &DAC_InitType);      //��ʼ��DACͨ��2

    DAC_Cmd(DAC_Channel_2, ENABLE);              //ʹ��DAC2

    DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
}

/**
 ****************************************************************************
 * @brief  ����ͨ��1�����ѹ
 * @author lizdDong
 * @note   None
 * @param  vol:0~3300,����0~3.3V
 * @retval None
 ****************************************************************************
*/
void dac_ch1_setVoltage(uint16_t vol)
{
    float temp = vol;
    temp /= 1000;
    temp = temp * 4095 / 3.3;
    DAC_SetChannel1Data(DAC_Align_12b_R, temp);//12λ�Ҷ������ݸ�ʽ����DACֵ
}

/**
 ****************************************************************************
 * @brief  ����ͨ��2�����ѹ
 * @author lizdDong
 * @note   None
 * @param  vol:0~3300,����0~3.3V
 * @retval None
 ****************************************************************************
*/
void dac_ch2_setVoltage(uint16_t vol)
{
    float temp = vol;
    temp /= 1000;
    temp = temp * 4095 / 3.3;
    DAC_SetChannel2Data(DAC_Align_12b_R, temp);//12λ�Ҷ������ݸ�ʽ����DACֵ
}


/**
 ****************************************************************************
 * @brief  DACͨ��1�����������
 * @author lizdDong
 * @note   ʹ����DMA2_Channel3 �� TIM6
 * @param  None
 * @retval None
 ****************************************************************************
*/
void dac_ch1_init_waveOut(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    /* ����DAC1 GPIO�˿� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ����DAC1 DMA */
    DMA_DeInit(DMA2_Channel3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R1;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_aWave1;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = DAC_WAVE1_BUFF;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel3, ENABLE);

    /* ����DAC1 ͨ��*/
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_DMACmd(DAC_Channel_1, ENABLE);
    DAC_Cmd(DAC_Channel_1, ENABLE);
}

/**
 ****************************************************************************
 * @brief  DACͨ��2�����������
 * @author lizdDong
 * @note   ʹ����DMA2_Channel4 �� TIM7
 * @param  None
 * @retval None
 ****************************************************************************
*/
void dac_ch2_init_waveOut(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

    /* ����DAC2 GPIO�˿� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ����DAC2 DMA */
    DMA_DeInit(DMA2_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R2;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_aWave2;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = DAC_WAVE2_BUFF;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel4, ENABLE);

    /* ����DAC2 ͨ��*/
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);
    DAC_DMACmd(DAC_Channel_2, ENABLE);
    DAC_Cmd(DAC_Channel_2, ENABLE);
}

/**
 ****************************************************************************
 * @brief  �������Ҳ�����
 * @author lizdDong
 * @note   None
 * @param  offset_x��X��ƫ��������ʼ��λ����offset_y��Y��ƫ������amplitude�����
 * @retval None
 ****************************************************************************
*/
#include <math.h>
void dac_makeSinWave(uint16_t *buff, uint16_t size, int16_t offset_x, int16_t offset_y, uint16_t amplitude)
{
   uint16_t i;
   int16_t v;

   for(i = 0; i < size; i++)
   {
       v = offset_y + amplitude * sin(2 * 3.141592654f * (i + offset_x) / size);
       if(v >= 0)
           buff[i] = v;
       else
           buff[i] = 0;
   }
}

/**
 ****************************************************************************
 * @brief  ���ɾ��β�����
 * @author lizdDong
 * @note   None
 * @param  offset_y��Y��ƫ������amplitude�������duty��ռ�ձ�
 * @retval None
 ****************************************************************************
*/
void dac_makeRectWave(uint16_t *buff, uint16_t size, int16_t offset_y, uint16_t amplitude, uint8_t duty)
{
   uint16_t i;
   int16_t v;

   for(i = 0; i < size * duty / 100; i++)
   {
        v = offset_y + amplitude;
       if(v >= 0)
           buff[i] = v;
       else
           buff[i] = 0;
   }
   for(i = size * duty / 100; i < size; i++)
   {
        v = offset_y;
       if(v >= 0)
           buff[i] = v;
       else
           buff[i] = 0;
   }
}

/**
 ****************************************************************************
 * @brief  DAC CH1�������
 * @author lizdDong
 * @note   None
 * @param  freq��Ƶ��
 * @retval None
 ****************************************************************************
*/
void dac_ch1_waveOut(uint32_t freq)
{
    TIM_Cmd(TIM6, DISABLE);
    
    TIM_PrescalerConfig(TIM6, 0, TIM_PSCReloadMode_Update);
    TIM_SetAutoreload(TIM6, 72000000 / DAC_WAVE1_BUFF / freq);
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
    
    TIM_Cmd(TIM6, ENABLE);
}

/**
 ****************************************************************************
 * @brief  DAC CH2�������
 * @author lizdDong
 * @note   None
 * @param  freq��Ƶ��
 * @retval None
 ****************************************************************************
*/
void dac_ch2_waveOut(uint32_t freq)
{
    TIM_Cmd(TIM7, DISABLE);
    
    TIM_PrescalerConfig(TIM7, 0, TIM_PSCReloadMode_Update);
    TIM_SetAutoreload(TIM7, 72000000 / DAC_WAVE2_BUFF / freq);
    TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);
    
    TIM_Cmd(TIM7, ENABLE);
}


/****************************** End of file ***********************************/

