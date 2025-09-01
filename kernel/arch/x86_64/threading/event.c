#include <apic_timer.h>
#include <asm.h>
#include <cls.h>
#include <decls.h>
#include <libk/macros.h>
#include <libk/rbtree.h>
#include <libk/spinlock.h>
#include <mem/memory.h>
#include <stddef.h>
#include <threading/threading.h>

typedef struct event_struct {
  void *data;
  void (*handler)(void *);
  rbnode_t deadline_node;
  rbnode_t handle_node;
} event_t;

static size_t current_handle = 1;

void init_events(void) {}

static void event_handler(void) {
  cls_t *cls = get_cls();

  event_t *event = cls->current_event;

  /// If no event is currently running then no event will be run in the future
  if (event == NULL) {
    return;
  }

  void (*handler)(void *) = event->handler;
  void *data = event->data;

  gfree(event);

  spinlock_acquire(&cls->event_lock);
  rbnode_t *node_to_run =
      rb_find_min(&cls->event_deadline_tree, cls->event_deadline_tree.root);
  if (node_to_run == NULL) {
    apic_interrupt_at(MAX_64, event_handler);
    cls->current_event = NULL;
    goto done;
  }

  event_t *event_to_run = container_of(node_to_run, event_t, deadline_node);
  cls->current_event = event_to_run;
  apic_interrupt_at(event_to_run->deadline_node.value, event_handler);
  spinlock_release(&cls->event_lock);

done:
  handler(data);
}

size_t schedule_event(uint64_t ms, void *data, void (*handler)(void *)) {
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

  event_t *event_to_run = container_of(
      rb_find_min(&cls->event_deadline_tree, cls->event_deadline_tree.root),
      event_t, deadline_node);
  cls->current_event = event_to_run;
  apic_interrupt_at(event_to_run->deadline_node.value, event_handler);

  spinlock_release(&cls->event_lock);

  return handle;
}

void cancel_event(size_t handle) {
  if (handle == 0) {
    return;
  }

  cls_t *cls = get_cls();

  spinlock_acquire(&cls->event_lock);
  rbnode_t *event_node =
      rb_search(&cls->event_handle_tree, cls->event_handle_tree.root, handle);
  if (event_node == NULL) {
    spinlock_release(&cls->event_lock);
    return;
  }

  event_t *event = container_of(event_node, event_t, handle_node);
  rb_delete(&cls->event_deadline_tree, &event->deadline_node);
  rb_delete(&cls->event_handle_tree, &event->handle_node);

  rbnode_t *node_to_run =
      rb_find_min(&cls->event_deadline_tree, cls->event_deadline_tree.root);
  if (node_to_run == NULL) {
    apic_interrupt_at(MAX_64, event_handler);
    cls->current_event = NULL;
    goto done;
  }

  event_t *event_to_run = container_of(node_to_run, event_t, deadline_node);
  cls->current_event = event_to_run;
  apic_interrupt_at(event_to_run->deadline_node.value, event_handler);

done:
  spinlock_release(&cls->event_lock);

  gfree(event);
}
