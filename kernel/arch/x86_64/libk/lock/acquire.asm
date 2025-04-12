BITS 64

global lock_acquire

section .text
lock_acquire:
    lock bts word [rdi], 0
    jc .spin
    ret

.spin:
    pause
    test word [rdi], 1
    jnz .spin
    jmp lock_acquire
