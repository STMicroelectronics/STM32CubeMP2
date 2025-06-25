/*
******************************************************************************
**
**  File        : LinkerScript.ld (debug in RAM dedicated)
**
**  Author      : Intropack Team
**
**  Abstract    : Linker script for STM32MP2xx Device from STM32MP2 series
**
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed as is without any warranty
**                of any kind.
**
*****************************************************************************
** @attention
**
** Copyright (c) 2025 STMicroelectronics.
** All rights reserved.
**
** This software component is licensed by ST under Apache License, Version 2.0,
** the "License"; You may not use this file except in compliance with the
** License. You may obtain a copy of the License at:
**                        opensource.org/licenses/Apache-2.0
**
*****************************************************************************
*/

/*
 * STM32MP2 Non-Secure Linker Script (DDR, M33 NS)
 *
 * This linker script configures memory regions for the Non-Secure (NS) image.
 * The layout is derived from the device tree reserved-memory nodes and platform macros.
 *
 * Memory regions (from extDT: m33td/tfm/stm32mp215f-dk-cm33tdcid-ostl-resmem.dtsi):
 *   tfm_code:        0x80000000, size 0x80000   (Secure code)
 *   cm33_cube_fw:    0x80080000, size 0x80000   (Non-secure code)
 *   tfm_data:        0x80100000, size 0x100000  (Secure data)
 *   cm33_cube_data:  0x80200000, size 0x800000  (Non-secure data)
 *
 */

/* Macro definitions for linker script use */
NS_DDR_ALIAS_BASE   = 0x80000000;
IMAGE_S_CODE_SIZE   = 0x80000;
IMAGE_NS_CODE_SIZE  = 0x80000;
IMAGE_S_DATA_SIZE   = 0x100000;
IMAGE_NS_DATA_SIZE  = 0x800000;
BL2_TRAILER_SIZE    = 0x400;

NS_CODE_START = NS_DDR_ALIAS_BASE + IMAGE_S_CODE_SIZE;
NS_CODE_SIZE  = IMAGE_NS_CODE_SIZE - BL2_TRAILER_SIZE;
NS_DATA_START = NS_DDR_ALIAS_BASE + (IMAGE_S_CODE_SIZE + IMAGE_NS_CODE_SIZE) + IMAGE_S_DATA_SIZE;
NS_DATA_SIZE  = IMAGE_NS_DATA_SIZE;

/* Entry Point */
ENTRY(Reset_Handler)

/* Memories definition */
MEMORY
{
    FLASH (rx) : ORIGIN = NS_CODE_START, LENGTH = NS_CODE_SIZE
    RAM   (rw) : ORIGIN = NS_DATA_START, LENGTH = NS_DATA_SIZE
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapBase
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __Vectors_End
 *   __Vectors_Size
 */
ENTRY(Reset_Handler)

TFM_LINKER_DEFAULT_ALIGNMENT = 32;

SECTIONS
{
    .text : ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT)
    {
        KEEP(*(.vectors))
        __Vectors_End = .;
        __Vectors_Size = __Vectors_End - __Vectors;
        __end__ = .;

        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        /* uncomment .copy.table section and,
         * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
        /*
        /* .copy.table */
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__etext)
        LONG (__data_start__)
        LONG ((__data_end__ - __data_start__) / 4)
        LONG (DEFINED(__etext2) ? __etext2 : 0)
        LONG (DEFINED(__data2_start__) ? __data2_start__ : 0)
        LONG (DEFINED(__data2_start__) ? ((__data2_end__ - __data2_start__) / 4) : 0)
        __copy_table_end__ = .;
        */

        /* To clear multiple BSS sections,
         * uncomment .zero.table section and,
         * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
        /*
        /* .zero.table */
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG ((__bss_end__ - __bss_start__) / 4)
        LONG (DEFINED(__bss2_start__) ? __bss2_start__ : 0)
        LONG (DEFINED(__bss2_start__) ? ((__bss2_end__ - __bss2_start__) / 4) : 0)
        __zero_table_end__ = .;
        */

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > FLASH

    .ARM.extab : ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
    } > FLASH

    .ARM : ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT)
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        . = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
    } > FLASH

    __etext = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);

    /* Used by the startup to initialize data */
    _sidata = LOADADDR(.data);

    .data : AT (__etext)
    {
        __data_start__ = .;
        _sdata = .;        /* create a global symbol at data start */
        *(vtable)
        *(.data*)

        KEEP(*(.jcr*))
        . = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
        /* All data end */
        __data_end__ = .;
        _edata = .;        /* define a global symbol at data end */
    } > RAM

    /* Uninitialized data section into "RAM" Ram type memory */
    .bss : ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT)
    {
        /* This is used by the startup in order to initialize the .bss section */
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;
        *(.bss*)
        *(COMMON)

        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
        . = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
    } > RAM

    .heap (COPY):
    {
        __HeapBase = .;
        __end__ = .;
        end = __end__;
        KEEP(*(.heap*))
        __HeapLimit = .;
    } > RAM

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        KEEP(*(.stack*))
    } > RAM

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);
    PROVIDE(_estack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
