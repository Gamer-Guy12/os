#include <libk/list.h>
#include <libk/spinlock.h>
#include <stddef.h>

static list_node_t *handle_head(list_t *list) {
  list_node_t *node = list->head;

  if (!node) {
    return NULL;
  }

  if (node->next) {
    list->head = node->next;
    node->next->prev = NULL;
  } else {
    list->head = NULL;
  }

  node->next = NULL;
  node->prev = NULL;

  if (node) {
    __atomic_fetch_sub(&list->count, 1, __ATOMIC_RELEASE);
  }

  return node;
}

list_node_t *list_delete(list_t *list, list_node_t *node) {
  spinlock_acquire(&list->lock);

  if (!node) {
    return handle_head(list);
  }

  if (node->prev) {
    node->prev->next = node->next;
  } else {
    list->head = node->next;
    if (node->next)
      node->next->prev = NULL;
  }

  if (node->next)
    node->next->prev = node->prev;

  node->next = NULL;
  node->prev = NULL;

  spinlock_release(&list->lock);

  if (node)
    __atomic_fetch_sub(&list->count, 1, __ATOMIC_RELEASE);

  return node;
}
