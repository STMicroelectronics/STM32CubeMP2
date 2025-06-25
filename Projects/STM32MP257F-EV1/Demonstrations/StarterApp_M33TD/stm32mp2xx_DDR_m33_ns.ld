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
** All rights reserved
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
 * Memory regions (from extDT : m33td/tfm/stm32mp257f-ev1-cm33tdcid-ostl-resmem.dtsi):
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

NS_CODE_START       = NS_DDR_ALIAS_BASE + IMAGE_S_CODE_SIZE;
NS_CODE_SIZE        = IMAGE_NS_CODE_SIZE - BL2_TRAILER_SIZE;
NS_DATA_START       = NS_DDR_ALIAS_BASE + (IMAGE_S_CODE_SIZE + IMAGE_NS_CODE_SIZE) + IMAGE_S_DATA_SIZE;
NS_DATA_SIZE        = IMAGE_NS_DATA_SIZE;

NS_HEAP_SIZE        = 0x0001000;
NS_STACK_SIZE       = 0x0001000;

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack             = ORIGIN(RAM) + LENGTH(RAM);    /* end of "RAM" Ram type memory */

_Min_Heap_Size      = NS_HEAP_SIZE;     /* required amount of heap  */
_Min_Stack_Size     = NS_STACK_SIZE;    /* required amount of stack */

/* Memories definition */
MEMORY
{
    FLASH   (rx)    : ORIGIN = NS_CODE_START, LENGTH = NS_CODE_SIZE
    RAM     (rw)    : ORIGIN = NS_DATA_START, LENGTH = NS_DATA_SIZE
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

TFM_LINKER_DEFAULT_ALIGNMENT = 32;

SECTIONS
{
    /* The program code and other data into "RAM" Ram type memory */
    .text : ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT)
    {
        KEEP(*(.isr_vectors))
        __Vectors_End   = .;
        __Vectors_Size  = __Vectors_End - __Vectors;
        __end__         = .;

        *(.text)            /* .text sections (code) */
        *(.text*)           /* .text* sections (code) */
        *(.glue_7)          /* glue arm to thumb code */
        *(.glue_7t)         /* glue thumb to arm code */
        *(.eh_frame)

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

        KEEP (*(.init))
        KEEP (*(.fini))

        . = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
    } >FLASH

    /* Constant data into "RAM" Ram type memory */
    .rodata : ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT)
    {
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
    } >FLASH

    .ARM.extab : ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
    } >FLASH

    .ARM : ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT)
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        . = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
    } >FLASH

    /* Used by the startup to initialize data */
    __etext = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
    _sidata = LOADADDR(.data);

    /* Initialized data sections into "RAM" Ram type memory */
    .data :  AT (__etext)
    {
        _sdata = .;        /* create a global symbol at data start */
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */

        _edata = .;        /* define a global symbol at data end */
        . = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
    } >RAM

    /* Uninitialized data section into "RAM" Ram type memory */
    .bss : ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT)
    {
        /* This is used by the startup in order to initialize the .bss section */
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)

        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
        . = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
    } >RAM

    /* User_heap_stack section, used to check that there is enough "RAM" Ram  type memory left */
    ._user_heap_stack : ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT)
    {
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(TFM_LINKER_DEFAULT_ALIGNMENT);
    } >RAM

    /* Remove information from the compiler libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
