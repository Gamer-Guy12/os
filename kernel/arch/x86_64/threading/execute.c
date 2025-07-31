#include <asm.h>
#include <cls.h>
#include <libk/kio.h>
#include <libk/queue.h>
#include <stdbool.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

#define FS_MSR 0xC0000100

void run_next_thread(void) {
  TCB_t *tcb = (TCB_t *)rdmsr(FS_MSR);
  // Increment the amount of quantums used
  tcb->rb_node.value++;
  // This means that if there are no threads this thread could be queued again
  if (tcb->state != THREAD_TERMINATED) {
    queue_thread(tcb, tcb->priority);
  }

  TCB_t *next = pop_thread();

  while (next == NULL) {
    next = pop_thread();
  }

  if (tcb->state == THREAD_TERMINATED) {
    queue_enqueue(&get_cls()->dead_queue, &tcb->queue_node);
  }

  swap_threads(next);
}
