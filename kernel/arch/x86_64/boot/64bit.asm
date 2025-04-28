BITS 64

global start64
extern kernel_start
extern multiboot_ptr
extern _bs_stack_top
extern kernel_gp_end
extern _higher_l2_page
extern _higher_l1_page

extern _l4_page
extern _l3_page
extern _l2_page
extern _l1_page

section .loading

start64:

mov esp, _bs_stack_top
mov ebp, esp

mov ax, 0
mov ss, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

; Fill the last two entries in the l3 page table for the last two gigabytes
mov rcx, 510
.l2_loop:

    ; Put the address into rdx
    ; multiply the size of a page by the rcx
    mov rdx, _higher_l2_page
    mov rax, 4096
    mul rcx
    add rax, rdx
    or rax, 3
    ; The plus 4096 moves u into the second table
    mov [_l3_page + 4096 + rcx * 8], rax

    inc rcx
    cmp rcx, 512
    jne .l2_loop

; Calculate the size of the kernel
; Everything is gaurneteed to be aligned to 2 MB boundries
mov rax, kernel_gp_end
; Get rid of higher half
sub rax, 0xFFFFFFFF80000000
; Divide by 2 MB
mov rcx, 0x200000
div rcx
; put the size into r8
; Yay for the new registers so i dont have to work with the other
mov r8, rax


    ; 512 entries per a big table
mov rax, 512
mul r8
mov r8, rax

; Start counter
mov rcx, 0
.l1_loop:

    ; See .l2_loop for the logic
    mov rdx, _higher_l1_page
    mov rax, 4096
    mul rcx
    add rax, rdx
    or rax, 3
    mov [_higher_l2_page + rcx * 8], rax
    
    inc rcx
    cmp rcx, r8
    jne .l1_loop
mov rcx, 0
.l1_fill_loop:

    mov rax, 4096
    mul rcx
    or rax, 3
    mov [_higher_l1_page + rcx * 8], rax

    inc rcx
    cmp rcx, r8
    jne .l1_fill_loop
; Set the higher half l3 page table
mov rax, _l3_page
; We want the second one
add rax, 4096
or rax, 3
mov [_l4_page + 511 * 8], rax

mov rax, _l4_page
mov cr3, rax

call kernel_early_start

stop:
    cli
    hlt
    jmp stop

section .startup
kernel_early_start:

    mov word [0xb8000], 0x0f31

    jmp stop
    mov rsp, stack_top
    mov rbp, rsp

    mov rdi, qword [multiboot_ptr]

    call kernel_start

.stop:
    cli
    hlt
    jmp .stop

section .bss
align 4096
stack_bottom:
resb 16384
stack_top: