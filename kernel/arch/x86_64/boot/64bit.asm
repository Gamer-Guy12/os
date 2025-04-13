BITS 64

global start64
extern kernel_start
extern multiboot_ptr

section .text

start64:

mov ax, 0
mov ss, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

mov rdi, qword [multiboot_ptr]

call kernel_start

stop:
    cli
    hlt
    jmp stop