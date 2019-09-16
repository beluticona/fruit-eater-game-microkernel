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

void mincost(int city, int ary[16][16], int completed[16], int path[16], uint8_t i, int n);

void task() {
    /*
     * Barre con todo:
     * Recorrer el mapa en áreas de 5x5. En cada área encuentra el camino mínimo
     * para comer toda la fruta. Cuando terminó de comer, salta a la siguiente
     * área.
     *
     * Resultó peor que "Pan dulce con fruta" pero por muy poco. Con muchas
     * distribuciones de frutas distintas debería prevalecer esta.
     *
     * Mejora pendiente: leer la siguiente área antes de moverse para saltar
     * directamente a una fruta y ahorrar movimientos.
     */
    int ary[16][16], completed[16], path[16];
    int n;

    uint8_t fruits_x[15];
    uint8_t fruits_y[15];

    ary[0][0] = 0;

    uint8_t i;

    int32_t super_x = 0;
    int32_t super_y = 0;
    int32_t regional_x = 0;
    int32_t regional_y = 0;
    int32_t offset_x;
    int32_t offset_y;
    int32_t next_x;
    int32_t next_y;

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
        

    start:

    // Leer área de 4x4:
    n = 0;
    completed[0] = 0;
    for (offset_x = 0 - regional_x; offset_x < 5 - regional_x; ++offset_x) {
        for (offset_y = 0 - regional_y; offset_y < 5 - regional_y; ++offset_y) {
            if (syscall_read(offset_x, offset_y) == Food) {
                fruits_x[n] = regional_x + offset_x;
                fruits_y[n] = regional_y + offset_y;   

                // Completar matriz para camino mínimo
                ++n;
                ary[n][n] = 0;
                for (i = 0; i < n; ++i) {
                    if (offset_x == 0 || offset_y == 0) {
                        ary[n][i] = ary[i][n] = 1;
                    } else {
                        ary[n][i] = ary[i][n] = 2;
                    }
                }
                completed[n] = 0;
            }
        }
    }

    // Calcular camino mínimo:
    mincost(0, ary, completed, path, 0, ++n);

    // Comer todo
    for (i = 1; i < n; ++i) {
        next_x = fruits_x[path[i]-1];
        next_y = fruits_y[path[i]-1];
        if (next_x != regional_x) {
            if (next_x > regional_x) {
                syscall_move(next_x - regional_x, Right);
            } else {
                syscall_move(regional_x - next_x, Left);
            }
            regional_x = next_x;
        }
        if (next_y != regional_y) {
            if (next_y > regional_y) {
                syscall_move(next_y - regional_y, Down);
            } else {
                syscall_move(regional_y - next_y, Up);
            }
            regional_y = next_y;
        }
    }

    // Saltar a la siguiente área de 4x4

    if (super_x == 4) {
        
        if ((regional_y == 0 && dir_y == Down) || (regional_y == 4 && dir_y == Up)) {
            regional_y = (regional_y + 4) % 8;
            syscall_move(4, dir_y);
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

        if ((regional_x == 0 && dir_x == Right) || (regional_x == 4 && dir_x == Left)) {
            regional_x = (regional_x + 4) % 8;
            syscall_move(4, dir_x);
        }

        syscall_move(4, dir_x);
        if (dir_x == Right) {
            regional_x = (regional_x + 4) % 5;
        } else {
            regional_x = (regional_x + 6) % 5;
        }
    }

    goto start;

    while(1) { __asm __volatile("mov $2, %%eax":::"eax"); }
}

void mincost(int city, int ary[16][16], int completed[16], int path[16], uint8_t i, int n) {
    completed[city] = 1;
    path[i] = city;
    
    int ncity = 999;
    int min = 999;
    for (int j = 0; j < n; ++j) {
        if ((ary[city][j] != 0) && (completed[j] == 0) && 
                (ary[city][j] + ary[j][city] < min)) {
            min = ary[j][0] + ary[city][j];
            ncity = j;
        }
    }

    if (ncity == 999) {
        return;
    }

    mincost(ncity, ary, completed, path, ++i, n);
}
