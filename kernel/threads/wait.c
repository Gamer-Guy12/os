#include "kernel/kprintf.h"
#include "kernel/threads.h"
#include "lib/rbtree.h"
#include <stdint.h>

static struct rbtree wait_tree;

static int compare_ids(struct rbnode *n1, struct rbnode *n2) {
  struct thread *thread1 =
      (struct thread *)((uintptr_t)n1 - offsetof(struct thread, wait_node));
  struct thread *thread2 =
      (struct thread *)((uintptr_t)n2 - offsetof(struct thread, wait_node));

  return thread1->tid - thread2->tid;
}

void init_waiting(void) { rb_create(&wait_tree, compare_ids); }

void wait(void) {
  get_cur_thread()->state = THREAD_WAITING;
  schedule();
}

void awaken_thread(tid_t tid) {
  struct thread dummy = {.tid = tid};
  struct rbnode *node = rb_delete_search(&wait_tree, NULL, &dummy.wait_node);
  if (node == NULL) {
    return;
  }

  struct thread *thread =
      (struct thread *)((uintptr_t)node - offsetof(struct thread, wait_node));
  schedule_thread(thread);
}

void __insert_wait_thread(struct thread *thread) {
  rb_insert(&wait_tree, &thread->wait_node);
}
