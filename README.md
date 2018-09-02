# IMXRT_IARFlashloader_SFDP

#### 本工程面向i.MXRT1052型MCU，实现了基于[SFDP216协议](https://www.lijingquan.net/wp-content/uploads/2017/07/JESD216.pdf)的自适配IAR flashloader。理论上可以支持绝大多数2011年以后生产的，支持JEDEC SFDP标准的qspi-flash。

---

**工程的基本结构如下:**

**"/IMXRT_IARFlashloader/Flashloader/"**: Readonly. 该路径下有多个文件夹，是针对市面上目前存在的多款RT1052开发板建立的flashloader，仅作为参考使用，与本工程代码无关。使用对应文件夹下的.board, .flash和.out文件当作代码工程的flashloader，可以实现将代码下载至片外flash。

**"/IMXRT_IARFlashloader/Framework/"**: Readonly. IAR提供的Flashloader框架代码，无需修改任何内容。编写flashloader的关键在于需要自行实现FlashInit, FlashWrite和FlashErase三个函数的实现。

**"/IMXRT_IARFlashloader/FSL_Drivers/**": Readonly. 基于KSDK2.3的驱动代码，无需修改任何内容。

**"/IMXRT_IARFlashloader/iar/"**: 工程文件夹。

**"/IMXRT_IARFlashloader/sfdp/**": 由[SFUD](https://github.com/armink/SFUD)项目修改而来的，基于[SFDP216协议](https://www.lijingquan.net/wp-content/uploads/2017/07/JESD216.pdf)的flash自适配框架。
	
> 1. **"/IMXRT_IARFlashloader/sfdp/inc/sfdp_cfg.h"** 中可通过设置`SFDP_DEBUG_MODE`, 来选择是否打LOG。默认使用`LPUART1`输出，如需修改，请自行适配`sfdp_port.c`.
>       
> 2. **"/IMXRT_IARFlashloader/sfdp/port/sfdp_port.c"** 是移植文件，若有需求移植sfdp组件，需要自行适配平台。

本工程对`IAR flashloader`框架的适配主要在`/IMXRT_IARFlashloader/device.c`中完成，对`SFDP`框架的适配在`/IMXRT_IARFlashloader/sfdp/port/sfdp_port.c`中完成。

---

**SFDP作为相对独立的组件，可参考[SFUD](https://github.com/armink/SFUD)的移植说明自行移植到其他平台:**

> ### 2.4 移植说明
> 
> 移植文件位于 `/sfud/port/sfud_port.c` ，文件中的 `sfud_err sfud_spi_port_init(sfud_flash *flash)` 方法是库提供的移植方法，在里面完成各个设备 SPI 读写驱动（必选）、重试次数（必选）、重试接口（可选）及 SPI 锁（可选）的配置。更加详细的移植内容，可以参考 demo 中的各个平台的移植文件。

---

**flashloader使用了以下SFDP标准指令:**

`normal read: 0x03`, `write enable: 0x06`, `write enable(volatile): 0x50(**unused**)`, `erase chip: 0xC7`, `page program: 0x02`, `write status: 0x01`, `read status: 0x05`

**从flash SFDP数据表中读取了以下指令:**

`erase sector: flash_table[n].sfdp_table->DWORD1.erase_4k_cmd`

**从flash SFDP数据表中还可以额外读取以下指令(若flash不支持, 则指令值为0x00):**

`1-1-1 fastread`, `1-2-2 fastread`, `2-2-2 fastread`, `4-4-4 fastread`, `erase type 1`, `erase type 2`, `erase type 3`, `erase type 4`

**如何具体解析SFDP数据表，请参考[SFDP216协议](https://www.lijingquan.net/wp-content/uploads/2017/07/JESD216.pdf)文档**

---

**LOG输出样例**

![image](https://github.com/SphinxEVK/IMXRT_IARFlashloader_SFDP/blob/master/log.png)

