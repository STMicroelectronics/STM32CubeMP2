/*
********************************************************************************
**
**  File        : stm32mp25xx_DDR_a35_M33TD_BM.ld
**
**  Abstract    : Linker script for STM32MP25 series
**                A35 EL1 Aarch32 application in DDR to be launched
**                by jumper(fsbla).
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed "as is" without any warranty
**                of any kind.
**
********************************************************************************
** @attention
**
** <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
** All rights reserved.</center></h2>
**
** This software component is licensed by ST under BSD 3-Clause license,
** the License; You may not use this file except in compliance with the
** License. You may obtain a copy of the License at:
**                       opensource.org/licenses/BSD-3-Clause
**
********************************************************************************
*/

/******************/
/* Memory mapping */
/******************/

MEMORY
{
    FLASH (rx)            : ORIGIN = 0x82000800,  LENGTH = 512K
    RAM (rw)              : ORIGIN = ORIGIN(FLASH) + LENGTH(FLASH), LENGTH = 8M
    SCMI_SHMEM (rw)       : ORIGIN = 0x20041000,  LENGTH = 4K
    IPC_SHMEM(rw)         : ORIGIN = 0x80a00000,  LENGTH = 1M
}

IPC_SHM_VIRTIO_SIZE  = 32K;
IPC_SHM_RSC_TAB_SIZE = 1K;

/* Fixed entry point address for Reset_Handler */
__RESET_HANDLER_ADDR__ = ORIGIN(FLASH);
ENTRY(Reset_Handler)

/* Symbols needed for scmiagent to enable mailbox communication */
__SCMI_SHM_region_start__  = ORIGIN(SCMI_SHMEM);
__SCMI_SHM_region_end__    = ORIGIN(SCMI_SHMEM) + LENGTH(SCMI_SHMEM);

/* Symbols needed for OpenAMP to enable rpmsg */
__RSC_TABLE_region_start__  = ORIGIN(IPC_SHMEM);
__RSC_TABLE_region_end__    = ORIGIN(IPC_SHMEM) + IPC_SHM_RSC_TAB_SIZE;

/* Symbols needed for OpenAMP to enable rpmsg */
__OPENAMP_region_start__    = ORIGIN(IPC_SHMEM) + IPC_SHM_RSC_TAB_SIZE;
__OPENAMP_region_end__      = ORIGIN(IPC_SHMEM) + IPC_SHM_RSC_TAB_SIZE + IPC_SHM_VIRTIO_SIZE;

_Min_Heap_Size = 0x200;     /* required amount of heap  */
_Min_Stack_Size = 0x400;    /* required amount of stack */

__FIQ_STACK_SIZE = 0x400;
__IRQ_STACK_SIZE = 0x400;
__ABT_STACK_SIZE = 0x400;
__SVC_STACK_SIZE = 0x400;
__UND_STACK_SIZE = 0x400;

/* Put stacks at the end of the RAM region */
__RAM_END__ = ORIGIN(RAM) + LENGTH(RAM);

FIQ_STACK = __RAM_END__;
IRQ_STACK = FIQ_STACK - __FIQ_STACK_SIZE;
ABT_STACK = IRQ_STACK - __IRQ_STACK_SIZE;
SVC_STACK = ABT_STACK - __ABT_STACK_SIZE;
UND_STACK = SVC_STACK - __SVC_STACK_SIZE;
SYS_STACK = UND_STACK - __UND_STACK_SIZE;

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

PHDRS
{
    text    PT_LOAD FLAGS(5);  /* READ and EXECUTE */
    data    PT_LOAD FLAGS(6);  /* READ and WRITE */
    bss     PT_LOAD FLAGS(6);  /* READ and WRITE */
}

SECTIONS
{
    . = __RESET_HANDLER_ADDR__;

    .RESET __RESET_HANDLER_ADDR__ :
    {
        __TEXT_START__ = .;

        /* Your Reset_Handler is in input section ".RESET" */
        KEEP(*(.RESET))
        KEEP(*(.RESET.*))
    } > FLASH :text

    .text :
    {
        *(.text*)

        /* .init is used by libc_nano */
        KEEP (*(.init))
        KEEP (*(.fini))

        __TEXT_END_UNALIGNED__ = .;

        /*
         * Memory page(s) mapped to this section will be marked as
         * read-only, executable.  No non-executable data from the next section must
         * creep in.  Ensure the rest of the current memory page is unused.
         */
        __TEXT_END__ = .;

        . = NEXT(4096);
    } > FLASH :text

    .rodata :
    {
        __RO_START__ = .;
        *(.rodata*)

        __RO_END_UNALIGNED__ = .;
        /*
         * Memory page(s) mapped to this section will be marked as
         * read-only, non-executable.  No RW data from the next section must
         * creep in.  Ensure the rest of the current memory page is unused.
         */
        . = NEXT(4096);
        __RO_END__ = .;
    } > FLASH :text

    .ARM.extab   : {
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(4);
    } > FLASH :text

    .ARM.exidx :
    {
        . = ALIGN(4);
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        . = ALIGN(4);
    } > FLASH :text

    /* .init_array is used by libc_nano */
    .preinit_array     :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
    } > FLASH :text

    .init_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
    } > FLASH :text

    .fini_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);
    } > FLASH :text

    .data :
    {
        . = ALIGN(8);
        RW_DATA = .;
        *(.data*)
        __DATA_END__ = .;
    } > RAM :data

    .bss (NOLOAD) : ALIGN(32)
    {
        ZI_DATA = .;
        *(SORT_BY_ALIGNMENT(.bss*))
        *(COMMON)
        . = ALIGN(32);
        __BSS_END__ = .;
    } > RAM :bss

    __BSS_SIZE__ = SIZEOF(.bss);

    /* User_heap_stack section, used to check that there is enough RAM left */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } > RAM :data

    /* Remove information from the compiler libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
}
