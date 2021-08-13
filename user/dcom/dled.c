/**
  ******************************************************************************
  * @file    dled.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-3-10
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "dled.h"


typedef struct
{
    uint32_t onMs;
    uint32_t offMs;
    uint32_t runMs;
    uint32_t times;
    uint8_t status;
    uint8_t runFlag;
} led_t;

static led_t g_asLed[LED_COUNTS];

static void led_ctrl(uint8_t n, uint8_t value);


/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
static void led_io_cfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 打开GPIO时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    led_off(LED_ALL);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
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
static void led_ctrl(uint8_t n, uint8_t value)
{
    if(n == 0)        GPIO_WriteBit(GPIOA, GPIO_Pin_15, (BitAction)value);
    else if(n == 1)   GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)value);
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
int led_init(void)
{
    led_io_cfg();

    return 0;
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
void led_on(uint8_t n)
{
    if(n == LED_ALL)
    {
        uint8_t i;
        for(i = 0; i < LED_COUNTS; i++)
        {
            g_asLed[i].runFlag = 0;
            led_ctrl(i, LED_ON);
        }
        return;
    }

    g_asLed[n].runFlag = 0;
    led_ctrl(n, LED_ON);
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
void led_off(uint8_t n)
{
    if(n == LED_ALL)
    {
        uint8_t i;
        for(i = 0; i < LED_COUNTS; i++)
        {
            g_asLed[i].runFlag = 0;
            led_ctrl(i, LED_OFF);
        }
        return;
    }
    g_asLed[n].runFlag = 0;
    led_ctrl(n, LED_OFF);
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
void led_blink(uint8_t n, uint32_t on_ms, uint32_t off_ms, uint32_t times)
{
    if(n >= LED_COUNTS)
    {
        return;
    }

    if( g_asLed[n].onMs == on_ms && \
        g_asLed[n].offMs == off_ms  && \
        g_asLed[n].times == times)
    {
        // 相同的配置将不会重新执行配置
        return;
    }
    
    g_asLed[n].onMs = on_ms;
    g_asLed[n].offMs = off_ms;
    g_asLed[n].runMs = 0;
    if(times > 0)
    {
        g_asLed[n].times = times;
        g_asLed[n].runFlag = 1;
        g_asLed[n].status = LED_ON;
        led_ctrl(n, LED_ON);
    }
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   需要被周期性（1ms/1次）的调用
 * @param  None
 * @retval None
 ****************************************************************************
 */
void led_blinkHandle(void)
{
    uint8_t n;

    for(n = 0; n < LED_COUNTS; ++n)
    {
        if(g_asLed[n].runFlag)
        {
            if(g_asLed[n].status == LED_ON)
            {
                if(g_asLed[n].runMs >= g_asLed[n].onMs)
                {
                    g_asLed[n].status = LED_OFF;
                    g_asLed[n].runMs = 0;
                }
            }
            else
            {
                if(g_asLed[n].runMs >= g_asLed[n].offMs)
                {
                    if(g_asLed[n].times != LED_ALWAYS)
                    {
                        if(g_asLed[n].times > 0)
                        {
                            g_asLed[n].times--;
                        }
                    }
                    
                    if(g_asLed[n].times == 0)
                    {
                        g_asLed[n].runFlag = 0;
                    }
                    else
                    {
                        g_asLed[n].status = LED_ON;
                        g_asLed[n].runMs = 0;
                    }
                }
            }
            g_asLed[n].runMs++;
            led_ctrl(n, g_asLed[n].status);
        }
    }
}


/*********************************** End of file *****************************************/
