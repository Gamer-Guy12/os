; This file is where the jump to long mode is performed

BITS 32

global _start
extern enable_paging
global after_paging
extern start64
extern l4_page_table
global multiboot_ptr

section .loading
_start:

    mov [multiboot_ptr], ebx

    ; This checks if long mode is supported
    mov eax, 0x80000001
    cpuid
    test edx, (1 << 29)

    jz no_long

    ; Disable Paging
    ; First, I am making sure it is enabled, then I'm flipping it so that it is disabled
    mov edx, cr0
    or edx, (1 << 31)
    xor edx, (1 << 31)
    mov cr0, edx

    mov esp, stack_top
    mov ebp, esp

    ; Enables PAE (Physical Adress Extension)
    mov edx, cr4
    or edx, (1 << 5)
    mov cr4, edx

    ; Initial page tables that will be setup
    ; Higher half starts at 0x800000000000
    ; Page tables will be mapped to 0x8FFFFFFFFFFF
    ; Kernel will be mapped to 0x800000200000
    ; By default the kernel starts at address 0x200000
    ; This means that 0x200000 will be mapped to 0x800000200000
    ; And it will go to 0x80003FFFFFFF
    ; The kernel will be in the first 4 gb
    ; Map range 0x200000 - 0x3FFFFFFF to 0x800000200000
    ; First 4gb will be identity mapped (0x0 - 0x3FFFFFFF)
    ; Once in longmode, map multiboot structure to 0x800000000000
    jmp enable_paging
after_paging:

    mov ecx, l4_page_table
    mov cr3, ecx

    ; Enable long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, (1 << 8)
    wrmsr

    ; Enable paging again
    mov edx, cr0
    or edx, (1 << 31)
    mov cr0, edx

    lgdt [gdt64.pointer]
    jmp gdt64.segmen:start64

BITS 32
no_long:
    ; This says NL (no longmode)
    mov dword [0xb8000], 0x0f4c0f4e

; This code stops interupts, stops the cpu, and then if the cpu restarts it jumps to stop to redo the process
stop:
    cli
    hlt
    jmp stop

section .bss

align 16
stack_bottom:
    resb 16384
stack_top:

section .rodata
gdt64:
    dq 0
.segmen: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.pointer:
    dw $ - gdt64 -1
    dq gdt64

section .data
multiboot_ptr:
    dq 0