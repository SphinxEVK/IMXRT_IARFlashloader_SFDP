/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2018, AlexYzhov, <AlexYzhov@163.com>
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
 * Function: It is the macro definition head file for this library.
 * Created on: 2018-08-31
 */

#include <stdarg.h>

#include "sfdp.h"

#include "string.h"
#include "fsl_lpuart.h"
#include "fsl_lpspi.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "clock_config.h"

static char log_buf[256];

////////////////////////////////////////////////////////////////////////////////

static void clock_init() {
    BOARD_BootClockRUN();
    CLOCK_SetPllBypass(CCM_ANALOG, kCLOCK_PllUsb1, true);
    CLOCK_InitUsb1Pll(&(clock_usb_pll_config_t){.loopDivider = 0U});    /* Start PLL3 PFD0 clock 480MHZ. */
    CLOCK_InitUsb1Pfd(kCLOCK_Pfd0,  24);   /* Set PLL3 PFD0 clock to 360MHZ. */
    
    //CLOCK_DisableClock(kCLOCK_Lpuart1);
    
    CLOCK_EnableClock(kCLOCK_Lpuart1);
    
    CLOCK_DisableClock(kCLOCK_Lpspi2);
    CLOCK_SetMux(kCLOCK_LpspiMux, 1);      /* Choose PLL3 PFD0 clock as lpspi2 source clock. */
    CLOCK_SetDiv(kCLOCK_LpspiDiv, 4);      /* flexspi clock 360/(4+1)=72M. */
    CLOCK_EnableClock(kCLOCK_Lpspi2);
    
    CLOCK_DisableClock(kCLOCK_FlexSpi);
    CLOCK_SetMux(kCLOCK_FlexspiMux, 0x3);  /* Choose PLL3 PFD0 clock as flexspi source clock. */
    CLOCK_SetDiv(kCLOCK_FlexspiDiv, 4);    /* flexspi clock 360/4=90M. */
    CLOCK_EnableClock(kCLOCK_FlexSpi);
    
    CLOCK_EnableClock(kCLOCK_Rom);
}

static void iomux_init() {

#if defined ( SPHINX_EVK )
    
    /* Set LPUART1/FlexSPI2/GPIO3 Pin mux && Pin config */
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_12_LPUART1_TX, 1U); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_13_LPUART1_RX, 1U); 
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_06_LPSPI2_PCS0, 1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_07_LPSPI2_SCK,  1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_08_LPSPI2_SD0,  1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_09_LPSPI2_SDI,  1U);
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_10_GPIO3_IO10,  1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_11_GPIO3_IO11,  1U);
    
    ////////////////////////////////////////////////////////////
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_12_LPUART1_TX, 0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_13_LPUART1_RX, 0x10F1);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_06_LPSPI2_PCS0, 0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_07_LPSPI2_SCK,  0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_08_LPSPI2_SD0,  0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_09_LPSPI2_SDI,  0x10F1);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_10_GPIO3_IO10,  0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_11_GPIO3_IO11,  0x10F1);
    
    ////////////////////////////////////////////////////////////
    
    GPIO_PinInit(GPIO3, 10U, &(gpio_pin_config_t){kGPIO_DigitalInput, 1, kGPIO_NoIntmode});
    GPIO_PinInit(GPIO3, 11U, &(gpio_pin_config_t){kGPIO_DigitalInput, 1, kGPIO_NoIntmode});
    
#elif defined ( SPHINX_DAP )
    
    /* Set LPUART1/FlexSPI2/GPIO3 Pin mux && Pin config */
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_06_LPUART1_TX, 1U); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_07_LPUART1_RX, 1U); 
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_06_LPSPI2_PCS0, 1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_07_LPSPI2_SCK,  1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_08_LPSPI2_SDO,  1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_09_LPSPI2_SDI,  1U);
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_14_GPIO1_IO30,  1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_15_GPIO1_IO31,  1U);
    
    ////////////////////////////////////////////////////////////
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_06_LPUART1_TX, 0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_07_LPUART1_RX, 0x10F1);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_06_LPSPI2_PCS0, 0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_07_LPSPI2_SCK,  0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_08_LPSPI2_SDO,  0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_09_LPSPI2_SDI,  0x10F1);
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_14_GPIO1_IO30,  0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_15_GPIO1_IO31,  0x10F1);
    
    ////////////////////////////////////////////////////////////
    
    GPIO_PinInit(GPIO1, 30U, &(gpio_pin_config_t){kGPIO_DigitalOutput, 1, kGPIO_NoIntmode});
    GPIO_PinInit(GPIO1, 31U, &(gpio_pin_config_t){kGPIO_DigitalOutput, 1, kGPIO_NoIntmode});
    
#endif
}

static void lpuart_init() {
    lpuart_config_t lpuart_config;
    LPUART_GetDefaultConfig(&lpuart_config);
    
    lpuart_config.baudRate_Bps = 115200U;
    lpuart_config.txFifoWatermark = 16;
    lpuart_config.enableTx = true;
    lpuart_config.enableRx = true;
    
    LPUART_Init(LPUART1, &lpuart_config, ((CLOCK_GetPllFreq(kCLOCK_PllUsb1)/6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U)));
}

static void lpspi_init() {
    static lpspi_master_config_t lpspi_config = {
        .baudRate = 36*1000*1000U,
        .bitsPerFrame = 8,
        .cpol = kLPSPI_ClockPolarityActiveHigh,
        .cpha = kLPSPI_ClockPhaseFirstEdge,
        .direction = kLPSPI_MsbFirst,
        .whichPcs = kLPSPI_Pcs0,
        .pcsActiveHighOrLow = kLPSPI_PcsActiveLow,
        .pinCfg = kLPSPI_SdiInSdoOut,
        .dataOutConfig = kLpspiDataOutTristate,
    };

    lpspi_config.pcsToSckDelayInNanoSec = 1000*1000*1000 / lpspi_config.baudRate;
    lpspi_config.lastSckToPcsDelayInNanoSec = 1000*1000*1000 / lpspi_config.baudRate;
    lpspi_config.betweenTransferDelayInNanoSec = 1000*1000*1000 / lpspi_config.baudRate;
    
    const lpspi_master_config_t *p_lpspi_config = &lpspi_config;
    
    LPSPI_Deinit(LPSPI2);
    LPSPI_MasterInit(LPSPI2, p_lpspi_config, CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk)/(CLOCK_GetDiv(kCLOCK_LpspiDiv) + 1U));
}

static void spi_lock(const sfdp_spi *spi) {
    __disable_irq();
}

static void spi_unlock(const sfdp_spi *spi) {
    __enable_irq();
}

/* about 100 microsecond delay */
static void retry_delay_100us(void) {
    volatile uint32_t delay = (CLOCK_GetFreq(kCLOCK_CpuClk)/(10*1000));
    while(delay--);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * SPI write data then read data
 */
static sfdp_err spi_write_read(const sfdp_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
        size_t read_size) {
    sfdp_err result = SFDP_SUCCESS;
    static uint8_t lpspiXfer_buf[48];
    
    memset(lpspiXfer_buf, SFDP_DUMMY_DATA, (sizeof(lpspiXfer_buf)/sizeof(uint8_t)));
    
    memcpy((uint8_t *)lpspiXfer_buf, (uint8_t *)write_buf, write_size);
    
    lpspi_transfer_t lpspiXfer = {
        .rxData = (uint8_t *)lpspiXfer_buf,
        .txData = (uint8_t *)lpspiXfer_buf,
        .dataSize = write_size+read_size,
        .configFlags = kLPSPI_MasterPcs0|kLPSPI_MasterPcsContinuous,
    };
    
    result = (sfdp_err)LPSPI_MasterTransferBlocking(LPSPI2, &lpspiXfer);
    memcpy((uint8_t *)read_buf, (uint8_t *)(lpspiXfer_buf+write_size), read_size);
    
    return result;
}

sfdp_err sfdp_spi_port_init(sfdp_flash *flash) {
    sfdp_err result = SFDP_SUCCESS;

    /**
     * add your port spi bus and device object initialize code like this:
     * 1. rcc initialize
     * 2. gpio initialize
     * 3. spi device initialize
     * 4. flash->spi and flash->retry item initialize
     *    flash->spi.wr = spi_write_read; //Required
     *    flash->spi.lock = spi_lock;
     *    flash->spi.unlock = spi_unlock;
     *    flash->spi.user_data = &spix;
     *    flash->retry.delay = null;
     *    flash->retry.times = 10000; //Required
     */
    
    clock_init();
    iomux_init();
    lpuart_init();
    lpspi_init();
    
    /* 同步 Flash 移植所需的接口及数据 */
    flash->spi.wr = spi_write_read;
    flash->spi.lock = spi_lock;
    flash->spi.unlock = spi_unlock;
    /* about 100 microsecond delay */
    flash->retry.delay = retry_delay_100us;
    /* adout 60 seconds timeout */
    flash->retry.times = 10000;

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
void sfdp_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    LPUART_WriteBlocking(LPUART1, "[SFDP]", strlen("[SFDP]"));
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    LPUART_WriteBlocking(LPUART1, (const uint8_t *)log_buf, strlen(log_buf));
    LPUART_WriteBlocking(LPUART1, "\r\n", strlen("\r\n"));
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfdp_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    LPUART_WriteBlocking(LPUART1, "[SFDP]", strlen("[SFDP]"));
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    LPUART_WriteBlocking(LPUART1, (const uint8_t *)log_buf, strlen(log_buf));
    LPUART_WriteBlocking(LPUART1, "\r\n", strlen("\r\n"));
    va_end(args);
}
