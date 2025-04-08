BITS 64

global start64

section .text

start64:

mov dword [0xb8000], 0x0f4b0f4f

stop:
    cli
    hlt
    jmp stop