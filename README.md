# IMXRT_IARFlashloader_sfdp

#### 本工程面向i.MXRT1052型MCU，实现了基于[SFDP216协议](https://www.lijingquan.net/wp-content/uploads/2017/07/JESD216.pdf)的自适配IAR flashloader。理论上可以支持绝大多数2011年以后生产的，支持JEDEC SFDP标准的qspi-flash。

工程的基本结构如下:

**"/IMXRT_IARFlashloader/Flashloader/"**: 该路径下有多个文件夹，是针对市面上目前存在的多款RT1052开发板建立的flashloader。使用对应文件夹下的.board, .flash和.out文件当作代码工程的flashloader，可以实现将代码下载至片外flash。

**"/IMXRT_IARFlashloader/Framework/"**: Readonly. IAR提供的Flashloader框架代码，无需修改任何内容。编写flashloader的关键在于需要自行实现FlashInit, FlashWrite和FlashErase三个函数的实现。

**"/IMXRT_IARFlashloader/FSL_Drivers/**": Readonly. 基于KSDK2.3的驱动代码，无需修改任何内容。

**"/IMXRT_IARFlashloader/iar/"**: 工程文件夹。

**"/IMXRT_IARFlashloader/sfdp/**": 由[SFUD](https://github.com/armink/SFUD)项目修改而来的，基于[SFDP216协议](https://www.lijingquan.net/wp-content/uploads/2017/07/JESD216.pdf)的flash自适配框架。
	
> 1. **"/IMXRT_IARFlashloader/sfdp/inc/sfdp_cfg.h"** 中可通过设置‘SFDP_DEBUG_MODE’, 来选择是否打LOG。默认使用LPUART1输出，如需修改，请自行适配**sfdp_port.c**.
>       
> 2. **"/IMXRT_IARFlashloader/sfdp/port/sfdp_port.c"** 是移植文件，若有需求移植sfdp组件，需要自行适配平台。

---

**SFDP作为相对独立的组件，可参考[SFUD](https://github.com/armink/SFUD)的移植说明自行移植到其他平台:**

> ### 2.4 移植说明
> 
> 移植文件位于 `/sfud/port/sfud_port.c` ，文件中的 `sfud_err sfud_spi_port_init(sfud_flash *flash)` 方法是库提供的移植方法，在里面完成各个设备 SPI 读写驱动（必选）、重试次数（必选）、重试接口（可选）及 SPI 锁（可选）的配置。更加详细的移植内容，可以参考 demo 中的各个平台的移植文件。
