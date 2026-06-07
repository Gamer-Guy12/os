#include "lib/freelist.h"
#include <stddef.h>

void freelist_insert(struct freelist_node *list, void *node) {
  struct freelist_node *list_node = node;
  list_node->next = list->next;
  list->next = node;
}

void *freelist_remove(struct freelist_node *list) {
  if (list == list->next) return NULL;

  struct freelist_node *node = list->next;
  list->next = node->next;
  return node;
}

