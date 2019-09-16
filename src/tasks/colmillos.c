v/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "colors.h"
#include "defines.h"
#include "syscall.h" 


void task() {
    int16_t i = 0;
    int16_t j = 0;

    uint8_t copy = syscall_divide(); 
    if(copy){
        syscall_move(1, Down);
        copy = syscall_divide();
        if(copy){
            syscall_move(2, Down);
            goto speeder;   
        }
        goto goForIt;  
    }
    copy = syscall_divide();
    if(copy){
        syscall_move(2, Down); 
    }

goForIt:
    syscall_move(4, Up);   
    while(1){
        while( j < 13 ){ // 8 ciclos de 8 recorro una línea, luego bajo
            while( i< 5){ // leo 8 posiciones adyacentes a derecha
                if(syscall_read(i,0) == Food){
                    syscall_move(i, Right); //muevo solo si hay comida
                    i = 0;
                }
                i++;
            }
            syscall_move(4, Right);
            i = 0;
            j++;            
        }
        syscall_move(5, Down);
        j = 0;
    }

speeder:
    syscall_move(4, Up);
    copy = syscall_divide();
    if(copy){
        syscall_move(1, Down);
    }
    while(1){
        while( j < 8 ){ // 8 ciclos de 8 recorro una línea, luego bajo
            while( i< 8){ // leo 8 posiciones adyacentes a derecha
                if(syscall_read(i,0) == Food){
                    syscall_move(i, Right); //muevo solo si hay comida
                    i = 0;
                }
                i++;
            }
            syscall_move(7, Right);
            i = 0;
            j++;            
        }
        syscall_move(5, Down);
        j = 0;
    }

}
