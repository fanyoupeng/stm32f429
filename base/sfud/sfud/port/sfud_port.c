/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include <stdio.h>
#include "stm32f429xx.h"
#include "bsp_init.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define	W25QXX_CS 		PEout(4)  		//W25QXX的片选信号
static SemaphoreHandle_t flashMutex;

typedef struct {
    SPI_TypeDef *spix;
    GPIO_TypeDef *cs_gpiox;
    uint16_t cs_gpio_pin;
} spi_user_data, *spi_user_data_t;

static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);

static void spi_lock(const sfud_spi *spi) {
	xSemaphoreTakeRecursive(flashMutex, portMAX_DELAY);
//    __disable_irq();
}

static void spi_unlock(const sfud_spi *spi) {
	xSemaphoreGiveRecursive(flashMutex);
//    __enable_irq();
}

extern SPI_HandleTypeDef SPI4_Handler;

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
        size_t read_size) {
    sfud_err result = SFUD_SUCCESS;
    uint8_t send_data, read_data;
//    spi_user_data_t spi_dev = (spi_user_data_t) spi->user_data;

    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }

//    GPIO_ResetBits(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin);
	W25QXX_CS=0;
    /* 开始读写数据 */
    for (size_t i = 0; i < write_size + read_size; i++) {
        /* 先写缓冲区中的数据到 SPI 总线，数据写完后，再写 dummy(0xFF) 到 SPI 总线 */
        if (i < write_size) {
            send_data = *write_buf++;
        } else {
            send_data = SFUD_DUMMY_DATA;
        }
//        /* 发送数据 */
//        retry_times = 1000;
//        while (SPI_I2S_GetFlagStatus(spi_dev->spix, SPI_I2S_FLAG_TXE) == RESET) {
//            SFUD_RETRY_PROCESS(NULL, retry_times, result);
//        }
//        if (result != SFUD_SUCCESS) {
//            goto exit;
//        }
//        SPI_I2S_SendData(spi_dev->spix, send_data);
//        /* 接收数据 */
//        retry_times = 1000;
//        while (SPI_I2S_GetFlagStatus(spi_dev->spix, SPI_I2S_FLAG_RXNE) == RESET) {
//            SFUD_RETRY_PROCESS(NULL, retry_times, result);
//        }
//        if (result != SFUD_SUCCESS) {
//            goto exit;
//        }
//        read_data = SPI_I2S_ReceiveData(spi_dev->spix);
		W25QXX_CS=0;
		HAL_SPI_TransmitReceive(&SPI4_Handler, &send_data, &read_data, 1, 1000);
		
        /* 写缓冲区中的数据发完后，再读取 SPI 总线中的数据到读缓冲区 */
        if (i >= write_size) {
            *read_buf++ = read_data;
        }
    }

//exit:
//    GPIO_SetBits(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin);
	W25QXX_CS=1;
    return result;
}

/* about 100 microsecond delay */
static void retry_delay_100us(void) {
	delay_us(100);
}

static spi_user_data spi4 = { .spix = SPI4, .cs_gpiox = GPIOE, .cs_gpio_pin = GPIO_PIN_4 };
extern void W25QXX_Init(void);
extern UINT16 W25QXX_ReadID(void);
sfud_err sfud_spi_port_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;
	flashMutex = xSemaphoreCreateMutex();
    switch (flash->index) {
    case SFUD_W25Q32_DEVICE_INDEX: {
        /* FLASH 初始化 */
		W25QXX_Init();
        /* 同步 Flash 移植所需的接口及数据 */
        flash->spi.wr = spi_write_read;
        flash->spi.lock = spi_lock;
        flash->spi.unlock = spi_unlock;
        flash->spi.user_data = &spi4;
        /* about 100 microsecond delay */
        flash->retry.delay = retry_delay_100us;
        /* adout 60 seconds timeout */
        flash->retry.times = 60 * 10000;

        break;
    }
    }

    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}
