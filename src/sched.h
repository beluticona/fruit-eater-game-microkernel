/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "stdint.h"
#include "screen.h"
#include "tss.h"
#include "i386.h"
#include "game.h"

bool task_alive[TASK_LIMIT];
bool idle_informed;
uint8_t current_task_id;  // gdt tss index
void sched_init();
int16_t sched_next_task();
void sched_remove_task(uint8_t task_id);
void sched_add_task(uint8_t task_id);
uint8_t sched_current_task_id();
void sched_inform_idle();

#endif /* !__SCHED_H__ */
