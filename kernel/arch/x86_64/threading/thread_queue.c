#include "libk/kio.h"
#include <threading/tcb.h>
#include <asm.h>
#include <libk/queue.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/threading.h>

queue_t thread_queue;

void init_threading(void) { queue_create(&thread_queue); }

void queue_thread(TCB_t *tcb, thread_priority_t priority) {
  MFENCE;
  queue_enqueue(&thread_queue, &tcb->queue_node);
}

TCB_t *pop_thread(void) {
  queue_node_t *node = NULL;
  TCB_t *tcb = NULL;
  do {

    node = queue_dequeue(&thread_queue);

    if (node == NULL) {
      kio_printf("Return\n");
      return NULL;
    }

    tcb = (TCB_t *)((size_t)node - offsetof(TCB_t, queue_node));

    if (tcb->flags & TCB_LOADING) {
      kio_printf("Issue\n");
      queue_thread(tcb, tcb->priority);
    } else {
      break;
    }
  } while (true);
  return tcb;
}
