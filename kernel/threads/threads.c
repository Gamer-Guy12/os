#include "kernel/threads.h"
#include "interrupts.h"
#include "kernel/cores.h"
#include "kernel/gheap.h"
#include "kernel/mem.h"
#include "lib/atomic.h"
#include <stdbool.h>
#include <stddef.h>

static struct gheap_cache thread_cache;
static atomic_t cur_tid;
CLS(struct thread *, cur_thread);

#define GET_TID (atomic_add(&cur_tid, 1) - 1)

void init_general_threading(void) {
  gheap_cache_create(&thread_cache, sizeof(struct thread), ZONE_ANY);
  init_global_thread_queue();
}

void init_threading(void *stack) {
  init_local_thread_queue();
  struct thread *thread = gheap_cache_alloc(&thread_cache);

  thread->entry = NULL;
  thread->state = THREAD_RUNNING;
  thread->page_tables = __null_pages();
  thread->tid = GET_TID;
  thread->stack = stack;

  struct thread **cpu_thread = GET_CLS(cur_thread);
  *cpu_thread = thread;
}

// Threads must be different
void switch_threads(struct thread *old_thread, struct thread *new_thread) {
  disable_interrupts();
  if (new_thread->page_tables != __cur_pages() &&
      !__pages_null(new_thread->page_tables))
    __switch_pages(new_thread->page_tables);
  __switch_context(&old_thread->context, &new_thread->context);
  switch_tail(old_thread, new_thread);
}

void switch_tail(struct thread *old_thread, struct thread *new_thread) {
  if (old_thread->state == THREAD_TERMINATED)
    destroy_thread(old_thread);
  else if (old_thread->state == THREAD_RUNNING) {
    old_thread->state = THREAD_READY;
    // Requeue thread
    requeue_thread(old_thread);
  } else if (old_thread->state == THREAD_WAITING) {
    // Do nothing
  }

  struct thread **cpu_thread = GET_CLS(cur_thread);
  *cpu_thread = new_thread;
  enable_interrupts();
}

void thread_trampoline(struct thread *old_thread, struct thread *new_thread) {
  switch_tail(old_thread, new_thread);
  new_thread->entry();
  terminate();
}

struct thread *get_cur_thread(void) {
  struct thread **cpu_thread = GET_CLS(cur_thread);

  return *cpu_thread;
}

uint64_t create_thread(void (*entry)(void)) {
  struct thread *thread = gheap_cache_alloc(&thread_cache);

  thread->entry = entry;
  thread->stack = alloc_pages(STACK_ORDER, ZONE_ANY);
  thread->tid = GET_TID;
  thread->state = THREAD_READY;
  thread->page_tables = __null_pages();
  __create_context(thread);

  waitqueue_create(&thread->thread_dependencies);

  schedule_thread(thread);
  return thread->tid;
}

void destroy_thread(struct thread *thread) {
  // Awaken all child threads
  while (true) {
    struct list_node *node = thread->thread_dependencies.list.next;
    list_remove(thread->thread_dependencies.list.next);

    if (node == NULL) break;

    struct wait_queue_node *wnode = WQ_NODE(node);
    waitqueue_awaken(&thread->thread_dependencies, wnode);
  }

  free_pages(thread->stack, STACK_ORDER);
  gheap_cache_free(&thread_cache, thread);
}
