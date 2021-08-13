/**
  ******************************************************************************
  * @file    dpwm.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-4-13
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "dpwm.h"

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  frequency = Tim3Clock / prescaler / period
 * @retval None
 ****************************************************************************
*/
void pwm_TIM3_init(uint16_t prescaler, uint16_t period)
{
    GPIO_InitTypeDef          GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    TIM_OCInitTypeDef         TIM_OCInitStructure;

    /* ʹ��GPIO, AFIO, TIM3ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* ����TIM3��ȫ��ӳ��, TIM3���ĸ�ͨ��CH1, CH2, CH3, CH4�ֱ��ӦPC6, PC7, PC8, PC9 */
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

    /* PWM������ų�ʼ�� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 |  GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* ��ʼ��TIM3 */
    TIM_DeInit(TIM3);
    //TIM_InternalClockConfig(TIM3);    //�����ڲ�ʱ�Ӹ�TIM2�ṩʱ��Դ
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;        //Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_Period = period - 1;              //�Զ���װ�ؼĴ���
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָ�
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* ��ʼ��TIM3_CH1 PWM */
    TIM_OCStructInit(&TIM_OCInitStructure);                         //����ȱʡֵ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //TIM�����ȵ���ģʽ
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //�������:TIM����Ƚϼ��Ը�
    //TIM_OCInitStructure.TIM_Pulse = period / 2;                     //���ñȽϼĴ���Ԥ����ֵ
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���

    /* ��ʼ��TIM3_CH2 PWM */
    TIM_OCStructInit(&TIM_OCInitStructure);                         //����ȱʡֵ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //TIM�����ȵ���ģʽ
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //�������:TIM����Ƚϼ��Ը�
    //TIM_OCInitStructure.TIM_Pulse = period / 2;                     //���ñȽϼĴ���Ԥ����ֵ
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���

    TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
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
void pwm_TIM3_setDutyCycle(uint8_t channel, uint16_t duty_cycle)
{
    if(channel == 1)
    {
        TIM_SetCompare1(TIM3, duty_cycle);
    }
    else if(channel == 2)
    {
        TIM_SetCompare2(TIM3, duty_cycle);
    }
    else if(channel == 3)
    {
        TIM_SetCompare3(TIM3, duty_cycle);
    }
    else if(channel == 4)
    {
        TIM_SetCompare4(TIM3, duty_cycle);
    }
}


/****************************** End of file ***********************************/

