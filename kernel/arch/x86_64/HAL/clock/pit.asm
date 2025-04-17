BITS 64

global init_pit:function

section .text
init_pit:
    mov al, 0x36
    out 0x43, al

    mov ax, 1193182 / 1000
    out 0x40, al
    mov al, ah
    out 0x40, al
