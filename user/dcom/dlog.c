/**
  ******************************************************************************
  * @file    dlog.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-12-3
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "duart.h"
#include "dlog.h"


#if LOG_TIME_OUT
#include "drtc.h"
static struct
{
    uint16_t year;
    uint8_t mon;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t ms;
} g_sLogTime;
#endif

#if (USE_RTOS_RT_THREAD)
#include "rtthread.h"
#ifndef RT_USING_MUTEX
#error "Please define [RT_USING_MUTEX] at <rtconfig.h> "
#endif
static struct rt_mutex mutex_dlog;
#endif

/**************  user code begin **************/

static void log_sendString(char *pstr)
{
    uart_sendStr(UART_COM3, pstr);
}

#if LOG_TIME_OUT
static void log_getTime(void)
{
    //rtc_get();
    g_sLogTime.year = g_sCalendar.year;
    g_sLogTime.mon = g_sCalendar.month;
    g_sLogTime.day = g_sCalendar.date;
    g_sLogTime.hour = g_sCalendar.hour;
    g_sLogTime.min = g_sCalendar.min;
    g_sLogTime.sec = g_sCalendar.sec;
    g_sLogTime.ms = g_sCalendar.ms;
}
#endif

/**************  user code end **************/


static logLevel_t g_eDlogLevel;
static char DlogLevelBrief[][8] =
{
    {"Verbose"},
    {"Debug"},
    {"Info"},
    {"Warning"},
    {"Error"},
    {"Assert"},
    {"None"}
};
#if (LOG_USE_HEAP == 0)
static char g_aLogBuff[LOG_BUFF_SIZE];
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
int log_init(void)
{
#if (LOG_USE_LEVEL_SAVE_BKP_DR)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
#endif

#if (USE_RTOS_RT_THREAD)
    rt_mutex_init(&mutex_dlog, "mdlog", RT_IPC_FLAG_FIFO);
#endif

    log_setLevel(LOG_LVL_DEFAULT);
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
void log_print(logLevel_t level, const char *fmt, ...)
{
    va_list args;
    char *s = g_aLogBuff;
    uint16_t n;
    char *f;

    if(g_eDlogLevel > level)
    {
        return;
    }

    switch(level)
    {
        case LOG_LEVEL_VERBOSE:
            f = LOG_FLAG_VERBOSE;
            break;
        case LOG_LEVEL_DEBUG:
            f = LOG_FLAG_DEBUG;
            break;
        case LOG_LEVEL_INFO:
            f = LOG_FLAG_INFO;
            break;
        case LOG_LEVEL_WARN:
            f = LOG_FLAG_WARN;
            break;
        case LOG_LEVEL_ERROR:
            f = LOG_FLAG_ERROR;
            break;
        case LOG_LEVEL_ASSERT:
            f = LOG_FLAG_ASSERT;
            break;
        default:
            return;
    }

    s[0] = '\0';
    n = 0;
    
#if LOG_TIME_OUT
    log_getTime();
    sprintf(s, "[%04d-%02d-%02d %02d:%02d:%02d.%03d] ", \
            g_sLogTime.year, g_sLogTime.mon, g_sLogTime.day, g_sLogTime.hour, g_sLogTime.min, g_sLogTime.sec, g_sLogTime.ms);
    n = strlen(s);
#endif

    sprintf(&s[n], "[%s]> ", f);
    n = strlen(s);

    va_start(args, fmt);
    vsprintf(&s[n], fmt, args);
    va_end(args);

#if (USE_RTOS_RT_THREAD)
    rt_mutex_take(&mutex_dlog, RT_WAITING_FOREVER);
#endif
    log_sendString(s);
    log_sendString(LOG_END_SYMBOL);
#if (USE_RTOS_RT_THREAD)
    rt_mutex_release(&mutex_dlog);
#endif

#if LOG_USE_HEAP
    free(s);
#endif
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
void log_setLevel(logLevel_t level)
{
    if(level > LOG_LVL_NONE)
    {
        log_print(LOG_LEVEL_ERROR, "debug level[%d] set error.", level);
        return;
    }
    g_eDlogLevel = (logLevel_t)level;
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
void log_levelReport(void)
{
    char *s = g_aLogBuff;

    sprintf(s, "%s", DlogLevelBrief[g_eDlogLevel]);
    log_sendString(s);
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
void log_printHex(logLevel_t level, uint8_t *p, uint16_t len)
{
    uint16_t i, div, rem;
    uint8_t j;
    char *s = g_aLogBuff;

    if(g_eDlogLevel > level)
    {
        return;
    }

    div = len / 16;
    rem = len % 16;
    for(i = 0; i < div; i++)
    {
        for(j = 0; j < 16; j++)
        {
            sprintf(s, "%02X ", p[i * 16 + j]);
            log_sendString(s);
        }
        log_sendString("     ");
        for(j = 0; j < 16; j++)
        {
            char c = p[i * 16 + j];
            if(c >= 0x20 && c <= 0x7E)
            {
                sprintf(s, "%c", c);
                log_sendString(s);
            }
            else
            {
                log_sendString(".");
            }
        }
        log_sendString(LOG_END_SYMBOL);
    }
    if(rem > 0)
    {
        for(j = 0; j < rem; j++)
        {
            sprintf(s, "%02X ", p[div * 16 + j]);
            log_sendString(s);
        }
        for(; j < 16; j++)
        {
            log_sendString("   ");
        }
        log_sendString("     ");
        for(j = 0; j < rem; j++)
        {
            char c = p[i * 16 + j];
            if(c >= 0x20 && c <= 0x7E)
            {
                sprintf(s, "%c", c);
                log_sendString(s);
            }
            else
            {
                log_sendString(".");
            }
        }
        log_sendString(LOG_END_SYMBOL);
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
void log_printBin(logLevel_t level, uint32_t v)
{
    uint8_t i;

    if(g_eDlogLevel > level)
    {
        return;
    }

    log_sendString("BIN: ");
    for(i = 0; i < 32;)
    {
        if((v & (0x80000000 >> i)) != 0)
        {
            log_sendString("1");
        }
        else
        {
            log_sendString("0");
        }

        if(++i % 4 == 0)
            log_sendString(" ");
    }
    log_sendString(LOG_END_SYMBOL);
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
uint8_t log_levelSave(void)
{
#if (LOG_USE_LEVEL_SAVE_FLASH)
    uint16_t lev = (uint16_t)g_eDlogLevel;
    if(dflash_Write(LOG_LEVEL_SAVE_FLASH_ADDR, (uint8_t *)&lev, sizeof(lev)) == sizeof(lev))
    {
        return 1;
    }
#endif

#if (LOG_USE_LEVEL_SAVE_BKP_DR)
    BKP_WriteBackupRegister(LEVEL_SAVE_BKP_DR, lev);
    return 1;
#endif
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
logLevel_t log_levelRead(void)
{
    uint16_t lev = LOG_LVL_DEFAULT;

#if (LOG_USE_LEVEL_SAVE_FLASH)
    dflash_Read(LOG_LEVEL_SAVE_FLASH_ADDR, (uint8_t *)&lev, sizeof(lev));
#endif

#if (LOG_USE_LEVEL_SAVE_BKP_DR)
    lev = BKP_ReadBackupRegister(LEVEL_SAVE_BKP_DR);
#endif
    if(lev <= LOG_LVL_NONE)
    {
        return (logLevel_t)lev;
    }
    return LOG_LVL_DEFAULT;
}


/*********************************** End of file *****************************************/
