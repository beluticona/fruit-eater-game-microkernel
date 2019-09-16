; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "print.mac"


global start
global try
extern GDT_DESC
extern IDT_DESC
extern idt_init
extern mmu_init
extern mmu_init_kernel_directory
extern mmu_init_task_directory
extern pic_enable
extern pic_reset
extern tss_init
extern sched_init
extern game_init


;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
start_rm_msg db     'Iniciando kernel en Modo Real'
start_rm_len equ    $ - start_rm_msg

start_pm_msg db     'Iniciando kernel en Modo Protegido'
start_pm_len equ    $ - start_pm_msg

%define GDT_SEL_CODE_LVL_0 0xC0
%define GDT_SEL_DATA_LVL_0 0xB0
%define GDT_SEL_SCREEN_LVL_0 0xD0

%define idle_task 0x180:0
%define GDT_SEL_INITIAL_TASK 0xD8

%define TURN_ON_PAGINATION 		0x80000000
%define STACK_BASE_KERNEL 		0x27000
%define C_BG_LIGHT_GREY_x2         	0x70007000 ;(0x7 << 4)  repìte 2 veces para completar 32 bits
%define KERNEL_PAGE_DIRECTORY_ADDRESS 	0x27000


;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    print_text_rm start_rm_msg, start_rm_len, 0x07, 0, 0
    
    ; Habilitar A20
    call A20_enable
    
    ; Cargar la GDT
    lgdt [GDT_DESC]				;Carga lo que tiene en GDT_DESCriptor o sea limite++puntero a tabla

    ; Setear el bit PE del registro CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Saltar a modo protegido
    jmp GDT_SEL_CODE_LVL_0:modo_protegido ; JMP <selector>:<offset> JMP kernel.asm : modo_protegido
    ; selector des segmento
    ; TI[2] : GDT/LDT  1
    ; RPL[1-0] : level  00

BITS 32
modo_protegido:

    ; Establecer la base de la pila
    mov esp, STACK_BASE_KERNEL
    mov ebp, esp 

    ; Establecer selectores de segmentos
    xor eax, eax
    mov ax, GDT_SEL_DATA_LVL_0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov gs, ax
    mov ax, GDT_SEL_SCREEN_LVL_0
    mov fs, ax

    ; Imprimir mensaje de bienvenida
    print_text_pm start_pm_msg, start_pm_len, 0x0D, 2, 0

    ; Inicializar pantalla
    call limpiar_pantalla
    
    ; Inicializar el manejador de memoria
    call mmu_init
 
    ; Inicializar el directorio de paginas
    call mmu_init_kernel_directory
    
    ; Cargar directorio de paginas
    mov eax, KERNEL_PAGE_DIRECTORY_ADDRESS
    mov cr3, eax

    ; Habilitar paginacion
    mov eax, cr0
    or eax, TURN_ON_PAGINATION
    mov cr0, eax  

    ; Inicializar tss
    ; (llena las tss de la tarea idle y de todas las tareas de los jugadores)
    call tss_init

    ; Inicializar el scheduler
    call sched_init

    ; Inicializar la IDT
    call idt_init

    ; Inicializar juego
    ; (pinta la pantalla y pone las tareas iniciales de los jugadores en TO_EXEC para que las agarre el scheduler en la próxima interrupción de reloj)    
    call game_init   

    ; Cargar IDT
    lidt [IDT_DESC]

    ; Generar excepción de prueba
    ; xor edi, edi
    ; div edi
    
    ; Código para probar el ejercicio 3D:
    ; xor eax, eax
    ; xchg bx, bx
    ; int 71 ; cambia el valor de eax por 42

    ; Configurar controlador de interrupciones
    call pic_reset	
    call pic_enable	

    ; Ejercicio 5D
    ; xchg bx, bx
    ; call mmu_init_task_directory
    ; mov cr3, eax

    ; Cargar tarea inicial
    mov cx, GDT_SEL_INITIAL_TASK
    ltr cx

    ; Habilitar interrupciones
    sti	

    ; Saltar a la primera tarea: Idle
    jmp idle_task
    
    ; Jump a tarea con RPL 0--> OK!
    ; jmp ;0b11101000:0

    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $


limpiar_pantalla: ; 50x50 gris + 30x50 negro
	
	xor eax,eax	
	xor ecx, ecx			; ecx<-negro	
	mov edi, 50
	xor esi, esi

.bigLoop:
	mov esi, 25
	.greyLoop:
		
		;xchg bx, bx		;magic breakpoint

		mov dword[fs:eax], C_BG_LIGHT_GREY_x2 ; ok
		add eax, 4
		dec esi
		cmp esi, 0
		jne .greyLoop

	mov esi, 15
	
	.blackLoop:
		
		mov dword[fs:eax], ecx
		add eax, 4
		dec esi
		cmp esi, 0
		jne .blackLoop	

	dec edi
	cmp edi, 0
	jne .bigLoop
	
	ret

;colorear_pantalla:

;C_BG_LIGHT_GREY_x2	
;C_BG_BLACK_x2 
	
;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
