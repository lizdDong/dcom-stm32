/**
  ******************************************************************************
  * @file    drtc.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-5-5
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "drtc.h"


calendar_t g_sCalendar;    // 日历结构体

/* 平年的每月天数表 */
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static uint8_t rtc_calcWeek(uint16_t _year, uint8_t _mon, uint8_t _day);


/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void rtc_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* 我们在BKP的后备寄存器1中，存了一个特殊字符0xA5A5, 第一次上电或后备电源掉电后，该寄存器数据丢失，
    表明RTC数据丢失，需要重新配置 */
    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        /* PWR时钟（电源控制）与BKP时钟（RTC后备寄存器）使能 */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
        /* Allow access to BKP Domain */ /*使能RTC和后备寄存器访问 */
        PWR_BackupAccessCmd(ENABLE);
        /* Reset Backup Domain */  /* 将外设BKP的全部寄存器重设为缺省值 */
        BKP_DeInit();
        /* Enable LSE */
        RCC_LSEConfig(RCC_LSE_ON);

        /* Wait till LSE is ready */ /* 等待外部晶振震荡稳定输出 */
        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

        /* Select LSE as RTC Clock Source */ /*使用外部32.768KHz晶振作为RTC时钟 */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        /* Enable RTC Clock */
        RCC_RTCCLKCmd(ENABLE);
        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        RTC_EnterConfigMode();

        /* Set RTC prescaler: set RTC period to 1sec */
        /* 32.768KHz晶振预分频值是32767,如果对精度要求很高可以修改此分频值来校准晶振 */
        RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        rtc_set(2015, 8, 8, 0, 0, 0);//默认时间

        /* 配置完成后，向后备寄存器中写特殊字符0xA5A5 */
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
    else
    {
        /* 若后备寄存器没有掉电，则无需重新配置RTC */
        if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
            /* 上电复位 */
        }
        else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
            /* 外部RST管脚复位 */
        }
        /* 清除RCC中复位标志 */
        RCC_ClearFlag();

        //虽然RTC模块不需要重新配置，且掉电后依靠后备电池依然运行
        //但是每次上电后，还是要使能RTCCLK
        RCC_RTCCLKCmd(ENABLE);
        //等待RTC时钟与APB1时钟同步
        RTC_WaitForSynchro();
        //等待操作完成
        RTC_WaitForLastTask();
    }

    /* 以下使能秒中断 */
    #if(1)
    {
        NVIC_InitTypeDef NVIC_InitStructure;

        NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        //使能秒中断
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
    }
    #endif

    rtc_get();//更新时间
    
    return;
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
static uint8_t is_leap_year(uint16_t year)
{
    if(year % 4 == 0)  /* 必须能被4整除 */
    {
        if(year % 100 == 0)
        {
            if(year % 400 == 0)
            {
                return 1;   /* 如果以00结尾,还要能被400整除 */
            }
            else
            {
                return 0;
            }

        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

/**
 ****************************************************************************
 * @brief  设置时钟
 * @author lizdDong
 * @note   以1970年1月1日为基准，1970~2099年为合法年份
 * @param  None
 * @retval 0，成功；其它：错误
 ****************************************************************************
*/
int rtc_set(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint16_t t;
    uint32_t seccount = 0;

    if(year < 2000 || year > 2099)
    {
        return -1;  /* _year范围1970-2099，此处设置范围为2000-2099 */
    }

    for(t = 1970; t < year; t++)    /* 把所有年份的秒钟相加 */
    {
        if(is_leap_year(t))         /* 判断是否为闰年 */
        {
            seccount += 31622400;   /* 闰年的秒钟数 */
        }
        else
        {
            seccount += 31536000;   /* 平年的秒钟数 */
        }
    }

    mon -= 1;

    for(t = 0; t < mon; t++)          /* 把前面月份的秒钟数相加 */
    {
        seccount += (uint32_t)mon_table[t] * 86400; /* 月份秒钟数相加 */

        if(is_leap_year(year) && t == 1)
        {
            seccount += 86400;  /* 闰年2月份增加一天的秒钟数 */
        }
    }

    seccount += (uint32_t)(day - 1) * 86400;    /* 把前面日期的秒钟数相加 */
    seccount += (uint32_t)hour * 3600;      /* 小时秒钟数 */
    seccount += (uint32_t)min * 60; /* 分钟秒钟数 */
    seccount += sec;    /* 最后的秒钟加上去 */
    PWR_BackupAccessCmd(ENABLE);    /* 必须要 */
    RTC_WaitForLastTask();
    RTC_SetCounter(seccount);
    RTC_WaitForLastTask();          /* 必须加 PWR_BackupAccessCmd(ENABLE); 不然进入死循环 */

    return 0;
}

/**
 ****************************************************************************
 * @brief  得到当前的时间
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void rtc_get(void)
{
    static uint16_t daycnt = 0;
    uint32_t timecount = 0;
    uint32_t temp = 0;
    uint16_t temp1 = 0;

    timecount = RTC_GetCounter();

    temp = timecount / 86400;   /* 得到天数 */

    if(daycnt != temp)  /* 超过一天了 */
    {
        daycnt = temp;
        temp1 = 1970;  /* 从1970年开始 */

        while(temp >= 365)
        {
            if(is_leap_year(temp1))     /* 是闰年 */
            {
                if(temp >= 366)
                {
                    temp -= 366;        /* 闰年的秒钟数 */
                }
                else
                {
                    //temp1++;      /* armfly: 这里闰年处理错误，不能加1 */
                    break;
                }
            }
            else
            {
                temp -= 365;       /* 平年 */
            }
            temp1++;
        }
        g_sCalendar.year = temp1;   /* 得到年份 */

        temp1 = 0;
        while(temp >= 28)   /* 超过了一个月 */
        {
            if(is_leap_year(g_sCalendar.year) && temp1 == 1)    /* 当年是不是闰年/2月份 */
            {
                if(temp >= 29)
                {
                    temp -= 29; /* 闰年的秒钟数 */
                }
                else
                {
                    break;
                }
            }
            else
            {
                if(temp >= mon_table[temp1])
                {
                    temp -= mon_table[temp1];   /* 平年 */
                }
                else
                {
                    break;
                }
            }
            temp1++;
        }
        g_sCalendar.month = temp1 + 1;  /* 得到月份 */
        g_sCalendar.date = temp + 1;  /* 得到日期 */
    }

    temp = timecount % 86400;    /* 得到秒钟数 */
    g_sCalendar.hour = temp / 3600; /* 小时 */
    g_sCalendar.min = (temp % 3600) / 60; /* 分钟 */
    g_sCalendar.sec = (temp % 3600) % 60; /* 秒钟 */
    g_sCalendar.week = rtc_calcWeek(g_sCalendar.year, g_sCalendar.month, g_sCalendar.date); /* 计算星期 */
}

/**
 ****************************************************************************
 * @brief  计算星期
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
static uint8_t rtc_calcWeek(uint16_t _year, uint8_t _mon, uint8_t _day)
{
    /*
    蔡勒（Zeller）公式
        历史上的某一天是星期几？未来的某一天是星期几？关于这个问题，有很多计算公式（两个通用计算公式和
    一些分段计算公式），其中最著名的是蔡勒（Zeller）公式。
        即w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1

        公式中的符号含义如下，
         w：星期；
         c：年的高2位，即世纪-1
         y：年（两位数）；
         m：月（m大于等于3，小于等于14，即在蔡勒公式中，某年的1、2月要看作上一年的13、14月来计算，
            比如2003年1月1日要看作2002年的13月1日来计算）；
         d：日；
         [ ]代表取整，即只要整数部分。

        算出来的W除以7，余数是几就是星期几。如果余数是0，则为星期日。
        如果结果是负数，负数求余数则需要特殊处理：
            负数不能按习惯的余数的概念求余数，只能按数论中的余数的定义求余。为了方便
        计算，我们可以给它加上一个7的整数倍，使它变为一个正数，然后再求余数

        以2049年10月1日（100周年国庆）为例，用蔡勒（Zeller）公式进行计算，过程如下：
        蔡勒（Zeller）公式：w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
        =49+[49/4]+[20/4]-2×20+[26× (10+1)/10]+1-1
        =49+[12.25]+5-40+[28.6]
        =49+12+5-40+28
        =54 (除以7余5)
        即2049年10月1日（100周年国庆）是星期5。
    */
    uint8_t y, c, m, d;
    int16_t w;

    if(_mon >= 3)
    {
        m = _mon;
        y = _year % 100;
        c = _year / 100;
        d = _day;
    }
    else    /* 某年的1、2月要看作上一年的13、14月来计算 */
    {
        m = _mon + 12;
        y = (_year - 1) % 100;
        c = (_year - 1) / 100;
        d = _day;
    }

    w = y + y / 4 +  c / 4 - 2 * c + ((uint16_t)26 * (m + 1)) / 10 + d - 1;
    if(w == 0)
    {
        w = 7;  /* 表示周日 */
    }
    else if(w < 0)  /* 如果w是负数，则计算余数方式不同 */
    {
        w = 7 - (-w) % 7;
    }
    else
    {
        w = w % 7;
    }
    return w;
}

/**
 ****************************************************************************
 * @brief  RTC时钟中断
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void RTC_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_SEC) != RESET)    //秒钟中断
    {
        RTC_ClearITPendingBit(RTC_IT_SEC);      //清闹钟中断
        g_sCalendar.ms = 0;
        rtc_get();      //更新时间
    }

    if(RTC_GetITStatus(RTC_IT_ALR) != RESET)    //闹钟中断
    {
        RTC_ClearITPendingBit(RTC_IT_ALR);      //清闹钟中断
    }

    if(RTC_GetITStatus(RTC_IT_OW) != RESET)     //计数溢出中断
    {
        RTC_ClearITPendingBit(RTC_IT_OW);      //清计数溢出中断
    }

    //RTC_WaitForLastTask();
}

/*********************************** End of file *****************************************/
