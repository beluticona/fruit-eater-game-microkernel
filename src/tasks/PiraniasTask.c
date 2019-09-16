/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "colors.h"
#include "defines.h"
#include "syscall.h" 

#define Player 20
#define Food 40

#define Right 1
#define Left 2
#define Up 3
#define Down 4


void task() {
    /*
     * Pirañas: son pequeñas pero atacan juntas; están siempre buscando comida.
     *
     * Buscan comida alcanzable por un solo movimiento, si no la encuentran se
     * mueven cuatro a la derecha.
     * 
     * Podría pasar que no tengan comida en toda una banda horizontal y se
     * queden moviendo a la derecha infinitamente.
     */
    int32_t dir;
    int32_t dist;
    int32_t read;

    // 2 de 64
    syscall_divide(); // 4 de 32
    syscall_divide(); // 8 de 16

    start:

    for (dist = 1; dist <= 4; ++dist) {
        for (dir = 1; dir <= 4; ++dir) {
            switch (dir) {
                case Right:
                    read = syscall_read(dist, 0);
                    break;
                case Left:
                    read = syscall_read(dist*-1, 0);
                    break;
                case Up:
                    read = syscall_read(0, dist*-1);
                    break;
                case Down:
                    read = syscall_read(0, dist);
            }
            if (read == Food) {
                syscall_move(dist, dir);
                goto start;
            }
        }
    }

    syscall_move(4, Right);
    goto start;

    while(1) { __asm __volatile("mov $2, %%eax":::"eax"); }
}