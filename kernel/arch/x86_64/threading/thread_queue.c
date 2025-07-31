#include <cls.h>
#include <libk/list.h>
#include <libk/queue.h>
#include <libk/rbtree.h>
#include <libk/spinlock.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

list_t *cls_list = NULL;
size_t core_count = 0;

size_t cur_core = 0;

void queue_thread_any(TCB_t *tcb, thread_priority_t priority) {
  core_count = get_core_count();
  size_t index =
      (__atomic_add_fetch(&cur_core, 1, __ATOMIC_ACQUIRE) - 1) % core_count;

  cls_t *cls = get_cls_at(index);

  switch (priority) {
  case TP_IDLE:
    queue_enqueue(&cls->idle_queue, &tcb->queue_node);
    break;
  case TP_NORMAL:
    rb_insert(&cls->normal_queue, &tcb->rb_node);
    break;
  case TP_HIGH:
    rb_insert(&cls->priority_queue, &tcb->rb_node);
    break;
  case TP_IO:
    queue_enqueue(&cls->io_queue, &tcb->queue_node);
    break;
  default:
    rb_insert(&cls->normal_queue, &tcb->rb_node);
    break;
  }
}

void queue_thread(TCB_t *tcb, thread_priority_t priority) {
  tcb->priority = priority;

  cls_t *cls = get_cls();

  switch (priority) {
  case TP_IDLE:
    queue_enqueue(&cls->idle_queue, &tcb->queue_node);
    break;
  case TP_NORMAL:
    rb_insert(&cls->normal_queue, &tcb->rb_node);
    break;
  case TP_HIGH:
    rb_insert(&cls->priority_queue, &tcb->rb_node);
    break;
  case TP_IO:
    queue_enqueue(&cls->io_queue, &tcb->queue_node);
    break;
  default:
    rb_insert(&cls->normal_queue, &tcb->rb_node);
    break;
  }
}

TCB_t *pop_thread(void) {
  cls_t *cls = get_cls();

  TCB_t *ret = NULL;

  queue_node_t *io_node = queue_dequeue(&cls->io_queue);
  if (io_node) {
    ret = (TCB_t *)((size_t)io_node - offsetof(TCB_t, queue_node));
    return ret;
  }

  rbnode_t *high_node =
      rb_find_min(&cls->priority_queue, cls->priority_queue.root);
  if (high_node) {
    rb_delete(&cls->priority_queue, high_node);
    ret = (TCB_t *)((size_t)high_node - offsetof(TCB_t, rb_node));
    return ret;
  }

  rbnode_t *normal_node =
      rb_find_min(&cls->normal_queue, cls->normal_queue.root);
  if (normal_node) {
    rb_delete(&cls->normal_queue, normal_node);
    ret = (TCB_t *)((size_t)normal_node - offsetof(TCB_t, rb_node));
    return ret;
  }

  queue_node_t *idle_node = queue_dequeue(&cls->idle_queue);
  if (idle_node) {
    ret = (TCB_t *)((size_t)idle_node - offsetof(TCB_t, queue_node));
    return ret;
  }

  return NULL;
}
