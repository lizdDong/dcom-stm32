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


calendar_t g_sCalendar;    // �����ṹ��

/* ƽ���ÿ�������� */
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

    /* ������BKP�ĺ󱸼Ĵ���1�У�����һ�������ַ�0xA5A5, ��һ���ϵ��󱸵�Դ����󣬸üĴ������ݶ�ʧ��
    ����RTC���ݶ�ʧ����Ҫ�������� */
    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        /* PWRʱ�ӣ���Դ���ƣ���BKPʱ�ӣ�RTC�󱸼Ĵ�����ʹ�� */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
        /* Allow access to BKP Domain */ /*ʹ��RTC�ͺ󱸼Ĵ������� */
        PWR_BackupAccessCmd(ENABLE);
        /* Reset Backup Domain */  /* ������BKP��ȫ���Ĵ�������Ϊȱʡֵ */
        BKP_DeInit();
        /* Enable LSE */
        RCC_LSEConfig(RCC_LSE_ON);

        /* Wait till LSE is ready */ /* �ȴ��ⲿ�������ȶ���� */
        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

        /* Select LSE as RTC Clock Source */ /*ʹ���ⲿ32.768KHz������ΪRTCʱ�� */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        /* Enable RTC Clock */
        RCC_RTCCLKCmd(ENABLE);
        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        RTC_EnterConfigMode();

        /* Set RTC prescaler: set RTC period to 1sec */
        /* 32.768KHz����Ԥ��Ƶֵ��32767,����Ծ���Ҫ��ܸ߿����޸Ĵ˷�Ƶֵ��У׼���� */
        RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        rtc_set(2015, 8, 8, 0, 0, 0);//Ĭ��ʱ��

        /* ������ɺ���󱸼Ĵ�����д�����ַ�0xA5A5 */
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
    else
    {
        /* ���󱸼Ĵ���û�е��磬��������������RTC */
        if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
            /* �ϵ縴λ */
        }
        else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
            /* �ⲿRST�ܽŸ�λ */
        }
        /* ���RCC�и�λ��־ */
        RCC_ClearFlag();

        //��ȻRTCģ�鲻��Ҫ�������ã��ҵ���������󱸵����Ȼ����
        //����ÿ���ϵ�󣬻���Ҫʹ��RTCCLK
        RCC_RTCCLKCmd(ENABLE);
        //�ȴ�RTCʱ����APB1ʱ��ͬ��
        RTC_WaitForSynchro();
        //�ȴ��������
        RTC_WaitForLastTask();
    }

    /* ����ʹ�����ж� */
    #if(1)
    {
        NVIC_InitTypeDef NVIC_InitStructure;

        NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        //ʹ�����ж�
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
    }
    #endif

    rtc_get();//����ʱ��
    
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
    if(year % 4 == 0)  /* �����ܱ�4���� */
    {
        if(year % 100 == 0)
        {
            if(year % 400 == 0)
            {
                return 1;   /* �����00��β,��Ҫ�ܱ�400���� */
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
 * @brief  ����ʱ��
 * @author lizdDong
 * @note   ��1970��1��1��Ϊ��׼��1970~2099��Ϊ�Ϸ����
 * @param  None
 * @retval 0���ɹ�������������
 ****************************************************************************
*/
int rtc_set(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint16_t t;
    uint32_t seccount = 0;

    if(year < 2000 || year > 2099)
    {
        return -1;  /* _year��Χ1970-2099���˴����÷�ΧΪ2000-2099 */
    }

    for(t = 1970; t < year; t++)    /* ��������ݵ�������� */
    {
        if(is_leap_year(t))         /* �ж��Ƿ�Ϊ���� */
        {
            seccount += 31622400;   /* ����������� */
        }
        else
        {
            seccount += 31536000;   /* ƽ��������� */
        }
    }

    mon -= 1;

    for(t = 0; t < mon; t++)          /* ��ǰ���·ݵ���������� */
    {
        seccount += (uint32_t)mon_table[t] * 86400; /* �·���������� */

        if(is_leap_year(year) && t == 1)
        {
            seccount += 86400;  /* ����2�·�����һ��������� */
        }
    }

    seccount += (uint32_t)(day - 1) * 86400;    /* ��ǰ�����ڵ���������� */
    seccount += (uint32_t)hour * 3600;      /* Сʱ������ */
    seccount += (uint32_t)min * 60; /* ���������� */
    seccount += sec;    /* �������Ӽ���ȥ */
    PWR_BackupAccessCmd(ENABLE);    /* ����Ҫ */
    RTC_WaitForLastTask();
    RTC_SetCounter(seccount);
    RTC_WaitForLastTask();          /* ����� PWR_BackupAccessCmd(ENABLE); ��Ȼ������ѭ�� */

    return 0;
}

/**
 ****************************************************************************
 * @brief  �õ���ǰ��ʱ��
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

    temp = timecount / 86400;   /* �õ����� */

    if(daycnt != temp)  /* ����һ���� */
    {
        daycnt = temp;
        temp1 = 1970;  /* ��1970�꿪ʼ */

        while(temp >= 365)
        {
            if(is_leap_year(temp1))     /* ������ */
            {
                if(temp >= 366)
                {
                    temp -= 366;        /* ����������� */
                }
                else
                {
                    //temp1++;      /* armfly: �������괦����󣬲��ܼ�1 */
                    break;
                }
            }
            else
            {
                temp -= 365;       /* ƽ�� */
            }
            temp1++;
        }
        g_sCalendar.year = temp1;   /* �õ���� */

        temp1 = 0;
        while(temp >= 28)   /* ������һ���� */
        {
            if(is_leap_year(g_sCalendar.year) && temp1 == 1)    /* �����ǲ�������/2�·� */
            {
                if(temp >= 29)
                {
                    temp -= 29; /* ����������� */
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
                    temp -= mon_table[temp1];   /* ƽ�� */
                }
                else
                {
                    break;
                }
            }
            temp1++;
        }
        g_sCalendar.month = temp1 + 1;  /* �õ��·� */
        g_sCalendar.date = temp + 1;  /* �õ����� */
    }

    temp = timecount % 86400;    /* �õ������� */
    g_sCalendar.hour = temp / 3600; /* Сʱ */
    g_sCalendar.min = (temp % 3600) / 60; /* ���� */
    g_sCalendar.sec = (temp % 3600) % 60; /* ���� */
    g_sCalendar.week = rtc_calcWeek(g_sCalendar.year, g_sCalendar.month, g_sCalendar.date); /* �������� */
}

/**
 ****************************************************************************
 * @brief  ��������
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
static uint8_t rtc_calcWeek(uint16_t _year, uint8_t _mon, uint8_t _day)
{
    /*
    ���գ�Zeller����ʽ
        ��ʷ�ϵ�ĳһ�������ڼ���δ����ĳһ�������ڼ�������������⣬�кܶ���㹫ʽ������ͨ�ü��㹫ʽ��
    һЩ�ֶμ��㹫ʽ�����������������ǲ��գ�Zeller����ʽ��
        ��w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1

        ��ʽ�еķ��ź������£�
         w�����ڣ�
         c����ĸ�2λ��������-1
         y���꣨��λ������
         m���£�m���ڵ���3��С�ڵ���14�����ڲ��չ�ʽ�У�ĳ���1��2��Ҫ������һ���13��14�������㣬
            ����2003��1��1��Ҫ����2002���13��1�������㣩��
         d���գ�
         [ ]����ȡ������ֻҪ�������֡�

        �������W����7�������Ǽ��������ڼ������������0����Ϊ�����ա�
        �������Ǹ�������������������Ҫ���⴦��
            �������ܰ�ϰ�ߵ������ĸ�����������ֻ�ܰ������е������Ķ������ࡣΪ�˷���
        ���㣬���ǿ��Ը�������һ��7����������ʹ����Ϊһ��������Ȼ����������

        ��2049��10��1�գ�100������죩Ϊ�����ò��գ�Zeller����ʽ���м��㣬�������£�
        ���գ�Zeller����ʽ��w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
        =49+[49/4]+[20/4]-2��20+[26�� (10+1)/10]+1-1
        =49+[12.25]+5-40+[28.6]
        =49+12+5-40+28
        =54 (����7��5)
        ��2049��10��1�գ�100������죩������5��
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
    else    /* ĳ���1��2��Ҫ������һ���13��14�������� */
    {
        m = _mon + 12;
        y = (_year - 1) % 100;
        c = (_year - 1) / 100;
        d = _day;
    }

    w = y + y / 4 +  c / 4 - 2 * c + ((uint16_t)26 * (m + 1)) / 10 + d - 1;
    if(w == 0)
    {
        w = 7;  /* ��ʾ���� */
    }
    else if(w < 0)  /* ���w�Ǹ����������������ʽ��ͬ */
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
 * @brief  RTCʱ���ж�
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void RTC_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_SEC) != RESET)    //�����ж�
    {
        RTC_ClearITPendingBit(RTC_IT_SEC);      //�������ж�
        g_sCalendar.ms = 0;
        rtc_get();      //����ʱ��
    }

    if(RTC_GetITStatus(RTC_IT_ALR) != RESET)    //�����ж�
    {
        RTC_ClearITPendingBit(RTC_IT_ALR);      //�������ж�
    }

    if(RTC_GetITStatus(RTC_IT_OW) != RESET)     //��������ж�
    {
        RTC_ClearITPendingBit(RTC_IT_OW);      //���������ж�
    }

    //RTC_WaitForLastTask();
}

/*********************************** End of file *****************************************/
