/**
 ******************************************************************************
 * @file    dflash.c
  * @author  lizdDong (lizddong@126.com)
 * @version V1.0
 * @date    2021-4-19
 * @brief   None
 * @attention
 *
 ******************************************************************************
**/

#include "dcom.h"
#include "dflash.h"

#include "app_analog.h"
param_saved_t g_sParam[] = {
    {&g_sCalibParam, sizeof(g_sCalibParam)},
    {0, 0}
};


static uint16_t FlashTemp[PAGE_SIZE / 2]; //Up to 2K bytes

/**
 ****************************************************************************
 * @brief  There is no check writing.
 * @author lizdDong
 * @note   This function can be used for all STM32F10x devices.
 * @param  writeAddr: The starting address to be written.
 * @param  pBuffer: The pointer to the data.
 * @param  numToWrite:  The number of half words written
 * @retval None
 ****************************************************************************
*/
static FLASH_Status flash_writeNoCheck(uint32_t writeAddr, uint16_t *pBuffer, uint16_t numToWrite)
{
    uint16_t i;
    FLASH_Status status = FLASH_COMPLETE;
    for(i = 0; i < numToWrite; i++)
    {
        status = FLASH_ProgramHalfWord(writeAddr, pBuffer[i]);
        if(status != FLASH_COMPLETE)
        {
            break;
        }
        writeAddr += 2;   //add addr 2.
    }
    return status;
}

/**
 ****************************************************************************
 * @brief  Write data from the specified address to the specified length.
 * @author lizdDong
 * @note   This function can be used for all STM32F10x devices.
 * @param  addr: The starting address to be written.(The address must be a multiple of two)
 * @param  pBuff: The pointer to the data.
 * @param  size: The number of byte written(8bit), The number should beat to a multiple of two.
 * @retval None
 ****************************************************************************
*/
uint32_t flash_write(uint32_t addr, const uint8_t *pBuff, uint32_t size)
{
    uint32_t secpos;     //????????
    uint16_t secoff;     //??????????????(16????????)
    uint16_t secremain;  //??????????????(16????????)
    uint16_t i;
    uint8_t f_need_erase = 0;
    uint8_t f_need_write = 0;
    uint32_t offaddr;    //????0X08000000????????

    uint32_t writeAddr = addr;
    uint16_t *pBuffer = (uint16_t *)(pBuff);    //??????????????
    uint32_t numToWrite = size / 2;             //??????????????
    uint32_t numOfWrited = 0;     //??????????????????

    if((writeAddr < FLASH_BASE) || (writeAddr >= FLASH_BASE + FLASH_SIZE))
    {
        //????????
        return 0;
    }

    if((writeAddr & 0x00000001) == 0x00000001)
    {
        //??????????????
        return 0;
    }

    if(writeAddr + size >= FLASH_BASE + FLASH_SIZE)
    {
        //????????????????????
        numToWrite = (FLASH_BASE + FLASH_SIZE - writeAddr) / 2;
    }

    offaddr = writeAddr - FLASH_BASE;     //????????????.
    secpos = offaddr / PAGE_SIZE;         //????????  0~127 for STM32F103RBT6
    secoff = (offaddr % PAGE_SIZE) / 2;   //??????????????(2????????????????.)
    secremain = PAGE_SIZE / 2 - secoff;   //????????????????(16????????)

    if(numToWrite <= secremain)
    {
        //??????????????????
        secremain = numToWrite;
    }

    //__set_PRIMASK(1);     /* ?????? */
    FLASH_Unlock();       //????
    /* Clear pending flags (if any) */
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    while(1)
    {
        //??????????????????
        flash_read(secpos * PAGE_SIZE + FLASH_BASE, (uint8_t *)FlashTemp, PAGE_SIZE);

        //????????????????????????
        for(i = 0; i < secremain; i++)
        {
            //????????????????????????????????????
            if(FlashTemp[secoff + i] != pBuffer[i])
            {
                f_need_write = 1;

                //????????????????????0xFFFF??
                if(FlashTemp[secoff + i] != 0XFFFF)
                {
                    //????????
                    f_need_erase = 1;
                    break;
                }
            }
        }

        if(f_need_erase)
        {
            //????????
            if(FLASH_ErasePage(secpos * PAGE_SIZE + FLASH_BASE) != FLASH_COMPLETE)   //????????????
            {
                return numToWrite;
            }
            for(i = 0; i < secremain; i++)
            {
                //????????????????
                FlashTemp[secoff + i] = pBuffer[i];
            }
            //????????????????
            if(flash_writeNoCheck(secpos * PAGE_SIZE + FLASH_BASE, FlashTemp, PAGE_SIZE / 2) != FLASH_COMPLETE)
            {
                return numToWrite;
            }
            numOfWrited += (secremain * 2);
        }
        else
        {
            //????????,????????????????????
            if(f_need_write)
            {
                if(flash_writeNoCheck(writeAddr, pBuffer, secremain) != FLASH_COMPLETE)
                {
                    return numToWrite;
                }
            }
            numOfWrited += (secremain * 2);
        }

        if(numToWrite == secremain)
        {
            break;           //????????
        }
        else
        {
            //????????????????
            secpos++;     //??????????1
            secoff = 0;   //??????????0
            pBuffer += secremain;            //????????
            writeAddr += (secremain * 2);    //??????????
            numToWrite -= secremain;         //????(16??)??????
            if(numToWrite > (PAGE_SIZE / 2))
            {
                //??????????????????
                secremain = PAGE_SIZE / 2;
            }
            else
            {
                //??????????????????
                secremain = numToWrite;
            }
        }
    }
    FLASH_Lock();//????
    __set_PRIMASK(0);     /* ?????? */

    return numOfWrited;
}

/**
 ****************************************************************************
 * @brief  Start reading the specified data from the specified address.
 * @author lizdDong
 * @note   This function can be used for all STM32F10x devices.
 * @param  readAddr: Start addr
 * @param  pBuff: The pointer to the data.
 * @param  size: The number of byte written(8bit)
 * @retval The real size to read
 ****************************************************************************
*/
uint32_t flash_read(uint32_t addr, uint8_t *pBuff, uint32_t size)
{
    uint32_t i;
    uint32_t readAddr = addr;

    if(readAddr + size >= FLASH_BASE + FLASH_SIZE)
    {
        //????????????????????
        size = FLASH_BASE + FLASH_SIZE - readAddr;
    }

    for(i = 0; i < size; i++)
    {
        *pBuff++ = *(uint8_t *)readAddr++;
    }

    return size;
}

/**
 ****************************************************************************
 * @brief  ????????
 * @author lizdDong
 * @note   None
 * @param  n??????????????????????param_saved_t g_sParam[]????
 * @retval The real size to saved
 ****************************************************************************
*/
int flash_paramSave(uint8_t n)
{
    uint32_t addr, num;
    uint8_t i;
    
    if(n >= sizeof(g_sParam) / sizeof(param_saved_t))
        return DEOR;
    
    addr = PARAM_SAVED_FLASH_ADDR;
    for(i = 0; i < n; i++)
    {
        addr += g_sParam[i].size;
    }
    
    num = flash_write(addr, g_sParam[n].pParam, g_sParam[n].size);
    return num;
}

/**
 ****************************************************************************
 * @brief  ????????
 * @author lizdDong
 * @note   None
 * @param  n??????????????????????param_saved_t g_sParam[]????
 * @retval The real size to saved
 ****************************************************************************
*/
int flash_paramLoad(uint8_t n)
{
    uint32_t addr, num;
    uint8_t i;
    
    if(n >= sizeof(g_sParam) / sizeof(param_saved_t))
        return DEOR;
    
    addr = PARAM_SAVED_FLASH_ADDR;
    for(i = 0; i < n; i++)
    {
        addr += g_sParam[i].size;
    }
    
    num = flash_read(addr, g_sParam[n].pParam, g_sParam[n].size);
    return num;
}



/****************************** End of file ***********************************/



