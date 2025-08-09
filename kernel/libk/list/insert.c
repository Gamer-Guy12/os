#include <libk/list.h>
#include <libk/spinlock.h>
#include <stddef.h>

static void insert_after(list_t *list, list_node_t *after, list_node_t *node) {
  list_node_t *next = after->next;

  if (next) {
    next->prev = node;
    node->next = next;
  } else {
    node->next = NULL;
  }

  after->next = node;
  node->prev = node;
}

static void insert_head(list_t *list, list_node_t *node) {
  node->next = list->head;
  if (list->head)
    list->head->prev = node;
  node->prev = NULL;

  list->head = node;
}

void list_insert(list_t *list, list_node_t *after, list_node_t *node) {
  spinlock_acquire(&list->lock);

  if (after)
    insert_after(list, after, node);

  insert_head(list, node);

  spinlock_release(&list->lock);

  __atomic_fetch_add(&list->count, 1, __ATOMIC_RELEASE);
}
