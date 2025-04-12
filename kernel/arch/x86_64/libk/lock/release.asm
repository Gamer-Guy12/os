BITS 64

global lock_release

section .text
lock_release:
    mov word [rdi], 0
    ret
