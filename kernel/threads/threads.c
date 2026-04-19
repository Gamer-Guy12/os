#include "kernel/threads.h"
#include "interrupts.h"
#include "kernel/cores.h"
#include "kernel/gheap.h"
#include "kernel/mem.h"
#include "util.h"

static struct gheap_cache thread_cache;
CLS(struct thread *, cpu_thread);

void init_threading(void *stack) {
  BSP { gheap_cache_create(&thread_cache, sizeof(struct thread), ZONE_ANY); }

  struct thread *thread = gheap_cache_alloc(&thread_cache);

  thread->entry = kinit;
  thread->stack = stack;
  thread->state = THREAD_RUNNING;
  thread->page_tables = __null_pages();
  
  struct thread **core_thread = GET_CLS(cpu_thread);
  *core_thread = thread;
}

struct thread *create_thread(void (*entry)(void)) {
  struct thread *thread = gheap_cache_alloc(&thread_cache);

  thread->entry = entry;
  thread->stack = alloc_pages(STACK_ORDER, ZONE_ANY);
  thread->state = THREAD_READY;
  thread->page_tables = __null_pages();
  __create_context(thread);

  return thread;
}

void switch_threads(struct thread *old_thread, struct thread *new_thread) {
  disable_interrupts();
  if (new_thread->page_tables != __cur_pages() &&
      !__pages_null(new_thread->page_tables))
    __switch_pages(new_thread->page_tables);
  __switch_context(&old_thread->context, &new_thread->context);
  switch_tail(old_thread, new_thread);
}

void switch_tail(struct thread *old_thread, struct thread *new_thread) {
  switch (old_thread->state) {
  case THREAD_RUNNING:
    old_thread->state = THREAD_READY;
  default:;
  }

  new_thread->state = THREAD_RUNNING;
  struct thread **thread = GET_CLS(cpu_thread);
  *thread = new_thread;
}

void thread_trampoline(struct thread *old_thread, struct thread *new_thread) {
  switch_tail(old_thread, new_thread);
  new_thread->entry();
}

struct thread *get_cur_thread(void) {
  struct thread **thread = GET_CLS(cpu_thread);
  return *thread;
}

