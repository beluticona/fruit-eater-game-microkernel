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
    uint8_t i = 0; 
    uint8_t j = 0;
    uint8_t copy = syscall_divide(); 
    if(copy){
        syscall_move(1, Down);
        goto goForIt;  
    }
    copy = syscall_divide();
    if(copy){
        syscall_move(2, Down); 
    }

goForIt:   
    while(1){
        while( j < 8 ){ // 8 ciclos de 8 recorro una línea, luego bajo
            while( i< 8){ // leo 8 posiciones adyacentes a derecha
                if(syscall_read(i,0) == Food){
                    syscall_move(i, Right); //muevo solo si hay comida
                }
                i++;
            }
            syscall_move(8, Right);
            i = 0;
            j++;            
        }
        syscall_move(3, Down);
        j = 0;
    }
}
