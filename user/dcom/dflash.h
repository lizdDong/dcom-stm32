/**
  ******************************************************************************
  * @file    dflash.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-4-19
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DFLASH_H_
#define _DFLASH_H_

#include <stdint.h>
#include "stm32f10x.h"

/* define device type */
#define STM32F103xC


#if defined (STM32F103x8)
#define FLASH_SIZE                        ((uint32_t)0x10000)  /* 64 KBytes */
#define PAGE_SIZE                         ((uint32_t)0x400)    /* 1 Kbytes */
#define SRAM_SIZE                         ((uint32_t)0x5000)   /* 20 KBytes */
#elif defined (STM32F103xB)
#define FLASH_SIZE                        ((uint32_t)0x20000)  /* 128 KBytes */
#define PAGE_SIZE                         ((uint32_t)0x400)    /* 1 Kbytes */
#define SRAM_SIZE                         ((uint32_t)0x5000)   /* 20 KBytes */
#elif defined (STM32F103xC)
#define FLASH_SIZE                        ((uint32_t)0x40000)  /* 256 KBytes */
#define PAGE_SIZE                         ((uint32_t)0x800)    /* 2 Kbytes */
#define SRAM_SIZE                         ((uint32_t)0xC000)   /* 48 KBytes */
#elif defined (STM32F103xD)
#define FLASH_SIZE                        ((uint32_t)0x60000)  /* 384 KBytes */
#define PAGE_SIZE                         ((uint32_t)0x800)    /* 2 Kbytes */
#define SRAM_SIZE                         ((uint32_t)0x10000)  /* 64 KBytes */
#elif defined (STM32F103xE)
#define FLASH_SIZE                        ((uint32_t)0x80000)  /* 512 KBytes */
#define PAGE_SIZE                         ((uint32_t)0x800)    /* 2 Kbytes */
#define SRAM_SIZE                         ((uint32_t)0x10000)  /* 64 KBytes */
#else
#error "Please define the target serie such as (STM32F103xC, STM32F103xD, STM32F103xE)."
#endif

#define LAST0_PAGE    ((uint32_t)(FLASH_BASE + FLASH_SIZE - PAGE_SIZE * 1))
#define LAST1_PAGE    ((uint32_t)(FLASH_BASE + FLASH_SIZE - PAGE_SIZE * 2))
#define LAST2_PAGE    ((uint32_t)(FLASH_BASE + FLASH_SIZE - PAGE_SIZE * 3))
#define LAST3_PAGE    ((uint32_t)(FLASH_BASE + FLASH_SIZE - PAGE_SIZE * 4))


/* define the save address of the parameter */
#if (LOG_USE_LEVEL_SAVE_FLASH)
#include "dlog_cfg.h"
#define LOG_LEVEL_SAVE_FLASH_ADDR        (FLASH_BASE + FLASH_SIZE - PAGE_SIZE / 2)
#endif


#define IAP_BOOT_SIZE            (1024 * 16)

#define IAP_APP_ADDR             (FLASH_BASE + IAP_BOOT_SIZE)
#define IAP_APP_SIZE             (1024 * 96)

#define IAP_IMAGE_ADDR           (IAP_APP_ADDR + IAP_APP_SIZE)
#define IAP_IMAGE_SIZE           IAP_APP_SIZE

#define IAP_FLAG_ADDR            (FLASH_SIZE - 4)
#define IAP_FLAG                 0xA5B6


#define PARAM_SAVED_FLASH_ADDR   LAST0_PAGE
#define PARAM_SAVED_FLAG_ADDR    (CALIB_PARAM_ADDR + sizeof(param_saved_t))
#define PARAM_SAVED_FLAG         0xA5B6

typedef struct
{
    void *pParam;
    uint16_t size;
}param_saved_t;


uint32_t flash_write(uint32_t addr, const uint8_t *pBuff, uint32_t size);
uint32_t flash_read(uint32_t addr, uint8_t *pBuff, uint32_t size);

int flash_paramSave(uint8_t n);
int flash_paramLoad(uint8_t n);

#endif

















