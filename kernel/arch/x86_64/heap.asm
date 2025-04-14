BITS 64

global heap_start
global heap_end

section .bss
; Reserve 16 kilobytes for the (temp) heap
align 16
heap_start:
    resb 1024 * 16
heap_end: