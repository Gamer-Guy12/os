global trampoline
global page_table_ptr

BITS 16

section .smp_code

trampoline:
  cli
  cld

  ; How to do it
  ; First disable interrupts (done)
  ; Load the gdt
  ; Jump to a 32 bit code segment
  ; Turn on protected mode enable

  ; Clear ds
  xor ax, ax
  mov ds, ax

  lgdt [gdt_ptr]

  mov eax, cr0
  or eax, 1
  mov cr0, eax

  jmp 8:bit32

section .smp_data
align 16
gdt:
; Null segment
dq 0
; 32-bit code segment
dq 0x00CF9A000000FFFF
; 32 bit data segment
dq 0x00CF92000000FFFF
gdt_end:

gdt_ptr:
dw gdt_end - gdt - 1
dq gdt

BITS 32

section .smp_code

bit32:
  ; Enable PAE
  mov eax, cr4
  or eax, (1 << 5)
  mov cr4, eax

  ; Set up page tables
  mov eax, [page_table_ptr] 
  mov cr3, eax

  ; Enable Long Mode
  mov ecx, 0xC0000080
  rdmsr
  or eax, (1 << 8)
  wrmsr

  hlt

section .smp_data
page_table_ptr:
dd 0

