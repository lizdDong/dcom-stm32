/**
  ******************************************************************************
  * @file    dlog.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-12-3
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DLOG_H_
#define _DLOG_H_

#include <stdint.h>


#define LOG_LVL_DEFAULT              LOG_LEVEL_INFO

#define LOG_USE_HEAP                 0
#define LOG_BUFF_SIZE                256

#define LOG_TIME_OUT                 0
#define LOG_FILE_OUT                 1

#define LOG_END_SYMBOL               "\r\n"

#define LOG_FLAG_VERBOSE        "VERB"
#define LOG_FLAG_DEBUG          "DBUG"
#define LOG_FLAG_INFO           "INFO"
#define LOG_FLAG_WARN           "WARN"
#define LOG_FLAG_ERROR          "EROR"
#define LOG_FLAG_ASSERT         "ASRT"

#define LOG_USE_LEVEL_SAVE_BKP_DR    0
#if (LOG_USE_LEVEL_SAVE_BKP_DR)
#define LEVEL_SAVE_BKP_DR             BKP_DR2
#endif

#define LOG_USE_LEVEL_SAVE_FLASH     0
#if (LOG_USE_LEVEL_SAVE_FLASH)
#include "dev_flash.h"
#include "dev_flash_cfg.h"
#endif

typedef enum
{
    LOG_LEVEL_VERBOSE = 0,  //Verbose
    LOG_LEVEL_DEBUG,        //Debug
    LOG_LEVEL_INFO,         //Info
    LOG_LEVEL_WARN,         //Warning
    LOG_LEVEL_ERROR,        //Error
    LOG_LEVEL_ASSERT,       //Assert
    LOG_LVL_NONE          //None
} logLevel_t;

#if (LOG_FILE_OUT)
#include <string.h>
#define FILENAME(file) (strrchr(file, '\\') ? strrchr(file, '\\') + 1 : file)     // 去除文件路径
#define logv(fmt, ...) log_print(LOG_LEVEL_VERBOSE, "[ %s, L%-4d] " fmt, FILENAME(__FILE__), __LINE__, ##__VA_ARGS__)
#define logd(fmt, ...) log_print(LOG_LEVEL_DEBUG  , "[ %s, L%-4d] " fmt, FILENAME(__FILE__), __LINE__, ##__VA_ARGS__)
#define logi(fmt, ...) log_print(LOG_LEVEL_INFO   , "[ %s, L%-4d] " fmt, FILENAME(__FILE__), __LINE__, ##__VA_ARGS__)
#define logw(fmt, ...) log_print(LOG_LEVEL_WARN   , "[ %s, L%-4d] " fmt, FILENAME(__FILE__), __LINE__, ##__VA_ARGS__)
#define loge(fmt, ...) log_print(LOG_LEVEL_ERROR  , "[ %s, L%-4d] " fmt, FILENAME(__FILE__), __LINE__, ##__VA_ARGS__)
#define loga(fmt, ...) log_print(LOG_LEVEL_ASSERT , "[ %s, L%-4d] " fmt, FILENAME(__FILE__), __LINE__, ##__VA_ARGS__)
#else
#define logv(fmt, ...) log_print(LOG_LEVEL_VERBOSE, fmt, ##__VA_ARGS__)
#define logd(fmt, ...) log_print(LOG_LEVEL_DEBUG  , fmt, ##__VA_ARGS__)
#define logi(fmt, ...) log_print(LOG_LEVEL_INFO   , fmt, ##__VA_ARGS__)
#define logw(fmt, ...) log_print(LOG_LEVEL_WARN   , fmt, ##__VA_ARGS__)
#define loge(fmt, ...) log_print(LOG_LEVEL_ERROR  , fmt, ##__VA_ARGS__)
#define loga(fmt, ...) log_print(LOG_LEVEL_ASSERT , fmt, ##__VA_ARGS__)
#endif

#define log_hex_v(fmt, ...) log_printHex(LOG_LEVEL_VERBOSE, fmt, ##__VA_ARGS__)
#define log_hex_d(fmt, ...) log_printHex(LOG_LEVEL_DEBUG  , fmt, ##__VA_ARGS__)
#define log_hex_i(fmt, ...) log_printHex(LOG_LEVEL_INFO   , fmt, ##__VA_ARGS__)
#define log_hex_w(fmt, ...) log_printHex(LOG_LEVEL_WARN   , fmt, ##__VA_ARGS__)
#define log_hex_e(fmt, ...) log_printHex(LOG_LEVEL_ERROR  , fmt, ##__VA_ARGS__)
#define log_hex_a(fmt, ...) log_printHex(LOG_LEVEL_ASSERT , fmt, ##__VA_ARGS__)

#define log_bin_v(v)    log_printBin(LOG_LEVEL_VERBOSE, v)
#define log_bin_d(v)    log_printBin(LOG_LEVEL_DEBUG  , v)
#define log_bin_i(v)    log_printBin(LOG_LEVEL_INFO   , v)
#define log_bin_w(v)    log_printBin(LOG_LEVEL_WARN   , v)
#define log_bin_e(v)    log_printBin(LOG_LEVEL_ERROR  , v)
#define log_bin_a(v)    log_printBin(LOG_LEVEL_ASSERT , v)

int log_init(void);
void log_setLevel(logLevel_t level);
void log_levelReport(void);
void __attribute__((format(printf, 2, 3))) log_print(logLevel_t level, const char *fmt, ...);
void log_printHex(logLevel_t level, uint8_t *p, uint16_t len);
void log_printBin(logLevel_t level, uint32_t v);

#if (LOG_USE_LEVEL_SAVE_FLASH)
uint8_t log_levelSave(void);
logLevel_t log_levelRead(void);
#endif

#endif

/*********************************** End of file *****************************************/
