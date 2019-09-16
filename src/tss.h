/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#ifndef __TSS_H__
#define __TSS_H__

#include "stdint.h"
#include "defines.h"
#include "i386.h"
#include "gdt.h"
#include "mmu.h"

typedef struct str_tss {
  uint16_t prev_task_link;
  uint16_t unused0;
  uint32_t esp0;
  uint16_t stack_seg_0;
  uint16_t unused1;
  uint32_t esp1;
  uint16_t stack_seg_1;
  uint16_t unused2;
  uint32_t esp2;
  uint16_t stack_seg_2;
  uint16_t unused3;
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint16_t es;
  uint16_t unused4;
  uint16_t cs;
  uint16_t unused5;
  uint16_t ss;
  uint16_t unused6;
  uint16_t ds;
  uint16_t unused7;
  uint16_t fs;
  uint16_t unused8;
  uint16_t gs;
  uint16_t unused9;
  uint16_t ldt;
  uint16_t unused10;
  uint16_t dtrap;
  uint16_t iomap;
} __attribute__((__packed__, aligned(8))) tss;

extern tss tss_initial;
extern tss tss_idle;
extern tss monster_tss[TASK_LIMIT];
extern void return_from_new_task();  // El cuerpo de la función está en ASM

void tss_init();
void tss_init_idle_task();
void tss_init_user_task(tss* tss_user_task, uint8_t task_index);
void tss_init_user_tasks();
void tss_init_gdt_entry(uint32_t base, uint8_t index);
void tss_set(uint32_t esp, uint8_t new_task_id);

#endif /* !__TSS_H__ */