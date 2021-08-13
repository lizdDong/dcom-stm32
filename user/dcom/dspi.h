/**
  ******************************************************************************
  * @file    dev_spi.h
  * @author  lizdDong (lizddong@126.com)
  * @version V1.0
  * @date    2021-4-28
  * @brief   None
  * @attention
  *
  ******************************************************************************
  */

#ifndef _DEV_SPI_H_
#define _DEV_SPI_H_

#include "dcom.h"

//#define STM32_SPI1
#define STM32_SPI2

#define SPI_SPI                           SPI1
#define SPI_SPI_CLK                       RCC_APB2Periph_SPI1
#define SPI_SPI_SCK_PIN                   GPIO_Pin_5                  /* PA.05 */
#define SPI_SPI_SCK_GPIO_PORT             GPIOA                       /* GPIOA */
#define SPI_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOA
#define SPI_SPI_MISO_PIN                  GPIO_Pin_6                  /* PA.06 */
#define SPI_SPI_MISO_GPIO_PORT            GPIOA                       /* GPIOA */
#define SPI_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOA
#define SPI_SPI_MOSI_PIN                  GPIO_Pin_7                  /* PA.07 */
#define SPI_SPI_MOSI_GPIO_PORT            GPIOA                       /* GPIOA */
#define SPI_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOA
#define SPI_CS_PIN                        GPIO_Pin_4                  /* PA.04 */
#define SPI_CS_GPIO_PORT                  GPIOA                       /* GPIOA */
#define SPI_CS_GPIO_CLK                   RCC_APB2Periph_GPIOA

#define SPI_CS_LOW()                     GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI_CS_HIGH()                    GPIO_SetBits(GPIOA, GPIO_Pin_4)

void spi_init(void);
u8 spi_readByte(void);
u8 spi_sendByte(u8 byte);

#endif /* __SPI_H */
