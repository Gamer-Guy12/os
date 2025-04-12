BITS 64

global start64
extern kernel_start

section .text

start64:

mov ax, 0
mov ss, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

mov rdi, rbx

call kernel_start

stop:
    cli
    hlt
    jmp stop