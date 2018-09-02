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

#ifndef _SFDP_DEF_H_
#define _SFDP_DEF_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "sfdp_cfg.h"

////////////////////////////////////////////////////////////////////////////////

/* debug print function. Must be implement by user. */
#if SFDP_DEBUG_MODE == true
    #ifndef SFDP_DEBUG
        extern void sfdp_log_debug(const char *file, const long line, const char *format, ...);
        #define SFDP_DEBUG(...) sfdp_log_debug(__FILE__, __LINE__, __VA_ARGS__)
    #endif /* SFDP_DEBUG */
#else
        #define SFDP_DEBUG(...)
#endif /* SFDP_DEBUG_MODE */

#if SFDP_DEBUG_MODE == true
    extern void sfdp_log_info(const char *format, ...);
    #define SFDP_INFO(...)  sfdp_log_info(__VA_ARGS__)
#else
    #define SFDP_INFO(...)
#endif

/* assert for developer. */
#if SFDP_DEBUG_MODE == true
    #define SFDP_ASSERT(EXPR)                                                      \
    if (!(EXPR))                                                                   \
    {                                                                              \
        SFDP_DEBUG("(%s) has assert failed at %s.", #EXPR, __FUNCTION__);          \
        while (1);                                                                 \
    }
#else
    #define SFDP_ASSERT(EXPR)
#endif

////////////////////////////////////////////////////////////////////////////////

/* support maximum SFDP major revision by driver */
#define SUPPORT_MAX_SFDP_MAJOR_REV                  2

/* the JEDEC basic flash parameter table length is 9 DWORDs (288-bit) on JESD216 (V1.0) initial release standard */
#define BASIC_TABLE_LEN                             9

/* the smallest eraser in SFDP eraser table */
#define SMALLEST_ERASER_INDEX                       0

/* maximum number of erase type support on JESD216 (V1.0) */
#define SFDP_SFDP_ERASE_TYPE_MAX_NUM                4

/*
 * all defined supported command
 */
#ifndef SFDP_CMD_WRITE_ENABLE
#define SFDP_CMD_WRITE_ENABLE                          0x06
#endif

#ifndef SFDP_CMD_WRITE_DISABLE
#define SFDP_CMD_WRITE_DISABLE                         0x04
#endif

#ifndef SFDP_CMD_READ_STATUS_REGISTER
#define SFDP_CMD_READ_STATUS_REGISTER                  0x05
#endif

#ifndef SFDP_VOLATILE_SR_WRITE_ENABLE
#define SFDP_VOLATILE_SR_WRITE_ENABLE                  0x50
#endif

#ifndef SFDP_CMD_WRITE_STATUS_REGISTER
#define SFDP_CMD_WRITE_STATUS_REGISTER                 0x01
#endif

#ifndef SFDP_CMD_PAGE_PROGRAM
#define SFDP_CMD_PAGE_PROGRAM                          0x02
#endif

#ifndef SFDP_CMD_AAI_WORD_PROGRAM
#define SFDP_CMD_AAI_WORD_PROGRAM                      0xAD
#endif

#ifndef SFDP_CMD_ERASE_CHIP
#define SFDP_CMD_ERASE_CHIP                            0xC7
#endif

#ifndef SFDP_CMD_READ_DATA
#define SFDP_CMD_READ_DATA                             0x03
#endif

#ifndef SFDP_CMD_MANUFACTURER_DEVICE_ID
#define SFDP_CMD_MANUFACTURER_DEVICE_ID                0x90
#endif

#ifndef SFDP_CMD_JEDEC_ID
#define SFDP_CMD_JEDEC_ID                              0x9F
#endif

#ifndef SFDP_CMD_READ_UNIQUE_ID
#define SFDP_CMD_READ_UNIQUE_ID                        0x4B
#endif

#ifndef SFDP_CMD_READ_SFDP_REGISTER
#define SFDP_CMD_READ_SFDP_REGISTER                    0x5A
#endif

#ifndef SFDP_CMD_ENABLE_RESET
#define SFDP_CMD_ENABLE_RESET                          0x66
#endif

#ifndef SFDP_CMD_RESET
#define SFDP_CMD_RESET                                 0x99
#endif

#ifndef SFDP_CMD_ENTER_4B_ADDRESS_MODE
#define SFDP_CMD_ENTER_4B_ADDRESS_MODE                 0xB7
#endif

#ifndef SFDP_CMD_EXIT_4B_ADDRESS_MODE
#define SFDP_CMD_EXIT_4B_ADDRESS_MODE                  0xE9
#endif

#ifndef SFDP_WRITE_MAX_PAGE_SIZE
#define SFDP_WRITE_MAX_PAGE_SIZE                        256
#endif

/* send dummy data for read data */
#ifndef SFDP_DUMMY_DATA
#define SFDP_DUMMY_DATA                                0xFF
#endif

////////////////////////////////////////////////////////////////////////////////

/**
 * error code
 */
typedef enum {
    SFDP_SUCCESS = 0,                                      /**< success */
    SFDP_ERR_NOT_FOUND = 1,                                /**< not found or not supported */
    SFDP_ERR_READ = 3,                                     /**< read error */
    SFDP_ERR_TIMEOUT = 4,                                  /**< timeout error */
} sfdp_err;

////////////////////////////////////////////////////////////////////////////////
/**
 * the SFDP (Serial Flash Discoverable Parameters) parameter info which used on this library
 */
typedef struct {
    bool available;                              /**< available when read SFDP OK */
    uint8_t major_rev;                           /**< SFDP Major Revision */
    uint8_t minor_rev;                           /**< SFDP Minor Revision */
    uint16_t write_gran;                         /**< write granularity (bytes) */
    uint8_t erase_4k;                            /**< 4 kilobyte erase is supported throughout the device */
    uint8_t erase_4k_cmd;                        /**< 4 Kilobyte erase command */
    bool sr_is_non_vola;                         /**< status register is supports non-volatile */
    uint8_t vola_sr_we_cmd;                      /**< volatile status register write enable command */
    bool addr_3_byte;                            /**< supports 3-Byte addressing */
    bool addr_4_byte;                            /**< supports 4-Byte addressing */
    uint32_t capacity;                           /**< flash capacity (bytes) */
    struct {
        uint32_t size;                           /**< erase sector size (bytes). 0x00: not available */
        uint8_t cmd;                             /**< erase command */
    } eraser[3];                                 /**< supported eraser types table */
    //TODO lots of fast read-related stuff (like modes supported and number of wait states/dummy cycles needed in each)
    
} sfdp_para, *sfdp_para_t;

/**
 *  SFDP parameter header structure
 */
typedef struct {
    uint8_t id;                                  /**< Parameter ID LSB */
    uint8_t minor_rev;                           /**< Parameter minor revision */
    uint8_t major_rev;                           /**< Parameter major revision */
    uint8_t len;                                 /**< Parameter table length(in double words) */
    uint32_t ptp;                                /**< Parameter table 24bit pointer (byte address) */
} sfdp_para_header_t;

/**
 *  SFDP parameter table structure
 */
#pragma pack(1)
typedef struct {
    union {
        struct {
            /*LSB*/
            uint32_t erase_gran:2;                  //指示flash Block/Sector粒度
            uint32_t write_gran:1;                  //指示flash写入粒度
            uint32_t is_sr_volatile:1;              //指示flash状态寄存器是否为非易失性(1:true, 0:false)
            uint32_t we_instruction_sel:1;          //选择使用50h或者06h作为Write Enable指令(只有当is_status_reg_volatile==1时才有效)
            uint32_t :3;
            uint32_t erase_4k_cmd:8;                //4k擦除指令
            uint32_t addr_mode:2;                   //指示flash地址模式
            uint32_t support_dtr:1;                 //指示flash是否支持DTR(双边沿)模式
            uint32_t support_112_fastread:1;        //指示flash是否支持 1-2-2 fastread 模式
            uint32_t support_144_fastread:1;        //指示flash是否支持 1-4-4 fastread 模式
            uint32_t support_114_fastread:1;        //指示flash是否支持 1-1-4 fastread 模式
            uint32_t :10;
            /*MSB*/
        };
        uint32_t value;
    }DWORD1;

    union {
        struct {
            /*LSB*/
            uint32_t flash_density:32;              //flash容量(2^N bits)
            /*MSB*/
        };
        uint32_t value;
    }DWORD2;

    union {
        struct {
            /*LSB*/
            uint32_t dummy_clocks_before_144_output:5;          //1-4-4 fastread 模式下接口等待输出时序有效时所需的dummy clock数
            uint32_t clocks_144_fastread:3;                     //1-4-4 fastread 模式下所需时钟(若不支持该模式则值为0)
            uint32_t fastread_144_cmd:8;                        //1-4-4 fastread 指令
            uint32_t dummy_clocks_before_114_output:5;          //1-1-4 fastread 模式下接口等待输出时序有效时所需的dummy clock数
            uint32_t clocks_114_fastread:3;                     //1-1-4 fastread 模式下所需时钟(若不支持该模式则值为0)
            uint32_t fastread_114_cmd:8;                        //1-1-4 fastread 指令
            /*MSB*/
        };
        uint32_t value;
    }DWORD3;

    union {
        struct {
            /*LSB*/
            uint32_t dummy_clocks_before_112_output:5;          //1-1-2 fastread 模式下接口等待输出时序有效时所需的dummy clock数
            uint32_t clocks_112_fastread:3;                     //1-1-2 fastread 模式下所需时钟(若不支持该模式则值为0)
            uint32_t fastread_112_cmd:8;                        //1-1-2 fastread 指令
            uint32_t dummy_clocks_before_122_output:5;          //1-2-2 fastread 模式下接口等待输出时序有效时所需的dummy clock数
            uint32_t clocks_122_fastread:3;                     //1-2-2 fastread 模式下所需时钟(若不支持该模式则值为0)
            uint32_t fastread_122_cmd:8;                        //1-2-2 fastread 指令
            /*MSB*/
        };
        uint32_t value;
    }DWORD4;

    union {
        struct {
            /*LSB*/
            uint32_t support_222_fastread:1;        //指示flash是否支持 2-2-2 fastread 模式
            uint32_t :3;
            uint32_t support_444_fastread:1;        //指示flash是否支持 4-4-4 fastread 模式
            uint32_t :27;
            /*MSB*/
        };
        uint32_t value;
    }DWORD5;

    union {
        struct {
            /*LSB*/
            uint32_t :16;
            uint32_t dummy_clocks_before_222_output:5;          //2-2-2 fastread 模式下接口等待输出时序有效时所需的dummy clock数
            uint32_t clocks_222_fastread:3;                     //2-2-2 fastread 模式下所需时钟(若不支持该模式则值为0)
            uint32_t fastread_222_cmd:8;                        //2-2-2 fastread 指令
            /*MSB*/
        };
        uint32_t value;
    }DWORD6;

    union {
        struct {
            /*LSB*/
            uint32_t :16;
            uint32_t dummy_clocks_before_444_output:5;          //4-4-4 fastread 模式下接口等待输出时序有效时所需的dummy clock数
            uint32_t clocks_444_fastread:3;                     //4-4-4 fastread 模式下所需时钟(若不支持该模式则值为0)
            uint32_t fastread_444_cmd:8;                        //4-4-4 fastread 指令
            /*MSB*/
        };
        uint32_t value;
    }DWORD7;

    union {
        struct {
            /*LSB*/
            uint32_t erase_type1_size:8;                        //第一类擦除大小
            uint32_t erase_type1_cmd:8;                         //第一类擦除命令
            uint32_t erase_type2_size:8;                        //第二类擦除大小
            uint32_t erase_type2_cmd:8;                         //第二类擦除命令
            /*MSB*/
        };
        uint32_t value;
    }DWORD8;

    union {
        struct {
            /*LSB*/
            uint32_t erase_type3_size:8;                        //第三类擦除大小
            uint32_t erase_type3_cmd:8;                         //第三类擦除命令
            uint32_t erase_type4_size:8;                        //第四类擦除大小
            uint32_t erase_type4_cmd:8;                         //第四类擦除命令
            /*MSB*/
        };
        uint32_t value;
    }DWORD9;
    
    union {
        struct {
            /*LSB*/
            uint32_t ratio_to_max_erase_time:4;                 //最大擦除时间计算系数(2*(ratio+1)*Typical Time)
            uint32_t erase_type1_time:7;                        //第一类擦除所需时间
            uint32_t erase_type2_time:7;                        //第二类擦除所需时间
            uint32_t erase_type3_time:7;                        //第三类擦除所需时间
            uint32_t erase_type4_time:7;                        //第四类擦除所需时间
            /*MSB*/
        };
        uint32_t value;
    }DWORD10;
    
    union {
        struct {
            /*LSB*/
            uint32_t ratio_to_max_program_time:4;               //最大写入时间计算系数(2*(ratio+1)*Typical Time)
            uint32_t page_size:4;                               //页大小(2^N bytes)
            uint32_t page_program_time:6;                       //写单页指令所需时间
            uint32_t byte_program_time_1:5;                     //写头单字节所需时间
            uint32_t byte_program_time_2:5;                     //写后续单字节所需时间
            uint32_t chip_erase_time:7;                         //全片擦除所需时间:([6:5]+1)*[4:0]秒
            uint32_t :1;
            /*MSB*/
        };
        uint32_t value;
    }DWORD11;
    
    union {
        uint32_t value;
    }DWORD12;
    
    union {
        uint32_t value;
    }DWORD13;
    
    union {
        uint32_t value;
    }DWORD14;
    
    union {
        uint32_t value;
    }DWORD15;
    
    union {
        uint32_t value;
    }DWORD16;
    
    union {
        uint32_t value;
    }DWORD17;
    
    union {
        uint32_t value;
    }DWORD18;
    
    union {
        uint32_t value;
    }DWORD19;
    
    union {
        uint32_t value;
    }DWORD20;

}sfdp_para_table_t;

////////////////////////////////////////////////////////////////////////////////

/* flash chip information */
typedef struct {
    char       *name;                           /**< flash chip name */
    uint8_t     mf_id;                          /**< manufacturer ID */
    uint8_t     type_id;                        /**< memory type ID */
    uint8_t     capacity_id;                    /**< capacity ID */
    uint32_t    capacity;                       /**< flash capacity (bytes) */
    uint16_t    write_mode;                     /**< write mode  @see sfud_write_mode */
    uint32_t    erase_gran;                     /**< erase granularity (bytes) */
    uint8_t     erase_gran_cmd;                 /**< erase granularity size block command */
} sfdp_flash_chip;

/**
 * SPI device / Flash Communication interface
 */
typedef struct __sfdp_spi {
    /* SPI device name */
    char *name;
    /* SPI bus write read data function */
    sfdp_err (*wr)(const struct __sfdp_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
            size_t read_size);
    /* lock SPI bus */
    void (*lock)(const struct __sfdp_spi *spi);
    /* unlock SPI bus */
    void (*unlock)(const struct __sfdp_spi *spi);
    /* some user data */
    void *user_data;
} sfdp_spi, *sfdp_spi_t;

/**
 * serial flash device
 */
typedef struct {
    char *name;                                  /**< serial flash name */
    sfdp_flash_chip chip;                        /**< flash chip information */
    sfdp_spi spi;                                /**< SPI device */
    bool init_ok;                                /**< initialize OK flag */
    bool addr_in_4_byte;                         /**< flash is in 4-Byte addressing */
    struct {
        void (*delay)(void);                    /**< every retry's delay */
        size_t times;                            /**< default times for error retry */
    } retry;
    void *user_data;                             /**< some user data */

    sfdp_para          sfdp;                    /**< serial flash discoverable parameters by JEDEC standard */
    sfdp_para_header_t sfdp_header;
    sfdp_para_table_t  *sfdp_table;

} sfdp_flash, *sfdp_flash_t;

#endif
