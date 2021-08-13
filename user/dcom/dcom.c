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
#include "dinc.h"
#include <math.h>

static volatile uint32_t g_TickCount = 0;


/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
int str_to_int(char *str)
{
	uint8_t flag;
	char *p;
	int value;

	p = str;
	if (*p == '-')
	{
		flag = 1;	/* 负数 */
		p++;
	}
	else
	{
		flag = 0;
	}

	value = 0;
	while(*p != '\0')
    {
		if ((*p >= '0') && (*p <= '9'))
		{
			value = value * 10 + (*p - '0');
			p++;
		}
		else
		{
			break;
		}
    }
    
	if (flag == 1)
	{
		return -value;
	}
    
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
float str_to_float(char *str)
{
	uint8_t flag, dec = 0;
	char *p;
	float value;

	p = str;
	if (*p == '-')
	{
		flag = 1;	/* 负数 */
		p++;
	}
	else
	{
		flag = 0;
	}

	value = 0;
	while(*p != '\0')
    {
        if(*p == '.')
        {
            if(dec > 0)
                break;
            dec++;
        }
		else if ((*p >= '0') && (*p <= '9'))
		{
			value = value * 10 + (*p - '0');
            if(dec > 0)
                dec++;
		}
		else
		{
			break;
		}
        p++;
        
    }
    
    if(dec > 0)
        value /= pow(10, dec - 1);
    
	if (flag == 1)
	{
		return -value;
	}
    
	return value;
}


/**
 ****************************************************************************
 * @brief  四舍五入
 * @author lizdDong
 * @note   将小数点后1位四舍五入后取整
 * @param  None
 * @retval None
 ****************************************************************************
*/
int rounding_off(float v)
{
    if(v >= 0)
    {
        return (int)(v + 0.5);
    }
    return (int)(v - 0.5);
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
void systick_init(void)
{
    SysTick_Config(SystemCoreClock / 1000);
    
    /* set Priority for Systick Interrupt to highest, default Priority numer is 15 */
    //NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 15);
}

/**
 ****************************************************************************
 * @brief  临界处理
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint32_t enter_critical(void)
{
    uint32_t regPriMask;
    
    regPriMask = __get_PRIMASK();
    __disable_irq();
    return regPriMask;
}

void exit_critical(uint32_t priMask)
{
    __set_PRIMASK(priMask);
}


#if !USE_RTOS_RT_THREAD 
/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void HardFault_Handler(void)
{
  uint8_t count = 0;
  
  while(count == 0)
  {
  }
}

#if LOG_TIME_OUT
#include "drtc.h"
#endif


/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void SysTick_Handler(void)
{
#if LOG_TIME_OUT
    g_sCalendar.ms++;
#endif
    systick_callback();
}
#endif


/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void systick_callback(void)
{
    extern void led_blinkHandle(void);
    extern void io_inputFilterHandle(void);
    
    #ifdef _DIO_H_
    io_inputFilterHandle();
    #endif
    
    #ifdef _DLED_H_
    led_blinkHandle();
    #endif
    
    g_TickCount++;
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
uint32_t systick_getCount(void)
{
    uint32_t tick;
    tick = g_TickCount;
    return tick;
}


/*********************************** End of file *****************************************/
