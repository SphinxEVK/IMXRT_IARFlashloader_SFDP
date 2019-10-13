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

#include "sfdp.h"

#include "fsl_lpspi.h"

////////////////////////////////////////////////////////////////////////////////

sfdp_flash flash_table[] = SFDP_FLASH_DEVICE_TABLE;
static uint8_t sfdp_table[sizeof(sfdp_para_table_t)] = { 0 };
sfdp_para_table_t *sfdp_para_table = (sfdp_para_table_t *)sfdp_table;

////////////////////////////////////////////////////////////////////////////////

static sfdp_err read_jedec_id(sfdp_flash *flash);
static bool read_sfdp(sfdp_flash *flash);
static bool read_sfdp_header(sfdp_flash *flash);
static bool read_basic_header(const sfdp_flash *flash, sfdp_para_header_t *basic_header);
static bool read_basic_table(sfdp_flash *flash, sfdp_para_header_t *basic_header);
static sfdp_err read_sfdp_data(const sfdp_flash *flash, uint32_t addr, uint8_t *read_buf, size_t size);

/* ../port/sfup_port.c */
extern void sfud_log_debug(const char *file, const long line, const char *format, ...);
extern void sfud_log_info(const char *format, ...);

////////////////////////////////////////////////////////////////////////////////

sfdp_err sfdp_init(void) {
    extern sfdp_err sfdp_spi_port_init(sfdp_flash *flash);
    
    sfdp_err result = SFDP_SUCCESS;
    sfdp_flash *flash = &flash_table[0];
    
    //Initialize Configured Porting hardware
    result = sfdp_spi_port_init(flash);
    if (result != SFDP_SUCCESS) {
        return result;
    }
    
    //Initialize SFDP configuration
    result = read_jedec_id(flash);
    if (result != SFDP_SUCCESS) {
        return result;
    }
    
    if (!read_sfdp(flash)) {
        return result;
    }
    
    return result;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Read JEDEC basic information
 *
 * @param flash flash device
 *
 * @return SFDP_SUCCESS: read OK
 */
static sfdp_err read_jedec_id(sfdp_flash *flash) {
    sfdp_err result = SFDP_SUCCESS;
    const sfdp_spi *spi = &flash->spi;
    uint8_t cmd_data[1] = {SFDP_CMD_JEDEC_ID};
    uint8_t recv_data[3] = { 0, 0, 0 };

    SFDP_ASSERT(flash);

    cmd_data[0] = SFDP_CMD_JEDEC_ID;
    result = spi->wr(spi, cmd_data, sizeof(cmd_data), recv_data, sizeof(recv_data));
    
    if (result == SFDP_SUCCESS) {
        flash->chip.mf_id = recv_data[0];
        flash->chip.type_id = recv_data[1];
        flash->chip.capacity_id = recv_data[2];
        SFDP_DEBUG("The flash device manufacturer ID is 0x%02X, memory type ID is 0x%02X, capacity ID is 0x%02X.",
                flash->chip.mf_id, flash->chip.type_id, flash->chip.capacity_id);
    } else {
        SFDP_INFO("Error: Read flash device JEDEC ID error.");
    }

    return result;
}

/**
 * Read SFDP parameter information
 *
 * @param flash flash device
 *
 * @return true: read OK
 */
static bool read_sfdp(sfdp_flash *flash) {
    SFDP_ASSERT(flash);

    /* JEDEC basic flash parameter header */
    sfdp_para_header_t basic_header;
    if (read_sfdp_header(flash) && read_basic_header(flash, &basic_header)) {
        return read_basic_table(flash, &basic_header);
    } else {
        SFDP_INFO("Warning: Read SFDP parameter header information failed. The %s does not support JEDEC SFDP.", flash->name);
        return false;
    }
}

/**
 * Read SFDP parameter header
 *
 * @param flash flash device
 *
 * @return true: read OK
 */
static bool read_sfdp_header(sfdp_flash *flash) {
    sfdp_para *sfdp = &flash->sfdp;
    /* The SFDP header is located at address 000000h of the SFDP data structure.
     * It identifies the SFDP Signature, the number of parameter headers, and the SFDP revision numbers. */
    /* sfdp parameter header address */
    uint32_t header_addr = 0;
    /* each parameter header being 2 DWORDs (64-bit) */
    uint8_t header[2 * 4] = { 0 };

    SFDP_ASSERT(flash);

    sfdp->available = false;
    /* read SFDP header */
    if (read_sfdp_data(flash, header_addr, header, sizeof(header)) != SFDP_SUCCESS) {
        SFDP_INFO("Error: Can't read SFDP header.");
        return false;
    }
    /* check SFDP header */
    if (!(header[0] == 'S' &&
          header[1] == 'F' &&
          header[2] == 'D' &&
          header[3] == 'P')) {
        SFDP_DEBUG("Error: Check SFDP signnature error. It's must be 50444653h('S' 'F' 'D' 'P').");
        return false;
    }
    sfdp->minor_rev = header[4];
    sfdp->major_rev = header[5];
    if (sfdp->major_rev > SUPPORT_MAX_SFDP_MAJOR_REV) {
        SFDP_INFO("Error: This reversion(V%d.%d) SFDP is not supported.", sfdp->major_rev, sfdp->minor_rev);
        return false;
    }
    SFDP_DEBUG("Check SFDP header is OK. The reversion is V%d.%d, NPN is %d.", sfdp->major_rev, sfdp->minor_rev,
            header[6]);

    return true;
}

/**
 * Read JEDEC basic parameter header
 *
 * @param flash flash device
 *
 * @return true: read OK
 */
static bool read_basic_header(const sfdp_flash *flash, sfdp_para_header_t *basic_header) {
    /* The basic parameter header is mandatory, is defined by this standard, and starts at byte offset 08h. */
    uint32_t header_addr = 8;
    /* each parameter header being 2 DWORDs (64-bit) */
    uint8_t header[2 * 4] = { 0 };

    SFDP_ASSERT(flash);
    SFDP_ASSERT(basic_header);

    /* read JEDEC basic flash parameter header */
    if (read_sfdp_data(flash, header_addr, header, sizeof(header)) != SFDP_SUCCESS) {
        SFDP_INFO("Error: Can't read JEDEC basic flash parameter header.");
        return false;
    }
    basic_header->id        = header[0];
    basic_header->minor_rev = header[1];
    basic_header->major_rev = header[2];
    basic_header->len       = header[3];
    basic_header->ptp       = (long)header[4] | (long)header[5] << 8 | (long)header[6] << 16;
    /* check JEDEC basic flash parameter header */
    if (basic_header->major_rev > SUPPORT_MAX_SFDP_MAJOR_REV) {
        SFDP_INFO("Error: This reversion(V%d.%d) JEDEC basic flash parameter header is not supported.",
                basic_header->major_rev, basic_header->minor_rev);
        return false;
    }
    if (basic_header->len < BASIC_TABLE_LEN) {
        SFDP_INFO("Error: The JEDEC basic flash parameter table length (now is %d) error.", basic_header->len);
        return false;
    }
    SFDP_DEBUG("Check JEDEC basic flash parameter header is OK. The table id is %d, reversion is V%d.%d,"
            " length is %d, parameter table pointer is 0x%06lX.", basic_header->id, basic_header->major_rev,
            basic_header->minor_rev, basic_header->len, basic_header->ptp);

    return true;
}

/**
 * Read JEDEC basic parameter table
 *
 * @param flash flash device
 *
 * @return true: read OK
 */
static bool read_basic_table(sfdp_flash *flash, sfdp_para_header_t *basic_header) {
    sfdp_para *sfdp = &flash->sfdp;
    /* parameter table address */
    uint32_t table_addr = basic_header->ptp;
    /* temporary variables */
    uint8_t i, j;

    SFDP_ASSERT(flash);
    SFDP_ASSERT(basic_header);

    /* read JEDEC basic flash parameter table */
    if (read_sfdp_data(flash, table_addr, sfdp_table, sizeof(sfdp_table)) != SFDP_SUCCESS) {
        SFDP_INFO("Warning: Can't read JEDEC basic flash parameter table.");
        return false;
    }
    
    flash->sfdp_table = (sfdp_para_table_t *)sfdp_table;
    
    /* print JEDEC basic flash parameter header info */
    SFDP_DEBUG("JEDEC basic flash parameter header info:");
    for (i = 0; i < 2; i++) {
        SFDP_DEBUG("[%04d] 0x%02X 0x%02X 0x%02X 0x%02X", i + 1, ((uint8_t *)basic_header)[i * 4 + 3], ((uint8_t *)basic_header)[i * 4 + 2], ((uint8_t *)basic_header)[i * 4 + 1],
                ((uint8_t *)basic_header)[i * 4]);
    }
    
    /* print JEDEC basic flash parameter table info */
    SFDP_DEBUG("JEDEC basic flash parameter table info:");
    SFDP_DEBUG("MSB-LSB  3    2    1    0");
    for (i = 0; i < basic_header->len; i++) {
        SFDP_DEBUG("[%04d] 0x%02X 0x%02X 0x%02X 0x%02X", i + 1, sfdp_table[i * 4 + 3], sfdp_table[i * 4 + 2], sfdp_table[i * 4 + 1],
                sfdp_table[i * 4]);
    }
    
    SFDP_DEBUG("Chip Erase will take %d seconds.", ((sfdp_para_table->DWORD11.chip_erase_time>>5)+1)*sfdp_para_table->DWORD11.chip_erase_time&0x1F);

    /* get block/sector 4 KB erase supported and command */
    sfdp->erase_4k_cmd = sfdp_table[1];
    switch (sfdp_table[0] & 0x03) {
    case 1:
        sfdp->erase_4k = true;
        SFDP_DEBUG("4 KB Erase is supported throughout the device. Command is 0x%02X.", sfdp->erase_4k_cmd);
        break;
    case 3:
        sfdp->erase_4k = false;
        SFDP_DEBUG("Uniform 4 KB erase is unavailable for this device.");
        break;
    default:
        SFDP_INFO("Error: Uniform 4 KB erase supported information error.");
        return false;
    }
    /* get write granularity */
    //TODO 目前为 1.0 所提供的方式，后期支持 V1.5 及以上的方式读取 page size
    switch ((sfdp_table[0] & (0x01 << 2)) >> 2) {
    case 0:
        sfdp->write_gran = 1;
        SFDP_DEBUG("Write granularity is 1 byte.");
        break;
    case 1:
        sfdp->write_gran = 256;
        SFDP_DEBUG("Write granularity is 64 bytes or larger.");
        break;
    }
    /* volatile status register block protect bits */
    switch ((sfdp_table[0] & (0x01 << 3)) >> 3) {
    case 0:
        /* Block Protect bits in device's status register are solely non-volatile or may be
         * programmed either as volatile using the 50h instruction for write enable or non-volatile
         * using the 06h instruction for write enable.
         */
        sfdp->sr_is_non_vola = true;
        SFDP_DEBUG("Target flash status register is non-volatile.");
        break;
    case 1:
        /* block protect bits in device's status register are solely volatile. */
        sfdp->sr_is_non_vola = false;
        SFDP_DEBUG("Block Protect bits in device's status register are solely volatile.");
        /* write enable instruction select for writing to volatile status register */
        switch ((sfdp_table[0] & (0x01 << 4)) >> 4) {
        case 0:
            sfdp->vola_sr_we_cmd = SFDP_VOLATILE_SR_WRITE_ENABLE;
            SFDP_DEBUG("Flash device requires instruction 50h as the write enable prior "
                    "to performing a volatile write to the status register.");
            break;
        case 1:
            sfdp->vola_sr_we_cmd = SFDP_CMD_WRITE_ENABLE;
            SFDP_DEBUG("Flash device requires instruction 06h as the write enable prior "
                    "to performing a volatile write to the status register.");
            break;
        }
        break;
    }
    /* get address bytes, number of bytes used in addressing flash array read, write and erase. */
    switch ((sfdp_table[2] & (0x03 << 1)) >> 1) {
    case 0:
        sfdp->addr_3_byte = true;
        sfdp->addr_4_byte = false;
        SFDP_DEBUG("3-Byte only addressing.");
        break;
    case 1:
        sfdp->addr_3_byte = true;
        sfdp->addr_4_byte = true;
        SFDP_DEBUG("3- or 4-Byte addressing.");
        break;
    case 2:
        sfdp->addr_3_byte = false;
        sfdp->addr_4_byte = true;
        SFDP_DEBUG("4-Byte only addressing.");
        break;
    default:
        sfdp->addr_3_byte = false;
        sfdp->addr_4_byte = false;
        SFDP_INFO("Error: Read address bytes error!");
        return false;
    }
    /* get flash memory capacity */
    uint32_t table2_temp = ((long)sfdp_table[7] << 24) | ((long)sfdp_table[6] << 16) | ((long)sfdp_table[5] << 8) | (long)sfdp_table[4];
    switch ((sfdp_table[7] & (0x01 << 7)) >> 7) {
    case 0:
        sfdp->capacity = 1 + (table2_temp >> 3);
        break;
    case 1:
        table2_temp &= 0x7FFFFFFF;
        if (table2_temp > sizeof(sfdp->capacity) * 8 + 3) {
            sfdp->capacity = 0;
            SFDP_INFO("Error: The flash capacity is grater than 32 Gb/ 4 GB! Not Supported.");
            return false;
        }
        sfdp->capacity = 1L << (table2_temp - 3);
        break;
    }
    SFDP_DEBUG("Capacity is %ld KBytes / %ld Mbytes.", sfdp->capacity/1024, sfdp->capacity/1024/1024);
    /* get erase size and erase command  */
    for (i = 0, j = 0; i < SFDP_SFDP_ERASE_TYPE_MAX_NUM; i++) {
        if (sfdp_table[28 + 2 * i] != 0x00) {
            sfdp->eraser[j].size = 1L << sfdp_table[28 + 2 * i];
            sfdp->eraser[j].cmd = sfdp_table[28 + 2 * i + 1];
            SFDP_DEBUG("Flash device supports %ldKB block erase. Command is 0x%02X.", sfdp->eraser[j].size / 1024,
                    sfdp->eraser[j].cmd);
            j++;
        }
    }
    /* sort the eraser size from small to large */
    for (i = 0, j = 0; i < SFDP_SFDP_ERASE_TYPE_MAX_NUM; i++) {
        if (sfdp->eraser[i].size) {
            for (j = i + 1; j < SFDP_SFDP_ERASE_TYPE_MAX_NUM; j++) {
                if (sfdp->eraser[j].size != 0 && sfdp->eraser[i].size > sfdp->eraser[j].size) {
                    /* swap the small eraser */
                    uint32_t temp_size = sfdp->eraser[i].size;
                    uint8_t temp_cmd = sfdp->eraser[i].cmd;
                    sfdp->eraser[i].size = sfdp->eraser[j].size;
                    sfdp->eraser[i].cmd = sfdp->eraser[j].cmd;
                    sfdp->eraser[j].size = temp_size;
                    sfdp->eraser[j].cmd = temp_cmd;
                }
            }
        }
    }
    
    uint8_t read_buf[3];
    uint8_t cmd[] = {
        SFDP_CMD_READ_STATUS_REGISTER,
        0x35,
        0x15
    };
    flash->spi.wr(&flash->spi, &cmd[0], sizeof(cmd), &read_buf[0], 1);
    flash->spi.wr(&flash->spi, &cmd[1], sizeof(cmd), &read_buf[1], 1);
    flash->spi.wr(&flash->spi, &cmd[2], sizeof(cmd), &read_buf[2], 1);
    
    SFDP_DEBUG("Status Register-1: 0x%02x", read_buf[0]);
    SFDP_DEBUG("Status Register-2: 0x%02x", read_buf[1]);
    SFDP_DEBUG("Status Register-3: 0x%02x", read_buf[2]);
    
    sfdp->available = true;
    return true;
}

static sfdp_err read_sfdp_data(const sfdp_flash *flash, uint32_t addr, uint8_t *read_buf, size_t size) {
    uint8_t cmd[] = {
            SFDP_CMD_READ_SFDP_REGISTER,
            (addr >> 16) & 0xFF,
            (addr >> 8) & 0xFF,
            (addr >> 0) & 0xFF,
            SFDP_DUMMY_DATA,
    };

    SFDP_ASSERT(flash);
    SFDP_ASSERT(addr < 1L << 24);
    SFDP_ASSERT(read_buf);
    SFDP_ASSERT(flash->spi.wr);
    
    flash->retry.delay();

    return flash->spi.wr(&flash->spi, cmd, sizeof(cmd), read_buf, size);
}
