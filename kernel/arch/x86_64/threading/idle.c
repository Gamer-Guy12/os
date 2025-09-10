#include <cls.h>
#include <stddef.h>
#include <threading.h>
#include <threading/tcb.h>
#include <threading/threading.h>

static void steal_io_threads(void) {
  thread_queue_t *cur_queue = &get_cls()->thread_queue;
  thread_queue_t *best_queue = NULL;

  for (size_t i = 0; i < get_core_count(); i++) {
    thread_queue_t *current = &get_cls_at(i)->thread_queue;

    if (best_queue == NULL) {
      best_queue = current;
    } else {
      if (best_queue->io_queue.count < current->io_queue.count) {
        best_queue = current;
      }
    }
  }

  while (best_queue->io_queue.count > cur_queue->io_queue.count) {
    TCB_t *thread = pop_thread(best_queue);
    queue_thread(thread, TP_IO, cur_queue);
  }
}

static void steal_priority_threads(void) {
  thread_queue_t *cur_queue = &get_cls()->thread_queue;
  thread_queue_t *best_queue = NULL;

  for (size_t i = 0; i < get_core_count(); i++) {
    thread_queue_t *current = &get_cls_at(i)->thread_queue;

    if (best_queue == NULL) {
      best_queue = current;
    } else {
      if (best_queue->priority_queue.count < current->priority_queue.count) {
        best_queue = current;
      }
    }
  }

  while (best_queue->priority_queue.count > cur_queue->priority_queue.count) {
    TCB_t *thread = pop_thread(best_queue);
    queue_thread(thread, TP_HIGH, cur_queue);
  }
}

static void steal_normal_threads(void) {
  thread_queue_t *cur_queue = &get_cls()->thread_queue;
  thread_queue_t *best_queue = NULL;

  for (size_t i = 0; i < get_core_count(); i++) {
    thread_queue_t *current = &get_cls_at(i)->thread_queue;

    if (best_queue == NULL) {
      best_queue = current;
    } else {
      if (best_queue->normal_queue.count < current->normal_queue.count) {
        best_queue = current;
      }
    }
  }

  while (best_queue->normal_queue.count > cur_queue->normal_queue.count) {
    TCB_t *thread = pop_thread(best_queue);
    queue_thread(thread, TP_NORMAL, cur_queue);
  }
}

static void steal(void) {
  steal_io_threads();
  steal_priority_threads();
  steal_normal_threads();
}

void idle(void) {
  while (1) {
    steal();

    run_next_thread();
  }
}
