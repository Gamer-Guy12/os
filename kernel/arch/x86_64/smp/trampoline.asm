global trampoline
global started
global page_table_ptr
extern smp_start
global bspdone
extern ap_running
extern stack_ptrs

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

  ; Load gdt
  lgdt [gdt_64_ptr]

  ; Enable paging again
  mov edx, cr0
  or edx, (1 << 31)
  mov cr0, edx

  jmp 0x8:long_start

  ;; 8 is the code segment
  ;jmp 8:smp_start

.stop:
  cli
  hlt
  jmp .stop

section .smp_data
page_table_ptr:
dd 0

gdt_64:
dq 0
; 64 bit code segment
dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) | (1 << 55)
; 64 bit data segment
dq (1 << 44) | (1 << 47) | (1 << 53) | (1 << 41) | (1 << 55)
gdt_64_end:

gdt_64_ptr:
dw gdt_64_end - gdt_64 - 1
dq gdt_64

BITS 64
section .smp_code
long_start:
  xor rax, rax
  mov ax, 0x10
  mov ds, ax
  mov ss, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  jmp long_land

BITS 64
section .text

long_land:
  ; Enable mmx
  ; Clear EM in cr0
  mov rax, cr0
  mov rbx, (1 << 2)
  not rbx
  and rax, rbx

  ; Set MP bit in cr0
  or rax, (1 << 1)

  ; Clear TS bit in cr0
  mov rbx, (1 << 3)
  not rbx
  and rax, rbx
  
  mov cr0, rax

  ; Enable sse
  mov rax, cr4

  or rax, (1 << 9)
or rax, (1 << 10)

  mov cr4, rax

  ; Enable NX Bit
  mov rcx, 0xc0000080
  rdmsr
  or rax, (1 << 11)
  wrmsr

  mov rbx, 0
  mov eax, 1
  cpuid
  shr ebx, 24
  mov rdi, rbx

  lock inc byte [ap_running]

  mov rax, 8
  mul rbx
  add rax, stack_ptrs

  mov rsp, [rax]
  mov rbp, rsp

  call smp_start

  .stop:
  cli
  hlt
  jmp .stop

section .data
bspdone:
dd 0

