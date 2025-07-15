#include <mem/memory.h>
#include <threading/threading.h>

void delete_thread(TCB_t *tcb) {
  gfree(tcb->registers);

  if (tcb->prev)
    tcb->prev->next = tcb->next;
  if (tcb->next)
    tcb->next->prev = tcb->prev;

  gfree(tcb);
}
