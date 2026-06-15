#include "lib/queue.h"
#include "kernel/kprintf.h"
#include "kernel/threads.h"
#include "lib/spinlock.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static SPINLOCK(thread_queue_lock);
static struct thread_queue queue;

void init_queues(void) {
  BSP {
    for (int i = 0; i < PRIORITY_COUNT; i++) {
      QUEUE_INIT(&queue.queues[i]);
    }
  }
}

void schedule_thread(struct thread *thread, int priority) {
  queue_thread(thread, &queue, priority);
}

// check_priority means that if the current priority is higher just return
// nothing
static struct thread *__do_dequeue(struct thread_queue *queue,
                                   bool check_priority) {
  struct thread *cur = get_cur_thread();
  if (cur->state != THREAD_RUNNING)
    check_priority = false;
  const int cur_priority = cur->priority;
  spinlock_acquire(&thread_queue_lock);
  for (int i = PRIORITY_COUNT - 1; i >= 0; i--) {
    if (check_priority && i < cur_priority) {
      spinlock_release(&thread_queue_lock);
      return NULL;
    }

    // Pop
    struct queue_node *node = queue_dequeue(&queue->queues[i]);
    if (!node)
      continue;

    uintptr_t ptr = (uintptr_t)node - offsetof(struct thread, node);
    spinlock_release(&thread_queue_lock);
    return (struct thread *)ptr;
  }
  spinlock_release(&thread_queue_lock);

  return NULL;
}

static struct thread *__dequeue_thread(struct thread_queue *queue,
                                       bool check_priority) {
  while (true) {
    struct thread *thread = __do_dequeue(queue, check_priority);
    if (!thread)
      return NULL;

    switch (thread->state) {
    case THREAD_RUNNING:
      kprintf("Error: Thread in running state while in queue, tid: 0x%x\n",
              thread->tid);
      panic();
    case THREAD_READY:
      return thread;
    case THREAD_TERMINATED:
      destroy_thread(thread);
      break;
    default:
      kprintf("Invalid thread state: 0x%x, tid: 0x%x\n", thread->state,
              thread->tid);
      panic();
    }
  }
}

// Get the next thread
struct thread *pop_thread(void) { return __do_dequeue(&queue, true); }

void queue_thread(struct thread *thread, struct thread_queue *queue,
                  int priority) {
  if (priority >= PRIORITY_COUNT) {
    kprintf("Invalid priority %d for thread 0x%x\n", priority, thread->tid);
    panic();
  }

  spinlock_acquire(&thread_queue_lock);
  queue_enqueue(&queue->queues[priority], &thread->node);
  spinlock_release(&thread_queue_lock);
}

struct thread *dequeue_thread(struct thread_queue *queue) {
  return __dequeue_thread(queue, false);
}
