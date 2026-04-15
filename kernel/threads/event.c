#include "kernel/mem.h"
#include "kernel/threads.h"
#include "kernel/gheap.h"
#include "lib/list.h"
#include "lib/spinlock.h"
#include <stdbool.h>
#include <stdint.h>

struct gheap_cache event_node_cache;
static spinlock_t init_spinlock = SPINLOCK_ZERO;
static bool inited_cache = false;

void event_create(struct event *event, bool uses_filter) {
  spinlock_acquire(&init_spinlock);
  if (!inited_cache)
    gheap_cache_create(&event_node_cache, sizeof(struct event_node), ZONE_ANY);
  inited_cache = true;
  spinlock_release(&init_spinlock);

  event->uses_filter = uses_filter;
  event->lock = (spinlock_t)SPINLOCK_ZERO;
  waitqueue_create(&event->queue);
  LIST_INIT(&event->threads);
}

void event_destroy(struct event *event) {
  spinlock_acquire(&event->lock);
  while (!LIST_EMPTY(&event->threads)) {
    struct list_node *node = event->threads.next;
    list_remove(node);
    struct event_node *event_node = (struct event_node *)((uintptr_t)node - offsetof(struct event_node, node));
    gheap_cache_free(&event_node_cache, event_node);
  }

  waitqueue_destroy(&event->queue, true);
  spinlock_release(&event->lock);
}

void event_trigger(struct event *event, size_t filter, size_t value) {
  spinlock_acquire(&event->lock);

  if (event->uses_filter) {
    while (!LIST_EMPTY(&event->threads)) {
      struct list_node *node = event->threads.next;
      list_remove(node);
      struct event_node *event_node = (struct event_node *)((uintptr_t)node - offsetof(struct event_node, node));
      struct thread *thread = thread_id(event_node->tid);
      thread->event_return = value;
      waitqueue_awaken(&event->queue, event_node->tid);
      gheap_cache_free(&event_node_cache, event_node);
    } 
  } else {
    struct list_node *node = NULL;

    LIST_FOREACH(node, &event->threads) {
      struct event_node *event_node = (struct event_node *)((uintptr_t)node - offsetof(struct event_node, node));
      if (event_node->filter == filter) {
        struct list_node *temp_node = node;
        node = node->prev;
        list_remove(temp_node);
        struct thread *thread = thread_id(event_node->tid);
        thread->event_return = value;

        waitqueue_awaken(&event->queue, event_node->tid);
        gheap_cache_free(&event_node_cache, event_node);
      }
    }
  }

  spinlock_release(&event->lock);
}

size_t event_wait(struct event *event, size_t filter) {
  struct event_node *event_node = gheap_cache_alloc(&event_node_cache);

  event_node->filter = filter;
  event_node->tid = get_cur_thread()->tid;

  spinlock_acquire(&event->lock);
  list_insert(&event->threads, &event_node->node);
  spinlock_release(&event->lock);

  waitqueue_wait(&event->queue);

  return get_cur_thread()->event_return;
}

