#include "kernel/threads.h"
#include "lib/string.h"
#include "kernel/cores.h"
#include "kernel/gheap.h"
#include <stdint.h>

static struct gheap_cache thread_cache;
CLS(struct thread, cur_thread);
static uint64_t cur_tid;

#define GET_TID __atomic_fetch_add(&cur_tid, 1, __ATOMIC_RELEASE)

void init_threading(void) {
  gheap_cache_create(&thread_cache, sizeof(struct thread), ZONE_HIGH);
}

void switch_threads(struct thread *old_thread, struct thread *new_thread) {
  __switch_context(&old_thread->context, &new_thread->context);
}

struct thread *thread_copy(struct thread *thread) {
  struct thread *new_thread = gheap_cache_alloc(&thread_cache);

  new_thread->state = thread->state;
  new_thread->tid = GET_TID;
  new_thread->stack = alloc_pages(STACK_ORDER, ZONE_HIGH);
  memcpy(new_thread->stack, thread->stack, PAGE_SIZE * (1 << STACK_ORDER));
  __clone_context(&thread->context, &new_thread->context);

  return new_thread;
}

struct thread *get_cur_thread(void) { return GET_CLS(cur_thread); }
