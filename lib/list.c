#include "lib/list.h"
#include <stddef.h>

void list_insert(struct list_node *list, struct list_node *node) {
  node->next = list->next;
  node->next->prev = node;

  node->prev = list;
  list->next = node;
}

void list_remove(struct list_node *list, struct list_node *node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;

  node->prev = NULL;
  node->next = NULL;
}
