/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de la tabla de descriptores globales
*/

#include "gdt.h"

gdt_entry gdt[GDT_COUNT] = {
    /* Descriptor nulo*/
    /* Offset = 0x00 */
    [GDT_IDX_NULL_DESC] =
      (gdt_entry){
        (uint16_t)0x0000, /* limit[0:15]  */
        (uint16_t)0x0000, /* base[0:15]   */
        (uint8_t)0x00,    /* base[23:16]  */
        (uint8_t)0x00,    /* type         */
        (uint8_t)0x00,    /* s            */
        (uint8_t)0x00,    /* dpl          */
        (uint8_t)0x00,    /* p            */
        (uint8_t)0x00,    /* limit[16:19] */
        (uint8_t)0x00,    /* avl          */
        (uint8_t)0x00,    /* l            */
        (uint8_t)0x00,    /* db           */
        (uint8_t)0x00,    /* g            */
        (uint8_t)0x00,    /* base[31:24]  */
      },
    [GDT_IDX_DATA_LVL_0] =
      (gdt_entry){
        (uint16_t)0x99FF, /* limit[0:15] 666 2^8 4 K pag  */
        (uint16_t)0x0000, /* base[0:15]  ok */
        (uint8_t)0x00,    /* base[23:16] ok */
        (uint8_t)0x02,    /* type   0010 read write   */
        (uint8_t)0x01,    /* s            ok */
        (uint8_t)0x00,    /* dpl          ok */
        (uint8_t)0x01,    /* p            ok RAM*/
        (uint8_t)0x02,    /* limit[16:19] */
        (uint8_t)0x00,    /* avl          ok*/
        (uint8_t)0x00,    /* l            ok*/
        (uint8_t)0x01,    /* db           ok*/
        (uint8_t)0x01,    /* g            ok*/
        (uint8_t)0x00,    /* base[31:24]  */
      },
    [GDT_IDX_DATA_LVL_3] =
      (gdt_entry){
        (uint16_t)0x99FF, /* limit[0:15] 666 2^8 4 K pag  */
        (uint16_t)0x0000, /* base[0:15]  ok */
        (uint8_t)0x00,    /* base[23:16] ok */
        (uint8_t)0x02,    /* type   0010 read write   */
        (uint8_t)0x01,    /* s            ok */
        (uint8_t)0x03,    /* dpl          ok */
        (uint8_t)0x01,    /* p            ok RAM*/
        (uint8_t)0x02,    /* limit[16:19] */
        (uint8_t)0x00,    /* avl          ok*/
        (uint8_t)0x00,    /* l            ok*/
        (uint8_t)0x01,    /* db           ok*/
        (uint8_t)0x01,    /* g            ok*/
        (uint8_t)0x00,    /* base[31:24]  */
      },
    [GDT_IDX_CODE_LVL_0] =
      (gdt_entry){
        (uint16_t)0x99FF, /* limit[0:15] 666 2^8 4 K pag  */
        (uint16_t)0x0000, /* base[0:15]  ok */
        (uint8_t)0x00,    /* base[23:16] ok */
        (uint8_t)0x0A,    /* type   1010 read execute   */
        (uint8_t)0x01,    /* s            ok */
        (uint8_t)0x00,    /* dpl          ok */
        (uint8_t)0x01,    /* p            ok RAM*/
        (uint8_t)0x02,    /* limit[16:19] */
        (uint8_t)0x00,    /* avl          ok*/
        (uint8_t)0x00,    /* l            ok*/
        (uint8_t)0x01,    /* db           ok*/
        (uint8_t)0x01,    /* g            ok*/
        (uint8_t)0x00,    /* base[31:24]  */
      },
    [GDT_IDX_CODE_LVL_3] =
      (gdt_entry){
        (uint16_t)0x99FF, /* limit[0:15] 666 2^8 4 K pag  */
        (uint16_t)0x0000, /* base[0:15]  ok */
        (uint8_t)0x00,    /* base[23:16] ok */
        (uint8_t)0x0A,    /* type   1010 read execute   */
        (uint8_t)0x01,    /* s            ok */
        (uint8_t)0x03,    /* dpl          ok */
        (uint8_t)0x01,    /* p            ok RAM*/
        (uint8_t)0x02,    /* limit[16:19] */
        (uint8_t)0x00,    /* avl          ok*/
        (uint8_t)0x00,    /* l            ok*/
        (uint8_t)0x01,    /* db           ok*/
        (uint8_t)0x01,    /* g            ok*/
        (uint8_t)0x00,    /* base[31:24]  */
      },
    [GDT_IDX_SCREEN_LVL_0] =
      (gdt_entry){
        (uint16_t)0x1F3F, /* limit[0:15] (50*80 2*bytes) = 8000bytes -->
                             limite = tam - 1 = 7999 = F9F*/
        (uint16_t)0x8000, /* base[0:15]  ok  */
        (uint8_t)0x0B,    /* base[23:16] ok  a partir de la dir 0x0B8000 esta
                             el segmento de pantalla*/
        (uint8_t)0x02,    /* type   0010 read write   */
        (uint8_t)0x01,    /* s            ok */
        (uint8_t)0x00,    /* dpl          ok */
        (uint8_t)0x01,    /* p            ok RAM*/
        (uint8_t)0x00,    /* limit[16:19] */
        (uint8_t)0x00,    /* avl          ok*/
        (uint8_t)0x00,    /* l            ok*/
        (uint8_t)0x01,    /* db           ok*/
        (uint8_t)0x00,    /* g            ok*/
        (uint8_t)0x00,    /* base[31:24]  */
      }
};

gdt_descriptor GDT_DESC = { sizeof(gdt) - 1, (uint32_t)&gdt };
