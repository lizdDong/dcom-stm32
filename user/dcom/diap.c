/**
  ******************************************************************************
  * @file    diap.c
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-6-16
  * @brief   None
  * @attention
  *
  ******************************************************************************
**/

#include "dcom.h"
#include "dflash.h"
#include "diap.h"


iap_t g_sIap;


/**
 ****************************************************************************
 * @brief  None
 * @author lizdDong
 * @note   None
 * @param  None
 * @retval None
 ****************************************************************************
*/
void iap_start(uint8_t *pkg)
{
    uint8_t namelen;

    g_sIap.completedSize = 0;
    g_sIap.writeAddr = IAP_IMAGE_ADDR;

    // 获取文件大小和文件名
    g_sIap.fileSize = pkg[0];
    g_sIap.fileSize = (g_sIap.fileSize << 8) + pkg[1];
    g_sIap.fileSize = (g_sIap.fileSize << 8) + pkg[2];
    g_sIap.fileSize = (g_sIap.fileSize << 8) + pkg[3];
    memset(g_sIap.fileName, 0, sizeof(g_sIap.fileName));
    namelen = strlen((char *)(pkg + 4));
    if(namelen < 32)
    {
        strcpy(g_sIap.fileName, (char *)(pkg + 4));
    }
    g_sIap.fStart = 1;
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
int8_t iap_writeFlash(uint8_t *pkg, uint16_t pkgSize)
{
    if(g_sIap.fStart == 1)
    {
        flash_write(g_sIap.writeAddr, pkg, pkgSize);
        g_sIap.writeAddr += pkgSize;
        g_sIap.completedSize += pkgSize;
        g_sIap.progress = g_sIap.completedSize * 100 / g_sIap.fileSize;
        return DOK;
    }
    return DEOR;
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
void iap_end(void)
{
    uint16_t iap_flag = IAP_FLAG;
    flash_write(IAP_FLAG_ADDR, (uint8_t *)&iap_flag, 2);
}



/*********************************** End of file *****************************************/
