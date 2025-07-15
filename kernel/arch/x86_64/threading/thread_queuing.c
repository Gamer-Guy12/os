#include <cls.h>
#include <libk/kio.h>
#include <libk/spinlock.h>
#include <stdatomic.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/threading.h>

size_t queue_index = 0;
spinlock_t thread_queue_lock = ATOMIC_FLAG_INIT;

void queue_thread(TCB_t *tcb) {
  cls_t *cls_list = get_cls_list();
  size_t core_count = get_cls_count();

  spinlock_acquire(&thread_queue_lock);

  cls_t *cur_cls = cls_list;

  if (queue_index != 0) {
    for (size_t i = 0; i < queue_index; i++) {
      cur_cls = cur_cls->next;
    }
  }

  queue_index++;

  if (queue_index == core_count)
    queue_index = 0;

  if (cur_cls->tcb_queue == NULL) {
    cur_cls->tcb_queue = tcb;
    cur_cls->tcb_queue_end = tcb;
  } else {
    cur_cls->tcb_queue_end->queue_next = tcb;
    cur_cls->tcb_queue_end = tcb;
  }

  spinlock_release(&thread_queue_lock);
}

TCB_t *pop_thread(void) {
  cls_t *cls = get_cls();

  TCB_t *front = cls->tcb_queue;
  if (front == NULL) {
    while (1) {}
  }
  cls->tcb_queue = front->next->queue_next;

  front->next->queue_next = NULL;

  return front;
}
