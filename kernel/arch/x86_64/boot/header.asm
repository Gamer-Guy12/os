BITS 32

section .multiboot
MAGIC equ 0xE85250D6
ARCH equ 0 
LENGTH equ multiboot_end - multiboot_start
align 4 
multiboot_start:
dd MAGIC
dd ARCH
dd LENGTH
dd -(LENGTH + ARCH + MAGIC)

; End tag
dw 0
dw 0
dd 8

multiboot_end: