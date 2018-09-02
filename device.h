/*************************************************************************
 *
 * device definitons
 *
 **************************************************************************/
#include <stdint.h>
#include "fsl_common.h"
#include "flash_config.h"

#if defined(IMXRT1052_EVK)
	#define CPU_MIMXRT1052
#elif defined(SPHINX_EVK)
	#define CPU_MIMXRT1052
#elif defined(SHAREBOARD)
	#define CPU_MIMXRT1052
#elif defined(MCIMXRT1020)
#endif

#if defined(MCIMXRT1020)
	#include "MIMXRT1021.h"
#elif defined(CPU_MIMXRT1052)
	#include "MIMXRT1052.h"
#endif
     
////////////////////////////////////////////////////////////////////////////////
/*
 * User Definitions
 */
     
#define FLASH_SIZE 		0x1000          // 4MBytes
     
//LUT index
#define NOR_CMD_LUT_SEQ_IDX_READ_NORMAL 			0
//#define NOR_CMD_LUT_SEQ_IDX_READ_FAST                         1
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD 			2
//#define NOR_CMD_LUT_SEQ_IDX_READ_EA_STATUS 			3
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE 			4

#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR 			5
#define NOR_CMD_LUT_SEQ_IDX_ERASEBLOCK 				6

#define NOR_CMD_LUT_SEQ_IDX_ERASECHIP				7 		 		
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE 		        8
//#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD 		        9
//#define NOR_CMD_LUT_SEQ_IDX_READID                            9
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG 			10
//#define NOR_CMD_LUT_SEQ_IDX_ENTERQPI 				11
//#define NOR_CMD_LUT_SEQ_IDX_EXITQPI 				12
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG 			13
//#define NOR_CMD_LUT_SEQ_IDX_WRITE_EA_STATUS			14
#define NOR_CMD_LUT_SEQ_IDX_WRITE_ENABLE_VOLATILE               15
     
//Control Definitions
#define FLASH_BUSY_STATUS_POL 		1
#define FLASH_BUSY_STATUS_OFFSET 	0
     
//FLEXSPI Instruction operand[7:0]
#define FLEXSPI_INSTRUCTION_OPERAND_ANY_NONE_ZERO_VALUE         0x04
     
////////////////////////////////////////////////////////////////////////////////

/*device struct*/
typedef struct{
#if USE_ARGC_ARGV
  uint32_t (*init)(void *base_of_flash,int argc, char const *argv[]);
#else
  uint32_t (*init)(void *base_of_flash);
#endif /* USE_ARGC_ARGV */
  uint32_t (*write)(uint32_t addr,uint32_t count,char const *buffer);
  uint32_t (*erase)(void *block_start);
  uint32_t (*erase_chip)(void);
  uint32_t (*signoff)(void);
} device_t;

/** necessary functions as device_t call-backs **/
#if USE_ARGC_ARGV
static uint32_t init(void *base_of_flash,int argc, char const *argv[]);
#else
static uint32_t init(void *base_of_flash);
#endif /* USE_ARGC_ARGV */
static uint32_t write(uint32_t addr,uint32_t count,char const *buffer);
static uint32_t erase(void *block_start);
static uint32_t erase_chip(void);
static uint32_t signoff(void);

extern const device_t flash_device;

////////////////////////////////////////////////////////////////////////////////

static void flexspi_set_iomux(void);
static void flexspi_init(void);
static void flexspi_set_lut(void);

/** internal functions to check status **/
static status_t flexspi_nor_Erase_Sector(FLEXSPI_Type *base, uint32_t address);
static status_t flexspi_nor_Write_Page(FLEXSPI_Type *base, uint32_t dstAddr, uint32_t *src);
static status_t flexspi_nor_Write_Enable(FLEXSPI_Type *base);
static status_t flexspi_nor_Wait_Bus_If_Busy(FLEXSPI_Type *base);
