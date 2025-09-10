#include <apic_timer.h>
#include <asm.h>
#include <cls.h>
#include <decls.h>
#include <libk/macros.h>
#include <libk/rbtree.h>
#include <libk/spinlock.h>
#include <mem/memory.h>
#include <stddef.h>
#include <threading.h>
#include <threading/threading.h>

typedef struct event_struct {
  void *data;
  void (*handler)(void *);
  rbnode_t deadline_node;
  rbnode_t handle_node;
} event_t;

static size_t current_handle = 1;

void init_events(void) {}

static void run_next_event(cls_t *cls);

static void event_handler(void) {
  cls_t *cls = get_cls();

  CLI;
  spinlock_acquire(&cls->event_lock);
  event_t *event = cls->current_event;
  void *data = event->data;
  void (*handler)(void *) = event->handler;

  rb_delete(&cls->event_handle_tree, &event->handle_node);
  rb_delete(&cls->event_deadline_tree, &event->deadline_node);

  cls->current_event = NULL;
  gfree(event);

  run_next_event(cls);
  spinlock_release(&cls->event_lock);
  STI;

  if (handler != NULL)
    handler(data);
}

inline static void run_next_event(cls_t *cls) {
  CLI;
  while (true) {
    rbnode_t *node_to_run =
        rb_find_min(&cls->event_deadline_tree, cls->event_deadline_tree.root);
    if (node_to_run == NULL) {
      pause_apic_timer();
      cls->current_event = NULL;
      STI;
      return;
    }

    event_t *event_to_run = container_of(node_to_run, event_t, deadline_node);
    if (event_to_run->deadline_node.value <= rdtsc()) {
      void (*handler)(void *) = event_to_run->handler;
      void *data = event_to_run->data;

      rb_delete(&cls->event_handle_tree, &event_to_run->handle_node);
      rb_delete(&cls->event_deadline_tree, &event_to_run->deadline_node);

      handler(data);

      gfree(event_to_run);

      continue;
    }

    cls->current_event = event_to_run;
    apic_interrupt_at(event_to_run->deadline_node.value, event_handler);
    break;
  }
  STI;
}

size_t schedule_event(uint64_t ms, void *data, void (*handler)(void *)) {
  CLI;
  event_t *event = gmalloc(sizeof(event_t));
  cls_t *cls = get_cls();

  event->handler = handler;
  event->data = data;

  size_t handle = ATOMIC_INC(current_handle);
  event->handle_node.value = handle;
  event->deadline_node.value = ms_to_deadline(ms);

  rb_insert(&cls->event_deadline_tree, &event->deadline_node);
  rb_insert(&cls->event_handle_tree, &event->handle_node);

  spinlock_acquire(&cls->event_lock);

  run_next_event(cls);

  spinlock_release(&cls->event_lock);
  STI;

  return handle;
}

void cancel_event(size_t handle) {
  if (handle == 0) {
    return;
  }

  cls_t *cls = get_cls();

  CLI;
  spinlock_acquire(&cls->event_lock);
  rbnode_t *event_node =
      rb_search(&cls->event_handle_tree, cls->event_handle_tree.root, handle);
  if (event_node == NULL) {
    spinlock_release(&cls->event_lock);
    STI;
    return;
  }

  event_t *event = container_of(event_node, event_t, handle_node);
  rb_delete(&cls->event_deadline_tree, &event->deadline_node);
  rb_delete(&cls->event_handle_tree, &event->handle_node);

  run_next_event(cls);
  spinlock_release(&cls->event_lock);
  STI;

  gfree(event);
}
