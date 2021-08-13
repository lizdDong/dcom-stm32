/**
  ******************************************************************************
  * @file    dfifo.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2020-12-3
  * @brief   �����ȳ��Ļ��ζ���
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "dfifo.h"



/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void fifo_init(fifo_t *fifo, uint8_t *pbuff, uint32_t size, fifo_cb lock, fifo_cb unlock)
{
    fifo->pBuff = pbuff;
    fifo->buffSize = size;
    fifo->waitReadCount = 0;
    fifo->pWirte = fifo->pBuff;
    fifo->pRead = fifo->pBuff;
    fifo->lock = lock;
    fifo->unlock = unlock;
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   ��ʱ������������δ�ʱ�����������жϺ����У�timeout����Ϊ0
 * @param  None
 * @retval ʵ��д����ֽ���
 ****************************************************************************
*/
uint32_t fifo_write(fifo_t *fifo, uint8_t *pbuff, uint32_t num, uint32_t timeout)
{
    uint32_t count = num;
    uint32_t pri_mask;
    uint32_t tickstart;
    
    while(count)
    {
        /* buff���󣬵ȴ��ռ䣬��ʱʱ��timeout */
        tickstart = systick_getCount();
        while(fifo->waitReadCount >= fifo->buffSize)
        {
            if(timeout == 0 || systick_getCount() - tickstart > timeout)
            {
                return (num - count);
            }
        }
        
        if(fifo->waitReadCount < fifo->buffSize)
        {
            *fifo->pWirte = *pbuff++;
            if(++fifo->pWirte >= (fifo->pBuff + fifo->buffSize))
            {
                fifo->pWirte = fifo->pBuff;
            }

            if(fifo->lock != NULL)
            {
                fifo->lock(&pri_mask);
            }
            
            fifo->waitReadCount++;

            if(fifo->unlock != NULL)
            {
                fifo->unlock(&pri_mask);
            }
            count--;
        }
    }

    return (num - count);
}

/**
 ****************************************************************************
 * @brief  ��̽���ݣ���Ӱ���ȡָ��
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval ʵ�ʶ�ȡ���ֽ���
 ****************************************************************************
*/
uint32_t fifo_peep(fifo_t *fifo, uint8_t *pbuff, uint32_t num)
{
    uint32_t count = num;
    uint8_t *pread;
    uint32_t noread;
    
    pread = fifo->pRead;
    noread = fifo->waitReadCount;

    while(count)
    {
        if(noread > 0)
        {
            noread--;
            *pbuff++ = *pread;
            if(++pread >= (fifo->pBuff + fifo->buffSize))
            {
                pread = fifo->pBuff;
            }
            count--;
        }
        else
        {
            break;
        }
    }

    return (num - count);
}

/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   ��ʱ������������δ�ʱ�����������жϺ����У�timeout����Ϊ0
 * @param  None
 * @retval ʵ�ʶ�ȡ���ֽ���
 ****************************************************************************
*/
uint32_t fifo_read(fifo_t *fifo, uint8_t *pbuff, uint32_t num, uint32_t timeout)
{
    uint32_t count = num;
    uint32_t pri_mask;
    uint32_t tickstart;

    while(count)
    {
        // �ȴ����ݣ���ʱʱ��timeout
        tickstart = systick_getCount();
        while(fifo->waitReadCount == 0)
        {
            if(timeout == 0 || systick_getCount() - tickstart > timeout)
            {
                return (num - count);
            }
        }
        
        if(fifo->waitReadCount > 0)
        {
            *pbuff++ = *fifo->pRead;
            if(++fifo->pRead >= (fifo->pBuff + fifo->buffSize))
            {
                fifo->pRead = fifo->pBuff;
            }

            if(fifo->lock != NULL)
            {
                fifo->lock(&pri_mask);
            }
            
            fifo->waitReadCount--;

            if(fifo->unlock != NULL)
            {
                fifo->unlock(&pri_mask);
            }
            count--;
        }
    }

    return (num - count);
}


/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval ����δ��ȡ���ֽ���
 ****************************************************************************
*/
uint32_t fifo_waitReadCount(fifo_t *fifo)
{
    return fifo->waitReadCount;
}


/*********************************** End of file *****************************************/
