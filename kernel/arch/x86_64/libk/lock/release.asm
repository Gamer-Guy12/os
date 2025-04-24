BITS 64

global lock_release:function

section .text
lock_release:
    mov word [rdi], 0
    ret
