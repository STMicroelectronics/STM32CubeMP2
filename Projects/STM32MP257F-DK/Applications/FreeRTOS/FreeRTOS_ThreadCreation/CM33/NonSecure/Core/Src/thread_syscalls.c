/**
  *****************************************************************************
  **
  **  File        : syscalls.c
  **
  **  Abstract    : System Workbench Minimal System calls file
  **
  **              For more information about which c-functions
  **                need which of these lowlevel functions
  **                please consult the Newlib libc-manual
  **
  **  Environment : System Workbench for MCU
  **
  **  Distribution: The file is distributed �as is,� without any warranty
  **                of any kind.
  **
  **  (c)Copyright System Workbench for MCU.
  **  You may use this file as-is or modify it according to the needs of your
  **  project. Distribution of this file (unmodified or modified) is not
  **  permitted. System Workbench for MCU permit registered System Workbench(R) users the
  **  rights to distribute the assembled, compiled & linked contents of this
  **  file as part of an application binary file, provided that it is built
  **  using the System Workbench for MCU toolchain.
  **
  *****************************************************************************
  */

/* Includes */
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>


/* Variables */
//#undef errno
extern int errno;
#define FreeRTOS
#define MAX_STACK_SIZE 0x2000

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));


#ifdef A7_DUAL_CORE_SMP
#define locked   1
#define unlocked 0
extern void lock_mutex(void *mutex);
extern void unlock_mutex(void *mutex);
#endif


#ifndef FreeRTOS
register char *stack_ptr asm("sp");
#endif


int _read(int file, char *ptr, int len)
{
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    *ptr++ = __io_getchar();
  }

  return len;
}

int _write(int file, char *ptr, int len)
{
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    __io_putchar(*ptr++);
  }
  return len;
}


