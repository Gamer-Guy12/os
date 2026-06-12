#include "kernel/threads.h"
#include "interrupts.h"
#include "kernel/cores.h"
#include "kernel/gheap.h"
#include "kernel/mem.h"
#include "util.h"

// Holds the current thread for each core
CLS(struct thread *, cpu_thread);

static tid_t cur_tid = 0;
#define GET_TID __atomic_fetch_add(&cur_tid, 1, __ATOMIC_ACQ_REL)

static GHEAP_CACHE(thread_cache);

void init_threading(void) {
  BSP { gheap_cache_create(&thread_cache, sizeof(struct thread), ZONE_ANY); }
}

void switch_threads(struct thread *old, struct thread *new) {
  disable_interrupts();
  RMEMB();
  struct thread **thread = GET_CLS(cpu_thread);
  new->prev = old;
  *thread = new;

  if (new->pages != __pages_null() && new->pages != old->pages)
    __switch_pages(new->pages);
  __switch_context(&old->context, &new->context);
  switch_tail();
}

void switch_tail(void) {
  WMEMB();
  enable_interrupts();
}

struct thread *create_thread(void (*entry)(void *), void *param) {
  struct thread *thread = gheap_cache_alloc(&thread_cache);
  if (!thread)
    return NULL;

  thread->tid = GET_TID;
  thread->pages = __pages_null();
  thread->entry = entry;
  thread->param = param;
  thread->stack = alloc_pages(STACK_ORDER, ZONE_ANY);
  __create_context(thread);

  return thread;
}

void thread_trampoline(void) {
  switch_tail();
  struct thread *thread = get_cur_thread();
  thread->entry(thread->param);
}

struct thread *get_cur_thread(void) {
  struct thread **thread = GET_CLS(cpu_thread);
  return *thread;
}
