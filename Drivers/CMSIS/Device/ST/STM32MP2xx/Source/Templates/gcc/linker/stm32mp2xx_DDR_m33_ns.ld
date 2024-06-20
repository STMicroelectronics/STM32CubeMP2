/*
******************************************************************************
**
**  File        : LinkerScript.ld (debug in RAM dedicated)
**
**  Author		: Intropack Team
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
** <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
** All rights reserved.</center></h2>
**
** This software component is licensed by ST under Apache License, Version 2.0,
** the "License"; You may not use this file except in compliance with the
** License. You may obtain a copy of the License at:
**                        opensource.org/licenses/Apache-2.0
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);	/* end of "RAM" Ram type memory */

_Min_Heap_Size = 0x200;	/* required amount of heap  */
_Min_Stack_Size = 0x400;	/* required amount of stack */

_virtio_shmem_size = 32K;  /* size of the virtio pool used for RPMsg communication */

/* Memories definition */
MEMORY
{
  NS_VECTOR_TBL (xrw)   : ORIGIN = 0x80100000,	LENGTH = 0x00000600
  FLASH         (rx)    : ORIGIN = 0x80100600,  LENGTH = 8M - LENGTH(NS_VECTOR_TBL)
  ROM           (rx)   : ORIGIN = 0x80A00000,  LENGTH = 4M
  RAM           (rwx)   : ORIGIN = 0x80E00000,  LENGTH = 4M
  IPC_SHMEM_1   (rw)    : ORIGIN = 0x81200000,  LENGTH = 1M - _virtio_shmem_size
  VIRTIO_SHMEM  (rw)    : ORIGIN = ORIGIN(IPC_SHMEM_1) + LENGTH(IPC_SHMEM_1), LENGTH = _virtio_shmem_size
}

 /* Symbols needed for OpenAMP to enable rpmsg */
__OPENAMP_region_start__  = ORIGIN(VIRTIO_SHMEM);
__OPENAMP_region_end__ = ORIGIN(VIRTIO_SHMEM)+LENGTH(VIRTIO_SHMEM);

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)


SECTIONS
{
  /* The startup code into "NS_VECTOR_TBL" Ram type memory
   * Do not update the ".isr_vectors" section name. the name
   * is used by the Linux kernel to retrieve the address
   * of the vector table*/
   .isr_vectors :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vectors))
        . = ALIGN(4);
    } >NS_VECTOR_TBL

    .resource_table :
    {
        . = ALIGN(4);
        KEEP (*(.resource_table*))
        . = ALIGN(4);
    } >IPC_SHMEM_1

  /* The program code and other data into "RAM" Ram type memory */
  .text :
  {
    . = ALIGN(8);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(8);
  } >ROM

  /* Constant data into "RAM" Ram type memory */
  .rodata :
  {
    . = ALIGN(8);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(8);
  } >ROM

  .ARM.extab   : {
    . = ALIGN(8);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(8);
  } >FLASH

  .ARM : {
    . = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(8);
  } >FLASH

  .preinit_array     :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(8);
  } >RAM

  .init_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(8);
  } >RAM

  .fini_array :
  {
    . = ALIGN(8);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(8);
  } >RAM

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections into "RAM" Ram type memory */
  .data :
  {
    . = ALIGN(8);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(8);
    _edata = .;        /* define a global symbol at data end */
  } >RAM

  /* Uninitialized data section into "RAM" Ram type memory */
  . = ALIGN(8);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(8);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough "RAM" Ram  type memory left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
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
