/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "colors.h"
#include "defines.h"
#include "syscall.h" 

void task() {
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t copy = syscall_divide(); 
    if(copy){
        // -1-1
        goto clon32_01;  
    }
    goto goForIt;

clon32_01:
    syscall_move(1, Down);
    syscall_move(1, Left);


goForIt:   
    while(1){
        while( j < 4 ){
            while( i< 16){
                if(syscall_read(i,0) == Food){
                    syscall_move(i, Right);
                }
                i++;
            }
            syscall_move(8, Right);
            i = 0;
            j++;            
        }
        syscall_move(1, Down);
        j = 0;
    }
}
