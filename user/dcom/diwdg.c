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
 * @brief  ��ʼ���������Ź�
 * @author lizdDong
 * @note   ʱ�����(���):Tout=((4*2^prer)*rlr)/40 (ms).
 * @param  prer:��Ƶ��:0~7(ֻ�е� 3 λ��Ч!)����Ƶ����=4*2^prer.�����ֵֻ���� 256
 * @param  rlr:��װ�ؼĴ���ֵ:�� 11 λ��Ч.
 * @retval None
 ****************************************************************************
*/
void iwdg_init(uint8_t prer, uint16_t rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); /*ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����*/
    IWDG_SetPrescaler(prer);                      /*����IWDGԤ��Ƶֵ:����IWDGԤ��Ƶֵ*/
    IWDG_SetReload(rlr);                          /*����IWDG��װ��ֵ*/
    IWDG_ReloadCounter();                         /*����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������*/
    IWDG_Enable();                                /*ʹ��IWDG*/
}

/**
 ****************************************************************************
 * @brief  ι�������Ź�
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
