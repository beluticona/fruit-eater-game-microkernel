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
/*void print(const char* text, uint32_t x, uint32_t y, uint16_t attr);
void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y, uint16_t attr);
void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y, uint16_t attr);*/

void task() {
    /*
     * Pirañas super inteligentes: son pequeñas pero atacan juntas; encuentran
     * el camino mínimo para comer toda la comida cercana.
     *
     * Van recorriendo el mapa en áreas de 5x5. En cada área encuentran el
     * el camino mínimo para comer todo y luego saltan a la siguiente área.
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

    // 2 de 64
    syscall_divide(); // 4 de 32
    syscall_divide(); // 8 de 16

    start:

    // Leer área de 4x4:
    /*print("Pos actual:  ,  |  ,", 50, 40, C_FG_WHITE | C_BG_BLACK);
    print_dec(regional_x, 1, 62, 40, C_FG_WHITE | C_BG_BLACK);
    print_dec(regional_y, 1, 64, 40, C_FG_WHITE | C_BG_BLACK);
    print_dec(super_x, 1, 68, 40, C_FG_WHITE | C_BG_BLACK);
    print_dec(super_y, 1, 70, 40, C_FG_WHITE | C_BG_BLACK);*/
    n = 0;
    completed[0] = 0;
    for (offset_x = 0 - regional_x; offset_x < 5 - regional_x; ++offset_x) {
        for (offset_y = 0 - regional_y; offset_y < 5 - regional_y; ++offset_y) {
            if (syscall_read(offset_x, offset_y) == Food) {
                fruits_x[n] = regional_x + offset_x;
                fruits_y[n] = regional_y + offset_y;

                //print_dec(fruits_x[n], 1, 51, 40, C_FG_WHITE | C_BG_BLACK);
                //print_dec(fruits_y[n], 1, 53, 40, C_FG_WHITE | C_BG_BLACK);   

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

    //print_hex((int32_t) fruits_x, 8, 51, 42, C_FG_WHITE | C_BG_BLACK);
    //print_hex((int32_t) fruits_y, 8, 51, 43, C_FG_WHITE | C_BG_BLACK);
    //print_hex((int32_t) path, 8, 51, 44, C_FG_WHITE | C_BG_BLACK);
    //breakpoint();

    // Calcular camino mínimo:
    mincost(0, ary, completed, path, 0, ++n);

    // Comer todo
    /*for (i = 0; i < n; ++i) {
        print_dec(path[i], 1, i*2+50, 45, C_FG_WHITE | C_BG_BLACK);
    }*/
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
    if (super_x == 9) {
        if (regional_y == 0) {
            regional_y = 4;
            syscall_move(4, Down);
        }
        regional_y = (regional_y + 4) % 5;
        syscall_move(4, Down);
        super_y = (super_y + 1) % 10;
    }

    if (regional_x == 0) {
        regional_x = 4;
        syscall_move(4, Right);
    }
    regional_x = (regional_x + 4) % 5;
    syscall_move(4, Right);
    super_x = (super_x + 1) % 10;

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

/*void print(const char* text, uint32_t x, uint32_t y, uint16_t attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
    int32_t i;
    for (i = 0; text[i] != 0; i++) {
        p[y][x].c = (uint8_t) text[i];
        p[y][x].a = (uint8_t) attr;
        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y, uint16_t attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
    uint32_t i;
    uint8_t letras[16] = "0123456789";

    for(i = 0; i < size; i++) {
        uint32_t resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;
    }
}

void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y, uint16_t attr) {
    ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO; // magia
    int32_t i;
    uint8_t hexa[8];
    uint8_t letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}*/
