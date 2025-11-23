#include "kernel/threads.h"
#include "kernel/cores.h"
#include "kernel/gheap.h"
#include "util.h"
#include <stdint.h>

static struct gheap_cache thread_cache;
CLS(struct thread, cur_thread);
static uint64_t cur_tid;

#define GET_TID __atomic_fetch_add(&cur_tid, 1, __ATOMIC_RELEASE)

void init_threading(void) {
  gheap_cache_create(&thread_cache, sizeof(struct thread), ZONE_HIGH);
}

void switch_threads(struct thread *old_thread, struct thread *new_thread) {
  if (new_thread->page_tables != __cur_pages() &&
      !__pages_null(new_thread->page_tables))
    __switch_pages(new_thread->page_tables);
  __switch_context(&old_thread->context, &new_thread->context);
}

struct thread *create_thread(NORETURN void (*entry)(void)) {
  struct thread *thread = gheap_cache_alloc(&thread_cache);

  thread->tid = GET_TID;
  thread->entry = entry;
  thread->state = THREAD_READY;

  thread->stack = alloc_pages(STACK_ORDER, ZONE_HIGH);

  __create_context(thread);
  return thread;
}

NORETURN void thread_trampoline(struct thread *old_thread,
                                struct thread *new_thread) {
  new_thread->entry();
}

struct thread *get_cur_thread(void) { return GET_CLS(cur_thread); }
