
/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#ifndef __GAME_H__
#define __GAME_H__

#include "stdint.h"
#include "defines.h"
#include "screen.h"
#include "mmu.h"
#include "sched.h"

typedef enum e_direction {
  Right = 1,
  Left = 2,
  Up = 3,
  Down = 4,
} e_direction_t;

typedef enum e_datatype {
  Null = 0,
  Empty = 10,
  Player = 20,
  Opponent = 30,
  Food = 40,
  ReadLimitExceeded = 50
} e_datatype_t;

typedef enum e_player { player_A = 0, player_B = 1 } e_player_t;

typedef enum e_state {
  Scheduled = 0,
  Unscheduled = 1,
  ToBeScheduled = 2,
  ToBeUnscheduled = 3
} e_state_t;

typedef enum e_fruit {
  None = 0,
  Grape = 16,
  Banana = 32,
  Strawberry = 64
} e_fruit_t;

typedef struct position_t {
  uint32_t x;
  uint32_t y;
} s_position_t;

typedef struct monster_t {
  s_position_t position;
  s_position_t next_position;
  uint32_t weight;
  uint8_t task_id;  // 0-9 si es del jugador A y 10-19 si es de B
  uint8_t task_state;
  uint8_t read_count;  // Cuenta la cantidad de veces que se hizo una lectura
} s_monster_t;

typedef struct pesos_t {
  uint32_t peso_A;  // Inicialmente de valor cero
  uint32_t peso_B;
} s_pesos_t;

uint32_t score[2];  // Los índices son e_player_t

uint8_t exception_count; // Para la pantalla de debug

// Listados de monstruos por posición (hay 20 listados por si los 20 monstruos
// están en posiciones distintas, si hay al menos 2 que coinciden va a haber
// listados vacíos
s_monster_t* equal_positions[TASK_LIMIT][TASK_LIMIT];

s_pesos_t suma_pesos[TASK_LIMIT];

s_monster_t tareas_A[TASK_LIMIT_PER_PLAYER];

s_monster_t tareas_B[TASK_LIMIT_PER_PLAYER];

extern e_fruit_t fruits[MAP_WIDTH][MAP_HEIGHT];

// -----------------------------------------------------------------------------
// Funciones globales:
// -----------------------------------------------------------------------------

void game_init();
void game_next_step();
void print_digit(uint8_t scancode);
uint32_t game_read(int32_t x, int32_t y);
int32_t game_divide(uint32_t esp);
uint32_t game_move(uint32_t distance, e_direction_t direction);
void game_reset();
s_monster_t* current_monster();
void print_debug_screen(uint32_t stack_elems[5], uint16_t eflags, uint32_t cr0,
                        uint32_t cr2, uint32_t cr3, uint32_t cr4, uint32_t eip,
                        uint16_t cs, uint16_t ds, uint16_t es, uint16_t fs,
                        uint16_t gs, uint16_t ss, uint32_t edi, uint32_t esi,
                        uint32_t ebp, uint32_t esp, uint32_t ebx, uint32_t edx,
                        uint32_t ecx, uint32_t eax);
void print_exception_count();
void kill_monster(s_monster_t* monster, bool kill_now);

// -----------------------------------------------------------------------------
// Funciones internas:
// -----------------------------------------------------------------------------

void resolve_multitask_equal_positions(s_monster_t* monster);
void monster_war();
void init_fruits();
void print_screen_background();
void update_screen_fruit(uint8_t x, uint8_t y);
void update_screen_score(e_player_t player, uint32_t score);
void print_monster(s_monster_t* monster);
void update_screen();
s_position_t position(uint32_t x, uint32_t y);
s_position_t position_offset(s_position_t* pos, int32_t offset_x,
                             int32_t offset_y);
bool check_equal_positions(s_position_t* pos_a, s_position_t* pos_b);
e_player_t owner(s_monster_t* monster);
void set_monster(s_monster_t* monster, e_state_t task_state, uint8_t task_id, s_position_t position, uint32_t weight);
bool monster_alive(s_monster_t* monster);
bool monster_in_position(s_position_t* position,
                         s_monster_t monsters[TASK_LIMIT_PER_PLAYER]);
uint32_t absolute_value(int32_t value);
void clean_all();

#endif /* !__GAME_H__ */
