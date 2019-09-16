/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "stdint.h"
#include "defines.h"
#include "i386.h"
#include "tss.h"
#include "game.h"

typedef struct str_page_directory_entry {
  uint8_t p : 1;
  uint8_t r_w : 1;
  uint8_t u_s : 1;
  uint8_t pwt : 1;
  uint8_t pcd : 1;
  uint8_t a : 1;
  uint8_t ign : 1;
  uint8_t ps : 1;
  uint8_t g : 1;
  uint8_t avl : 3;
  uint32_t baseTable : 20;
} __attribute__((__packed__ /*, aligned (4)*/)) PDE;

typedef struct str_page_table_entry {
  uint8_t p : 1;
  uint8_t r_w : 1;
  uint8_t u_s : 1;
  uint8_t pwt : 1;
  uint8_t pcd : 1;
  uint8_t a : 1;
  uint8_t d : 1;
  uint8_t pat : 1;
  uint8_t g : 1;
  uint8_t avl : 3;
  uint32_t basePage : 20;
} __attribute__((__packed__, /*, aligned (4)*/)) PTE;

void mmu_map_kernel_pages(PDE* kernel_pd, PTE* kernel_pt);

uint32_t mmu_next_free_kernel_page;
uint32_t mmu_get_next_free_kernel_page();

uint32_t mmu_next_free_task_page;
uint32_t mmu_get_next_free_task_page();

uint32_t code_address[TASK_LIMIT];
uint32_t user_stack_address[TASK_LIMIT];
uint32_t directory_address[TASK_LIMIT];
uint32_t table_address_kernel[TASK_LIMIT];
uint32_t table_address_task[TASK_LIMIT];
uint32_t kernel_stack_address[TASK_LIMIT];

void mmu_map_page(uint32_t virtual, uint32_t cr3, uint32_t phy, PTE* new_table,
                  uint8_t U_S);
// El read_write lo seteamos siempre en 1 cuando mapeamos la pàgina
void mmu_unmap_page(uint32_t virtual, uint32_t cr3);
void mmu_init_kernel_directory();
void init_new_pt(PTE* page_table, bool identidad, uint8_t present, uint8_t U_S);
// set_new_page: Inicializa una entrada de pt (pte)
void set_new_page(PTE* page_table_entry, uint32_t basePage, bool is_present,
                  uint8_t u_s);
// El read_write lo seteamos siempre en 1 cuando mapeamos la pàgina
void mmu_init_task_directory(uint8_t task_index);
void mmu_init();
void mmu_mem_copy(uint32_t origen_virtual, uint32_t destino_fisica);

#endif /* !__MMU_H__ */
