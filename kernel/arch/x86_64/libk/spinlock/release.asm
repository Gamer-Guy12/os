BITS 64

global spinlock_release:function

section .text
spinlock_release:
    mfence
    mov word [rdi], 0
    ret
