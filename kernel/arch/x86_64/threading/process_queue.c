#include <libk/spinlock.h>
#include <stdatomic.h>
#include <threading/pcb.h>
#include <threading/threading.h>

PCB_t *process_queue = NULL;
PCB_t *process_queue_end = NULL;
spinlock_t queue_lock = ATOMIC_FLAG_INIT;

PCB_t *get_first_process(bool pop) {
  spinlock_acquire(&queue_lock);

  if (process_queue == NULL)
    return NULL;

  PCB_t *ret = process_queue;

  if (pop) {
    process_queue = ret->next;
    process_queue->prev = NULL;
    ret->next = NULL;
    ret->prev = NULL;
  }

  spinlock_release(&queue_lock);

  return ret;
}

void append_process(PCB_t *process) {
  spinlock_acquire(&queue_lock);

  if (process_queue_end == NULL) {
    process_queue = process;
    process_queue_end = process;
  } else {
    process_queue_end->next = process;
    process->prev = process_queue_end;
    process_queue_end = process;
  }

  spinlock_release(&queue_lock);
}

