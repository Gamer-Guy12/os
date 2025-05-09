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

; Request information
info_request:
dw 1
dw 0
dd info_request_end - info_request
dd 6
dd 15
dd 16
dd 8
info_request_end:

; End tag
dw 0
dw 0
dd 8

multiboot_end: