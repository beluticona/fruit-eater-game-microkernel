/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "colors.h"
#include "defines.h"
#include "syscall.h" 

#define Null 0
#define Empty 10
#define Player 20
#define Food 40

#define Right 1
#define Left 2
#define Up 3
#define Down 4

#define calculate_position(pos, off) (pos + off + 50) % 50   

void task() {
    /*
     * Pirañas semi inteligentes: son pequeñas pero atacan juntas; están siempre
     * buscando comida de forma semi inteligente.
     *
     * Primero buscan comida alcanzable por un solo movimiento. Si no la
     * encuentran, van recorriendo hacia la derecha y abajo.
     *
     * Evitan leer una posición ya leída.
     */
    uint8_t dir;
    int32_t dist;

    int32_t x;
    int32_t y;

    int32_t offset_x;
    int32_t offset_y;

    int32_t next_x;
    int32_t next_y;

    uint8_t position_visited[50][50];
    for (x = 0; x < 50; ++x) {
        for (y = 0; y < 50; ++y) {
            position_visited[x][y] = 0;
        }
    }
    position_visited[0][0] = 1;

    x = y = 0;


    // 2 de 64
    syscall_divide(); // 4 de 32
    syscall_divide(); // 8 de 16

    search_fruit_one_move_away:

    for (dist = 1; dist <= 4; ++dist) {
        for (dir = 1; dir <= 4; ++dir) {
            switch (dir) {
                case Right:
                    offset_x = dist;
                    offset_y = 0;
                    break;
                case Left:
                    offset_x = dist*-1;
                    offset_y = 0;
                    break;
                case Up:
                    offset_x = 0;
                    offset_y = dist*-1;
                    break;
                case Down:
                    offset_x = 0;
                    offset_y = dist;
            }

            next_x = calculate_position(x, offset_x);
            next_y = calculate_position(y, offset_y);

            if (position_visited[next_x][next_y] == 0) {
                position_visited[next_x][next_y] = 1;
                if (syscall_read(offset_x, offset_y) == Food) {
                    syscall_move(dist, dir);
                    x = next_x;
                    y = next_y;
                    goto search_fruit_one_move_away;
                }
            }
        }
    }

    // no_fruit_found:

    if (x < 4) {
        syscall_move(1, Down);
        y = calculate_position(y, 1);
        position_visited[x][y] = 1;
    }
    syscall_move(4, Right);
    x = calculate_position(x, 4);
    position_visited[x][y] = 1; 

    goto search_fruit_one_move_away;


    while(1) { __asm __volatile("mov $2, %%eax":::"eax"); }
}