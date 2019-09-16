/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#include "defines.h"
#include "idt.h"
#include "isr.h"
#include "tss.h"

idt_entry idt[255] = {};

idt_descriptor IDT_DESC = {sizeof(idt) - 1, (uint32_t)&idt};

#define IDT_ENTRY(numero)                                       \
  idt[numero].offset_0_15 =                                     \
      (uint16_t)((uint32_t)(&_isr##numero) & (uint32_t)0xFFFF); \
  idt[numero].segsel = (uint16_t)GDT_SEL_CODE_LVL_0;            \
  idt[numero].attr = (uint16_t)0x8E00;                          \
  idt[numero].offset_16_31 =                                    \
      (uint16_t)((uint32_t)(&_isr##numero) >> 16 & (uint32_t)0xFFFF);

void idt_init() {
  // Interrupciones internas (excepciones)
  IDT_ENTRY(0);
  IDT_ENTRY(1);
  IDT_ENTRY(2);
  IDT_ENTRY(3);
  IDT_ENTRY(4);
  IDT_ENTRY(5);
  IDT_ENTRY(6);
  IDT_ENTRY(7);
  IDT_ENTRY(8);
  IDT_ENTRY(9);
  IDT_ENTRY(10);
  IDT_ENTRY(11);
  IDT_ENTRY(12);
  IDT_ENTRY(13);
  IDT_ENTRY(14);
  IDT_ENTRY(15);
  IDT_ENTRY(16);
  IDT_ENTRY(17);
  IDT_ENTRY(18);
  IDT_ENTRY(19);

  // Interrupciones externas (reloj y teclado)
  IDT_ENTRY(32);
  IDT_ENTRY(33);

  // Interrupciones de software
  IDT_ENTRY(71);
  IDT_ENTRY(73);
  IDT_ENTRY(76);

  // Las syscall deben poder ser accedidas desde tareas de nivel 3
  idt[71].attr = idt[73].attr = idt[76].attr = 0xEE00;
}
