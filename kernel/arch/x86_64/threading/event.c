#include <apic_timer.h>
#include <asm.h>
#include <decls.h>
#include <libk/rbtree.h>
#include <libk/spinlock.h>
#include <mem/memory.h>
#include <stddef.h>
#include <stdint.h>
#include <threading.h>
#include <threading/threading.h>

/// The deadline is in the rbnode value
typedef struct {
  void *data;
  void (*event)(void *);
  uint64_t required_thread;
  size_t tid;
  rbnode_t node;
} event_t;

static rbtree_t tree;
static spinlock_t lock;
static event_t *current_event;

void init_events(void) { rb_create(&tree); }

static void handler(void) {
  spinlock_acquire(&lock);

  rb_delete(&tree, &current_event->node);
  void (*event)(void *) = current_event->event;
  void *data = current_event->data;
  bool run = current_event->tid == TCB->tid || current_event->tid == MAX_64;
  gfree(current_event);

  rbnode_t *node = rb_find_min(&tree, tree.root);
  if (node == NULL) {
    goto done;
  }

  event_t *event_to_run = (event_t *)((size_t)node - offsetof(event_t, node));
  current_event = event_to_run;
  apic_interrupt_at(event_to_run->node.value, handler);

done:
  spinlock_release(&lock);

  if (run)
    event(data);
}

void schedule_event(uint64_t ms, void *data, void (*event)(void *)) {
  spinlock_acquire(&lock);
  event_t *event_data = gmalloc(sizeof(event_t));

  event_data->data = data;
  event_data->event = event;
  event_data->node.value = ms_to_deadline(ms);
  event_data->tid = MAX_64;

  rb_insert(&tree, &event_data->node);

  rbnode_t *node = rb_find_min(&tree, tree.root);
  event_t *event_to_run = (event_t *)((size_t)node - offsetof(event_t, node));
  current_event = event_to_run;

  apic_interrupt_at(event_to_run->node.value, handler);
  spinlock_release(&lock);
}

void schedule_event_thread(uint64_t ms, void *data, void (*event)(void *),
                           size_t tid) {
  spinlock_acquire(&lock);
  event_t *event_data = gmalloc(sizeof(event_t));

  event_data->data = data;
  event_data->event = event;
  event_data->node.value = ms_to_deadline(ms);
  event_data->tid = tid;

  rb_insert(&tree, &event_data->node);

  rbnode_t *node = rb_find_min(&tree, tree.root);
  event_t *event_to_run = (event_t *)((size_t)node - offsetof(event_t, node));
  current_event = event_to_run;

  apic_interrupt_at(event_to_run->node.value, handler);
  spinlock_release(&lock);
}
