BITS 64

global inb:function
global outb:function

section .text
inb:
    mov dx, di
    in al, dx

    ret

outb:
    mov dx, di
    mov ax, si
    out dx, al

    ret
