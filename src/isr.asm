; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "print.mac"

BITS 32

global _isr32
global _isr33

global _isr71
global _isr73
global _isr76
global return_from_new_task

extern pic_finish1
extern sched_next_task
extern game_read
extern game_divide
extern game_move
extern game_next_step
extern kill_monster
extern current_monster
extern sched_inform_idle
extern print_debug_screen
extern print_screen_background
extern game_reset
extern update_screen
extern print_exception_count
extern print_digit

%define GDT_SEL_IDLE_TASK 0x180
%define GDT_SEL_NULL_DESC 0
%define idle_task 0x180:0
%define READ_LIMIT_EXCEEDED 50
%define KEY_CODE_J 0x24
%define KEY_CODE_A 0x1E

%define FALSE 0
%define TRUE 1

debug_mode_on: db FALSE

; Defines para hacer jump far:
offset: dd 0
selector: dw 0
%define selector_offset offset

;; MENSAJES DE EXCEPCIONES
int_0_mensaje: db "Divide Error", 0
int_0_long: equ $-int_0_mensaje

int_1_mensaje: db "RESERVED", 0 
int_1_long: equ $-int_1_mensaje

int_2_mensaje: db "NMI Interrupt", 0 
int_2_long: equ $-int_2_mensaje

int_3_mensaje: db "Breakpoint", 0
int_3_long: equ $-int_3_mensaje

int_4_mensaje: db "Overflow",0 
int_4_long: equ $-int_4_mensaje

int_5_mensaje: db "BOUND Range Exceeded", 0 
int_5_long: equ $-int_5_mensaje

int_6_mensaje: db "Invalid Opcode (Undefined Opcode)", 0 
int_6_long: equ $-int_6_mensaje

int_7_mensaje: db "Device Not Available (No Math Coprocessor)", 0 
int_7_long: equ $-int_7_mensaje

int_8_mensaje: db "Double Fault", 0 
int_8_long: equ $-int_8_mensaje

int_9_mensaje: db "Coprocessor Segment Overrun (reserved)", 0 
int_9_long: equ $-int_9_mensaje

int_10_mensaje: db "Invalid TSS", 0 
int_10_long: equ $-int_10_mensaje

int_11_mensaje: db "Segment Not Present", 0 
int_11_long: equ $-int_11_mensaje

int_12_mensaje: db "Stack-Segment Fault", 0 
int_12_long: equ $-int_12_mensaje

int_13_mensaje: db "General Protection", 0 
int_13_long: equ $-int_13_mensaje

int_14_mensaje: db "Page Fault", 0 
int_14_long: equ $-int_14_mensaje

int_15_mensaje: db "Intel reserved. Do not use", 0 
int_15_long: equ $-int_15_mensaje

int_16_mensaje: db "x87 FPU Floating-Point Error (Math Fault)", 0 
int_16_long: equ $-int_16_mensaje

int_17_mensaje: db "Alignment Check", 0 
int_17_long: equ $-int_17_mensaje

int_18_mensaje: db "Machine Check", 0 
int_18_long: equ $-int_18_mensaje

int_19_mensaje: db "SIMD Floating-Point Exception", 0 
int_19_long: equ $-int_19_mensaje

clean_debug_title: times 22 db " "
db 0


;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1

_isr%1:
  
  call current_monster
  push FALSE
  push eax
  call kill_monster
  pushad
  call toggle_debug_mode
  print_text_pm clean_debug_title, 22, 0, 3, 11
  print_text_pm int_%1_mensaje, int_%1_long, 0xE, 3, 11
  call print_exception_count
  call sched_inform_idle
  jmp idle_task

  ; No ponemos iret porque la tarea muere y nunca se vuelve a este punto de
  ; código. Por la misma razón no preservamos los registros.	
%endmacro


;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;

;;      %1: Puntero al mensaje
;;      %2: Longitud del mensaje
;;      %3: Color
;;      %4: Fila
;;      %5: Columna

; Reemplazo sintáctico de la macro
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19


;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
_isr32:
  pushad
  call pic_finish1
  call next_clock
  cmp byte [debug_mode_on], TRUE
  je .no_task_switch

  call sched_next_task
  
  cmp ax, GDT_SEL_NULL_DESC
  jne .end_if_0
    push eax
    call game_next_step
    pop eax
    jmp .no_task_switch
  .end_if_0:

  cmp ax, GDT_SEL_IDLE_TASK
  jne .end_if_1
    push eax
    call game_next_step
    pop eax
  .end_if_1:

  mov [selector], ax
  jmp far [selector_offset]
	
  .no_task_switch:
  popad
  iret

;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
_isr33:
  pushad
  call pic_finish1
  in al, 0x60 ; obtengo scan code leo el puerto del teclado y es un char 1 byte
  cmp al, KEY_CODE_J
  je .debug_mode
  ;cmp al, KEY_CODE_A
  ;je .reset
  push eax
  call print_digit
  add esp, 4
  jmp .end
  ;.reset:
  ;call game_reset
  ;call sched_inform_idle
  ;jmp idle_task
  .debug_mode:
  call toggle_debug_mode
  .end:
  popad
  iret

;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;

; 0x47 read
_isr71:
  ; in  eax = int32_t x
  ; in  ebx = int32_t y
  ; out eax = e_datatype_t r

  push ebp
  mov ebp, esp
  sub esp, 4    ; variable local [ebp-4] para el resultado
  pushad

  push ebx
  push eax
  call game_read
  mov [ebp-4], eax
  add esp, 8
	
  cmp eax, READ_LIMIT_EXCEEDED
  jne .continue_executing_current_task

  call sched_inform_idle
  jmp idle_task
	
  .continue_executing_current_task:
  popad
  pop eax
  pop ebp
  iret

; 0x49 move
_isr73:
  ; in  ecx = uint32_t distance
  ; in  edx = e_direction_t direction
  ; out eax = uint32_t max (distancia efectiva que se movió)
  push ebp
  mov ebp, esp
  sub esp, 4    ; variable local [ebp-4] para el resultado
  pushad

  push edx
  push ecx
  call game_move
  mov [ebp-4], eax
  add esp, 8

  call sched_inform_idle
  jmp idle_task
  popad
  pop eax
  pop ebp
  iret

; 0x4C divide
_isr76:
  ; STACK STATUS
  ; esp-->parámetro
  ; pushad
  ; xxx
  ; ebp
  ;-----
  push ebp
  mov ebp, esp
  sub esp, 4
  pushad
  push esp
  call game_divide
  add esp, 4
  mov [ebp-4], eax
  call sched_inform_idle
  jmp idle_task
  popad
  pop eax
  pop ebp
  iret

  return_from_new_task: ; x / 100 esp
  popad
  add esp, 4	
  xor eax, eax
  inc eax
  pop ebp
  iret

;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
isrNumber:           dd 0x00000000
isrClock:            db '|/-\'
next_clock:
  pushad
  inc DWORD [isrNumber]
  mov ebx, [isrNumber]
  cmp ebx, 0x4
  jl .ok
    mov DWORD [isrNumber], 0x0
    mov ebx, 0
  .ok:
  add ebx, isrClock
  print_text_pm ebx, 1, 0x0f, 49, 79
  popad
  ret

toggle_debug_mode:
  mov ax, [debug_mode_on]
  cmp ax, TRUE
  jne .else
    call print_screen_background
    call update_screen
    mov byte [debug_mode_on], FALSE
  jmp .end_if
  .else:
    popad
    pushad               ; push 32 bytes
    push ss
    push gs
    push fs
    push es
    push ds
    push cs
    call get_eip_in_eax
    push eax
    mov eax, cr4
    push eax
    mov eax, cr3
    push eax
    mov eax, cr2
    push eax
    mov eax, cr0
    push eax
    pushfd               ; push 4 bytes, total 80 (48 sin pushad)
    lea eax, [esp + 80]
    push eax
    call print_debug_screen
    add esp, 52
    mov byte [debug_mode_on], TRUE
  .end_if:
  ret

get_eip_in_eax:
  mov eax, [esp]
  ret
