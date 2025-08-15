#include <libk/list.h>
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

  if (!tcb->prev)
    tcb->pcb->tcbs = tcb->next;

  delete_kernel_stack(tcb->stack_num);

  void *xsave_addr = (void *)((size_t)tcb->xsave_page - IDENTITY_MAPPED_ADDR);
  phys_free(xsave_addr);
  gfree(tcb);
}
