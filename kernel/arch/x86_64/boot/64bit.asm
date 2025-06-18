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

mov rcx, 510
.l2_loop:
    
    mov rax, 4096
    mov rdx, rcx
    sub rdx, 510
    mul rdx
    add rax, _higher_l2_page
    or rax, 3
    mov [_l3_page + 4096 + rcx * 8], rax

    inc rcx
    cmp rcx, 512
    jne .l2_loop

mov rax, kernel_gp_end
mov rbx, 0xffffffff80000000
sub rax, rbx
mov rcx, 0x200000
div rcx

mov r8, rax

mov rcx, 0
.l1_loop:

    mov rax, 4096
    mul rcx
    add rax, _higher_l1_page
    or rax, 3
    mov [_higher_l2_page + rcx * 8], rax

    inc rcx
    mov rax, r8
    cmp rcx, rax
    jne .l1_loop

mov rax, r8
mov rdx, 512
mul rdx
mov r8, rax

mov rcx, 0
.l1_fill:

    mov rax, 4096
    mul rcx
    or rax, 3
    mov [_higher_l1_page + rcx * 8], rax

    inc rcx
    mov rax, r8
    cmp rcx, rax
    jne .l1_fill

; Set the higher half l3 page table
mov rax, _l3_page
; We want the second one
add rax, 4096
or rax, 3
mov [_l4_page + 511 * 8], rax

mov rax, _l4_page
mov cr3, rax

sti

sop:
  cli
  hlt
  jmp sop

call kernel_early_start

stop:
    cli
    hlt
    jmp stop

section .startup
kernel_early_start:

    mov rsp, stack_top
    mov rbp, rsp

    mov rdi, qword [multiboot_ptr]
    call kernel_start

.stop:
    cli
    hlt
    jmp .stop

section .data
page_count_startup:
dq 0

section .bss
align 4096
stack_bottom:
resb 16384
stack_top:
