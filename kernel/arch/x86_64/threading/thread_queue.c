#include <asm.h>
#include <libk/queue.h>
#include <libk/rbtree.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

queue_t idle_queue;
queue_t normal_queue;
rbtree_t high_queue;
rbtree_t io_queue;

void init_threading(void) {
  queue_create(&idle_queue);
  queue_create(&normal_queue);
  rb_create(&high_queue);
  rb_create(&io_queue);
}

void queue_thread(TCB_t *tcb, thread_priority_t priority) {
  CLI;
  MFENCE;
  switch (priority) {
  case TP_IDLE:
    queue_enqueue(&idle_queue, &tcb->queue_node);
    break;
  case TP_NORMAL:
    queue_enqueue(&normal_queue, &tcb->queue_node);
    break;
  case TP_HIGH:
    rb_insert(&high_queue, &tcb->rb_node);
    break;
  case TP_IO:
    rb_insert(&io_queue, &tcb->rb_node);
    break;
  }
  STI;
}

TCB_t *pop_thread(void) {
  MFENCE;
  rbnode_t *rb_node = rb_delete_min(&io_queue, io_queue.root);
  if (rb_node != NULL) {
    return (TCB_t *)((size_t)rb_node - offsetof(TCB_t, rb_node));
  }

  rb_node = rb_delete_min(&high_queue, high_queue.root);
  if (rb_node != NULL) {
    return (TCB_t *)((size_t)rb_node - offsetof(TCB_t, rb_node));
  }

  queue_node_t *queue_node = queue_dequeue(&normal_queue);
  if (queue_node != NULL) {
    return (TCB_t *)((size_t)queue_node - offsetof(TCB_t, queue_node));
  }

  queue_node = queue_dequeue(&idle_queue);
  if (queue_node != NULL) {
    return (TCB_t *)((size_t)queue_node - offsetof(TCB_t, queue_node));
  }

  return NULL;
}
