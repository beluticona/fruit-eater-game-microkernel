/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de la tabla de descriptores globales
*/

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "stdint.h"
#include "game.h"

#define LS_INLINE static __inline __attribute__((always_inline))

/*
 * Syscalls
 */

LS_INLINE int32_t syscall_read(uint32_t x, uint32_t y) {
    int32_t ret;
  __asm__ volatile (
    "int $71"        /* make the request to the OS */
    : "=a" (ret),    /* return result in eax ("a") */
      "+b" (y)       /* pass in ebx ("b") arg Y */
    : "a"  (x)       /* pass in eax ("a") arg X*/
    : "memory", "cc"); /* announce to the compiler that the memory and condition codes have been modified */
    return ret;
}

LS_INLINE int32_t syscall_move(uint32_t distance, e_direction_t dir) {
    int32_t ret;
  __asm__ volatile (
    "int $73"          /* make the request to the OS */
    : "=a" (ret),      /* return result in eax ("a") */
      "+c" (distance), /* pass arg2 in ecx ("c") */
      "+d" (dir)       /* pass arg3 in edx ("d") */
    :
    : "memory", "cc"); /* announce to the compiler that the memory and condition codes have been modified */
    return ret;
}

LS_INLINE int32_t syscall_divide() {
    int32_t ret;
  __asm__ volatile (
    "int $76"        /* make the request to the OS */
    : "=a" (ret)     /* return result in eax ("a") */
    : 
    : "memory", "cc"); /* announce to the compiler that the memory and condition codes have been modified */
    return ret;
}

#endif  /* !__SYSCALL_H__ */
