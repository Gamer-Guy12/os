#include "kernel/threads.h"
#include "kernel/gheap.h"
#include "kernel/mem.h"
#include "kernel/cores.h"
#include "lib/atomic.h"
#include <stddef.h>

static struct gheap_cache thread_cache;
static atomic_t cur_tid;
CLS(struct thread *, cur_thread);

#define GET_TID (atomic_add(&cur_tid, 1) - 1)

void init_general_threading(void) {
  gheap_cache_create(&thread_cache, sizeof(struct thread), ZONE_ANY);
}

void init_threading(void *stack) {
  struct thread *thread = gheap_cache_alloc(&thread_cache);

  thread->entry = NULL;
  thread->state = THREAD_RUNNING;
  thread->page_tables = __null_pages();
  thread->tid = GET_TID;
  thread->stack = stack;

  struct thread **cpu_thread = GET_CLS(cur_thread);
  *cpu_thread = thread;
}

void switch_threads(struct thread *old_thread, struct thread *new_thread) {
  if (new_thread->page_tables != __cur_pages() &&
      !__pages_null(new_thread->page_tables))
    __switch_pages(new_thread->page_tables);
  __switch_context(&old_thread->context, &new_thread->context);
  switch_tail(old_thread, new_thread);
}

void switch_tail(struct thread *old_thread, struct thread *new_thread) {
  if (old_thread->state == THREAD_TERMINATED)
    destroy_thread(old_thread);
  else if (old_thread->state == THREAD_RUNNING)
    old_thread->state = THREAD_READY;

  struct thread **cpu_thread = GET_CLS(cur_thread);
  *cpu_thread = new_thread;
}

void thread_trampoline(struct thread *old_thread, struct thread *new_thread) {
  switch_tail(old_thread, new_thread);
  new_thread->entry();
}

struct thread *get_cur_thread(void) {
  struct thread **cpu_thread = GET_CLS(cur_thread);

  return *cpu_thread;
}

struct thread *create_thread(NORETURN void (*entry)(void)) {
  struct thread *thread = gheap_cache_alloc(&thread_cache);

  thread->entry = entry;
  thread->stack = alloc_pages(STACK_ORDER, ZONE_ANY);
  thread->tid = GET_TID;
  thread->state = THREAD_READY;
  thread->page_tables = __null_pages();

  return thread;
}

void destroy_thread(struct thread *thread) {
  free_pages(thread->stack, STACK_ORDER);
  gheap_cache_free(&thread_cache, thread);
}

