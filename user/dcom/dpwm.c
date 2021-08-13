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

    /* 使能GPIO, AFIO, TIM3时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* 配置TIM3完全重映像, TIM3的四个通道CH1, CH2, CH3, CH4分别对应PC6, PC7, PC8, PC9 */
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

    /* PWM输出引脚初始化 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 |  GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 初始化TIM3 */
    TIM_DeInit(TIM3);
    //TIM_InternalClockConfig(TIM3);    //采用内部时钟给TIM2提供时钟源
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;        //预分频值
    TIM_TimeBaseStructure.TIM_Period = period - 1;              //自动重装载寄存器
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* 初始化TIM3_CH1 PWM */
    TIM_OCStructInit(&TIM_OCInitStructure);                         //设置缺省值
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //TIM脉冲宽度调制模式
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //输出极性:TIM输出比较极性高
    //TIM_OCInitStructure.TIM_Pulse = period / 2;                     //设置比较寄存器预加载值
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器

    /* 初始化TIM3_CH2 PWM */
    TIM_OCStructInit(&TIM_OCInitStructure);                         //设置缺省值
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //TIM脉冲宽度调制模式
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //输出极性:TIM输出比较极性高
    //TIM_OCInitStructure.TIM_Pulse = period / 2;                     //设置比较寄存器预加载值
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器

    TIM_Cmd(TIM3, ENABLE);  //使能TIM3
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

