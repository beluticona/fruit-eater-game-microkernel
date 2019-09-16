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
#define Left 2
#define Up 3
#define Down 4

void mincost(int city, int ary[16][16], int completed[16], int path[16], uint8_t i, int n);
uint32_t abs(int32_t value);

void task() {
    /*
     * Barre con todo:
     * Recorrer el mapa en áreas de 4x4. En cada área encontrar el camino mínimo
     * para comer todo y luego saltar a la siguiente área.
     *
     * 2da versión de barre con todo en áreas de 4x4 para no superar el límite
     * de lecturas. Tiene como desventaja que 4 no divide a 50, no aprovecha la
     * distancia máxima de movimiento de 4 para comer y hace más veces el
     * cálculo de camino mínimo. Resultó peor que la versión 5x5.
     */

    // Variables para camino mínimo:
    int moves_required[16][16], completed[16], path[16];
    int n;
    moves_required[0][0] = 0;
    moves_required[1][1] = 0;
    moves_required[2][2] = 0;
    moves_required[3][3] = 0;
    moves_required[4][4] = 0;
    moves_required[5][5] = 0;
    moves_required[6][6] = 0;
    moves_required[7][7] = 0;
    moves_required[8][8] = 0;
    moves_required[9][9] = 0;
    moves_required[10][10] = 0;
    moves_required[11][11] = 0;
    moves_required[12][12] = 0;
    moves_required[13][13] = 0;
    moves_required[14][14] = 0;
    moves_required[15][15] = 0;
    uint8_t fruits_x[15];
    uint8_t fruits_y[15];

    // Variables generales
    uint8_t i;
    int32_t super_x = 0;
    //int32_t super_y = 0;
    int32_t regional_x = 0;
    int32_t regional_y = 0;
    int32_t offset_x;
    int32_t offset_y;
    int32_t next_x;
    int32_t next_y;
    uint8_t dir_x = Right;
    uint8_t dir_y = Down;
    int32_t offset_read_x = 0;
    int32_t offset_read_y = 0;
    uint32_t moves;

    // 2 de 64
    if (syscall_divide() == 1) { // 4 de 32
        syscall_move(1, Up);
        regional_y = 3;
        dir_y = Up;
    }
    if (syscall_divide() == 1) { // 8 de 16
        syscall_move(1, Left);
        if (dir_y == Up) {
            regional_y = 3;
        }
        regional_x = 3;
        dir_x = Left;
    }
        

    start:

    // Leer siguiente área de 4x4:

    n = 0; // number of fruits
    completed[0] = 0;
    for (offset_x = 0 - regional_x; offset_x <= 3 - regional_x; ++offset_x) {
        for (offset_y = 0 - regional_y; offset_y <= 3 - regional_y; ++offset_y) {
            if (syscall_read(offset_x + offset_read_x, offset_y + offset_read_y) == Food) {
                fruits_x[n] = regional_x + offset_x;
                fruits_y[n] = regional_y + offset_y;   

                // Completar matriz para camino mínimo
                ++n;
                for (i = 0; i < n; ++i) {
                    moves = (offset_x != 0) + (offset_y != 0) + (offset_read_x != 0) + (offset_read_y != 0);
                    /*moves = 0;
                    if (offset_x != 0) ++moves;
                    if (offset_y != 0) ++moves;
                    moves = abs(offset_read_x + offset_read_y);
                    moves = (abs(offset_x + offset_read_x) >> 2) + (abs(offset_y + offset_read_y) >> 2);*/
                    moves_required[n][i] = moves;
                    moves_required[i][n] = moves;
                }
                completed[n] = 0;
            }
        }
    }

    // Calcular camino mínimo:
    mincost(0, moves_required, completed, path, 0, n+1);

    
    // Moverse a la siguiente área de 4x4
    if (offset_read_x != 0) {
        syscall_move(abs(offset_read_x), dir_x);

        // Para el siguiente movimiento:
        if (++super_x == 6) {
            if (dir_y == Down) {
                offset_read_y = 4;
            } else {
                offset_read_y = -4;
            }
            offset_read_x = 0;
            super_x = 0;
        }
    } else if (offset_read_y != 0) {
        syscall_move(abs(offset_read_y), dir_y);

        // Para el siguiente movimiento:
        offset_read_y = 0;
        if (dir_x == Right) {
            offset_read_x = -4;
            dir_x = Left;
        } else {
            offset_read_x = 4;
            dir_x = Right;
        }

    }

    // Comer frutas
    for (i = 0; i < n; ++i) {
        next_x = fruits_x[path[i+1]-1];
        next_y = fruits_y[path[i+1]-1];
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

uint32_t abs(int32_t value) {
    return value < 0 ? -1*value : value;
}