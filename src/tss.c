/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
tss tss_initial;
tss tss_idle;
tss monster_tss[TASK_LIMIT];

void tss_init_idle_task() {
  // Entrada en la GDT que describe el segmento de estado de la tarea idle
  const uint32_t segment_base = (uint32_t)&tss_idle;
  const uint8_t gdt_index = GDT_IDX_IDLE_TASK;
  tss_init_gdt_entry(segment_base, gdt_index);

  // Datos que contiene el segmento
  tss_idle.prev_task_link = 0x0000;
  tss_idle.unused0 = 0x0000;
  tss_idle.esp0 = STACK_BASE_KERNEL;
  tss_idle.stack_seg_0 = GDT_SEL_DATA_LVL_0;
  tss_idle.unused1 = 0x0000;
  tss_idle.esp1 = 0x00000000;
  tss_idle.stack_seg_1 = 0x0000;
  tss_idle.unused2 = 0x0000;
  tss_idle.esp2 = 0x00000000;
  tss_idle.stack_seg_2 = 0x0000;
  tss_idle.unused3 = 0x0000;
  tss_idle.cr3 = KERNEL_PAGE_DIR;
  tss_idle.eip = TASK_IDLE_CODE_SRC_ADDR;  // dir. mem. donde inicia el código
  tss_idle.eflags = INT_ON_EFLAGS;         // interrupciones habilitadas
  tss_idle.eax = 0x00000000;
  tss_idle.ecx = 0x00000000;
  tss_idle.edx = 0x00000000;
  tss_idle.ebx = 0x00000000;
  tss_idle.esp = STACK_BASE_KERNEL;
  tss_idle.ebp = STACK_BASE_KERNEL;
  tss_idle.esi = 0x00000000;
  tss_idle.edi = 0x00000000;
  tss_idle.es = GDT_SEL_DATA_LVL_0;
  tss_idle.unused4 = 0x0000;
  tss_idle.cs = GDT_SEL_CODE_LVL_0;
  tss_idle.unused5 = 0x0000;
  tss_idle.ss = GDT_SEL_DATA_LVL_0;
  tss_idle.unused6 = 0x0000;
  tss_idle.ds = GDT_SEL_DATA_LVL_0;
  tss_idle.unused7 = 0x0000;
  tss_idle.fs = GDT_SEL_SCREEN_LVL_0;
  tss_idle.unused8 = 0x0000;
  tss_idle.gs = GDT_SEL_DATA_LVL_0;
  tss_idle.unused9 = 0x0000;
  tss_idle.ldt = 0x0000;  // Local descriptor table no se usa
  tss_idle.unused10 = 0x0000;
  tss_idle.dtrap = 0x0000;
  tss_idle.iomap = IOMAP_ACCESS_DENIED;  // Furfi dijo no usar
}

void tss_init_user_task(tss* tss_user_task, uint8_t task_index) {
  // task_index de 0 a 9 para jugador A y de 10 a 19 para jugador B

  const uint32_t segment_base = (uint32_t)tss_user_task;
  const uint8_t gdt_index = GDT_IDX_FIRST_USER_TASK + task_index;
  tss_init_gdt_entry(segment_base, gdt_index);

  // Datos que contiene el segmento
  tss_user_task->prev_task_link = 0x0000;
  tss_user_task->unused0 = 0x0000;
  tss_user_task->esp0 = kernel_stack_address[task_index] + PAGE_SIZE_4K;
  tss_user_task->stack_seg_0 = GDT_SEL_DATA_LVL_0;
  tss_user_task->unused1 = 0x0000;
  tss_user_task->esp1 = 0x00000000;
  tss_user_task->stack_seg_1 = 0x0000;
  tss_user_task->unused2 = 0x0000;
  tss_user_task->esp2 = 0x00000000;
  tss_user_task->stack_seg_2 = 0x0000;
  tss_user_task->unused3 = 0x0000;
  tss_user_task->cr3 = directory_address[task_index];
  tss_user_task->eip =
      TASK_CODE_VIRTUAL_ADDR;             // Dir. virtual donde inicia el código
  tss_user_task->eflags = INT_ON_EFLAGS;  // Interrupciones habilitadas
  tss_user_task->eax = 0x00000000;
  tss_user_task->ecx = 0x00000000;
  tss_user_task->edx = 0x00000000;
  tss_user_task->ebx = 0x00000000;
  tss_user_task->esp = TASK_STACK_VIRTUAL_ADDR_LVL_3 + PAGE_SIZE_4K;
  tss_user_task->ebp = TASK_STACK_VIRTUAL_ADDR_LVL_3 + PAGE_SIZE_4K;
  tss_user_task->esi = 0x00000000;
  tss_user_task->edi = 0x00000000;
  tss_user_task->es = GDT_SEL_DATA_LVL_3;
  tss_user_task->unused4 = 0x0000;
  tss_user_task->cs = GDT_SEL_CODE_LVL_3;
  tss_user_task->unused5 = 0x0000;
  tss_user_task->ss = GDT_SEL_DATA_LVL_3;
  tss_user_task->unused6 = 0x0000;
  tss_user_task->ds = GDT_SEL_DATA_LVL_3;
  tss_user_task->unused7 = 0x0000;
  tss_user_task->fs = GDT_SEL_DATA_LVL_3;
  tss_user_task->unused8 = 0x0000;
  tss_user_task->gs = GDT_SEL_DATA_LVL_3;
  tss_user_task->unused9 = 0x0000;
  tss_user_task->ldt = 0x0000;
  tss_user_task->unused10 = 0x0000;
  tss_user_task->dtrap = 0x0000;
  tss_user_task->iomap = IOMAP_ACCESS_DENIED;
}

void tss_init_gdt_entry(uint32_t base, uint8_t index) {
  const uint32_t limit = 0x67;
  gdt[index].limit_0_15 = limit & 0xFFFF;
  gdt[index].limit_16_19 = (limit >> 16) & 0xF;
  gdt[index].base_0_15 = base & 0xFFFF;
  gdt[index].base_23_16 = (base >> 16) & 0xFF;
  gdt[index].base_31_24 = (base >> 24);
  gdt[index].type = TASK_ATRR_NOT_BUSY;
  gdt[index].s = 0x00;
  gdt[index].dpl = DPL_0;
  gdt[index].p = 0x01;
  gdt[index].avl = 0x00;
  gdt[index].l = 0x00;
  gdt[index].db = 0x01;
  gdt[index].g = 0x00;
}

// Requiere que se haya llamado previamente a mmu_init para inicializar los
// arreglos de direcciones de las tareas.
void tss_init() {
  const uint32_t segment_base = (uint32_t)&tss_initial;
  const uint8_t gdt_index = GDT_IDX_INITIAL_TASK;
  tss_init_gdt_entry(segment_base, gdt_index);
  tss_init_idle_task();
  tss_init_user_tasks();
}

// Inicializa las tss de las 20 tareas en memoria estática
void tss_init_user_tasks() {
  for (uint8_t i = 0; i < TASK_LIMIT; ++i) {
    tss_init_user_task(&monster_tss[i], i);
  }
}

// Setea los datos necesarios de una tarea creada por la syscall divide
void tss_set(uint32_t esp, uint8_t new_task_id) {
  tss* tss_new = &monster_tss[new_task_id];
  tss_init_user_task(tss_new, new_task_id);  // resetea tss para reutilizarla

  tss_new->esp = (kernel_stack_address[new_task_id] | (esp & 0xFFF));
  // A la dir más baja de la pila nivel 0 le suma el offset

  tss_new->eip = (uint32_t)&return_from_new_task;
  tss_new->cs = GDT_SEL_CODE_LVL_0;
  tss_new->ss = GDT_SEL_DATA_LVL_0;
}