/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "syscall.h"

#define Food 40

#define Right 1
#define Left 2
#define Up 3
#define Down 4

void task() {
    /*
     * Pan dulce sin fruta:
     * Recorrer el mapa en áreas de 5x5. En cada área intenta comer lo más
     * rápidamente posible (greedy). Busca primero en línea recta y luego en
     * diagonal. Cuando se aseguró de comer todo, salta a la siguiente área.
     *
     * Resultó mejor que "Barre con todo".
     *
     * Mejora pendiente: leer la siguiente área antes de moverse para saltar
     * directamente a una fruta y ahorrar movimientos.
     */
    uint32_t area[5][5];
    uint8_t x, y;
    for (x = 0; x < 5; ++x) {
        for (y = 0; y < 5; ++y) {
            area[x][y] = 0;
        }
    }
    uint32_t step = 0;
    uint8_t cells_remaining = 24;

    int32_t super_x = 0;
    int32_t super_y = 0;
    int32_t regional_x = 0;
    int32_t regional_y = 0;

    uint8_t dir_x = Right;
    uint8_t dir_y = Down;

    // 2 de 64
    if (syscall_divide() == 1) { // 4 de 32
        syscall_move(1, Up);
        regional_y = 4;
        dir_y = Up;
    }
    if (syscall_divide() == 1) { // 8 de 16
        syscall_move(1, Left);
        if (dir_y == Up) {
            regional_y = 4;
        }
        regional_x = 4;
        dir_x = Left;
    }

    area[regional_x][regional_y] = 1;

    start:

    // Buscar comida en línea horizontal (requiere un movimiento)
    for (x = 0; x < 5; ++x) {
        if ((x != regional_x) && (area[x][regional_y] == step)) {
            cells_remaining -= 1;
            area[x][regional_y] += 1;
            if (syscall_read(x - regional_x, 0) == Food) {
                if (x > regional_x) {
                    syscall_move(x - regional_x, Right);
                } else {
                    syscall_move(regional_x - x, Left);
                }
                regional_x = x;
            }
            if (cells_remaining == 0) goto next;
        }
    }

    // Buscar comida en línea vertical (requiere un movimiento)
    for (y = 0; y < 5; ++y) {
        if ((y != regional_y) && (area[regional_x][y] == step)) {
            cells_remaining -= 1;
            area[regional_x][y] += 1;
            if (syscall_read(0, y - regional_y) == Food) {
                if (y > regional_y) {
                    syscall_move(y - regional_y, Down);
                } else {
                    syscall_move(regional_y - y, Up);
                }
                regional_y = y;
            }

            if (cells_remaining == 0) goto next;
        }
    }

    // Buscar comida en diagonal (requiere dos movimientos)
    for (x = 0; x < 5; ++x) {
        for (y = 0; y < 5; ++y) {
            if ((x != regional_x) && (y != regional_y) && (area[x][y] == step)) {
                cells_remaining -= 1;
                area[x][y] += 1;
                if (syscall_read(x - regional_x, y - regional_y) == Food) {
                    if (y > regional_y) {
                        syscall_move(y - regional_y, Down);
                    } else {
                        syscall_move(regional_y - y, Up);
                    }
                    regional_y = y;
                    if (area[regional_x][regional_y] == step) {
                        cells_remaining -= 1;
                        area[regional_x][regional_y] += 1;
                    }

                    if (x > regional_x) {
                        syscall_move(x - regional_x, Right);
                    } else {
                        syscall_move(regional_x - x, Left);
                    }
                    regional_x = x;
                }

                if (cells_remaining == 0) goto next;
                else goto start;
            }
        }
    }

    next:

    // Saltar a la siguiente área de 5x5

    if (super_x == 4) {

        if (regional_y == 0 && dir_y == Down) {
            regional_y = 4;
            syscall_move(4, Down);
        } else if (regional_y == 4 && dir_y == Up) {
            regional_y = 0;
            syscall_move(4, Up);
        }

        syscall_move(4, dir_y);
        if (dir_y == Down) {
            regional_y = (regional_y + 4) % 5;
        } else {
            regional_y = (regional_y + 6) % 5;
        }

        super_x = 0;
        if (++super_y == 10) {
            super_y = 0;
            goto horizontal_move;
        }

        if (dir_x == Left) dir_x = Right;
        else dir_x = Left;
    } else {
        ++super_x; 
        horizontal_move:
        if (regional_x == 0 && dir_x == Right) {
            regional_x = 4;
            syscall_move(4, Right);
        } else if (regional_x == 4 && dir_x == Left) {
            regional_x = 0;
            syscall_move(4, Left);
        }

        syscall_move(4, dir_x);
        if (dir_x == Right) {
            regional_x = (regional_x + 4) % 5;
        } else {
            regional_x = (regional_x + 6) % 5;
        }
    }

    step += 1;
    cells_remaining = 24;
    area[regional_x][regional_y] += 1;

    goto start;

    while(1) { __asm __volatile("mov $2, %%eax":::"eax"); }
}