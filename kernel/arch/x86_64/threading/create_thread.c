#include <gdt.h>
#include <libk/spinlock.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stdatomic.h>
#include <stddef.h>
#include <threading/tcb.h>
#include <threading/threading.h>

bool check_in(void *addr, void *pml4) {
  size_t addr_bits = (size_t)addr & 0x0000fffffffff000;
  PML4_entry_t *entries4 =
      (PML4_entry_t *)((size_t)pml4 + IDENTITY_MAPPED_ADDR);

  size_t index_pt = (addr_bits >> 12) & 0x1ff;
  size_t index_pdt = (addr_bits >> 21) & 0x1ff;
  size_t index_pdpt = (addr_bits >> 30) & 0x1ff;
  size_t index_pml4 = (addr_bits >> 39) & 0x1ff;

  if (!(entries4[index_pml4].full_entry & PML4_PRESENT)) {
    return false;
  }

  PDPT_entry_t *pdpt =
      (PDPT_entry_t *)(IDENTITY_MAPPED_ADDR +
                       (entries4[index_pml4].full_entry & 0x00007ffffffff000));

  if (!(pdpt[index_pdpt].full_entry & PDPT_PRESENT)) {
    return false;
  }

  PDT_entry_t *pdt =
      (PDT_entry_t *)(IDENTITY_MAPPED_ADDR +
                      (pdpt[index_pdpt].full_entry & 0x00007ffffffff000));

  if (!(pdt[index_pdt].full_entry & PDT_PRESENT)) {
    return false;
  }

  PT_entry_t *pt =
      (PT_entry_t *)(IDENTITY_MAPPED_ADDR +
                     (pdt[index_pdt].full_entry & 0x00007ffffffff000));

  if (!(pt[index_pt].full_entry & PT_PRESENT)) {
    return false;
  }

  return true;
}

static size_t tid_cur = 0;
static spinlock_t tid_lock = ATOMIC_FLAG_INIT;

TCB_t *create_thread(PCB_t *process, void (*entry_point)(void), bool queue) {
  TCB_t *tcb = gmalloc(sizeof(TCB_t));

  spinlock_acquire(&tid_lock);
  tcb->tid = tid_cur;
  tid_cur++;
  spinlock_release(&tid_lock);

  tcb->pcb = process;

  tcb->rsp0 = (size_t)create_new_kernel_stack(process->kernel_region, false,
                                              &tcb->stack_num);

  size_t addr1 = tcb->rsp0 + 8 - PAGE_SIZE;
  size_t addr2 = addr1 - PAGE_SIZE;

  map_page_in((void *)addr1, PT_PRESENT | PT_READ_WRITE, 1,
              (PML4_entry_t *)((size_t)tcb->pcb->cr3 + IDENTITY_MAPPED_ADDR));
  map_page_in((void *)addr2, PT_PRESENT | PT_READ_WRITE, 1,
              (PML4_entry_t *)((size_t)tcb->pcb->cr3 + IDENTITY_MAPPED_ADDR));

  tcb->registers = gmalloc(sizeof(registers_t));
  tcb->registers->cs = KERNEL_CODE_SELECTOR;
  tcb->registers->ds = KERNEL_DATA_SELECTOR;
  tcb->registers->es = KERNEL_DATA_SELECTOR;
  tcb->registers->ss = KERNEL_DATA_SELECTOR;

  tcb->state = THREAD_STARTING;
  tcb->rip0 = (size_t)entry_point;

  spinlock_acquire(&process->pcb_lock);
  tcb->next = process->tcbs;
  tcb->prev = NULL;
  if (process->tcbs)
    process->tcbs->prev = tcb;
  process->tcbs = tcb;
  spinlock_release(&process->pcb_lock);

  if (queue)
    queue_thread(tcb);

  return tcb;
}
