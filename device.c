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

#include "fsl_iomuxc.h"
#include "fsl_lpuart.h"
#include "fsl_lpspi.h"
#include "fsl_flexspi.h"

#include "flash_loader.h"

#include "sfdp.h"

#include "device.h"

////////////////////////////////////////////////////////////////////////////////

#define FlexSPI_AHB_BASE        0x60000000U

////////////////////////////////////////////////////////////////////////////////

const device_t flash_device = {
    .init = init,
    .write = write,
    .erase = erase,
    .erase_chip = erase_chip,
    .signoff = signoff
};

////////////////////////////////////////////////////////////////////////////////

#if USE_ARGC_ARGV
static uint32_t init(void *base_of_flash,int argc, char const *argv[])
#else
static uint32_t init(void *base_of_flash)
#endif /* USE_ARGC_ARGV */
{
    uint32_t result = RESULT_OK;
    
    result = sfdp_init();
    if(result != RESULT_OK) {
        return result;
    }
    
    flexspi_init();
    
    SFDP_DEBUG("Flashloader Init Done.");
    
    return result;
}

static uint32_t write(uint32_t addr, uint32_t count, char const *buffer) {
    extern sfdp_flash flash_table[];
    uint32_t result = RESULT_OK;
    uint32_t index = addr-FlexSPI_AHB_BASE;
    
    const uint32_t page_size = (1<<flash_table[0].sfdp_table->DWORD11.page_size);
    uint32_t size = page_size-((uint32_t)addr%page_size);
    
    /* Write the Pages that needed */
    while(count)
    {
        SFDP_DEBUG("Writing Page at 0x%08lX", index+FlexSPI_AHB_BASE);
        
        flexspi_nor_Write_Page(FLEXSPI, index, (void*)buffer);
        if(kStatus_Success != result)
        {
            return RESULT_ERROR;
        }
        
        count -= size;
        index += size;
        buffer += size;
        size = page_size;
    }
    
    return result;
}

static uint32_t erase(void *block_start) {
    uint32_t result = RESULT_OK;
    uint32_t addr = (uint32_t)(block_start) - FlexSPI_AHB_BASE;
    
    SFDP_DEBUG("Erasing Sector at 0x%08lX", addr+FlexSPI_AHB_BASE);
    
    result = flexspi_nor_Erase_Sector(FLEXSPI, addr);    
    
    if(kStatus_Success != result)
    {
        return RESULT_ERROR;
    }
    
    return result;
}

static uint32_t erase_chip(void) {
    uint32_t result = RESULT_OK;
    flexspi_transfer_t flashXfer =
    {
        .deviceAddress = 0x00000000UL,
        .port = kFLEXSPI_PortA1,
        .cmdType = kFLEXSPI_Command,
    	.seqIndex = NOR_CMD_LUT_SEQ_IDX_ERASECHIP,
        .SeqNumber = 1,
        .data = 0x00000000UL,
        .dataSize = 0,
    };
    
    SFDP_DEBUG("Start Chip Erasing..");
    
    /* Enable Writting. */
    flexspi_nor_Write_Enable(FLEXSPI);
    
    /* Erase Chip. */
    result = FLEXSPI_TransferBlocking(FLEXSPI, &flashXfer);
    if(kStatus_Success != result)
    {
        return result;
    }
    
    return flexspi_nor_Wait_Bus_If_Busy(FLEXSPI);
}

static uint32_t signoff(void) {
    SFDP_DEBUG("Complete! Flashloader signing off..");
    SFDP_DEBUG("Deinit FLEXSPI, LPUART1 Done.");
    
    FLEXSPI_Deinit(FLEXSPI);
    LPUART_Deinit(LPUART1);
    
    return RESULT_OK;
}

////////////////////////////////////////////////////////////////////////////////

static void flexspi_set_iomux(void) {
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_06_FLEXSPIA_SS0_B,  1U); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_07_FLEXSPIA_SCLK,   1U); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_08_FLEXSPIA_DATA00, 1U); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_09_FLEXSPIA_DATA01, 1U); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_10_FLEXSPIA_DATA02, 1U); 
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_11_FLEXSPIA_DATA03, 1U); 
    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_06_FLEXSPIA_SS0_B,  0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_07_FLEXSPIA_SCLK,   0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_08_FLEXSPIA_DATA00, 0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_09_FLEXSPIA_DATA01, 0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_10_FLEXSPIA_DATA02, 0x10F1);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_11_FLEXSPIA_DATA03, 0x10F1);
    
    SFDP_DEBUG("Set FlexSPI IOMUX Done.");
}

static void flexspi_set_lut(void) {
    extern sfdp_flash flash_table[];
    uint32_t flash_lut[] =  {
        /* standard SFDP instructions */
        
        /* Normal read mode - SDR */
        [4*NOR_CMD_LUT_SEQ_IDX_READ_NORMAL] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x03, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 24),
        [4*NOR_CMD_LUT_SEQ_IDX_READ_NORMAL+1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        /* Write Enable */
        [4*NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        /* Erase Chip */
        [4*NOR_CMD_LUT_SEQ_IDX_ERASECHIP]	=
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0xC7, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
                        
        /* Page Program - single mode */
        [4*NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x02, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, (flash_table[0].addr_in_4_byte==true)?32:24),
        [4*NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE+1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
        
        /* Write result-1 register */
        [4*NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x01, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, FLEXSPI_INSTRUCTION_OPERAND_ANY_NONE_ZERO_VALUE),
            
        /* Read result register */
        [4*NOR_CMD_LUT_SEQ_IDX_READSTATUSREG] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, FLEXSPI_INSTRUCTION_OPERAND_ANY_NONE_ZERO_VALUE),
                
        /* Write Enable for volatile register */
        [4*NOR_CMD_LUT_SEQ_IDX_WRITE_ENABLE_VOLATILE] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x50, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
    };
    
    /* SFDP instructions rely on device SFDP parameter table */
    
    ///* Fast read quad mode - SDR */
    //flash_lut[4*NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD] =
    //    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, flash_table[0].sfdp_table->DWORD3.fastread_114_cmd, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, (flash_table[0].addr_in_4_byte==true)?32:24);
    //flash_lut[4*NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD+1] = 
    //    FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, flash_table[0].sfdp_table->DWORD3.dummy_clocks_before_114_output, kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, FLEXSPI_INSTRUCTION_OPERAND_ANY_NONE_ZERO_VALUE);

    /* Erase Sector  */
    flash_lut[4*NOR_CMD_LUT_SEQ_IDX_ERASESECTOR] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, flash_table[0].sfdp_table->DWORD1.erase_4k_cmd, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, (flash_table[0].addr_in_4_byte==true)?32:24);
    
    if(&flash_table[0].sfdp_table == 0) {
        SFDP_DEBUG("Failed to get SFDP parameter table.");
        return;
    }
    
    FLEXSPI_UpdateLUT(FLEXSPI, 0, flash_lut, sizeof(flash_lut)/sizeof(uint32_t));
    SFDP_DEBUG("Update FlexSPI LUT Done.");
}

static void flexspi_init(void) {
    extern sfdp_flash flash_table[];
    
    flexspi_config_t config = 
    {
        .rxSampleClock = kFLEXSPI_ReadSampleClkLoopbackInternally,
    	.enableSckFreeRunning = false,
    	.enableCombination = false,
    	.enableDoze = true,
    	.enableHalfSpeedAccess = true,
    	.enableSckBDiffOpt = false,
    	.enableSameConfigForAll = false,
    	.seqTimeoutCycle = 0xFFFFU,
    	.ipGrantTimeoutCycle = 0xFFU,
    	.txWatermark = 8,
    	.rxWatermark = 8,
    	.ahbConfig.enableAHBWriteIpTxFifo = false,
    	.ahbConfig.enableAHBWriteIpRxFifo = false,
    	.ahbConfig.ahbGrantTimeoutCycle = 0xFFU,
    	.ahbConfig.ahbBusTimeoutCycle = 0xFFFFU,
    	.ahbConfig.resumeWaitCycle = 0x20U,
        .ahbConfig.buffer = {0, 0, 0, 0},
    	.ahbConfig.enableClearAHBBufferOpt = false,
    	.ahbConfig.enableAHBPrefetch = false,
    	.ahbConfig.enableAHBBufferable = false,
    	.ahbConfig.enableAHBCachable = false,
    };
    
    flexspi_device_config_t flash_config = {
        .flexspiRootClk = 10000000UL,
        .flashSize = (flash_table[0].sfdp_table->DWORD2.flash_density>>31U)==0U?((flash_table[0].sfdp_table->DWORD2.flash_density+1)/(8*1024*1024)):(1U<<(flash_table[0].sfdp_table->DWORD2.flash_density&0x7FFFFFFF))/(8*1024*1024),
        .CSIntervalUnit = kFLEXSPI_CsIntervalUnit1SckCycle,
        .CSInterval = 2,
        .CSHoldTime = 3,
        .CSSetupTime = 3,
        .dataValidTime = 0,
        .columnspace = 0,
        .enableWordAddress = 0,
        .AWRSeqIndex = 0,
        .AWRSeqNumber = 0,
        .ARDSeqIndex = NOR_CMD_LUT_SEQ_IDX_READ_NORMAL,
        .ARDSeqNumber = 1,
        .AHBWriteWaitUnit = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
        .AHBWriteWaitInterval = 0,
    };
    
    /* DeInit LPSPI2 */
    LPSPI_Deinit(LPSPI2);
    
    /* Set FLEXSPI IOMUX. */
    flexspi_set_iomux();
    
    /* Set AHB buffer size for reading data through AHB bus. */
    FLEXSPI_Init(FLEXSPI, &config);
    
    /* Configure flash settings according to serial flash feature. */
    FLEXSPI_SetFlashConfig(FLEXSPI, &flash_config, kFLEXSPI_PortA1);
    
    /* Update LUT table. */
    flexspi_set_lut();
    
    SFDP_DEBUG("FlexSPI init Done.");
}

////////////////////////////////////////////////////////////////////////////////
static status_t flexspi_nor_Erase_Sector(FLEXSPI_Type *base, uint32_t address)
{
    status_t result = kStatus_Success;
    flexspi_transfer_t flashXfer =
    {		
        .deviceAddress = address,
        .port = kFLEXSPI_PortA1,
        .cmdType = kFLEXSPI_Command,
    	.seqIndex = NOR_CMD_LUT_SEQ_IDX_ERASESECTOR,
        .SeqNumber = 1,
        .data = 0x00000000UL,
        .dataSize = 0,
    };
    
    /* Enable Writting. */
    flexspi_nor_Write_Enable(base);
    
    /* Erase a Sector. */
    result = FLEXSPI_TransferBlocking(base, &flashXfer);
    if(kStatus_Success != result)
    {
        return result;
    }
    
    return flexspi_nor_Wait_Bus_If_Busy(base);
}

status_t flexspi_nor_Write_Page(FLEXSPI_Type *base, uint32_t dstAddr, uint32_t *src)
{
    extern sfdp_para_table_t *sfdp_para_table;
    status_t result = kStatus_Success;
    flexspi_transfer_t flashXfer = 
    {
        .deviceAddress = dstAddr,
        .port = kFLEXSPI_PortA1,
        .cmdType = kFLEXSPI_Write,
        .seqIndex = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE,
        .SeqNumber = 1,
        .data = src,
        .dataSize = (1<<sfdp_para_table->DWORD11.page_size),
    };
    
    /* Enable Writting. */
    flexspi_nor_Write_Enable(base);
    
    /* Write Page. */
    result = FLEXSPI_TransferBlocking(base, &flashXfer);
    if (kStatus_Success != result)
    {
        return result;
    }
    
    return flexspi_nor_Wait_Bus_If_Busy(base);
}

static status_t flexspi_nor_Write_Enable(FLEXSPI_Type *base)
{
    status_t result = kStatus_Success;
    flexspi_transfer_t flashXfer =
    {
        .deviceAddress = 0x00000000UL,
        .port = kFLEXSPI_PortA1,
        .cmdType = kFLEXSPI_Command,
    	.seqIndex = NOR_CMD_LUT_SEQ_IDX_WRITEENABLE,
        .SeqNumber = 1,
        .data = 0x00000000UL,
        .dataSize = 0,
    };
    
    /* Enable Writting. */
    result = FLEXSPI_TransferBlocking(base, &flashXfer);
    if(kStatus_Success != result)
    {
        return result;
    }
    
    return kStatus_Success;
}

static status_t flexspi_nor_Wait_Bus_If_Busy(FLEXSPI_Type *base)
{
    /* Wait result ready. */
    bool isBusy = true;
    uint32_t readValue;
    status_t result;
    flexspi_transfer_t flashXfer = 
    {
        .deviceAddress = 0x00000000UL,
        .port = kFLEXSPI_PortA1,
        .cmdType = kFLEXSPI_Read,
    	.seqIndex = NOR_CMD_LUT_SEQ_IDX_READSTATUSREG,
        .SeqNumber = 1,
        .data = &readValue,
        .dataSize = 1,
    };
    
    do
    {
        result = FLEXSPI_TransferBlocking(base, &flashXfer);
        if(kStatus_Success != result)
        {
            return result;
        }
        
        if(FLASH_BUSY_STATUS_POL)
        {
            if(readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = true;
            }
            else
            {
                isBusy = false;
            }
        }
        else
        {
            if(readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = false;
            }
            else
            {
                isBusy = true;
            }
        }
        
    } while(isBusy);
    
    return result;
}
