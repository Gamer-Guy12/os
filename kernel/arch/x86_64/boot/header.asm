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
; Memory map
dd 6
; RSDP
dd 14
; Networking stuff
dd 16
; Framebuffer
dd 8
info_request_end:

; Frame buffer request
framebuffer_request:
dw 5
dw 0
dd framebuffer_request_end - framebuffer_request
dd 1280
dd 720
dd 24
framebuffer_request_end:
; Padding to make this 8 byte aligned
dd 0


; End tag
dw 0
dw 0
dd 8

multiboot_end:
