#include "interrupts.h"
#include "kernel/threads.h"
#include "lib/list.h"
#include "lib/spinlock.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

void waitqueue_create(struct wait_queue *queue,
                      int (*check_thread)(struct thread *, void *)) {
  queue->check_thread = check_thread;
  LIST_INIT(&queue->waiting_threads);
  queue->wait_lock = (spinlock_t)SPINLOCK_ZERO(misc_wait);
}

void waitqueue_wait(struct wait_queue *queue) {
  temp_disable_interrupts();

  struct thread *thread = get_cur_thread();
  thread->wait_queue = queue;
  thread->state = THREAD_WAITING;

  do {
    schedule();
    RMEMB();
  } while (thread->state == THREAD_WAITING);
}

void waitqueue_awaken(struct wait_queue *queue, void *data) {
  struct list_node *pos = NULL;
  spinlock_acquire(&queue->wait_lock);
  size_t count = 0;
  LIST_FOREACH(pos, &queue->waiting_threads) {
    struct thread *thread =
        (struct thread *)((uintptr_t)pos - offsetof(struct thread, wait_node));

    int response = queue->check_thread(thread, data);
    bool accept = response & (1 << 1);
    bool cont = ~(response & 1);

    if (accept) {
      pos->next->prev = pos->prev;
      pos->prev->next = pos->next;
      count++;

      schedule_thread(thread);
    }

    if (!cont)
      break;
  }
  atomic_sub(&queue->wait_count, count);
  spinlock_release(&queue->wait_lock);
}

void waitqueue_awaken_all(struct wait_queue *queue) {
  struct list_node *pos = NULL;
  spinlock_acquire(&queue->wait_lock);
  LIST_FOREACH(pos, &queue->waiting_threads) {
    struct thread *thread =
        (struct thread *)((uintptr_t)pos - offsetof(struct thread, wait_node));

    schedule_thread(thread);
  }

  atomic_store(&queue->wait_count, 0);
  spinlock_release(&queue->wait_lock);
}

bool waitqueue_destroy(struct wait_queue *queue, bool force) {
  spinlock_acquire(&queue->wait_lock);
  bool success = false;

  if (LIST_EMPTY(&queue->waiting_threads)) {
    success = true;
  } else if (force) {
    struct list_node *pos = NULL;
    LIST_FOREACH(pos, &queue->waiting_threads) {
      pos->next->prev = pos->prev;
      pos->prev->next = pos->next;

      struct thread *thread =
          (struct thread *)((uintptr_t)pos -
                            offsetof(struct thread, wait_node));

      schedule_thread(thread);
    }
    success = true;
  }
  spinlock_release(&queue->wait_lock);

  return success;
}

void __insert_wait_thread(struct thread *thread) {
  struct wait_queue *queue = thread->wait_queue;

  spinlock_acquire(&queue->wait_lock);
  list_insert(&queue->waiting_threads, &thread->wait_node);
  atomic_add(&queue->wait_count, 1);
  spinlock_release(&queue->wait_lock);
}

// 2 bits
// bit 0: 0 means continue, 1 means stop
// bit 1: 1 means accept, 0 means don't
static int check_event(struct thread *thread, void *data) {
  size_t filter = (size_t)data;

  if (thread->event_filter == filter) {
    return 2;
  }

  return 0;
}

void event_create(struct event *event, bool uses_filter) {
  event->uses_filter = uses_filter;
  waitqueue_create(&event->queue, check_event);
}

void event_wait(struct event *event, size_t filter) {
  struct thread *thread = get_cur_thread();

  if (event->uses_filter) {
    thread->event_filter = filter;
  } else {
    thread->event_filter = 0;
  }

  waitqueue_wait(&event->queue);
}

void event_trigger(struct event *event, size_t filter) {
  if (!event->uses_filter) {
    filter = 0;
  }

  waitqueue_awaken(&event->queue, (void *)filter);
}

void event_destroy(struct event *event, bool force) {
  waitqueue_destroy(&event->queue, force);
}
