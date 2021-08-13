/**
  ******************************************************************************
  * @file    dcom.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-5-5
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "diwdg.h"

/**
 ****************************************************************************
 * @brief  初始化独立看门狗
 * @author lizdDong
 * @note   时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).
 * @param  prer:分频数:0~7(只有低 3 位有效!)，分频因子=4*2^prer.但最大值只能是 256
 * @param  rlr:重装载寄存器值:低 11 位有效.
 * @retval None
 ****************************************************************************
*/
void iwdg_init(uint8_t prer, uint16_t rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); /*使能对寄存器IWDG_PR和IWDG_RLR的写操作*/
    IWDG_SetPrescaler(prer);                      /*设置IWDG预分频值:设置IWDG预分频值*/
    IWDG_SetReload(rlr);                          /*设置IWDG重装载值*/
    IWDG_ReloadCounter();                         /*按照IWDG重装载寄存器的值重装载IWDG计数器*/
    IWDG_Enable();                                /*使能IWDG*/
}

/**
 ****************************************************************************
 * @brief  喂独立看门狗
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void iwdg_feed(void)
{
    IWDG_ReloadCounter();    /*reload*/
}


/*********************************** End of file *****************************************/
