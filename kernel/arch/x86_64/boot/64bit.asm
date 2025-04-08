BITS 64

global start64
extern kernel_main

section .text

start64:

call kernel_main

stop:
    cli
    hlt
    jmp stop