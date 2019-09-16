/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "colors.h"
#include "defines.h"
#include "syscall.h" 

void task() {
    /*creo las variables al principio así lo ejecuta la primera tarea 
    y el resto ya lo tiene en la pila */
    // puedo realizar 58 lecturas
    int16_t i = 0; 
    int16_t j = 0;
    uint8_t copy = syscall_divide(); // 4 de 32
    if(copy){
        syscall_move(1, Down);
        copy = syscall_divide(); // 2 de 32 --> 4 de 16
        if(copy){
            syscall_move(2, Down);
            copy = syscall_divide(); // 2 de 16--> 4 de 8
            if(copy){
                syscall_move(1, Down);// 2 de 32 + 2 de 16 + 4 de 8
            }
            goto goForIt;
        }
        copy = syscall_divide();
        if(copy){
            syscall_move(1, Down);
        }
        goto goForIt;
    }
    goto end;


// KAMIKAZE
end:
    while(i<8){
        syscall_move(2, Up);
        i++;
    }
    while(1){

    }

goForIt:
    syscall_move(7, Up); 
    while(1){
        while( j < 7 ){ // 8 ciclos de 8 recorro una línea, luego bajo
            while( i< 8){ // leo 8 posiciones adyacentes a derecha
                if(syscall_read(-i,0) == Food){
                    syscall_move(i, Left); //muevo solo si hay comida
                    i = 0;
                }
                i++;
            }
            syscall_move(7, Left); //@Belu: corregir de a 8
            i = 0;
            j++;            
        }
        syscall_move(4, Down);
        j = 0;
    }
}
