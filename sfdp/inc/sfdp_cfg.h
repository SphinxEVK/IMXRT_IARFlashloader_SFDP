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

#ifndef _SFDP_CFG_H_
#define _SFDP_CFG_H_

////////////////////////////////////////////////////////////////////////////////

/* 是否打DEBUG LOG, I/O定向在sfdp_port.c中自行适配, 默认使用LPUART1 */
#define SFDP_DEBUG_MODE true

////////////////////////////////////////////////////////////////////////////////

/* flash型号备注, 不影响功能 */
enum {
    SFDP_W25QxxxJV_DEVICE_INDEX = 0,
    SFDP_W25QxxxFV_DEVICE_INDEX = 1
};

#define SFDP_FLASH_DEVICE_TABLE                         \
{                                                       \
    [SFDP_W25QxxxJV_DEVICE_INDEX] = {                   \
        .name = "W25QxxxJV",                            \
    },                                                  \
                                                        \
    [SFDP_W25QxxxFV_DEVICE_INDEX] = {                   \
        .name = "W25QxxxFV",                            \
    },                                                  \
}                      

////////////////////////////////////////////////////////////////////////////////

#endif
