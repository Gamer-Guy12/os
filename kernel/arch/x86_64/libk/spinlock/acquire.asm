BITS 64

global spinlock_acquire:function

section .text
spinlock_acquire:
    lock bts word [rdi], 0
    jc .spin
    ret

.spin:
    pause
    test word [rdi], 1
    jnz .spin
    jmp spinlock_acquire
