/**
  ******************************************************************************
  * @file    dtimer.c
  * @author  lizdDong
  * @version V1.0
  * @date    2021-7-7
  * @brief   None
  * @attention 设计原型来自multi_timer(https://github.com/0x1abin/MultiTimer.git)，
  *            仅更改参数repeat为repeat_times，表示重复次数
  *
  ******************************************************************************
**/


#include <stdio.h>
#include "dtimer.h"

//timer handle list head.
static struct timer_t* head_handle = NULL;

static uint32_t _timer_ticks = 0;

/**
  * @brief  Initializes the timer struct handle.
  * @param  handle: the timer handle strcut.
  * @param  timeout_cb: timeout callback.
  * @param  timeout: delay to start the timer.
  * @param  repeat_times: repeat_times.
  * @param  arg: the input argument for timeout_cb fucntion.
  * @retval None
  */
void timer_init(struct timer_t* handle, void (*timeout_cb)(void *arg), \
                uint32_t timeout, uint32_t repeat_times, void *arg)
{
    handle->timeout_cb          = timeout_cb;
    handle->timeout             = timeout;
    handle->repeat_times        = repeat_times;
    handle->cur_ticks           = _timer_ticks;
    handle->cur_expired_time    = handle->timeout;
    handle->arg                 = arg;
}

/**
  * @brief  Start the timer work, add the handle into work list.
  * @param  btn: target handle strcut.
  * @retval 0: succeed. -1: already exist.
  */
int timer_start(struct timer_t* handle)
{
    struct timer_t* target = head_handle;

    while(target)
    {
        if(target == handle)
        {
            return -1;  //already exist.
        }
        target = target->next;
    }
    handle->next = head_handle;
    head_handle  = handle;

    return 0;
}

/**
  * @brief  Stop the timer work, remove the handle off work list.
  * @param  handle: target handle strcut.
  * @retval 0: succeed. -1: timer not exist.
  */
int timer_stop(struct timer_t* handle)
{
    struct timer_t** curr;

    for(curr = &head_handle; *curr;)
    {
        struct timer_t* entry = *curr;
        if(entry == handle)
        {
            *curr = entry->next;
            return 0; // found specified timer
        }
        else
        {
            curr = &entry->next;
        }
    }

    return 0;
}

/**
  * @brief  main loop.
  * @param  None.
  * @retval None
  */
void timer_loop(void)
{
    struct timer_t* target;

    for(target = head_handle; target; target = target->next)
    {
        if(_timer_ticks - target->cur_ticks >= target->cur_expired_time)
        {
            if(target->repeat_times == 0)
            {
                timer_stop(target);
            }
            else
            {
                target->cur_ticks = _timer_ticks;
                target->cur_expired_time = target->timeout;
                target->timeout_cb(target->arg);
                if(target->repeat_times != TIMER_REPEAT_UNLIMIT)
                    target->repeat_times--;
            }
        }
    }
}

/**
  * @brief  background ticks, timer repeat invoking interval nms.
  * @param  None.
  * @retval None.
  */
void timer_ticks(void)
{
    _timer_ticks += CFG_TIMER_1_TICK_N_MS;
}



/*********************************** End of file *****************************************/

