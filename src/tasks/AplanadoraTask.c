/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "colors.h"
#include "defines.h"
#include "syscall.h" 

#define Food 40

#define Right 1
#define Down 4

void task() {
    /*
     * Aplanadora: recorre de izquierda a derecha y de arriba a abajo.
     */

    int32_t x = 0;
    int32_t next_x;
    int32_t dist;

    // 2 de 64
    syscall_divide(); // 4 de 32
    syscall_divide(); // 8 de 16

    start:

    for (dist = 1; dist <= 4 && (next_x = x + dist) < 50; ++dist) {
        if (syscall_read(dist, 0) == Food) {
            syscall_move(dist, Right);
            x = next_x;
            goto start;
        }
    }

    if (next_x < 50) {
        syscall_move(4, Right);
        x = next_x;
        goto start;
    }

    while (dist <= 3) {
        if (syscall_read(dist, 1) == Food) {
            break;
        }
        ++dist;
    }

    syscall_move(dist, Right);
    syscall_move(1, Down);
    x = (x + dist) % 50;
    goto start;

    while(1) { __asm __volatile("mov $2, %%eax":::"eax"); }
}