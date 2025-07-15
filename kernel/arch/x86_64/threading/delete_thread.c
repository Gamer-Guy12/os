#include <mem/memory.h>
#include <threading/threading.h>

void delete_thread(TCB_t *tcb) {
  delete_kernel_stack(tcb->stack_num, tcb->pcb->kernel_region);
  gfree(tcb->registers);

  if (tcb->prev)
    tcb->prev->next = tcb->next;
  if (tcb->next)
    tcb->next->prev = tcb->prev;

  gfree(tcb);
}
