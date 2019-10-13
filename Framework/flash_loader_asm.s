;---------------------------------
;
; Copyright (c) 2008 IAR Systems
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;     http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.
;
; $Revision: 117855 $
;
; Functions accessed by the debugger to perform a flash download.
; All public symbols and the function FlashBreak() are looked up and called by the debugger.
;
;---------------------------------

#define _CORTEX_  ((__CORE__ == __ARM6M__) || (__CORE__ == __ARM6SM__) || (__CORE__ == __ARM7M__) \
|| (__CORE__ == __ARM7EM__) || (__CORE__ == __ARM8M__) || (__CORE__ == __ARM8M_BASELINE__)        \
|| (__CORE__ == __ARM8M_MAINLINE__) || (__CORE__ == __ARM8EM_MAINLINE__))

        PUBLIC FlashInitEntry
        PUBLIC FlashWriteEntry
        PUBLIC FlashEraseWriteEntry
        PUBLIC FlashChecksumEntry
        PUBLIC FlashSignoffEntry
        PUBLIC FlashBufferStart
        PUBLIC FlashBufferEnd

        EXTERN FlashBreak
        EXTERN Fl2FlashInitEntry
        EXTERN Fl2FlashWriteEntry
        EXTERN Fl2FlashEraseWriteEntry
        EXTERN Fl2FlashChecksumEntry
        EXTERN Fl2FlashSignoffEntry

        SECTION CSTACK:DATA:NOROOT(3)


;---------------------------------
;
; FlashInitEntry()
; Debugger interface function
;
;---------------------------------
        SECTION .text:CODE:ROOT(2)
#if !_CORTEX_
        ARM
#else
        THUMB
#endif

FlashInitEntry:
#if !_CORTEX_
        ;; Set up the normal stack pointer.
        LDR     sp, =SFE(CSTACK)        ; End of CSTACK
#endif
        BL       Fl2FlashInitEntry
        BL       FlashBreak


;---------------------------------
;
; FlashWriteEntry()
; Debugger interface function
;
;---------------------------------
        SECTION .text:CODE:ROOT(2)
#if !_CORTEX_
        ARM
#else
        THUMB
#endif

FlashWriteEntry:
        BL       Fl2FlashWriteEntry
        BL       FlashBreak


;---------------------------------
;
; FlashEraseWriteEntry
; Debugger interface function
;
;---------------------------------
        SECTION .text:CODE:ROOT(2)
#if !_CORTEX_
        ARM
#else
        THUMB
#endif

FlashEraseWriteEntry:
        BL       Fl2FlashEraseWriteEntry
        BL       FlashBreak


;---------------------------------
;
; FlashChecksumEntry
; Debugger interface function
;
;---------------------------------
        SECTION .text:CODE:NOROOT(2)
#if !_CORTEX_
        ARM
#else
        THUMB
#endif

FlashChecksumEntry:
        BL       Fl2FlashChecksumEntry
        BL       FlashBreak


;---------------------------------
;
; FlashSignoffEntry
; Debugger interface function
;
;---------------------------------
        SECTION .text:CODE:NOROOT(2)
#if !_CORTEX_
        ARM
#else
        THUMB
#endif

FlashSignoffEntry:
        BL       Fl2FlashSignoffEntry
        BL       FlashBreak


;---------------------------------
;
; Flash buffer and Cortex stack
;
;---------------------------------
        SECTION LOWEND:DATA(8)
        DATA
FlashBufferStart:

        SECTION HIGHSTART:DATA
        DATA
FlashBufferEnd:


#if _CORTEX_
        PUBLIC __vector_table

        SECTION .intvec:CODE:ROOT(2)
        DATA

__vector_table:
        DC32    SFE(CSTACK)
        DC32    FlashInitEntry
        ; The following dummy entries are needed for NXP devices that requires a checksum for the range __vector_table-__vector_table+0x1B
        DC32    NMI_Handler
        DC32    HardFault_Handler
        DC32    MemManage_Handler
        DC32    BusFault_Handler
        DC32    UsageFault_Handler
        DC32    0
__vector_table_0x1c
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     0                                             ;Reserved
        DCD     SVC_Handler                                   ;SVCall Handler
        DCD     DebugMon_Handler                              ;Debug Monitor Handler
        DCD     0                                             ;Reserved
        DCD     PendSV_Handler                                ;PendSV Handler
        DCD     SysTick_Handler                               ;SysTick Handler

        DCD     DefaultISR                                    ;DMA channel 0/16 transfer complete
        DCD     DefaultISR                                    ;DMA channel 1/17 transfer complete
        DCD     DefaultISR                                    ;DMA channel 2/18 transfer complete
        DCD     DefaultISR                                    ;DMA channel 3/19 transfer complete
        DCD     DefaultISR                                    ;DMA channel 4/20 transfer complete
        DCD     DefaultISR                                    ;DMA channel 5/21 transfer complete
        DCD     DefaultISR                                    ;DMA channel 6/22 transfer complete
        DCD     DefaultISR                                    ;DMA channel 7/23 transfer complete
        DCD     DefaultISR                                    ;DMA channel 8/24 transfer complete
        DCD     DefaultISR                                    ;DMA channel 9/25 transfer complete
        DCD     DefaultISR                                    ;DMA channel 10/26 transfer complete
        DCD     DefaultISR                                    ;DMA channel 11/27 transfer complete
        DCD     DefaultISR                                    ;DMA channel 12/28 transfer complete
        DCD     DefaultISR                                    ;DMA channel 13/29 transfer complete
        DCD     DefaultISR                                    ;DMA channel 14/30 transfer complete
        DCD     DefaultISR                                    ;DMA channel 15/31 transfer complete
        DCD     DefaultISR                                    ;DMA error interrupt channels 0-15 / 16-31
        DCD     DefaultISR                                    ;CTI0_Error
        DCD     DefaultISR                                    ;CTI1_Error
        DCD     DefaultISR                                    ;CorePlatform exception IRQ
        DCD     DefaultISR                                    ;LPUART1 TX interrupt and RX interrupt
        DCD     DefaultISR                                    ;LPUART2 TX interrupt and RX interrupt
        DCD     DefaultISR                                    ;LPUART3 TX interrupt and RX interrupt
        DCD     DefaultISR                                    ;LPUART4 TX interrupt and RX interrupt
        DCD     DefaultISR                                    ;LPUART5 TX interrupt and RX interrupt
        DCD     DefaultISR                                    ;LPUART6 TX interrupt and RX interrupt
        DCD     DefaultISR                                    ;LPUART7 TX interrupt and RX interrupt
        DCD     DefaultISR                                    ;LPUART8 TX interrupt and RX interrupt
        DCD     DefaultISR                                    ;LPI2C1 interrupt
        DCD     DefaultISR                                    ;LPI2C2 interrupt
        DCD     DefaultISR                                    ;LPI2C3 interrupt
        DCD     DefaultISR                                    ;LPI2C4 interrupt
        DCD     DefaultISR                                    ;LPSPI1 single interrupt vector for all sources
        DCD     DefaultISR                                    ;LPSPI2 single interrupt vector for all sources
        DCD     DefaultISR                                    ;LPSPI3 single interrupt vector for all sources
        DCD     DefaultISR                                    ;LPSPI4  single interrupt vector for all sources
        DCD     DefaultISR                                    ;CAN1 interrupt
        DCD     DefaultISR                                    ;CAN2 interrupt
        DCD     DefaultISR                                    ;FlexRAM address out of range Or access hit IRQ
        DCD     DefaultISR                                    ;Keypad nterrupt
        DCD     DefaultISR                                    ;TSC interrupt
        DCD     DefaultISR                                    ;GPR interrupt
        DCD     DefaultISR                                    ;LCDIF interrupt
        DCD     DefaultISR                                    ;CSI interrupt
        DCD     DefaultISR                                    ;PXP interrupt
        DCD     DefaultISR                                    ;WDOG2 interrupt
        DCD     DefaultISR                                    ;SRTC Consolidated Interrupt. Non TZ
        DCD     DefaultISR                                    ;SRTC Security Interrupt. TZ
        DCD     DefaultISR                                    ;ON-OFF button press shorter than 5 secs (pulse event)
        DCD     DefaultISR                                    ;CSU interrupt
        DCD     DefaultISR                                    ;DCP_IRQ interrupt
        DCD     DefaultISR                                    ;DCP_VMI_IRQ interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;TRNG interrupt
        DCD     DefaultISR                                    ;SJC interrupt
        DCD     DefaultISR                                    ;BEE interrupt
        DCD     DefaultISR                                    ;SAI1 interrupt
        DCD     DefaultISR                                    ;SAI1 interrupt
        DCD     DefaultISR                                    ;SAI3 interrupt
        DCD     DefaultISR                                    ;SAI3 interrupt
        DCD     DefaultISR                                    ;SPDIF interrupt
        DCD     DefaultISR                                    ;ANATOP interrupt
        DCD     DefaultISR                                    ;ANATOP interrupt
        DCD     DefaultISR                                    ;ANATOP interrupt
        DCD     DefaultISR                                    ;ANATOP interrupt
        DCD     DefaultISR                                    ;USBPHY (UTMI0), Interrupt
        DCD     DefaultISR                                    ;USBPHY (UTMI0), Interrupt
        DCD     DefaultISR                                    ;ADC1 interrupt
        DCD     DefaultISR                                    ;ADC2 interrupt
        DCD     DefaultISR                                    ;DCDC interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;Active HIGH Interrupt from INT0 from GPIO
        DCD     DefaultISR                                    ;Active HIGH Interrupt from INT1 from GPIO
        DCD     DefaultISR                                    ;Active HIGH Interrupt from INT2 from GPIO
        DCD     DefaultISR                                    ;Active HIGH Interrupt from INT3 from GPIO
        DCD     DefaultISR                                    ;Active HIGH Interrupt from INT4 from GPIO
        DCD     DefaultISR                                    ;Active HIGH Interrupt from INT5 from GPIO
        DCD     DefaultISR                                    ;Active HIGH Interrupt from INT6 from GPIO
        DCD     DefaultISR                                    ;Active HIGH Interrupt from INT7 from GPIO
        DCD     DefaultISR                                    ;Combined interrupt indication for GPIO1 signal 0 throughout 15
        DCD     DefaultISR                                    ;Combined interrupt indication for GPIO1 signal 16 throughout 31
        DCD     DefaultISR                                    ;Combined interrupt indication for GPIO2 signal 0 throughout 15
        DCD     DefaultISR                                    ;Combined interrupt indication for GPIO2 signal 16 throughout 31
        DCD     DefaultISR                                    ;Combined interrupt indication for GPIO3 signal 0 throughout 15
        DCD     DefaultISR                                    ;Combined interrupt indication for GPIO3 signal 16 throughout 31
        DCD     DefaultISR                                    ;Combined interrupt indication for GPIO4 signal 0 throughout 15
        DCD     DefaultISR                                    ;Combined interrupt indication for GPIO4 signal 16 throughout 31
        DCD     DefaultISR                                    ;Combined interrupt indication for GPIO5 signal 0 throughout 15
        DCD     DefaultISR                                    ;Combined interrupt indication for GPIO5 signal 16 throughout 31
        DCD     DefaultISR                                    ;FLEXIO1 interrupt
        DCD     DefaultISR                                    ;FLEXIO2 interrupt
        DCD     DefaultISR                                    ;WDOG1 interrupt
        DCD     DefaultISR                                    ;RTWDOG interrupt
        DCD     DefaultISR                                    ;EWM interrupt
        DCD     DefaultISR                                    ;CCM IRQ1 interrupt
        DCD     DefaultISR                                    ;CCM IRQ2 interrupt
        DCD     DefaultISR                                    ;GPC interrupt
        DCD     DefaultISR                                    ;SRC interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;GPT1 interrupt
        DCD     DefaultISR                                    ;GPT2 interrupt
        DCD     DefaultISR                                    ;PWM1 capture 0, compare 0, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM1 capture 1, compare 1, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM1 capture 2, compare 2, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM1 capture 3, compare 3, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM1 fault or reload error interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;FlexSPI0 interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;USDHC1 interrupt
        DCD     DefaultISR                                    ;USDHC2 interrupt
        DCD     DefaultISR                                    ;USBO2 USB OTG2
        DCD     DefaultISR                                    ;USBO2 USB OTG1
        DCD     DefaultISR                                    ;ENET interrupt
        DCD     DefaultISR                                    ;ENET_1588_Timer interrupt
        DCD     DefaultISR                                    ;XBAR1 interrupt
        DCD     DefaultISR                                    ;XBAR1 interrupt
        DCD     DefaultISR                                    ;ADCETC IRQ0 interrupt
        DCD     DefaultISR                                    ;ADCETC IRQ1 interrupt
        DCD     DefaultISR                                    ;ADCETC IRQ2 interrupt
        DCD     DefaultISR                                    ;ADCETC Error IRQ interrupt
        DCD     DefaultISR                                    ;PIT interrupt
        DCD     DefaultISR                                    ;ACMP interrupt
        DCD     DefaultISR                                    ;ACMP interrupt
        DCD     DefaultISR                                    ;ACMP interrupt
        DCD     DefaultISR                                    ;ACMP interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;ENC1 interrupt
        DCD     DefaultISR                                    ;ENC2 interrupt
        DCD     DefaultISR                                    ;ENC3 interrupt
        DCD     DefaultISR                                    ;ENC4 interrupt
        DCD     DefaultISR                                    ;TMR1 interrupt
        DCD     DefaultISR                                    ;TMR2 interrupt
        DCD     DefaultISR                                    ;TMR3 interrupt
        DCD     DefaultISR                                    ;TMR4 interrupt
        DCD     DefaultISR                                    ;PWM2 capture 0, compare 0, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM2 capture 1, compare 1, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM2 capture 2, compare 2, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM2 capture 3, compare 3, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM2 fault or reload error interrupt
        DCD     DefaultISR                                    ;PWM3 capture 0, compare 0, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM3 capture 1, compare 1, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM3 capture 2, compare 2, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM3 capture 3, compare 3, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM3 fault or reload error interrupt
        DCD     DefaultISR                                    ;PWM4 capture 0, compare 0, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM4 capture 1, compare 1, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM4 capture 2, compare 2, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM4 capture 3, compare 3, or reload 0 interrupt
        DCD     DefaultISR                                    ;PWM4 fault or reload error interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;Reserved interrupt
        DCD     DefaultISR                                    ;SJC ARM debug interrupt
        DCD     DefaultISR                                    ;NMI wake up
        DCD     DefaultISR                                    ;176
        DCD     DefaultISR                                    ;177
        DCD     DefaultISR                                    ;178
        DCD     DefaultISR                                    ;179
        DCD     DefaultISR                                    ;180
        DCD     DefaultISR                                    ;181
        DCD     DefaultISR                                    ;182
        DCD     DefaultISR                                    ;183
        DCD     DefaultISR                                    ;184
        DCD     DefaultISR                                    ;185
        DCD     DefaultISR                                    ;186
        DCD     DefaultISR                                    ;187
        DCD     DefaultISR                                    ;188
        DCD     DefaultISR                                    ;189
        DCD     DefaultISR                                    ;190
        DCD     DefaultISR                                    ;191
        DCD     DefaultISR                                    ;192
        DCD     DefaultISR                                    ;193
        DCD     DefaultISR                                    ;194
        DCD     DefaultISR                                    ;195
        DCD     DefaultISR                                    ;196
        DCD     DefaultISR                                    ;197
        DCD     DefaultISR                                    ;198
        DCD     DefaultISR                                    ;199
        DCD     DefaultISR                                    ;200
        DCD     DefaultISR                                    ;201
        DCD     DefaultISR                                    ;202
        DCD     DefaultISR                                    ;203
        DCD     DefaultISR                                    ;204
        DCD     DefaultISR                                    ;205
        DCD     DefaultISR                                    ;206
        DCD     DefaultISR                                    ;207
        DCD     DefaultISR                                    ;208
        DCD     DefaultISR                                    ;209
        DCD     DefaultISR                                    ;210
        DCD     DefaultISR                                    ;211
        DCD     DefaultISR                                    ;212
        DCD     DefaultISR                                    ;213
        DCD     DefaultISR                                    ;214
        DCD     DefaultISR                                    ;215
        DCD     DefaultISR                                    ;216
        DCD     DefaultISR                                    ;217
        DCD     DefaultISR                                    ;218
        DCD     DefaultISR                                    ;219
        DCD     DefaultISR                                    ;220
        DCD     DefaultISR                                    ;221
        DCD     DefaultISR                                    ;222
        DCD     DefaultISR                                    ;223
        DCD     DefaultISR                                    ;224
        DCD     DefaultISR                                    ;225
        DCD     DefaultISR                                    ;226
        DCD     DefaultISR                                    ;227
        DCD     DefaultISR                                    ;228
        DCD     DefaultISR                                    ;229
        DCD     DefaultISR                                    ;230
        DCD     DefaultISR                                    ;231
        DCD     DefaultISR                                    ;232
        DCD     DefaultISR                                    ;233
        DCD     DefaultISR                                    ;234
        DCD     DefaultISR                                    ;235
        DCD     DefaultISR                                    ;236
        DCD     DefaultISR                                    ;237
        DCD     DefaultISR                                    ;238
        DCD     DefaultISR                                    ;239
        DCD     DefaultISR                                    ;240
        DCD     DefaultISR                                    ;241
        DCD     DefaultISR                                    ;242
        DCD     DefaultISR                                    ;243
        DCD     DefaultISR                                    ;244
        DCD     DefaultISR                                    ;245
        DCD     DefaultISR                                    ;246
        DCD     DefaultISR                                    ;247
        DCD     DefaultISR                                    ;248
        DCD     DefaultISR                                    ;249
        DCD     DefaultISR                                    ;250
        DCD     DefaultISR                                    ;251
        DCD     DefaultISR                                    ;252
        DCD     DefaultISR                                    ;253
        DCD     DefaultISR                                    ;254
        DCD     0xFFFFFFFF                                    ; Reserved for user TRIM value

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B .

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B .

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B .

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B .

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B .

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B .

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B .

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B .

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B .
        
        PUBWEAK DefaultISR
        SECTION .text:CODE:REORDER:NOROOT(1)
DefaultISR
        B DefaultISR
#endif

        END
