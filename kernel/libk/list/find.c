#include <libk/list.h>
#include <libk/spinlock.h>
#include <stddef.h>

list_node_t *list_find(list_t *list, size_t index) {
  spinlock_acquire(&list->lock);

  list_node_t *cur_node = list->head;
  if (cur_node == NULL) {
    return NULL;
  }

  for (size_t i = 0; i < index; i++) {
    cur_node = cur_node->next;
    if (cur_node == NULL) {
      return NULL;
    }
  }

  spinlock_release(&list->lock);

  return cur_node;
}
