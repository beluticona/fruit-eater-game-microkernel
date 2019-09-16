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
    uint8_t copy0;
    uint8_t copy2;
    uint8_t copy = syscall_divide(); 
    if(copy){
        // -1-1
        goto clon32_01;  
    }
    copy = syscall_divide();
    if(copy){
        // +1 +1
        goto clon16_02;  
    }
    copy = syscall_divide();
    if(copy){
        syscall_move(1, Up);
        syscall_move(1, Right);
    }
    goto kill_them_all; 

clon16_02:
    syscall_move(1, Up);
    syscall_move(1, Right);
    copy0 = syscall_divide();
    if(copy0){
        // código que ejecuta primera  copia
        goto clon8_0;  
    }
     goto kill_them_all;
 
clon8_0:
    syscall_move(1, Up);
    syscall_move(1, Right);
    goto kill_them_all;  

clon32_01:
    syscall_move(1, Down);
    syscall_move(1, Left);
    copy2 = syscall_divide();
    if(copy2){
        // código que ejecuta primera  copia
        goto clon16_01;  
    }
     goto kill_them_all; 


clon16_01:
    syscall_move(1, Down);
    syscall_move(1, Left);
    goto kill_them_all;  


kill_them_all:
    while(1){    
        while (i < 50) {
           if (syscall_read(0, 0) != Food) {
                syscall_move(1, Right);
                i++;
           }
        }
        syscall_move(5, Down);
        i = 0;
    }



        //syscall_read(-1, 1);
        //syscall_divide();
        //breakpoint(); 
	/*while(1) {
	for(int i=0;i<50;i++) {
		syscall_move(1, Down);
	}
	syscall_move(1, Right);
	}*/
/*
    int32_t a = syscall_divide();
    if(a==0) {
        syscall_move(1, Down);
        syscall_move(1, Down);
    } else {
        syscall_move(1, Up);
        syscall_move(1, Up);
    }
*/

    while(1) { __asm __volatile("mov $2, %%eax":::"eax"); }
}
