/**
  ******************************************************************************
  * @file    dio.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-4-12
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DIO_H_
#define _DIO_H_

#include <stdint.h>


/* IO状态定义 */
enum io_stat_e
{
    IO_STAT_RESET = 0,
    IO_STAT_SET,
    IO_STAT_UNKNOWN
};


/* IO输出定义 */
enum io_output_e
{
    IO_CTRL_MAIN_PRIO = 0,
    IO_CTRL_MANUAL_SWITCH
};

/* IO输入滤波次数 （1ms/次） */
#define IO_INPUT_FILTER_TIMES     10

/* IO输入定义 */
#define IO_INPUT_COUNTS           7
enum io_input_e
{
    IO_MAIN_WORKING = 0,    // 主路工作中
    IO_STDBY_WORKING,       // 备路工作中
    IO_STDBY_PRIO,          // 自动模式下，备路优先
    IO_MAIN_PRIO,           // 自动模式下，主路优先
    IO_SWITCH_ONCE,         // 切换一次
    IO_MODE_AUTO,           // 自动模式
    IO_MODE_MANUAL          // 手动模式
};

#define IO_IN_EFFECT   IO_STAT_SET

void io_init(void);
void io_inputFilterHandle(void);
void io_setOut(uint8_t n, uint8_t value);
uint8_t io_getIn(uint8_t n);

#endif


