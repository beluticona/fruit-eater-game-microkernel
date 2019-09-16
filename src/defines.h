/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#include "stdint.h"
typedef uint8_t bool;

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Bool */
/* -------------------------------------------------------------------------- */
#define TRUE                    0x00000001
#define FALSE                   0x00000000
#define ERROR                   1

/* Atributos paginas */
/* -------------------------------------------------------------------------- */
#define PAG_P                   0x00000001
#define PAG_NP                  0x00000000
#define PAG_R                   0x00000000
#define PAG_RW                  0x00000001
#define PAG_US_U                0x00000001
#define PAG_US_S                0x00000000

/* Atributos GDT */
/* -------------------------------------------------------------------------- */
#define DPL_0                      0x00
#define DPL_3                      0x03	
#define TASK_ATRR_BUSY             0x0B
#define TASK_ATRR_NOT_BUSY         0x09	
 
/* Misc */
/* -------------------------------------------------------------------------- */
#define MAP_WIDTH                  50
#define MAP_HEIGHT                 50
#define TASK_LIMIT_PER_PLAYER      10
#define TASK_LIMIT                 TASK_LIMIT_PER_PLAYER*2

/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_COUNT                  50

#define GDT_IDX_NULL_DESC          0
#define GDT_IDX_DATA_LVL_0         22
#define GDT_IDX_DATA_LVL_3         23
#define GDT_IDX_CODE_LVL_0         24
#define GDT_IDX_CODE_LVL_3         25
#define GDT_IDX_SCREEN_LVL_0       26
#define	GDT_IDX_INITIAL_TASK       27
#define GDT_IDX_FIRST_USER_TASK    28
#define GDT_IDX_IDLE_TASK          48

/* Selectores de segmentos */
/* -------------------------------------------------------------------------- */
#define GDT_SEL_NULL_DESC          (GDT_IDX_NULL_DESC << 3)
#define GDT_SEL_DATA_LVL_0         (GDT_IDX_DATA_LVL_0 << 3)
#define GDT_SEL_CODE_LVL_0         (GDT_IDX_CODE_LVL_0 << 3)
#define GDT_SEL_DATA_LVL_3         ((GDT_IDX_DATA_LVL_3 << 3) | 0x3)
#define GDT_SEL_CODE_LVL_3         ((GDT_IDX_CODE_LVL_3 << 3) | 0x3)
#define GDT_SEL_SCREEN_LVL_0       (GDT_IDX_SCREEN_LVL_0 << 3)
#define GDT_SEL_IDLE_TASK          (GDT_IDX_IDLE_TASK << 3)

/* Direcciones de memoria */
/* -------------------------------------------------------------------------- */
#define BOOTSECTOR                 0x00001000 /* direccion fisica de comienzo del bootsector (copiado) */
#define KERNEL                     0x00001200 /* direccion fisica de comienzo del kernel */
#define VIDEO                      0x000B8000 /* direccion fisica del buffer de video */
#define STACK_BASE_KERNEL          0x00027000 /* de aquí para abajo la pila */



/* Direcciones virtuales de código, pila y datos */
/* -------------------------------------------------------------------------- */
#define TASK_CODE_VIRTUAL_ADDR        0x08000000 /* dir virtual del codigo */
#define TASK_STACK_VIRTUAL_ADDR_LVL_3 0x08001000

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */
#define TASK_IDLE_CODE_SRC_ADDR  	0x00014000
#define TASK_A_CODE_SRC_ADDR  		0x00010000
#define TASK_B_CODE_SRC_ADDR  		0x00012000

/* Tareas Atributos */
/* -------------------------------------------------------------------------- */

#define DEFAULT_EFLAGS  		0x00000002
#define INT_ON_EFLAGS			0x00000202
#define IOMAP_ACCESS_DENIED		0xFFFF

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
#define KERNEL_PAGE_DIR             0x00027000 /* de aquí para arriba*/
#define KERNEL_PAGE_TABLE_0         0x00028000

#define KERNEL_FREE_AREA_START      0x00100000
#define TASK_FREE_AREA_START        0x00400000
#define PAGE_SIZE_4K                0x00001000

#define PAGINATION_ARRAY_SIZE       1024			

#endif  /* !__DEFINES_H__ */
