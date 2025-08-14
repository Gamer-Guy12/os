#include "libk/kio.h"
#include <asm.h>
#include <cls.h>
#include <libk/queue.h>
#include <libk/rbtree.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

void init_threading(void) {
  cls_t *cls = get_cls();

  thread_queue_create(&cls->thread_queue);
  
  TCB_t *idle_task = create_thread(TCB->pcb, idle);
  idle_task->priority = TP_IDLE;
  schedule_thread(idle_task, idle_task->priority);
}

void schedule_thread(TCB_t *tcb, thread_priority_t priority) {
  queue_thread(tcb, priority, &get_cls()->thread_queue);
}

void queue_thread(TCB_t *tcb, thread_priority_t priority,
                  thread_queue_t *queue) {
  CLI;
  MFENCE;
  switch (priority) {
  case TP_IDLE:
    queue_enqueue(&queue->idle_queue, &tcb->queue_node);
    break;
  case TP_NORMAL:
    queue_enqueue(&queue->normal_queue, &tcb->queue_node);
    break;
  case TP_HIGH:
    rb_insert(&queue->priority_queue, &tcb->rb_node);
    break;
  case TP_IO:
    rb_insert(&queue->io_queue, &tcb->rb_node);
    break;
  }
  STI;
}

TCB_t *pop_thread(thread_queue_t *queue) {
  CLI;
  thread_priority_t cur_priority = TCB->priority;
  size_t cur_state = TCB->state;

  MFENCE;
  rbnode_t *rb_node = rb_delete_min(&queue->io_queue, queue->io_queue.root);
  if (rb_node != NULL) {
    return (TCB_t *)((size_t)rb_node - offsetof(TCB_t, rb_node));
  }

  if (cur_priority == TP_IO && cur_state != THREAD_TERMINATED) {
    return NULL;
  }

  rb_node = rb_delete_min(&queue->priority_queue, queue->priority_queue.root);
  if (rb_node != NULL) {
    return (TCB_t *)((size_t)rb_node - offsetof(TCB_t, rb_node));
  }

  if (cur_priority == TP_HIGH && cur_state != THREAD_TERMINATED) {
    return NULL;
  }

  queue_node_t *queue_node = queue_dequeue(&queue->normal_queue);
  if (queue_node != NULL) {
    return (TCB_t *)((size_t)queue_node - offsetof(TCB_t, queue_node));
  }

  if (cur_priority == TP_NORMAL && cur_state != THREAD_TERMINATED) {
    return NULL;
  }

  queue_node = queue_dequeue(&queue->idle_queue);
  if (queue_node != NULL) {
    return (TCB_t *)((size_t)queue_node - offsetof(TCB_t, queue_node));
  }

  return NULL;
}
