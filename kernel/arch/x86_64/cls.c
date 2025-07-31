#include <asm.h>
#include <cls.h>
#include <libk/list.h>
#include <libk/queue.h>
#include <libk/rbtree.h>
#include <libk/spinlock.h>
#include <mem/memory.h>
#include <stdatomic.h>
#include <stddef.h>

list_t list = {.head = NULL, .lock = ATOMIC_FLAG_INIT};
static spinlock_t lock = ATOMIC_FLAG_INIT;
size_t core_count_global = 0;

void init_cls(void) {
  cls_t *cls = gmalloc(sizeof(cls_t));

  list_insert(&list, NULL, &cls->node);
  spinlock_acquire(&lock);
  core_count_global++;
  spinlock_release(&lock);

  queue_create(&cls->idle_queue);
  rb_create(&cls->normal_queue);
  rb_create(&cls->priority_queue);
  queue_create(&cls->io_queue);
  queue_create(&cls->dead_queue);

#define GS_BASE_MSR 0xC0000101

  wrmsr(GS_BASE_MSR, (size_t)cls);
}

cls_t *get_cls(void) {
  size_t value = rdmsr(GS_BASE_MSR);

  return (cls_t *)value;
}

list_t *get_cls_list(void) { return &list; }

size_t get_core_count(void) { return core_count_global; }

cls_t *get_cls_at(size_t index) {
  list_node_t *node = list_find(&list, index);

  return (cls_t*)((size_t)node - offsetof(cls_t, node));
}
