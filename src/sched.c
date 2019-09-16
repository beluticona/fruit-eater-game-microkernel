/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#include "sched.h"

void sched_init() {
  current_task_id = GDT_IDX_IDLE_TASK;
  idle_informed = FALSE;
  for (uint8_t i = 0; i < TASK_LIMIT; ++i) {
    task_alive[i] = FALSE;
  }
}

// Devuelve primero las tareas de A y luego las de B en llamadas sucesivas
int16_t sched_next_task() {
  bool comes_from_idle;

  if (current_task_id == GDT_IDX_IDLE_TASK) {
    comes_from_idle = TRUE;
    current_task_id = GDT_IDX_FIRST_USER_TASK;
  } else {
    comes_from_idle = idle_informed;
    ++current_task_id;
  }

  idle_informed = FALSE;

  // Busca próxima tarea viva para ejecutarla
  while (current_task_id < GDT_IDX_IDLE_TASK &&
         task_alive[sched_current_task_id()] == FALSE) {
    ++current_task_id;
  }

  // Si ya estaba en idle, devuelve null
  if (current_task_id == GDT_IDX_IDLE_TASK && comes_from_idle) {
    return GDT_SEL_NULL_DESC;
  }

  return (current_task_id << 3);

  // Idle o null se deben interpretar como fin de turno
}

void sched_add_task(uint8_t task_id) {
  task_alive[task_id] = TRUE;
}

void sched_remove_task(uint8_t task_id) {
  task_alive[task_id] = FALSE;
}

uint8_t sched_current_task_id() {
  return current_task_id - GDT_IDX_FIRST_USER_TASK;
}

void sched_inform_idle() {
  idle_informed = TRUE;
}
