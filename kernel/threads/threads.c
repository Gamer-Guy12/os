#include "kernel/threads.h"
#include "interrupts.h"
#include "kernel/cores.h"
#include "kernel/gheap.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "lib/atomic.h"
#include "lib/list.h"
#include "lib/rbtree.h"
#include "lib/string.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static struct gheap_cache thread_cache;
static atomic_t cur_tid;
static struct rbtree thread_ids;
static struct event destroy_event;
CLS(struct thread *, cur_thread);

// Skips over 0
#define GET_TID (atomic_add(&cur_tid, 1))

static int compare_threads(struct rbnode *n1, struct rbnode *n2) {
  struct thread *t1 =
      (struct thread *)((uintptr_t)n1 - offsetof(struct thread, id_node));
  struct thread *t2 =
      (struct thread *)((uintptr_t)n2 - offsetof(struct thread, id_node));
  return t1->tid - t2->tid;
}

void init_general_threading(void) {
  gheap_cache_create(&thread_cache, sizeof(struct thread), ZONE_ANY);
  kprintf("\t[THREADS] Initialized Thread Allocation Cache\n");
  init_global_thread_queue();
  kprintf("\t[THREADS] Initialized Global Thread Queue\n");
  rb_create(&thread_ids, compare_threads);
  event_create(&destroy_event, true);
  kprintf("\t[THREADS] Created destruction event\n");
  // init_sleep();
  kprintf("\t[THREADS] Initialized Sleeping Infrastructure\n");
}

void init_threading(void *stack) {
  struct thread *thread = gheap_cache_alloc(&thread_cache);

  thread->entry = NULL;
  thread->state = THREAD_RUNNING;
  thread->page_tables = __null_pages();
  thread->tid = GET_TID;
  thread->stack = stack;
  thread->event_filter = 0;
  thread->wait_queue = NULL;
  rb_insert(&thread_ids, &thread->id_node);

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
    list_insert(&old_thread->wait_queue->list, &old_thread->wait_queue_node);
  }

  struct thread **cpu_thread = GET_CLS(cur_thread);
  *cpu_thread = new_thread;
  enable_interrupts();
}

void thread_trampoline(struct thread *old_thread, struct thread *new_thread) {
  switch_tail(old_thread, new_thread);
  new_thread->entry();
  terminate(0);
}

struct thread *get_cur_thread(void) {
  struct thread **cpu_thread = GET_CLS(cur_thread);

  return *cpu_thread;
}

uint64_t create_thread(void (*entry)(void)) {
  disable_interrupts();
  struct thread *thread = gheap_cache_alloc(&thread_cache);

  thread->entry = entry;
  thread->stack = alloc_pages(STACK_ORDER, ZONE_ANY);
  thread->tid = GET_TID;
  thread->state = THREAD_READY;
  thread->page_tables = __null_pages();
  thread->wait_queue = NULL;
  thread->event_filter = 0;
  __create_context(thread);
  rb_insert(&thread_ids, &thread->id_node);
  kprintf("%x Created thread\n", thread->tid);

  schedule_thread(thread);
  enable_interrupts();
  return thread->tid;
}

void destroy_thread(struct thread *thread) {
  // Awaken all child threads
  event_trigger(&destroy_event, thread->tid, thread->exit_code);

  free_pages(thread->stack, STACK_ORDER);
  gheap_cache_free(&thread_cache, thread);
}

struct thread *thread_id(uint64_t id) {
  struct thread dummy = {.tid = id};

  struct rbnode *node = rb_search(&thread_ids, NULL, &dummy.id_node);

  if (node == NULL)
    return NULL;
  struct thread *ret =
      (struct thread *)((uintptr_t)node - offsetof(struct thread, id_node));
  return ret;
}

int wait_thread(uint64_t tid) {
  return (int)event_wait(&destroy_event, tid);
}
