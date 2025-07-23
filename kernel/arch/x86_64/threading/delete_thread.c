#include <asm.h>
#include <libk/err.h>
#include <libk/spinlock.h>
#include <libk/sys.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

#define FS_MSR 0xC0000100

void delete_thread(TCB_t *tcb) {
  // Check for self termination
  TCB_t *cur_tcb = (TCB_t *)rdmsr(FS_MSR);
  if (cur_tcb == tcb) {
    sys_panic(SELF_TERM_ERR | THREAD_SELF_TERM);
  }

  // Remove thread from pcb
  if (tcb->next)
    tcb->next->prev = tcb->prev;
  if (tcb->prev)
    tcb->prev->next = tcb->next;

  if (!tcb->next && !tcb->prev) tcb->pcb->tcbs = NULL;

  void *addr =
      delete_kernel_stack(tcb->stack_num, tcb->pcb->kernel_region, false);

  unmap_page_in(addr, true,
                (PML4_entry_t *)((size_t)tcb->pcb->cr3 + IDENTITY_MAPPED_ADDR));
  unmap_page_in((void *)((size_t)addr + PAGE_SIZE), true,
                (PML4_entry_t *)((size_t)tcb->pcb->cr3 + IDENTITY_MAPPED_ADDR));

  gfree(tcb->registers);
  void* xsave_addr = (void*)((size_t)tcb->xsave_page - IDENTITY_MAPPED_ADDR);
  phys_free(xsave_addr);
  gfree(tcb);
}
