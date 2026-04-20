#include "kernel/threads.h"
#include "interrupts.h"
#include "kernel/cores.h"
#include "kernel/gheap.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "lib/rbtree.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

static struct gheap_cache thread_cache;
static struct rbtree id_tree;
static tid_t cur_tid = 0;
CLS(struct thread *, cpu_thread);

#define GET_TID __atomic_add_fetch(&cur_tid, 1, __ATOMIC_RELEASE)

static int compare_ids(struct rbnode *n1, struct rbnode *n2) {
  struct thread *thread1 =
      (struct thread *)((uintptr_t)n1 - offsetof(struct thread, id_node));
  struct thread *thread2 =
      (struct thread *)((uintptr_t)n2 - offsetof(struct thread, id_node));

  return thread1 - thread2;
}

INIT void init_threading(void *stack) {
  BSP {
    gheap_cache_create(&thread_cache, sizeof(struct thread), ZONE_ANY);
    rb_create(&id_tree, compare_ids);
    init_thread_queues();
  }

  struct thread *thread = gheap_cache_alloc(&thread_cache);

  thread->tid = GET_TID;
  thread->entry = kinit;
  thread->stack = stack;
  thread->state = THREAD_RUNNING;
  thread->page_tables = __null_pages();
  // Finish off everything that is needed in init and then it will be swapped to
  // idle
  thread->priority = TP_HIGH;

  struct thread **core_thread = GET_CLS(cpu_thread);
  *core_thread = thread;
}

struct thread *create_thread(void (*entry)(void),
                             enum thread_priority priority) {
  struct thread *thread = gheap_cache_alloc(&thread_cache);

  thread->tid = GET_TID;
  thread->entry = entry;
  thread->stack = alloc_pages(STACK_ORDER, ZONE_ANY);
  thread->state = THREAD_READY;
  thread->page_tables = __null_pages();
  thread->priority = priority;
  __create_context(thread);
  schedule_thread(thread);

  return thread;
}

void destroy_thread(struct thread *thread) {
  free_pages(thread->stack, STACK_ORDER);
  gheap_cache_free(&thread_cache, thread);
}

void switch_threads(struct thread *old_thread, struct thread *new_thread) {
  struct thread **thread = GET_CLS(cpu_thread);
  *thread = new_thread;
  new_thread->prev = old_thread;
  disable_interrupts();
  if (new_thread->page_tables != __cur_pages() &&
      !__pages_null(new_thread->page_tables))
    __switch_pages(new_thread->page_tables);
  __switch_context(&old_thread->context, &new_thread->context);
  switch_tail();
}

void switch_tail(void) {
  struct thread *new_thread = get_cur_thread();
  struct thread *old_thread = new_thread->prev;
  switch (old_thread->state) {
  case THREAD_RUNNING:
    old_thread->state = THREAD_READY;
    requeue_thread(old_thread);
  default:;
  }

  new_thread->state = THREAD_RUNNING;
}

void thread_trampoline(struct thread *old_thread, struct thread *new_thread) {
  switch_tail();
  new_thread->entry();
}

struct thread *get_cur_thread(void) {
  struct thread **thread = GET_CLS(cpu_thread);
  return *thread;
}

struct thread *thread_id(tid_t tid) {
  struct thread dummy = {.tid = tid};
  struct rbnode *node = rb_search(&id_tree, NULL, &dummy.id_node);

  if (node == NULL)
    return NULL;
  struct thread *thread =
      (struct thread *)((uintptr_t)node - offsetof(struct thread, id_node));
  return thread;
}
