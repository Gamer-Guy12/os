#include <asm.h>
#include <libk/err.h>
#include <libk/sys.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/threading.h>

#define FS_MSR 0xC0000100

typedef enum {
  LEVEL_PML4 = 4,
  LEVEL_PDPT = 3,
  LEVEL_PDT = 2,
  LEVEL_PT = 1,
  LEVEL_PAGE = 0
} page_level_t;

void unmap_level(size_t phys_addr, page_level_t level, size_t start, size_t to) {
  if (level == LEVEL_PAGE) {
    phys_free((void *)phys_addr);

    return;
  }

  PT_entry_t *entries = (PT_entry_t *)(phys_addr + IDENTITY_MAPPED_ADDR);

  for (size_t i = start; i < to; i++) {
    size_t addr = entries->full_entry & 0x0000fffffffff000;
    unmap_level(addr, level - 1, 0, 512);
  }

  phys_free((void *)phys_addr);
}

// Can't be deleted by the current process
void delete_process(PCB_t *pcb) {
  PCB_t *cur_pcb = ((TCB_t *)rdmsr(FS_MSR))->pcb;
  if (pcb == cur_pcb) {
    sys_panic(SELF_TERM_ERR | PROCESS_SELF_TERM);
  }

  TCB_t *tcb = pcb->tcbs;

  while (tcb != NULL) {
    TCB_t *next = tcb->next;
    delete_thread(tcb);
    tcb = next;
  }

  unmap_level((size_t)pcb->cr3, LEVEL_PML4, 0, 512);

  remove_process(pcb);

  gfree(pcb);
}
