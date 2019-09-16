/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"

// REVISAR DEFINES.H

/*seteo dirección desde la cual comienza el área de tareas y de kernel
para iniciliar directorio, tabla y pila de nivel 0 0x0100000
para código pila y memoria de tareas de nivel 3 0x0400000 */
void mmu_init() {
  mmu_next_free_kernel_page = KERNEL_FREE_AREA_START;
  mmu_next_free_task_page = TASK_FREE_AREA_START;

  // Pide las páginas para todas las tareas, se las guarda en los arrays y mapea
  // los directorios
  for (uint8_t i = 0; i < TASK_LIMIT; i++) {
    code_address[i] = mmu_get_next_free_task_page();
    user_stack_address[i] = mmu_get_next_free_task_page();
    directory_address[i] = mmu_get_next_free_kernel_page();
    table_address_kernel[i] = mmu_get_next_free_kernel_page();
    table_address_task[i] = mmu_get_next_free_kernel_page();
    kernel_stack_address[i] = mmu_get_next_free_kernel_page();
    mmu_init_task_directory(i);
  }
}

uint32_t mmu_get_next_free_kernel_page() {
  uint32_t free_page = mmu_next_free_kernel_page;
  mmu_next_free_kernel_page += PAGE_SIZE_4K;
  return free_page;
}

uint32_t mmu_get_next_free_task_page() {
  uint32_t free_page = mmu_next_free_task_page;
  mmu_next_free_task_page += PAGE_SIZE_4K;
  return free_page;
}

void mmu_init_task_directory(uint8_t task_index) {
  // inicializa el directorio de paginas para cada tarea
  mmu_map_kernel_pages((PDE*)(directory_address[task_index]),
                       (PTE*)(table_address_kernel[task_index]));
  mmu_map_page(TASK_CODE_VIRTUAL_ADDR, directory_address[task_index],
               code_address[task_index], (PTE*)table_address_task[task_index],
               PAG_US_U);
  mmu_map_page(TASK_STACK_VIRTUAL_ADDR_LVL_3, directory_address[task_index],
               user_stack_address[task_index],
               (PTE*)table_address_task[task_index], PAG_US_U);
}

void mmu_map_page(uint32_t virtual, uint32_t cr3, uint32_t phy, PTE* new_table,
                  uint8_t u_s) {
  /*
  > accedo a mi directorio base con cr3 (castear)
  > de la virtual obtener (shiftear según corresponda!) 1) indice directorio 2)
  indice pageTable 3) offset física
  > ingresar en la PDE correspondiente y obtener base de la PageTable
  > Una vez dentro de la PageTable, ingresar al indice correspondiente  y
  escribir en su basPage la física
  > Setear también atributos pasado por parámetros @Belu: si llegan como attrs
  por ahí conviene modificar el struct PTE o que no lo reciba así

      */
  PDE* directory = (PDE*)cr3;
  uint16_t directory_index = (virtual >> 22);
  uint16_t pageTable_index = ((virtual << 10) >> 22);
  PTE* base_table;

  // Obtenemos la tabla o la creamos
  if (directory[directory_index].p == PAG_P) {
    base_table = (PTE*)(directory[directory_index].baseTable << 12);
  } else {
    // acá solo va a entrar cuando mapeamos las páginas 0x8000000 y 0x8001000 de
    // una tarea
    base_table = new_table;
    init_new_pt(base_table, FALSE, PAG_NP, u_s);
    directory[directory_index].baseTable = ((uint32_t)base_table) >> 12;
    directory[directory_index].u_s = u_s;
    directory[directory_index].r_w = PAG_RW;
    directory[directory_index].p = PAG_P;
  }
  // Desmapeamos la dir virtual si ya estaba mapeada a otra direccion fisica

  if (base_table[pageTable_index].p == PAG_P) {
    mmu_unmap_page(virtual, cr3);
  }

  // Mapeamos la nueva dir virtual a la dir fisica
  set_new_page(base_table + pageTable_index, (phy >> 12), PAG_P, u_s);

  tlbflush();
}

void mmu_unmap_page(uint32_t virtual, uint32_t cr3) {
  PDE* directory = (PDE*)cr3;
  uint16_t directory_index = (virtual >> 22);
  uint16_t pageTable_index = ((virtual << 10) >> 22);
  PDE dir_entry = directory[directory_index];
  PTE* base_table = (PTE*)(dir_entry.baseTable << 12);
  base_table[pageTable_index].p = PAG_NP;
  // [TODO] poner la PDE como no presente si quedo la tabla vacia. Si
  // mejoraramos el manejo de memoria para reutilizar las paginas, aca podriamos
  // reutilizar la pagina vacia de la tabla

  tlbflush();
}

/*mmu_initKernelDir(): Mapear por identity mapping el directorio del kernel, sus
 * páginas, es decir, de las direcciones 0x0 a 0x3FFFFF. Ademas inicializar
 * directorio en 0x27000 y la tabla de páginas en la página contigua, en 0x28000
 * (siguientes 4 KB) */
void mmu_init_kernel_directory() {
  mmu_map_kernel_pages((PDE*)KERNEL_PAGE_DIR, (PTE*)KERNEL_PAGE_TABLE_0);
}

void mmu_map_kernel_pages(PDE* kernel_pd, PTE* kernel_pt) {
  // Seteo atributos de la primera PageTable de directory

  // Seteo bit de presente la Page Table #define PAG_P 0x00000001 @Belu: esto
  // vale? Si!
  // El resto de las paginas van seteadas en no presentes NP
  // inicializa todos los atributos de la estructura PDE en cero
  kernel_pd[0].p = PAG_P;
  kernel_pd[0].r_w = PAG_RW;
  kernel_pd[0].u_s = PAG_US_S; //PAG_US_U; para hacer print en nivel 3
  kernel_pd[0].pwt = 0;
  kernel_pd[0].pcd = 0;
  kernel_pd[0].a = 0;
  kernel_pd[0].ign = 0;
  kernel_pd[0].ps = 0;
  kernel_pd[0].g = 0;
  kernel_pd[0].avl = 0;

  // Seteo la base de la primera Page Table.
  kernel_pd[0].baseTable = ((uint32_t)kernel_pt) >> 12;
  // Cuando lo lea voy a agregar 12 bits con 0 a la derecha, ergo solo tomo los
  // 20 bits más significativos
  for (uint16_t i = 1; i < PAGINATION_ARRAY_SIZE; i++) {
    kernel_pd[i].p = PAG_NP;
  }

  init_new_pt(kernel_pt, TRUE, PAG_P, PAG_US_S); //PAG_US_U); para hacer print en nivel 3
}

void init_new_pt(PTE* page_table, bool identidad, bool is_present,
                 uint8_t u_s) {
  // Seteo atributos de las 1024 Pages correspondientes a esta primera Page
  // Table
  // Mapear 0x00000000 a 0x003FFFFF significa mapear las 1024 páginas de cada
  // una 4 KB, es decir 2*10 por 4 2*10 = 4 .2*20
  // De 0x0 a 0xFFFFF son 2*20, luego esto por 4 de 0x00000 a 0x300000

  if (identidad == TRUE) {
    for (uint32_t i = 0; i < PAGINATION_ARRAY_SIZE; i++) {
      set_new_page(page_table + i, i, is_present, u_s);
    }
  } else {
    for (uint32_t i = 0; i < PAGINATION_ARRAY_SIZE; i++) {
      set_new_page(page_table + i, 0, is_present, u_s);
    }
  }
}

void set_new_page(PTE* page_table_entry, uint32_t basePage, uint8_t present,
                  uint8_t u_s) {
  page_table_entry->r_w = PAG_RW;
  page_table_entry->u_s = u_s;
  page_table_entry->pwt = 0;
  page_table_entry->pcd = 0;
  page_table_entry->a = 0;
  page_table_entry->d = 0;
  page_table_entry->pat = 0;
  page_table_entry->g = 0;
  page_table_entry->avl = 0;
  page_table_entry->p = present;
  page_table_entry->basePage = basePage;
}

// tarea i se divide
// buscar una task libre--> j
// entrar en las struct de j (ss nuestros arrays) recorrerlos y escribir lo que
// tiene estrucutra[i]

// caso lanzar tarea por si sola (sin dvide)
void mmu_mem_copy(uint32_t origen_virtual, uint32_t destino_fisica) {
  uint32_t* dest = (uint32_t*)0x9000000;  // asumimos que no está mapeada
  uint32_t* src = (uint32_t*)origen_virtual;
  mmu_map_page((uint32_t)dest, rcr3(), destino_fisica, 0, PAG_US_S);
  const uint16_t LIM = PAGE_SIZE_4K/4;
  for (uint16_t i = 0; i < LIM; i++) {
    dest[i] = src[i];
  }
  mmu_unmap_page((uint32_t)dest, rcr3());
}
