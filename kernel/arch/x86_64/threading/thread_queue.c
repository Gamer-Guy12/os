#include <stdatomic.h>
#include <threading/threading.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <libk/spinlock.h>

static TCB_t* thread_queue;
static TCB_t* thread_queue_end;
static spinlock_t lock = ATOMIC_FLAG_INIT;

void queue_thread(TCB_t* tcb) {
  spinlock_acquire(&lock);
  
  if (thread_queue == NULL) {
    thread_queue = tcb;
    thread_queue_end = tcb;
  } else {
    thread_queue_end->queue_next = tcb;
    tcb->queue_next = thread_queue;
    thread_queue_end = tcb;
  }

  spinlock_release(&lock);
}

TCB_t* pop_thread(void) {
  spinlock_acquire(&lock);

  TCB_t* ret = thread_queue;
  if (thread_queue != NULL)
  {
    thread_queue = thread_queue->queue_next;
    thread_queue_end->queue_next = thread_queue;
  }

  spinlock_release(&lock);

  return ret;
}

