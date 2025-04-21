BITS 16

global ap_tramp
global ap_len
extern aps_running

section .text
ap_tramp:
mov word [0xb8000], 0x0f48
.stop:
    cli
    hlt
    jmp .stop

ap_len:
    dw $ - $$