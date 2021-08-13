/**
  ******************************************************************************
  * @file    dio.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-4-12
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#include "dcom.h"
#include "dio.h"


typedef struct
{
    uint8_t status;
    uint8_t filterCount;
} ioInput_t;

static ioInput_t g_asIoInput[IO_INPUT_COUNTS];


/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void io_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 打开GPIO时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    // IO输出配置
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // IO输入配置
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
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
static uint8_t io_read(uint8_t n)
{
    if(n == 0)        return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
    else if(n == 1)   return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
    else if(n == 2)   return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12);
    else if(n == 3)   return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
    else if(n == 4)   return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
    else if(n == 5)   return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
    else if(n == 6)   return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);
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
static void io_write(uint8_t n, uint8_t value)
{
    if(n == 0)        GPIO_WriteBit(GPIOB, GPIO_Pin_14, (BitAction)value);
    else if(n == 1)   GPIO_WriteBit(GPIOB, GPIO_Pin_15, (BitAction)value);
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
static void io_inputFilter(uint8_t curr_stu, uint8_t *pstatus, uint8_t *pfilter_count)
{
    // IO处于低电平状态
    if(curr_stu == Bit_RESET)
    {
        if(*pstatus == 1)
        {
            if(*pfilter_count > 0)
            {
                (*pfilter_count)--;
            }
            else
            {
                *pstatus = 0;
            }
        }
        else
        {
            *pfilter_count = 0;
        }
    }
    else
    {
        if(*pstatus == 0)
        {
            if(*pfilter_count < IO_INPUT_FILTER_TIMES)
            {
                (*pfilter_count)++;
            }
            else
            {
                *pstatus = 1;
            }
        }
        else
        {
            *pfilter_count = IO_INPUT_FILTER_TIMES;
        }
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
void io_inputFilterHandle(void)
{
    uint8_t i;

    for(i = 0; i < IO_INPUT_COUNTS; i++)
    {
        io_inputFilter(io_read(i), &g_asIoInput[i].status, &g_asIoInput[i].filterCount);
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
uint8_t io_getIn(uint8_t n)
{
    if(n >= IO_INPUT_COUNTS)
        return IO_STAT_UNKNOWN;

    return g_asIoInput[n].status;
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
void io_setOut(uint8_t n, uint8_t value)
{
    io_write(n, value);
}


/****************************** End of file ***********************************/

