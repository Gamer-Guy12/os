#include <asm.h>
#include <cls.h>
#include <libk/kio.h>
#include <libk/queue.h>
#include <stdbool.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

void run_next_thread(void) {
  TCB_t *tcb = TCB;
  tcb->rb_node.value++;
  queue_thread(tcb, tcb->priority);

  TCB_t *next = NULL;
  do {

  } while (!(next = pop_thread()));

  if (tcb->tid == next->tid) {
    return;
  }

  MFENCE;
  swap_threads(next);
}
