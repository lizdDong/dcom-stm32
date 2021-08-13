/**
  ******************************************************************************
  * @file    dsh.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-3-10
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DSH_
#define _DSH_

#include <stdint.h>

#define DSH_USE_HISTORY     1

#define DSH_HEAD              "dsh >"
#define DSH_BUFF_SIZE         128   //×î´ó
#if (DSH_USE_HISTORY)
#define DSH_HISTORY_COUNTS    5
#endif

typedef struct
{
    uint8_t pos;
#if (DSH_USE_HISTORY)
    uint8_t hisW;
    uint8_t lastHisW;
    uint8_t hisR;
    uint8_t hisFullFlag;
    char hisCmdList[DSH_HISTORY_COUNTS][DSH_BUFF_SIZE];
#endif
    char CmdBuff[DSH_BUFF_SIZE];
} DshInfoType_t;

extern DshInfoType_t g_tDshInfo;

void dsh_init(void);
uint8_t dsh_Handle(void);
uint8_t insert_char(uint8_t *pstr, uint8_t n, uint8_t c);
uint8_t delete_char(uint8_t *pstr, uint8_t n);


#endif

/*********************************** End of file *****************************************/
