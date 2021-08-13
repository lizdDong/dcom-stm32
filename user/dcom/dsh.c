/**
  ******************************************************************************
  * @file    dsh.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-3-10
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include <string.h>
#include <stdlib.h>
#include "duart.h"
#include "dsh.h"


enum input_stat
{
    WAIT_NORMAL,
    WAIT_SPEC_KEY,
    WAIT_FUNC_KEY,
};

DshInfoType_t g_tDshInfo;
const uint8_t LineClearCode[] = {0x1B, 0x5B, 0x32, 0x4B, 0x0D};
const uint8_t LeftCode[] = {0x1B, 0x5B, 0x44};
const uint8_t RightCode[] = {0x1B, 0x5B, 0x43};

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void dsh_init(void)
{
    memset(&g_tDshInfo, 0x00, sizeof(g_tDshInfo));
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
uint8_t dsh_getChar(uint8_t *pchar)
{
    return uart_getChar(UART_COM3, (char *)pchar);
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
void dsh_sendBuff(uint8_t *pstr, uint16_t len)
{
    uart_sendStr(UART_COM3, (char *)pstr);
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
void dsh_sendChar(char ch)
{
    uart_sendChar(UART_COM3, ch);
}

/**
 ****************************************************************************
 * @brief  模拟简单的shell交互
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
uint8_t dsh_Handle(void)
{
    uint8_t c, retn = 0;
#if (DSH_USE_HISTORY)
    uint8_t isHisCmd = 0;
    static enum input_stat stat = WAIT_NORMAL;
    static uint8_t isNewCmd = 0, isFromHiscmd = 0;
#endif

    if(dsh_getChar(&c))
    {
#if (DSH_USE_HISTORY)
        if(c == 0x1b)
        {
            stat = WAIT_SPEC_KEY;
            return retn;
        }
        else if(stat == WAIT_SPEC_KEY)
        {
            if(c == 0x5b)
            {
                stat = WAIT_FUNC_KEY;
                return retn;
            }

            stat = WAIT_NORMAL;
        }
        else if(stat == WAIT_FUNC_KEY)
        {
            stat = WAIT_NORMAL;

            if(c == 0x41)  /* up key */
            {
                if(!g_tDshInfo.hisFullFlag)
                {
                    if(g_tDshInfo.hisR > 0)
                    {
                        g_tDshInfo.hisR--;
                        isHisCmd = 1;
                    }
                    else
                    {
                        if(g_tDshInfo.hisW > 0)
                        {
                            g_tDshInfo.hisR = g_tDshInfo.hisW - 1;
                            isHisCmd = 1;
                        }
                    }
                }
                else
                {
                    if(g_tDshInfo.hisR > 0)
                    {
                        g_tDshInfo.hisR--;
                    }
                    else
                    {
                        g_tDshInfo.hisR = DSH_HISTORY_COUNTS - 1;
                    }
                    isHisCmd = 1;
                }
            }
            else if(c == 0x42)  /* down key */
            {
                if(!g_tDshInfo.hisFullFlag)
                {
                    if(g_tDshInfo.hisW > 0)
                    {
                        if(g_tDshInfo.hisR == g_tDshInfo.hisW)
                        {
                            g_tDshInfo.hisR = 0;
                        }
                        else
                        {
                            g_tDshInfo.hisR++;
                        }
                        isHisCmd = 1;
                    }
                }
                else
                {
                    if(g_tDshInfo.hisR == DSH_HISTORY_COUNTS)
                    {
                        g_tDshInfo.hisR = 0;
                    }
                    else
                    {
                        g_tDshInfo.hisR++;
                    }
                    isHisCmd = 1;
                }
            }
            else if(c == 0x44)  /* left key */
            {
//        if(g_tDshInfo.Pos > strlen(DSH_HEAD) - 1)
//        {
//          g_tDshInfo.Pos--;
//          dsh_sendBuff(LeftCode, sizeof(LeftCode));
//        }
            }
            else if(c == 0x43)  /* right key */
            {
            }

            if(isHisCmd)
            {
                //清空显示
                dsh_sendBuff((uint8_t *)LineClearCode, sizeof(LineClearCode));
                //显示历史命令
                dsh_sendBuff((uint8_t *)DSH_HEAD, strlen(DSH_HEAD));
                memset(g_tDshInfo.CmdBuff, 0x00, sizeof(g_tDshInfo.CmdBuff));
                memcpy(g_tDshInfo.CmdBuff, g_tDshInfo.hisCmdList[g_tDshInfo.hisR], strlen(g_tDshInfo.hisCmdList[g_tDshInfo.hisR]) - 1);
                dsh_sendBuff((uint8_t *)g_tDshInfo.CmdBuff, strlen(g_tDshInfo.CmdBuff));
                g_tDshInfo.pos = strlen(g_tDshInfo.CmdBuff);
                isFromHiscmd = 1;
            }
            return retn;
        }
#endif

        if(g_tDshInfo.pos < DSH_BUFF_SIZE - 1)
        {
            if(c == '\b') //退格键
            {
                if(g_tDshInfo.pos > 0)
                {
                    g_tDshInfo.pos--;
                    dsh_sendBuff((uint8_t *)"\b \b", 3);
                }
            }
            else if(c == 0x03)  //'ctrl+C'
            {
                dsh_sendBuff((uint8_t *)"^C\r\n", 4);
                dsh_sendBuff((uint8_t *)DSH_HEAD, strlen(DSH_HEAD));
                goto exit;
            }
            else if(c == '\r')  //回车键
            {
                dsh_sendChar('\r');
                dsh_sendChar('\n');
                retn = 1;
            }
            else
            {
                g_tDshInfo.CmdBuff[g_tDshInfo.pos] = c;   // 将输入内容存入缓存区
                dsh_sendChar(c);  //回显输入
                g_tDshInfo.pos++;
#if (DSH_USE_HISTORY)
                isNewCmd = 1;
#endif
            }
        }
    }

    if(retn)
    {
        g_tDshInfo.CmdBuff[g_tDshInfo.pos] = '\r';
        g_tDshInfo.CmdBuff[++g_tDshInfo.pos] = '\0';

#if (DSH_USE_HISTORY)
        if(isFromHiscmd)
        {
            if(strcmp(g_tDshInfo.CmdBuff, g_tDshInfo.hisCmdList[g_tDshInfo.lastHisW]) != 0)
            {
                //比较是否为上一条历史指令，如果不是则认为是新指令，需要存储到历史指令列表中
                isNewCmd = 1;
            }
        }

        //存储历史指令
        if(isNewCmd)
        {
            memset(g_tDshInfo.hisCmdList[g_tDshInfo.hisW], 0x00, sizeof(g_tDshInfo.hisCmdList[g_tDshInfo.hisW]));
            memcpy(g_tDshInfo.hisCmdList[g_tDshInfo.hisW], g_tDshInfo.CmdBuff, strlen(g_tDshInfo.CmdBuff));
            g_tDshInfo.lastHisW = g_tDshInfo.hisW;
            if(++g_tDshInfo.hisW == DSH_HISTORY_COUNTS)
            {
                g_tDshInfo.hisW = 0;
                g_tDshInfo.hisFullFlag = 1;
            }
        }
        g_tDshInfo.hisR = g_tDshInfo.hisW;
#endif

exit:
#if (DSH_USE_HISTORY)
        isFromHiscmd = 0;
        isNewCmd = 0;
#endif
        g_tDshInfo.pos = 0;
    }
    return retn;
}



/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  n-要插入的字符位置，>=0
 * @retval None
 ****************************************************************************
 */
uint8_t insert_char(uint8_t *pstr, uint8_t n, uint8_t c)
{
    uint8_t * pstr1;
    uint8_t len;

    len = strlen((char *)pstr);
    if(n > len)
    {
        return 0;
    }
    pstr1 = malloc(len + 1);
    if(pstr1 == NULL)
    {
        return 0;
    }
    memcpy(pstr1, pstr, n);
    pstr1[n] = c;
    memcpy(pstr1 + n + 1, pstr + n, len - n);
    memcpy(pstr, pstr1, len + 1);
    free(pstr1);
    return len + 1;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  n-要删除的字符位置，>=1
 * @retval None
 ****************************************************************************
 */
uint8_t delete_char(uint8_t *pstr, uint8_t n)
{
    uint8_t * pstr1;
    uint8_t len;

    len = strlen((char *)pstr);
    if(n > len || n < 1)
    {
        return 0;
    }
    pstr1 = malloc(len);
    if(pstr1 == NULL)
    {
        return 0;
    }
    memcpy(pstr1, pstr, n - 1);
    memcpy(pstr1 + n - 1, pstr + n, len - n + 1);
    strcpy((char *)pstr, (char *)pstr1);
    free(pstr1);
    return len - 1;
}


/*********************************** End of file *****************************************/
