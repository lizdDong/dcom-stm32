/**
  ******************************************************************************
  * @file    dtimer.h
  * @author  lizdDong
  * @version V1.0
  * @date    2021-7-7
  * @brief   None
  * @attention 设计原型来自multi_timer(https://github.com/0x1abin/MultiTimer.git)，
  *            仅更改参数repeat为repeat_times，表示重复次数
  *
  ******************************************************************************
**/

#ifndef _DTIMER_H_
#define _DTIMER_H_


#include <stdint.h>
#include <stddef.h>

/*
It means 1 tick for 1ms.
Your can configurate for your tick time such as 5ms/10ms and so on.
*/
#define CFG_TIMER_1_TICK_N_MS   1

#define TIMER_REPEAT_UNLIMIT    0xFFFFFFFF

typedef struct timer_t
{
    uint32_t        cur_ticks;          /* Record current timer start tick */
    uint32_t        cur_expired_time;   /* Record current timer expired time */
    uint32_t        timeout;            /* Delay time */
    uint32_t        repeat_times;       /* Repeattimes */
    void *          arg;                /* Input argument for timeout_cb function */
    void (*timeout_cb)(void *arg);      /* timer_t expired callback function */
    struct timer_t   *next;             /* Pointer to next timer */
} timer_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  Initializes the timer struct handle.
  * @param  handle: the timer handle strcut.
  * @param  timeout_cb: timeout callback.
  * @param  timeout: delay to start the timer.
  * @param  repeat_times: repeat_times interval time.
  * @param  arg: the input argument for timeout_cb fucntion.
  * @retval None
  */
void timer_init(struct timer_t *handle, void(*timeout_cb)(void *arg), \
                uint32_t timeout, uint32_t repeat_times, void *arg);

/**
  * @brief  Start the timer work, add the handle into work list.
  * @param  btn: target handle strcut.
  * @retval 0: succeed. -1: already exist.
  */
int  timer_start(struct timer_t *handle);

/**
  * @brief  Stop the timer work, remove the handle off work list.
  * @param  handle: target handle strcut.
  * @retval 0: succeed. -1: timer not exist.
  */
int timer_stop(struct timer_t *handle);

/**
  * @brief  background ticks, timer repeat_times invoking interval nms.
  * @param  None.
  * @retval None.
  */
void timer_ticks(void);

/**
  * @brief  main loop.
  * @param  None.
  * @retval None
  */
void timer_loop(void);

#ifdef __cplusplus
}
#endif

#endif


/*********************************** End of file *****************************************/
