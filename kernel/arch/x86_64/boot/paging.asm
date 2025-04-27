BITS 32

global enable_paging
extern after_paging
global l4_page_table

section .loading

enable_paging:

    ; The l3 page table used for identity mapping the first 4 gigabytes
    mov eax, l3_page_table
    or eax, 3
    ; This shifts everything into the higher half
    ; Essential evverything is identitiy mapped into the higher half and then more paging form there
    mov [l4_page_table + 2048*8], eax

    ; Loop to put all the l2 page tables in the l3 page table
    mov ecx, 0
.l2_table_setup:

    mov eax, 4096
    mul ecx
    add eax, l2_page_tables
    or eax, 3
    mov [l3_page_table + 8 * ecx], eax

    inc ecx
    cmp ecx, 4
    jne .l2_table_setup

mov ecx, 0
.l1_table_setup:

    mov eax, 4096
    mul ecx
    add eax, l1_page_tables
    or eax, 3
    mov [l2_page_tables + 8 * ecx], eax

    inc ecx
    cmp ecx, 2048
    jne .l1_table_setup

mov ecx, 0
.l1_entry_setup:

    mov eax, 4096
    mul ecx
    or eax, 3
    mov [l1_page_tables + 8 * ecx], eax
    
    inc ecx
    cmp ecx, 2048 * 512
    jne .l1_entry_setup

    jmp after_paging

section .bss
align 4096

; Quick Identity mapping of the first 4gb of memory and then jumping to 64 bit C

; The l4 page table (holds 256 tb)
l4_page_table:
    resb 4096
; 1 l3 page table (holds 512 gb)
l3_page_table:
    resb 4096
; The 4 l2 tables each holding 1 gb
l2_page_tables:
    resb 4096 * 4
; 2048 l1 page tables (512 per l2, 4 l2s)
l1_page_tables:
    resb 4096 * 2048