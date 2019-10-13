/*************************************************************************
*
*   Used with ICCARM and AARM.
*
*    (c) Copyright IAR Systems 2017
*
*    File name   : Flashloader_IMXRT.c
*    Description : iMX.RT Series flash loader
*
*    History :
*    1. Date        : April, 2018
*       Author      : Alex Yzhov
*       Description : Initial revision
*
*
*    $Revision: 5068 $
**************************************************************************/

/** include files **/
#include <intrinsics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "flash_loader.h"               // The flash loader framework API declarations.
#include "flash_loader_extra.h"

#include "sfdp.h"

#include "device.h"

static const device_t *device;

/*************************************************************************
* Function Name: FlashInit
* Parameters: Flash Base Address
*
* Return:  0 - Init Successful
*          1 - Init Fail
* Description: Init flash and build layout.
*
*************************************************************************/
#if USE_ARGC_ARGV
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags,
                   int argc, char const *argv[])
#else
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags)
#endif /* USE_ARGC_ARGV */
{
    uint32_t result = RESULT_OK;

    device = &flash_device;

    /*init*/
#if USE_ARGC_ARGV
    result = device->init(base_of_flash, argc, argv);
#else
    result = device->init(base_of_flash);
#endif /* USE_ARGC_ARGV */

    //if(RESULT_ERROR != result)
    //{
    //    if (FLAG_ERASE_ONLY & flags)
    //    {
    //        if(device->erase_chip)
    //        {
    //            result = device->erase_chip();
    //        }
    //    }
    //}

    //for(uint8_t i=0; i<15; i++)
    //{
    //    device->erase((void *)(0x60000000 + i*0x1000));
    //}
    //
    //char buffer[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    //                 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    ////device->write(0x60000200, sizeof(buffer), buffer);
    //FlashWrite((void *)0x60000200, 0u, sizeof(buffer), buffer);

    return result;
}

/*************************************************************************
* Function Name: FlashWrite
* Parameters: block base address, offet in block, data size, ram buffer
*             pointer
* Return: RESULT_OK - Write Successful
*         RESULT_ERROR - Write Fail
* Description. Writes data in to NOR
*************************************************************************/
uint32_t FlashWrite(void *block_start,
                    uint32_t offset_into_block,
                    uint32_t count,
                    char const *buffer)
{
    SFDP_DEBUG("FlashWrite start: 0x%08x, offset: %d, count: %d, buffer: 0x%08x", (uint32_t)block_start+offset_into_block, offset_into_block, count, buffer);

    uint32_t status = device->write((uint32_t)block_start+offset_into_block, count, buffer);

    SFDP_DEBUG("FlashWrite status: %d", status);

    return status;
}

/*************************************************************************
* Function Name: FlashErase
* Parameters:  Block Address, Block Size
*
* Return: RESULT_OK - Erase Successful
*         RESULT_ERROR - Erase Fail
* Description: Erase block
*************************************************************************/
uint32_t FlashErase(void *block_start,
                    uint32_t block_size)
{
    return device->erase(block_start);
}

OPTIONAL_SIGNOFF

uint32_t FlashSignoff(void)
{
    uint32_t result = RESULT_OK;

    if(device->signoff)
    {
        result = device->signoff();
    }

    return result;
}

#if USE_ARGC_ARGV
const char* FlFindOption(char* option, int with_value, int argc, char const* argv[])
{
    int i;

    for (i = 0; i < argc; i++)
    {
        if (strcmp(option, argv[i]) == 0)
        {
            if (with_value)
            {
                if (i + 1 < argc)
                    return argv[i + 1]; // The next argument is the value.
                else
                    return 0;           // The option was found but there is no value to return.
            }
            else
            {
                return argv[i];         // Return the flag argument itself just to get a non-zero pointer.
            }
        }
    }
    return 0;
}
#endif /*USE_ARGC_ARGV*/