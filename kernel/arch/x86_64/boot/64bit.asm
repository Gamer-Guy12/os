BITS 64

global start64
extern kernel_main

section .text

start64:

mov ax, 0
mov ss, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

mov rdi, rbx

call kernel_main

stop:
    cli
    hlt
    jmp stop