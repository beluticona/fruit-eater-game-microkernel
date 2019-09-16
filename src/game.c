/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "sched.h"

e_fruit_t fruits[MAP_WIDTH][MAP_HEIGHT];

void init_fruits() {
  uint16_t i;
  for (uint8_t x = 0; x < MAP_WIDTH; ++x) {
    for (uint8_t y = 0; y < MAP_HEIGHT; ++y) {
      i = MAP_HEIGHT * y + x;
      if (i % 44 == 0) {
        fruits[x][y] = Grape;
      } else if (i % 44 == 15) {
        fruits[x][y] = Banana;
      } else if (i % 88 == 30) {
        fruits[x][y] = Strawberry;
      } else {
        fruits[x][y] = None;
      }
    }
  }
}

void set_monster(s_monster_t* monster, e_state_t task_state, uint8_t task_id, s_position_t position, uint32_t weight) {
  monster->task_state = task_state;
  monster->task_id = task_id;
  monster->weight = weight;
  monster->position = position;
  monster->next_position = position;
  monster->read_count = 0;
}

void update_screen_score(e_player_t player, uint32_t score) {
  if (player == player_A) {
    print_dec(score, 10, 60, 5, C_FG_WHITE | C_BG_RED);
  } else {
    print_dec(score, 10, 60, 30, C_FG_WHITE | C_BG_BLUE);
  }
}

void update_screen_fruit(uint8_t x, uint8_t y) {
  switch (fruits[x][y]) {
    case Banana:
      print(" ", x, y, C_FG_WHITE | C_BG_BROWN);
      break;
    case Grape:
      print(" ", x, y, C_FG_WHITE | C_BG_CYAN);
      break;
    case Strawberry:
      print(" ", x, y, C_FG_WHITE | C_BG_MAGENTA);
      break;
    case None:
      print(" ", x, y, C_FG_BLACK | C_BG_LIGHT_GREY);
  }
}

void print_screen_background() {
  screen_drawBox(0, 0, 50, 50, '\0', C_BG_LIGHT_GREY);
  screen_drawBox(50, 0, 30, 50, '\0', C_BG_BLACK);
  screen_drawBox(58, 4, 14, 3, '\0', C_BG_RED);
  screen_drawBox(58, 29, 14, 3, '\0', C_BG_BLUE);
  print("0 1 2 3 4 5 6 7 8 9", 55, 10, C_FG_WHITE);
  print("0 1 2 3 4 5 6 7 8 9", 55, 35, C_FG_WHITE);
}

void game_reset() {
  // Para apretar una tecla y que vuelva a empezar el juego.
  // No funciona aún.
  sched_init();
  game_init();
}

void game_init() {
  exception_count = 0;
  score[player_A] = 0;
  score[player_B] = 0;

  init_fruits();
  print_screen_background();

  /* Cada jugador debe arrancar con dos tareas lanzadas.
  Para lanzar estas tareas iniciales y que luego el juego continúe avanzando se
  debe:
    1) [Hecho en tss_init] Completar los segmento de estado (TSS).
    2) [Hecho en tss_init] Indicar estas TSS en la GDT.
    3) [Hecho acá] Copiar el código de las tareas desde el origen a su
  correspondiente dirección física.
    4) [Hecho acá] Inicializar la estructura s_monster_t con estado TO_EXEC.
    5) [Hecho] Completar la función sched_next_task para que el scheduler
  ejecute las tareas pendientes en próximas interrupciones de reloj.
    6) [Hecho acá] Pintar pantalla inicial.
    7) [Falta hacer] Ir actualizando la pantalla y las s_monter_t luego de cada
  turno en game_next_step
    8) [Hecho en isr.asm y game.c] Completar las syscall move, divide y read
  para que la inteligencia artificial de los monstruos pueda jugar.
  */

  mmu_mem_copy(TASK_A_CODE_SRC_ADDR, code_address[0]);
  mmu_mem_copy(TASK_A_CODE_SRC_ADDR + PAGE_SIZE_4K, user_stack_address[0]);
  mmu_mem_copy(TASK_B_CODE_SRC_ADDR, code_address[10]);
  mmu_mem_copy(TASK_B_CODE_SRC_ADDR + PAGE_SIZE_4K, user_stack_address[10]);

  set_monster(&tareas_A[0], ToBeScheduled, 0, position(0, 0), 64);
  set_monster(&tareas_B[0], ToBeScheduled, 10, position(25, 25), 64);

  for (uint8_t i = 1; i < TASK_LIMIT_PER_PLAYER; ++i) {
    set_monster(&tareas_A[i], Unscheduled, i, position(0, 0), 0);
    set_monster(&tareas_B[i], Unscheduled, i + 10, position(0, 0), 0);
  }

  update_screen();
}

/*Agrego las tareas de ambos jugadores en el arreglo equal_positions*/
void resolve_multitask_equal_positions(s_monster_t* monster) {
  
  for (uint8_t j = 0; j < 20; ++j) {
    if (equal_positions[j][0] != 0){
      if(check_equal_positions(&equal_positions[j][0]->position,
                              &monster->position) == TRUE) {
      	/* Agrego la tarea en array d tareas con la misma posicion */      
	for (uint8_t k = 0; k < 20; ++k) {
          if (equal_positions[j][k] == 0) {
            equal_positions[j][k] = monster;
            break;
	  }
	}
      }
    }else {
      equal_positions[j][0] = monster;
      break;
    }
  }
}

void monster_war() {
  /*sumo los pesos totales de cada jugador y los almaceno en el arreglo
   * suma_pesos*/
  for (uint8_t i = 0; i < TASK_LIMIT; ++i) {
    for (uint8_t j = 0; j < TASK_LIMIT; ++j) {
      if (equal_positions[i][j] != 0) {
        if (owner(equal_positions[i][j]) == player_A) {
          suma_pesos[i].peso_A += equal_positions[i][j]->weight;
        } else {
          suma_pesos[i].peso_B += equal_positions[i][j]->weight;
        }
      }
    }
  }
  /*Voy recorriendo el arreglo suma pesos para determinar quien gana en el
   * casillero*/
  for (uint8_t i = 0; i < TASK_LIMIT; ++i) {
    if (equal_positions[i][0] !=
        0) { /* obtengo la posicion de la primer tarea en el arreglo
                equal_positions*/
      uint32_t x = (equal_positions[i][0]->position).x;
      uint32_t y = (equal_positions[i][0]->position).y;
      if (suma_pesos[i].peso_A > suma_pesos[i].peso_B &&
          suma_pesos[i].peso_B != 0) {
        // Caso: gana A
        for (uint8_t j = 0; j < TASK_LIMIT; ++j) {
          if (equal_positions[i][j] != 0) {
            if (owner(equal_positions[i][j]) == player_B) {
              // breakpoint();
              kill_monster(equal_positions[i][j], TRUE);
            }
          }
        }
        if (fruits[x][y] != None) {
          score[0] += fruits[x][y];
          fruits[x][y] = None;
        }
      } else if (suma_pesos[i].peso_B > suma_pesos[i].peso_A &&
                 suma_pesos[i].peso_A != 0) {
        // CASO: gana B
        for (uint8_t j = 0; j < TASK_LIMIT; ++j) {
          if (equal_positions[i][j] != 0) {
            if (owner(equal_positions[i][j]) == player_A) {
              // breakpoint();
              kill_monster(equal_positions[i][j], TRUE);
            }
          }
        }
        if (fruits[x][y] != None) {
          score[1] += fruits[x][y];
          fruits[x][y] = None;
        }
      } else if (suma_pesos[i].peso_A != 0 && suma_pesos[i].peso_B == 0) {
        // CASO: solo A
        if (fruits[x][y] != None) {
          score[0] += fruits[x][y];
          fruits[x][y] = None;
        }
      } else if (suma_pesos[i].peso_B != 0 && suma_pesos[i].peso_A == 0) {
        // CASO: solo B
        if (fruits[x][y] != None) {
          score[1] += fruits[x][y];
          fruits[x][y] = None;
        }
      } else {  //caso A y B con el mismo peso, mata todas las tareas y nadie come la fruta
		for (uint8_t j = 0; j < TASK_LIMIT; ++j) {
          if (equal_positions[i][j] != 0) {
              kill_monster(equal_positions[i][j], TRUE);
          }
        }
	  }
    }
  }
}

void game_next_step() {
  clean_all();
  for (uint8_t i = 0; i < TASK_LIMIT_PER_PLAYER; ++i) {
    // ACTUALIZO POSICION
    if ((monster_alive(&tareas_B[i]) == TRUE &&
         check_equal_positions(&tareas_B[i].position,
                               &tareas_B[i].next_position) == FALSE)) {
      
      tareas_B[i].position = tareas_B[i].next_position;
    }

    if ((monster_alive(&tareas_A[i]) == TRUE &&
         check_equal_positions(&tareas_A[i].position,
                               &tareas_A[i].next_position) == FALSE)) {
      tareas_A[i].position = tareas_A[i].next_position;
    }
    /*Agrego las tareas de ambos jugadores en el arreglo equal_positions*/
    if (monster_alive(&tareas_A[i]) == TRUE) {
      resolve_multitask_equal_positions(&tareas_A[i]);
      tareas_A[i].read_count = 0;
    }
    if (monster_alive(&tareas_B[i]) == TRUE) {
      resolve_multitask_equal_positions(&tareas_B[i]);
      tareas_B[i].read_count = 0;
    }

    switch (tareas_B[i].task_state) {
      case ToBeScheduled:
        sched_add_task(tareas_B[i].task_id);
        tareas_B[i].task_state = Scheduled;
        break;
      case ToBeUnscheduled:
        sched_remove_task(tareas_B[i].task_id);
        tareas_B[i].task_state = Unscheduled;
    }

    switch (tareas_A[i].task_state) {
      case ToBeScheduled:
        sched_add_task(tareas_A[i].task_id);
        tareas_A[i].task_state = Scheduled;
        break;
      case ToBeUnscheduled:
        sched_remove_task(tareas_A[i].task_id);
        tareas_A[i].task_state = Unscheduled;
    }
  }  // end for
  monster_war();  
  update_screen();
}

void print_exception_count() {
  print_dec(exception_count++, 2, 37, 3, C_FG_DARK_GREY | C_BG_BLACK);
}

void clean_all() {
  // limpio equal_positions
  for (uint8_t i = 0; i < TASK_LIMIT; ++i) {
    // clean suma_pesos
    suma_pesos[i].peso_A = 0;
    suma_pesos[i].peso_B = 0;
    for (uint8_t j = 0; j < TASK_LIMIT; ++j) {
      equal_positions[i][j] = 0;
    }
  }
}

void print_debug_screen(uint32_t stack_elems[5], uint16_t eflags, uint32_t cr0,
                        uint32_t cr2, uint32_t cr3, uint32_t cr4, uint32_t eip,
                        uint16_t cs, uint16_t ds, uint16_t es, uint16_t fs,
                        uint16_t gs, uint16_t ss, uint32_t edi, uint32_t esi,
                        uint32_t ebp, uint32_t esp, uint32_t ebx, uint32_t edx,
                        uint32_t ecx, uint32_t eax) {
  print("DEBUG MODE", 69, 1, C_FG_WHITE | C_BG_BLACK);
  screen_drawBox(10, 2, 30, 38, '\0', C_BG_BLACK);
  screen_drawBox(11, 5, 28, 34, '\0', C_BG_LIGHT_GREY);
  const uint16_t c_b = C_FG_BLACK | C_BG_LIGHT_GREY;
  const uint16_t c_w = C_FG_WHITE | C_BG_LIGHT_GREY;
  print("No exception generated", 11, 3, C_FG_DARK_GREY);
  print("eax", 12, 6, c_b);
  print_hex(eax, 8, 16, 6, c_w);
  print("ebx", 12, 8, c_b);
  print_hex(ebx, 8, 16, 8, c_w);
  print("ecx", 12, 10, c_b);
  print_hex(ecx, 8, 16, 10, c_w);
  print("edx", 12, 12, c_b);
  print_hex(edx, 8, 16, 12, c_w);
  print("esi", 12, 14, c_b);
  print_hex(esi, 8, 16, 14, c_w);
  print("edi", 12, 16, c_b);
  print_hex(edi, 8, 16, 16, c_w);
  print("ebp", 12, 18, c_b);
  print_hex(ebp, 8, 16, 18, c_w);
  print("esp", 12, 20, c_b);
  print_hex(esp, 8, 16, 20, c_w);
  print("eip", 12, 22, c_b);
  print_hex(eip, 8, 16, 22, c_w);
  print("cs", 12, 24, c_b);
  print_hex(((uint32_t)cs) & 0xFFFF, 4, 16, 24, c_w);
  print("ds", 12, 26, c_b);
  print_hex(((uint32_t)ds) & 0xFFFF, 4, 16, 26, c_w);
  print("es", 12, 28, c_b);
  print_hex(((uint32_t)es) & 0xFFFF, 4, 16, 28, c_w);
  print("fs", 12, 30, c_b);
  print_hex(((uint32_t)fs) & 0xFFFF, 4, 16, 30, c_w);
  print("gs", 12, 32, c_b);
  print_hex(((uint32_t)gs) & 0xFFFF, 4, 16, 32, c_w);
  print("ss", 12, 34, c_b);
  print_hex(((uint32_t)ss) & 0xFFFF, 4, 16, 34, c_w);
  print("eflags", 12, 37, c_b);
  print_hex(((uint32_t)eflags) & 0xFFFF, 8, 18, 37, c_w);
  print("cr0", 26, 6, c_b);
  print_hex(cr0, 8, 30, 6, c_w);
  print("cr2", 26, 8, c_b);
  print_hex(cr2, 8, 30, 8, c_w);
  print("cr3", 26, 10, c_b);
  print_hex(cr3, 8, 30, 10, c_w);
  print("cr4", 26, 12, c_b);
  print_hex(cr4, 8, 30, 12, c_w);

  print("stack", 26, 24, c_b);
  for (uint8_t i = 0; i < 5; ++i) {
    print_hex(stack_elems[i], 8, 26, 27 + i, c_w);
  }
}

void print_digit(uint8_t scancode) {
  // Los números 1, 2, ..., 9, 0 tienen scancodes 0x02, 0x03, ..., 0x0B
  if (0x02 <= scancode && scancode <= 0x0B) {
    // Los códigos ASCII de los números 1 a 9 son de 49 a 57, el código ASCII
    // del número 0 es 48 (el cero está primero, a diferencia de los scancodes)
    char str[2] = {scancode == 0x0B ? '0' : (char)(scancode + 47), '\0'};
    print(str, 79, 0, C_FG_LIGHT_CYAN);
  }
}

int32_t game_divide(uint32_t esp) {
  /*
          Game_divide:
          1. Copiar código de la tarea
          2. Copiar pila lvl 0 y 3
          3. Set_TSS_new_mostear:
                  I. TSS->esp = esp0 | esp & 0xFFFF;
                  II. TSS->eip = return_from_new_task;
                  II. Otras...

          Si current.peso = 1 sigue igual
          si es mayor a 2> y no hay espacio--> -1
          si es mayor a 2 y hay va 0
  */
  s_monster_t* monster = current_monster();
  e_player_t player = owner(monster);
  s_monster_t* monsters;  // defino el arrelo con un puntero a la primer posicion
  if (player == player_A) {
    monsters = tareas_A;
  } else {
    monsters = tareas_B;
  }
  if (monster->weight <= 1) {
    // Mi peso no me permite dividir
    return -1;
  }

  uint8_t monster_id = 0;
  monster->weight >>= 1;
  while (monster_id < TASK_LIMIT_PER_PLAYER && monsters[monster_id].task_state != Unscheduled) {
    ++monster_id;
  }
  if (monster_id == TASK_LIMIT_PER_PLAYER) {
    // No habìa lugar disponible
    return -1;
  }

  // Hay lugar
  uint8_t task_id = player == player_A ? monster_id : monster_id + 10;
  mmu_mem_copy(TASK_CODE_VIRTUAL_ADDR,
               code_address[task_id]);  // Copio página de codigo de tarea
  mmu_mem_copy(TASK_STACK_VIRTUAL_ADDR_LVL_3,
               user_stack_address[task_id]);  // Copio pila lvl 3
  mmu_mem_copy(kernel_stack_address[monster->task_id],
               kernel_stack_address[task_id]);  // Copio pila lvl 0
  set_monster(&monsters[monster_id], ToBeScheduled, task_id, monster->position, monster->weight);
  tss_set(esp, task_id);
  return 0;

}

bool monster_in_position(s_position_t* position,
                         s_monster_t monsters[TASK_LIMIT_PER_PLAYER]) {
  for (uint8_t i = 0; i < TASK_LIMIT_PER_PLAYER; ++i) {
    if (monster_alive(&monsters[i]) == TRUE &&
        check_equal_positions(&monsters[i].position, position) == TRUE) {
      return TRUE;
    }
  }
  return FALSE;
}

uint32_t game_read(int32_t offset_x, int32_t offset_y) {
  s_monster_t* monster = current_monster();

  if (++monster->read_count == monster->weight) {
    return ReadLimitExceeded;
  }

  if (absolute_value(offset_x) + absolute_value(offset_y) > monster->weight) {
    return Null;
  }

  s_position_t pos = position_offset(&monster->position, offset_x, offset_y);

  if (fruits[pos.x][pos.y] != None) {
    return Food;
  }

  // Si en la misma posición hay un amigo y un enemigo, la idea es devolver
  // siempre enemigo para que vaya a comerlo.

  if (owner(monster) == player_A) {
    if (monster_in_position(&pos, tareas_B) == TRUE) {
      return Opponent;
    }
    if (monster_in_position(&pos, tareas_A) == TRUE) {
      return Player;
    }
  }
  // Jugador actual es B:
  if (monster_in_position(&pos, tareas_A) == TRUE) {
    return Opponent;
  }
  if (monster_in_position(&pos, tareas_B) == TRUE) {
    return Player;
  }

  return Empty;

  // En la misma posición puede haber fruta y uno o más monstruos. En ese caso
  // es decisión nuestra qué devolver, ya que no se puede dar toda la info.
}

uint32_t absolute_value(int32_t value) {
  return value > 0 ? value : value * -1;
}

e_player_t owner(s_monster_t* monster) {
  return monster->task_id < 10 ? player_A : player_B;
}

bool monster_alive(s_monster_t* monster) {
  switch (monster->task_state) {
    case Scheduled:
    case ToBeUnscheduled:
      return TRUE;
    case Unscheduled:
    case ToBeScheduled:
    default:
      return FALSE;
  }
}

uint32_t game_move(uint32_t distance, e_direction_t direction) {
  s_monster_t* monster = current_monster();
  int32_t offset = (int32_t)(distance % monster->weight);
  switch (direction) {
    case Right:
      monster->next_position = position_offset(&monster->position, offset, 0);
      break;
    case Left:
      monster->next_position =
          position_offset(&monster->position, offset * (-1), 0);
      break;
    case Up:
      monster->next_position =
          position_offset(&monster->position, 0, offset * (-1));
      break;
    case Down:
      monster->next_position = position_offset(&monster->position, 0, offset);
      break;
  }
  return offset;
}

s_monster_t* current_monster() {
  uint8_t task_id = sched_current_task_id();
  if (task_id < 10) {
    return &tareas_A[task_id];
  }
  return &tareas_B[task_id - 10];
}

void update_screen() {
  for (uint32_t x = 0; x < MAP_WIDTH; ++x) {
    for (uint32_t y = 0; y < MAP_HEIGHT; ++y) {
      update_screen_fruit(x, y);
    }
  }
  for (uint8_t i = 0; i < TASK_LIMIT_PER_PLAYER; ++i) {
    print_monster(&tareas_B[i]);
    print_monster(&tareas_A[i]);
  }
  update_screen_score(player_B, score[player_B]);
  update_screen_score(player_A, score[player_A]);
}

void print_monster(s_monster_t* monster) {
  e_player_t player = owner(monster);
  char str_alive[2] = {'-', '\0'};
  if (monster_alive(monster) == TRUE) {
    uint16_t color = player == player_A ? C_FG_LIGHT_RED | C_BG_RED
                                        : C_FG_LIGHT_BLUE | C_BG_BLUE;
    char str_num[2] = {'0' + (char)(monster->task_id % 10), '\0'};
    print(str_num, monster->position.x, monster->position.y, color);
    str_alive[0] = 'O';
  }

  if (player == player_A) {
    print(str_alive, 55 + (monster->task_id * 2), 12, C_FG_WHITE | C_BG_BLACK);
  } else {
    print(str_alive, 55 + ((monster->task_id - 10) * 2), 37, C_FG_WHITE | C_BG_BLACK);
  }
}

s_position_t position(uint32_t x, uint32_t y) { return (s_position_t){x, y}; }

bool check_equal_positions(s_position_t* pos_a, s_position_t* pos_b) {
  return pos_a->x == pos_b->x && pos_a->y == pos_b->y;
}

s_position_t position_offset(s_position_t* pos, int32_t offset_x,
                             int32_t offset_y) {
  int32_t x = (((int32_t)(pos->x)) + offset_x) % MAP_WIDTH;
  int32_t y = (((int32_t)(pos->y)) + offset_y) % MAP_HEIGHT;
  if (x < 0) x += MAP_WIDTH;
  if (y < 0) y += MAP_HEIGHT;
  return position((uint32_t)x, (uint32_t)y);
}

void kill_monster(s_monster_t* monster, bool kill_now) {
  if (kill_now == TRUE) {
    monster->task_state = Unscheduled;
    sched_remove_task(monster->task_id);
  } else {
    monster->task_state = ToBeUnscheduled;
  }
}
