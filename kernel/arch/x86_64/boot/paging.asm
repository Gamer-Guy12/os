BITS 32

global enable_paging
extern after_paging

extern _l4_page
extern _l3_page
extern _l2_page
extern _l1_page

section .loading

enable_paging:

    ; The l3 page table used for identity mapping the first 4 gigabytes
    mov eax, _l3_page
    or eax, 3
    
    ; Identity map everything and change it later
    mov [_l4_page], eax

    ; Loop to put all the l2 page tables in the l3 page table
    mov ecx, 0
.l2_table_setup:

    mov eax, 4096
    mul ecx
    add eax, _l2_page
    or eax, 3
    mov [_l3_page + 8 * ecx], eax

    inc ecx
    cmp ecx, 4
    jne .l2_table_setup

mov ecx, 0
.l1_table_setup:

    mov eax, 4096
    mul ecx
    add eax, _l1_page
    or eax, 3
    mov [_l2_page + 8 * ecx], eax

    inc ecx
    cmp ecx, 2048
    jne .l1_table_setup

mov ecx, 0
.l1_entry_setup:

    mov eax, 4096
    mul ecx
    or eax, 3
    mov [_l1_page + 8 * ecx], eax
    
    inc ecx
    cmp ecx, 2048 * 512
    jne .l1_entry_setup

    jmp after_paging